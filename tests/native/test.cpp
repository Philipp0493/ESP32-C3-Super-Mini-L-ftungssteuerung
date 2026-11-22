// Native Tests: führen die echte Firmware (src/main.cpp) mit simulierten UART/Web/WLAN/NVS aus.
#include <cassert>
#include <iostream>
#include "../../src/main.cpp"

using Args = std::map<std::string, String>;
const std::string STX(1, 2), ETX(1, 3);

void tick(uint32_t n) { fakeNow = n; loop(); }
void expectAt(int line, const char *body) {
  if (bus.out != STX + body + ETX) { std::cerr << "line " << line << ": expected " << body << " got '" << bus.out << "'\n"; assert(false); }
  bus.out.clear();
}
#define expect(b) expectAt(__LINE__, b)
void expectSilentAt(int line) { if (!bus.out.empty()) { std::cerr << "line " << line << ": expected silence, got '" << bus.out << "'\n"; assert(false); } }
#define expectSilent() expectSilentAt(__LINE__)
void call(const char *route, Args a) { web.args = a; web.routes[route](); }
void request(const char *m) { call("/api/control", {{"mode", m}, {"key", apiKey}}); }
// Läuft bis zum Ende einer gerade begonnenen Motorpause und verwirft die Pausentelegramme.
void finishPause() { uint32_t t = fakeNow; tick(t + 1); tick(t + MOTOR_PAUSE_MS + 1); bus.out.clear(); }

void reboot() {
  mode = OFF; waiting = pauseSent = offSent = receiving = queryRequested = filterResetActive = false;
  apActive = apWindowExpired = mdnsStarted = restartPending = bootPressed = false;
  disconnectedSince = connectedSince = 0;
  WiFi.apStarts = WiFi.apStops = WiFi.configs = 0;
  setup();
  bus.out.clear();
}

int main() {
  // ---------- Ersteinrichtung: kein WLAN gespeichert → Einrichtungs-WLAN mit Standardpasswort
  setup();
  assert(WiFi.apStarts == 1 && WiFi.apPassword == SETUP_PASSWORD && apActive);
  assert(apiKey.length() == 32 && prefs.getString("key", "") == apiKey);
  assert(validHostName(hostName) && WiFi.hostname == hostName);
  assert(bootNo == 1 && ramLog.count >= 1 && persistLog.count == 1);  // Startereignis
  fakeResetReason = ESP_RST_SW;  // weitere simulierte Neustarts behalten das RAM-Protokoll
  apiKey = "test-key-0123456789";
  autoQuery = false;  // für deterministische Zeitabläufe; separat getestet

  // ---------- Busfreigabe, Authentifizierung, Validierung
  tick(1000); expectSilent();
  request("wrg"); assert(web.code == 409);
  call("/enable", {{"enabled", "1"}, {"key", apiKey}}); assert(web.code == 200 && enabled && prefs.getBool("enabled", false));
  tick(1050); expect(PAYLOAD_OFF);
  call("/api/control", {{"mode", "wrg"}, {"key", "wrong"}}); assert(web.code == 401);
  call("/api/control", {{"mode", "wrg"}, {"key", apiKey}, {"level", "4"}}); assert(web.code == 400);
  call("/api/control", {{"mode", "7"}, {"key", apiKey}}); assert(web.code == 400);
  call("/api/control", {{"key", apiKey}}); assert(web.code == 400);

  // ---------- WRG-Zyklus mit vollständiger Motorpause
  request("wrg"); assert(web.code == 200 && mode == WRG && waiting);
  tick(1100); expect(PAYLOAD_PAUSE);
  tick(11099); expect(PAYLOAD_PAUSE); assert(waiting);
  tick(11100); expectSilent(); assert(!waiting);
  tick(11600); expect("01290028");  // Zuluft Stufe 1
  auto start = phaseStart; request("wrg"); assert(phaseStart == start && !waiting);  // Wiederholung startet nichts neu
  // Reine Stufenänderung: ohne Pause, Zyklus läuft weiter
  call("/api/control", {{"mode", "wrg"}, {"level", "2"}, {"key", apiKey}}); assert(!waiting && phaseStart == start);
  tick(12100); expect("012A002B");
  call("/api/control", {{"level", "1.0"}, {"key", apiKey}}); assert(web.code == 200 && level == 1 && !waiting);  // Loxone-Analogwert
  tick(12600); expect("01290028");
  tick(71100); expectSilent(); assert(waiting && !supply);  // Richtungswechsel beginnt mit Pause
  tick(71101); expect(PAYLOAD_PAUSE);
  tick(81100); expect(PAYLOAD_PAUSE); assert(waiting);
  tick(81101); expectSilent(); assert(!waiting);
  tick(81601); expect("01250024");  // Abluft erst nach voller Pause

  // ---------- Boost: 20 Minuten, Wiederholung verlängert nicht, danach vorherige Betriebsart
  request("boost"); tick(82000); expect(PAYLOAD_PAUSE);
  tick(92000); expect("01270026"); auto boostAt = boostStart;
  tick(92500); expect("01270026"); request("boost"); assert(boostAt == boostStart && !waiting);
  assert(prefs.getUChar("mode", 99) == WRG);  // Boost wird nie gespeichert
  tick(boostAt + BOOST_MS - 1); expect("01270026"); assert(mode == BOOST);
  tick(boostAt + BOOST_MS); expect(PAYLOAD_PAUSE); assert(mode == WRG && waiting);

  // ---------- Statusabfrage: wartet die Pause ab, dann 3 s Sendesperre
  call("/api/query", {{"key", apiKey}}); assert(web.code == 202);
  uint32_t t0 = fakeNow; tick(t0 + 5000); expect(PAYLOAD_PAUSE);  // während der Pause keine Abfrage
  tick(t0 + MOTOR_PAUSE_MS); expect(PAYLOAD_QUERY); auto queried = fakeNow;
  bus.in = STX + "0010110000" + ETX; tick(queried + 100); expectSilent();
  assert(light == 1 && filter1 == 0 && filter2 == 1 && moist == 1 && filterAlarm() == 1);
  tick(queried + 2999); expectSilent();
  tick(queried + 3000); expect("01290028"); assert(commOk == 1);
  // Abfrage ohne Antwort → comm_ok = 0
  call("/api/query", {{"key", apiKey}}); tick(fakeNow + 500); expect(PAYLOAD_QUERY);
  tick(fakeNow + 3000); bus.out.clear(); assert(commOk == 0);
  // Ungültige Antworten werden verworfen
  for (auto body : {std::string("0010110000") + char(3), STX + "0010" + ETX, STX + "00x0110000" + ETX, STX + std::string(100, '0') + ETX}) {
    call("/api/query", {{"key", apiKey}}); tick(fakeNow + 500); bus.out.clear();
    auto last = lastReply; bus.in = body; tick(fakeNow + 100); assert(lastReply == last);
    tick(fakeNow + 3000); bus.out.clear();
  }

  // ---------- Automatische Abfrage alle 5 Minuten
  autoQuery = true; lastQuery = fakeNow; tick(fakeNow + 500); assert(bus.out.find(PAYLOAD_QUERY) == std::string::npos); bus.out.clear();
  // ohne Pause in diesem Moment (WRG läuft) wird nach Ablauf abgefragt
  phaseStart = lastQuery + AUTO_QUERY_MS - 10000; tick(lastQuery + AUTO_QUERY_MS); expect(PAYLOAD_QUERY);
  tick(fakeNow + 3000); bus.out.clear(); autoQuery = false;

  // ---------- Aus sofort, ohne laufende Phase abzuwarten
  request("off"); tick(fakeNow + 500); expect(PAYLOAD_OFF);

  // ---------- Telegramm-Encoder für alle Hand- und Automatiktelegramme
  for (int direction = 0; direction < 2; direction++)
    for (int speed = 0; speed < 4; speed++) {
      mode = direction ? SUPPLY : EXTRACT; supply = direction; level = speed; waiting = false; phaseStart = fakeNow;
      tick(fakeNow + 500); char body[9]; int c = 0x20 + (direction ? 8 : 4) + speed;
      snprintf(body, 9, "01%02X00%02X", c, c ^ 1); expect(body);
    }
  for (int h = 2; h <= 3; h++)
    for (int n = 0; n < 2; n++)
      for (int d = 0; d < 2; d++) {
        mode = AUTO; humidity = h; night = n; supply = d; phaseStart = fakeNow; waiting = false;
        tick(fakeNow + 500); char body[9]; int c = (h == 3 ? 0xa0 : 0x60) + (d ? (n ? 8 : 10) : (n ? 4 : 6));
        snprintf(body, 9, "01%02X00%02X", c, c ^ 1); expect(body);
      }
  level = 1; humidity = 2; night = false;
  request("off"); tick(fakeNow + 500); bus.out.clear();

  // ---------- Numerische Betriebsarten (Loxone-Radiotasten)
  request("2"); assert(mode == AUTO && web.code == 200);
  request("1.0"); assert(mode == WRG);
  request("0"); assert(mode == OFF);
  call("/api/control", {{"mode", "4"}, {"level", "3"}, {"key", apiKey}}); assert(mode == EXTRACT && level == 3 && !supply);
  // Ungültige Werte ändern nichts
  for (auto bad : {"-1", "4", "1abc", "256", " 1", "1.5"}) {
    Mode before = mode; call("/api/control", {{"mode", "1"}, {"key", apiKey}, {"level", bad}}); assert(web.code == 400 && mode == before);
  }
  request("off"); tick(fakeNow + 500); bus.out.clear();

  // ---------- Boost aus „Aus“ endet wieder in „Aus“
  request("boost"); finishPause(); assert(mode == BOOST && !waiting);
  tick(boostStart + BOOST_MS); assert(mode == OFF); expect(PAYLOAD_OFF);

  // ---------- Wiederholter Boost mit Formularwerten ändert nichts; Stufe ohne Betriebsart gilt danach
  level = 2; humidity = 3; night = true; request("wrg"); request("boost"); finishPause(); auto savedBoost = boostStart;
  call("/api/control", {{"mode", "boost"}, {"level", "1"}, {"night", "0"}, {"key", apiKey}}); assert(boostStart == savedBoost && night && beforeBoost.level == 2);
  call("/api/control", {{"level", "3"}, {"key", apiKey}}); assert(mode == BOOST && beforeBoost.level == 3 && prefs.getUChar("level", 0) == 3);
  tick(savedBoost + BOOST_MS); bus.out.clear(); assert(mode == WRG && level == 3 && humidity == 3 && night);

  // ---------- Filter-Reset: 4 × Reset-Telegramm, dann vorherige Betriebsart über Pause, dann Abfrage
  finishPause(); tick(fakeNow + 500); bus.out.clear();
  call("/api/filterreset", {{"key", apiKey}}); assert(web.code == 200 && filterResetActive);
  assert(statusJson().find("\"phase\":\"filter_reset\"") != std::string::npos);
  for (int i = 0; i < 4; i++) { tick(fakeNow + (i ? 500 : 0)); expect(PAYLOAD_FILTER_RESET); }
  tick(fakeNow + 500); expect(PAYLOAD_PAUSE); assert(mode == WRG && waiting && !filterResetActive && queryRequested);
  tick(fakeNow + MOTOR_PAUSE_MS); expect(PAYLOAD_QUERY); tick(fakeNow + 3000); bus.out.clear();
  // Filter-Reset während Boost → danach die Betriebsart vor dem Boost
  request("boost"); finishPause(); call("/api/filterreset", {{"key", apiKey}});
  for (int i = 0; i < 4; i++) { tick(fakeNow + 500); bus.out.clear(); }
  tick(fakeNow + 500); assert(mode == WRG); bus.out.clear();

  // ---------- Letzte Betriebsart über Neustart (Stromausfall)
  call("/api/control", {{"mode", "supply"}, {"level", "2"}, {"key", apiKey}});
  assert(prefs.getUChar("mode", 99) == SUPPLY && prefs.getUChar("level", 99) == 2);
  int writes = prefs.writes; request("supply"); assert(prefs.writes == writes);  // keine unnötigen Flash-Schreibzugriffe
  reboot(); assert(mode == SUPPLY && level == 2 && waiting && enabled);
  tick(fakeNow + 1); expect(PAYLOAD_PAUSE);  // Wiederanlauf immer mit Motorpause
  call("/api/settings", {{"resume", "0"}, {"key", apiKey}}); assert(!resumeAfterBoot);
  reboot(); assert(mode == OFF && level == 2);
  call("/api/settings", {{"resume", "1"}, {"key", apiKey}});
  apiKey = prefs.getString("key", ""); autoQuery = false;

  // ---------- Bus sperren erfordert „Aus“; Freigabe bei bereits freigegebenem Bus stoppt nichts
  request("wrg"); call("/enable", {{"enabled", "1"}, {"key", apiKey}}); assert(web.code == 200 && mode == WRG);
  call("/enable", {{"enabled", "0"}, {"key", apiKey}}); assert(web.code == 409 && enabled);
  request("off"); call("/enable", {{"enabled", "0"}, {"key", apiKey}}); assert(web.code == 409);
  tick(fakeNow + 500); bus.out.clear(); call("/enable", {{"enabled", "0"}, {"key", apiKey}}); assert(web.code == 200 && !enabled);
  tick(fakeNow + 500); expectSilent();
  call("/enable", {{"enabled", "1"}, {"key", apiKey}}); tick(fakeNow + 500); expect(PAYLOAD_OFF);

  // ---------- Bedienseite und Gerätepasswort
  web.routes["/"](); assert(web.code == 200 && web.body.find(apiKey) != std::string::npos);  // ohne Passwort offen
  assert(web.body.find("{{") == std::string::npos);
  call("/api/password", {{"password", "kurz"}, {"key", apiKey}}); assert(web.code == 400);
  call("/api/password", {{"password", "mit leer"}, {"key", apiKey}}); assert(web.code == 400);
  call("/api/password", {{"password", "Geheim-123"}, {"key", apiKey}}); assert(web.code == 200 && prefs.getString("devpw", "") == "Geheim-123");
  web.authOk = false; web.body = ""; web.routes["/"](); assert(web.code == 401 && web.authRequests == 1 && web.body.find(apiKey) == std::string::npos);
  web.authOk = true; web.routes["/"](); assert(web.code == 200);
  call("/api/settings", {{"key", "wrong"}}); assert(web.code == 401);

  // ---------- API-Schlüssel ändern (z. B. alten Schlüssel nach Neuflashen wiederherstellen)
  call("/api/apikey", {{"newkey", "zu-kurz"}, {"key", apiKey}}); assert(web.code == 400);
  call("/api/apikey", {{"newkey", "alter_schluessel_1234"}, {"key", apiKey}}); assert(web.code == 200 && apiKey == "alter_schluessel_1234");
  assert(prefs.getString("key", "") == apiKey);

  // ---------- WLAN speichern ohne Neustart
  call("/wifi", {{"ssid", "home"}, {"password", "short"}, {"key", apiKey}}); assert(web.code == 400);
  call("/wifi", {{"ssid", "home"}, {"password", "examplepass"}, {"key", apiKey}}); assert(web.code == 200 && WiFi.ssid == "home");

  // ---------- Einrichtungs-WLAN: mit gespeichertem WLAN nur bei Ausfall, geschützt mit Gerätepasswort
  WiFi.connection = 0; reboot(); assert(!apActive && WiFi.apStarts == 0);
  uint32_t b = fakeNow; tick(b + AP_GRACE_MS - 1); assert(!apActive);
  tick(b + AP_GRACE_MS); assert(apActive && WiFi.apPassword == "Geheim-123");
  tick(fakeNow + AP_DEFAULT_PW_WINDOW_MS + 1); assert(apActive);  // mit eigenem Passwort kein Zeitlimit
  WiFi.connection = 1; tick(fakeNow + 1); assert(apActive && MDNS.starts >= 1);
  tick(fakeNow + AP_LINGER_MS); assert(!apActive);
  // ohne Gerätepasswort (z. B. Update von v1): Standardpasswort, höchstens 10 Minuten je Neustart
  prefs.remove("devpw"); WiFi.connection = 0; reboot(); b = fakeNow;
  tick(b + AP_GRACE_MS); assert(apActive && WiFi.apPassword == SETUP_PASSWORD);
  tick(apStarted + AP_DEFAULT_PW_WINDOW_MS); assert(!apActive && apWindowExpired);
  tick(fakeNow + AP_GRACE_MS * 2); assert(!apActive);
  prefs.putString("devpw", "Geheim-123");

  // ---------- Netzwerk: Gerätename, statische IP, Sendeleistung → Neustart
  WiFi.connection = 1; reboot(); apiKey = prefs.getString("key", "");
  call("/api/network", {{"hostname", "-bad"}, {"ipmode", "dhcp"}, {"key", apiKey}}); assert(web.code == 400);
  call("/api/network", {{"hostname", "lueftung-bad"}, {"ipmode", "static"}, {"ip", "192.168.1.300"}, {"mask", "255.255.255.0"}, {"gw", "192.168.1.1"}, {"key", apiKey}}); assert(web.code == 400 && !restartPending);
  call("/api/network", {{"hostname", "Lueftung-Bad"}, {"ipmode", "static"}, {"ip", "192.168.1.80"}, {"mask", "255.255.255.0"}, {"gw", "192.168.1.1"}, {"txlow", "1"}, {"key", apiKey}});
  assert(web.code == 200 && restartPending && prefs.getString("dns", "") == "192.168.1.1");
  int restarts = ESP.restarts; tick(restartRequested + RESTART_DELAY_MS); assert(ESP.restarts == restarts + 1);
  reboot(); assert(hostName == "lueftung-bad" && WiFi.hostname == "lueftung-bad" && WiFi.configs == 1 && WiFi.txPower == WIFI_POWER_8_5dBm);
  assert(settingsJson().find("\"ipmode\":\"static\"") != std::string::npos);
  apiKey = prefs.getString("key", ""); autoQuery = false;

  // ---------- Firmware-Update über den Browser
  request("wrg"); tick(fakeNow + 1); bus.out.clear();
  web.args = {{"key", apiKey}}; web.up = HTTPUpload(); web.up.status = UPLOAD_FILE_START; web.uploads["/update"]();
  web.routes["/update"](); assert(web.code == 400 && updateError.find("Aus") != std::string::npos);  // nur bei „Aus“
  request("off"); tick(fakeNow + 500); bus.out.clear();
  auto upload = [](bool fullImage) {
    web.args = {{"key", apiKey}}; web.up = HTTPUpload(); web.up.status = UPLOAD_FILE_START; web.uploads["/update"]();
    for (uint32_t off = 0; off < 0x9000; off += sizeof(web.up.buf)) {
      web.up.status = UPLOAD_FILE_WRITE; web.up.currentSize = sizeof(web.up.buf); memset(web.up.buf, 0, sizeof(web.up.buf));
      for (uint32_t p : {0x8000u, 0x8001u})
        if (fullImage && p >= off && p < off + sizeof(web.up.buf)) web.up.buf[p - off] = p == 0x8000 ? 0xAA : 0x50;
      web.uploads["/update"]();
    }
    web.up.status = UPLOAD_FILE_END; web.uploads["/update"](); web.routes["/update"]();
  };
  restartPending = false; upload(true); assert(web.code == 400 && Update.aborted && !restartPending);  // Komplettimage abgelehnt
  upload(false); assert(web.code == 200 && Update.ended && restartPending);
  restartPending = false;
  web.args = {{"key", "wrong"}}; web.up.status = UPLOAD_FILE_START; web.uploads["/update"](); web.routes["/update"](); assert(web.code == 401);

  // ---------- Loxone-Text und JSON
  request("wrg"); finishPause();
  String lox = loxoneText();
  for (auto line : {"mode=1\n", "level=2\n", "enabled=1\n", "direction=1\n", "filter=1\n", "humidity_alarm=1\n"})
    if (lox.find(line) == std::string::npos) { std::cerr << "missing " << line << " in\n" << lox; assert(false); }
  String js = statusJson();
  assert(js.find("\"mode_code\":1") != std::string::npos && js.find("\"version\":\"" FW_VERSION "\"") != std::string::npos);

  // ---------- BOOT-Taster 10 s: Netzwerk zurücksetzen, API-Schlüssel und Freigabe bleiben
  restartPending = false; fakeBootPin = LOW; uint32_t p = fakeNow; tick(p); tick(p + FACTORY_RESET_HOLD_MS - 1); assert(!restartPending);
  tick(p + FACTORY_RESET_HOLD_MS); assert(restartPending && !prefs.has("ssid") && !prefs.has("devpw") && !prefs.has("ipmode"));
  assert(prefs.getString("key", "") == apiKey && prefs.getBool("enabled", false));
  fakeBootPin = HIGH;

  // ---------- Protokoll
  auto has = [](const String &text, const char *needle) {
    if (text.find(needle) == std::string::npos) { std::cerr << "Protokoll ohne '" << needle << "'\n" << text; assert(false); }
  };
  web.args = {{"key", "wrong"}}; web.routes["/api/log"](); assert(web.code == 401);
  web.args = {{"key", apiKey}}; web.routes["/api/log"](); assert(web.code == 200);
  String logBody = web.body;
  for (auto n : {"Firmware: " FW_VERSION, "Start #", "Grund letzter Start", "Befehl von 192.0.2.10", "Betriebsart -> WRG Stufe",
                 "Abgelehnt 401", "Abgelehnt 400", "Boost beendet", "Filteralarm-Reset", "Luefter antwortet nicht",
                 "Einrichtungs-WLAN", "WLAN verbunden", "Firmware-Update erfolgreich", "Komplettimage", "BOOT-Taster",
                 "=== Wichtige Ereignisse", "=== Ereignisprotokoll", "Erste Luefterantwort", "ungueltige Antwort"})
    has(logBody, n);
  // Keine Geheimnisse im Protokoll
  for (auto secret : {apiKey.c_str(), "test-key-0123456789", "Geheim-123", "examplepass"})
    assert(logBody.find(secret) == std::string::npos);
  // WLAN-Trennungsgrund aus dem Ereignis-Callback, Wiederholungen nur einmal
  arduino_event_info_t info{}; info.wifi_sta_disconnected.reason = 202;
  WiFi.connection = 0; WiFi.eventCb(ARDUINO_EVENT_WIFI_STA_DISCONNECTED, info); tick(fakeNow + 1);
  uint16_t c = ramLog.count; uint16_t h = ramLog.head;
  WiFi.eventCb(ARDUINO_EVENT_WIFI_STA_DISCONNECTED, info); tick(fakeNow + 1); assert(ramLog.head == h && ramLog.count == c);
  has(logText(), "Anmeldung fehlgeschlagen");
  WiFi.connection = 1; tick(fakeNow + 1);
  // Unerwartete Busdaten werden gesammelt gemeldet
  bus.in = STX + "01290028" + ETX; lastStrayReport = fakeNow - STRAY_REPORT_MS; tick(fakeNow + 1); has(logText(), "Unerwartete Daten auf dem Bus: 10 Byte");
  // Ringpuffer: nur die letzten LOG_SIZE Einträge, älteste zuerst
  for (int i = 0; i < 300; i++) LOG("Testeintrag %03d", i);
  assert(ramLog.count == LOG_SIZE);
  String full = logText(); has(full, "Testeintrag 299");
  assert(full.find("Testeintrag 099") == std::string::npos && full.find("Testeintrag 100") != std::string::npos);
  assert(full.find("Testeintrag 100") < full.find("Testeintrag 299"));
  // Lange Meldungen werden gekürzt, nie übergelaufen
  LOG("%s", std::string(500, 'x').c_str()); assert(strlen(ramLog.entries[(ramLog.head + LOG_SIZE - 1) % LOG_SIZE].msg) == LOG_MSG_LEN - 1);
  // Neustart nach Absturz: RAM-Protokoll bleibt, Grund wird dauerhaft vermerkt
  uint16_t before = bootNo; fakeResetReason = ESP_RST_PANIC; reboot();
  assert(bootNo == before + 1); full = logText(); has(full, "Testeintrag 299"); has(full, "ABSTURZ (Panic)");
  // Stromausfall: RAM-Protokoll leer, wichtige Ereignisse (dauerhaft) bleiben
  fakeResetReason = ESP_RST_POWERON; reboot(); full = logText();
  assert(full.find("Testeintrag") == std::string::npos); has(full, "ABSTURZ (Panic)"); has(full, "Einschalten/Stromausfall");
  // Beschädigter Speicher wird erkannt
  ramLog.head = 9999; fakeResetReason = ESP_RST_SW; reboot(); assert(ramLog.head < LOG_SIZE && ramLog.count < 10 && logText().find("Testeintrag") == std::string::npos);
  prefs.putBytes("implog", "kaputt", 6); reboot(); assert(persistLog.count == 1);
  // Löschen
  apiKey = prefs.getString("key", "");
  call("/api/log/clear", {{"key", apiKey}}); assert(web.code == 200 && persistLog.count == 0 && ramLog.count == 1);

  // ---------- millis()-Überlauf
  assert(elapsed(500, 0xfffffff0, 500)); assert(!elapsed(10, 0xfffffff0, 500));
  std::cout << "PASS: v2 firmware – Zyklus, Motorpausen, Boost, Statusabfrage (manuell/automatisch), Filter-Reset, "
               "Persistenz, Passwortschutz, API-Schlüssel, WLAN/Einrichtungs-WLAN, statische IP, OTA, BOOT-Reset, Protokoll, Überlauf\n";
}
