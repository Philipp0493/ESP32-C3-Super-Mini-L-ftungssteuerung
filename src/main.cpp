// Ambientika-Lüftersteuerung für ESP32-C3 Super Mini + RS485 – Firmware v2
// Protokollgrundlage: Südwind „Programmierung über RS485“, Juni 2025.
// Ein ESP steuert genau einen Lüfter (Master mit Wandpanel), kein Slave-Betrieb.
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <esp_system.h>
#include "ui.h"

#define FW_VERSION "2.0.0"

// ---------------------------------------------------------------- Hardware
constexpr int RX_PIN = 4, TX_PIN = 5;
constexpr int BOOT_PIN = 9;  // BOOT-Taster des Super Mini, im Betrieb als Reset-Taster genutzt

// ---------------------------------------------------------------- Zeiten
constexpr uint32_t FRAME_INTERVAL_MS = 500;            // Telegrammtakt laut Hersteller
constexpr uint32_t MOTOR_PAUSE_MS = 10000;             // Pause vor jedem (Neu-)Start / Richtungswechsel
constexpr uint32_t CYCLE_MS = 60000;                   // WRG/Automatik: 60 s je Richtung
constexpr uint32_t BOOST_MS = 20UL * 60 * 1000;        // Boost-Dauer
constexpr uint32_t QUERY_SILENCE_MS = 3000;            // Sendesperre nach Statusabfrage
constexpr uint32_t AUTO_QUERY_MS = 5UL * 60 * 1000;    // automatische Statusabfrage
constexpr uint32_t FIRST_QUERY_MS = 15000;             // erste Abfrage nach dem Start
constexpr uint8_t FILTER_RESET_FRAMES = 4;             // 4 × 500 ms Reset-Telegramm
constexpr uint32_t AP_GRACE_MS = 120000;               // so lange ohne WLAN → Einrichtungs-WLAN
constexpr uint32_t AP_LINGER_MS = 60000;               // nach Verbindung noch offen lassen
constexpr uint32_t AP_DEFAULT_PW_WINDOW_MS = 600000;   // Standardpasswort nur 10 Minuten
constexpr uint32_t FACTORY_RESET_HOLD_MS = 10000;      // BOOT-Taster 10 s halten
constexpr uint32_t RESTART_DELAY_MS = 1500;

constexpr const char *SETUP_PASSWORD = "Lueftung-Start";  // nur solange kein Gerätepasswort gesetzt ist
constexpr const char *WEB_USER = "admin";

// ---------------------------------------------------------------- Telegramme
constexpr const char *PAYLOAD_OFF = "01000001";           // Motor aus, Klappe geschlossen
constexpr const char *PAYLOAD_PAUSE = "01200021";         // Motor Pause, Klappe geöffnet
constexpr const char *PAYLOAD_FILTER_RESET = "01000100";  // Motor aus, Reset Filterwechsel
constexpr const char *PAYLOAD_QUERY = "0202";             // Statusabfrage

// Die Zahlenwerte sind Teil der API (Loxone-Radiotasten senden 0…6).
enum Mode : uint8_t { OFF = 0, WRG = 1, AUTO = 2, SUPPLY = 3, EXTRACT = 4, PAUSE = 5, BOOST = 6 };
const char *const MODE_NAMES[] = {"off", "wrg", "auto", "supply", "extract", "pause", "boost"};
constexpr uint8_t MODE_COUNT = 7;

struct Settings {
  Mode mode;
  uint8_t level, humidity;
  bool night;
  bool operator==(const Settings &o) const {
    return mode == o.mode && level == o.level && humidity == o.humidity && night == o.night;
  }
};

HardwareSerial bus(1);
WebServer web(80);
Preferences prefs;

// ---------------------------------------------------------------- Gespeicherte Konfiguration
String apiKey, devicePassword, hostName, wifiSsid;
bool enabled = false;          // Busausgabe freigegeben
bool resumeAfterBoot = true;   // letzte Betriebsart nach Neustart wiederherstellen
bool autoQuery = true;         // Sensorstatus automatisch abfragen

// ---------------------------------------------------------------- Betriebszustand
Mode mode = OFF;
uint8_t level = 1, humidity = 2;
bool night = false;
bool supply = true;                    // aktuelle Luftrichtung
bool waiting = false;                  // Motorpause läuft
bool pauseSent = false, offSent = false;
uint32_t lastSend = 0, phaseStart = 0, boostStart = 0;
Settings beforeBoost{WRG, 1, 2, false};
Settings savedState{OFF, 1, 2, false};
bool savedStateValid = false;

bool filterResetActive = false;
uint8_t filterResetFrames = 0;
Settings afterFilterReset{OFF, 1, 2, false};

// Statusabfrage
bool receiving = false, queryRequested = false, replyInWindow = false;
uint32_t receiveStart = 0, lastQuery = 0, lastReply = 0;
bool hasReply = false, inFrame = false;
int commOk = -1;  // -1 unbekannt, 0 letzte Abfrage ohne Antwort, 1 Antwort erhalten
String frame;
int light = -1, filter1 = -1, filter2 = -1, moist = -1;

// WLAN / System
bool apActive = false, apWindowExpired = false, mdnsStarted = false;
uint32_t apStarted = 0, disconnectedSince = 0, connectedSince = 0;
bool restartPending = false;
uint32_t restartRequested = 0;
bool bootPressed = false;
uint32_t bootPressedSince = 0;

// Firmware-Update
bool updateAccepted = false;
String updateError;
uint32_t updateOffset = 0;
uint8_t updateMagic0 = 0;

// ================================================================ Hilfsfunktionen
bool elapsed(uint32_t now, uint32_t since, uint32_t duration) { return uint32_t(now - since) >= duration; }
bool cycling(Mode m) { return m == WRG || m == AUTO; }
String chipSuffix() { return String((uint32_t)ESP.getEfuseMac(), HEX); }
String apName() { return "Ambientika-" + chipSuffix(); }
void scheduleRestart() { restartPending = true; restartRequested = millis(); }

bool parseMode(const String &s, Mode &out) {
  if (s.length() == 1 && s[0] >= '0' && s[0] < char('0' + MODE_COUNT)) {
    out = Mode(s[0] - '0');
    return true;
  }
  for (uint8_t i = 0; i < MODE_COUNT; i++)
    if (s == MODE_NAMES[i]) {
      out = Mode(i);
      return true;
    }
  return false;
}

// Loxone kann Analogwerte als "2" oder "2.0" senden; Nachkommanullen werden entfernt.
String numArg(const char *name) {
  String v = web.arg(name);
  int dot = v.indexOf('.');
  if (dot > 0) {
    for (unsigned i = dot + 1; i < v.length(); i++)
      if (v[i] != '0') return v;
    v = v.substring(0, dot);
  }
  return v;
}

// -1 = nicht angegeben, -2 = ungültig, sonst Wert
int optionalDigit(const char *name, int lo, int hi) {
  String v = numArg(name);
  if (v.isEmpty()) return -1;
  if (v.length() != 1 || v[0] < char('0' + lo) || v[0] > char('0' + hi)) return -2;
  return v[0] - '0';
}

String jsonEscape(const String &s) {
  String out;
  for (unsigned i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '"' || c == '\\') { out += '\\'; out += c; }
    else if ((uint8_t)c < 0x20) out += ' ';
    else out += c;
  }
  return out;
}

bool validHostName(const String &s) {
  if (s.isEmpty() || s.length() > 32 || s[0] == '-' || s[s.length() - 1] == '-') return false;
  for (unsigned i = 0; i < s.length(); i++) {
    char c = s[i];
    if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-')) return false;
  }
  return true;
}

bool validSecret(const String &s, unsigned minLen, unsigned maxLen, bool alnumOnly) {
  if (s.length() < minLen || s.length() > maxLen) return false;
  for (unsigned i = 0; i < s.length(); i++) {
    char c = s[i];
    bool alnum = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
    if (alnumOnly ? !(alnum || c == '-' || c == '_') : (c < 0x21 || c > 0x7e)) return false;
  }
  return true;
}

// ================================================================ Lüfterlogik
void sendPayload(const char *payload) {
  bus.write(2);
  bus.print(payload);
  bus.write(3);
  bus.flush();
}

Settings persistentSettings() {
  if (filterResetActive) return afterFilterReset;
  if (mode == BOOST) return beforeBoost;  // Boost wird nie über einen Neustart fortgesetzt
  return {mode, level, humidity, night};
}

void saveState() {
  Settings s = persistentSettings();
  if (savedStateValid && s == savedState) return;  // Flash nur bei Änderungen beschreiben
  prefs.putUChar("mode", s.mode);
  prefs.putUChar("level", s.level);
  prefs.putUChar("hum", s.humidity);
  prefs.putBool("night", s.night);
  savedState = s;
  savedStateValid = true;
}

// Jeder Start einer Lüfterbetriebsart beginnt mit einer vollständigen Motorpause.
void setMode(Mode next) {
  mode = next;
  offSent = false;
  filterResetActive = false;
  supply = !(next == EXTRACT || next == BOOST);
  pauseSent = false;
  waiting = !(next == OFF || next == PAUSE);
  saveState();
}

void applySettings(const Settings &s) {
  level = s.level;
  humidity = s.humidity;
  night = s.night;
  setMode(s.mode);
}

void endBoost() { applySettings(beforeBoost); }

void buildPayload(char out[9]) {
  uint8_t cmd;
  if (mode == AUTO) cmd = (humidity == 3 ? 0xA0 : 0x60) + (supply ? (night ? 8 : 10) : (night ? 4 : 6));
  else cmd = 0x20 + (supply ? 8 : 4) + (mode == BOOST ? 3 : level);
  snprintf(out, 9, "01%02X00%02X", cmd, cmd ^ 1);
}

String phaseName() {
  if (!enabled) return "disabled";
  if (filterResetActive) return "filter_reset";
  if (mode == OFF) return "off";
  if (waiting || mode == PAUSE) return "pause";
  return supply ? "supply" : "extract";
}

int directionCode() {  // 0 = steht, 1 = Zuluft, 2 = Abluft
  String p = phaseName();
  return p == "supply" ? 1 : p == "extract" ? 2 : 0;
}

uint32_t boostRemainingS() {
  if (mode != BOOST) return 0;
  if (waiting) return BOOST_MS / 1000;
  uint32_t run = millis() - boostStart;
  return run >= BOOST_MS ? 0 : (BOOST_MS - run) / 1000;
}

int filterAlarm() { return (filter1 < 0 && filter2 < 0) ? -1 : (filter1 == 1 || filter2 == 1) ? 1 : 0; }

void parseReply(uint32_t now) {
  // Laut Text 11 Byte, das Beispieltelegramm enthält aber "00" + 8 Register zwischen STX/ETX.
  if (frame.length() != 10 || !frame.startsWith("00")) return;
  for (int i = 2; i < 10; i++)
    if (frame[i] != '0' && frame[i] != '1') return;
  light = frame[2] - '0';
  filter1 = frame[3] - '0';
  filter2 = frame[4] - '0';
  moist = frame[5] - '0';
  lastReply = now;
  hasReply = true;
  replyInWindow = true;
}

void readBus(uint32_t now) {
  while (bus.available()) {
    char c = bus.read();
    if (c == 2) {
      frame = "";
      inFrame = true;
    } else if (c == 3) {
      if (inFrame && receiving && !elapsed(now, receiveStart, QUERY_SILENCE_MS)) parseReply(now);
      frame = "";
      inFrame = false;
    } else if (inFrame) {
      if (frame.length() < 32) frame += c;
      else { inFrame = false; frame = ""; }
    }
  }
}

void startQuery(uint32_t now) {
  sendPayload(PAYLOAD_QUERY);
  receiving = true;
  replyInWindow = false;
  inFrame = false;
  frame = "";
  receiveStart = now;
  lastSend = now;
  queryRequested = false;
}

void busLoop(uint32_t now) {
  readBus(now);
  if (!enabled) return;

  // Nach einer Statusabfrage 3 Sekunden lang nichts senden.
  if (receiving) {
    if (!elapsed(now, receiveStart, QUERY_SILENCE_MS)) return;
    receiving = false;
    lastQuery = now;
    commOk = replyInWindow ? 1 : 0;
  }

  if (filterResetActive) {
    if (filterResetFrames >= FILTER_RESET_FRAMES) {
      applySettings(afterFilterReset);  // vorherige Betriebsart, wieder mit Motorpause
      queryRequested = true;            // Filterstatus danach neu einlesen
    } else {
      if (elapsed(now, lastSend, FRAME_INTERVAL_MS)) {
        lastSend = now;
        sendPayload(PAYLOAD_FILTER_RESET);
        filterResetFrames++;
      }
      return;
    }
  }

  if (mode == BOOST && !waiting && elapsed(now, boostStart, BOOST_MS)) endBoost();

  // Die Pausenzeit zählt erst ab dem ersten tatsächlich gesendeten Pausentelegramm.
  if (waiting && !pauseSent) {
    sendPayload(PAYLOAD_PAUSE);
    phaseStart = now;
    lastSend = now;
    pauseSent = true;
    return;
  }
  if (waiting && elapsed(now, phaseStart, MOTOR_PAUSE_MS)) {
    waiting = false;
    phaseStart = now;
    if (mode == BOOST) boostStart = now;
  }
  if (!waiting && cycling(mode) && elapsed(now, phaseStart, CYCLE_MS)) {
    supply = !supply;  // Richtungswechsel nur über Motorpause
    waiting = true;
    pauseSent = false;
    return;
  }

  if (!elapsed(now, lastSend, FRAME_INTERVAL_MS)) return;

  // Statusabfrage nie während einer Motorpause, damit die Pause vollständig gesendet wird.
  bool queryDue = queryRequested || (autoQuery && elapsed(now, lastQuery, AUTO_QUERY_MS));
  if (queryDue && !waiting) {
    startQuery(now);
    return;
  }

  lastSend = now;
  if (mode == OFF) {
    sendPayload(PAYLOAD_OFF);
    offSent = true;
    return;
  }
  if (waiting || mode == PAUSE) {
    sendPayload(PAYLOAD_PAUSE);
    return;
  }
  char payload[9];
  buildPayload(payload);
  sendPayload(payload);
}

// ================================================================ Status
String statusJson() {
  uint32_t now = millis();
  String s = "{";
  s += "\"version\":\"" FW_VERSION "\"";
  s += ",\"enabled\":" + String(enabled ? "true" : "false");
  s += ",\"mode\":\"" + String(MODE_NAMES[mode]) + "\"";
  s += ",\"mode_code\":" + String((int)mode);
  s += ",\"phase\":\"" + phaseName() + "\"";
  s += ",\"direction\":" + String(directionCode());
  s += ",\"level\":" + String((int)level);
  s += ",\"night\":" + String(night ? "true" : "false");
  s += ",\"humidity_level\":" + String((int)humidity);
  s += ",\"boost_remaining_s\":" + String(boostRemainingS());
  s += ",\"device_status_received\":" + String(hasReply ? "true" : "false");
  s += ",\"status_age_ms\":" + (hasReply ? String(uint32_t(now - lastReply)) : String("null"));
  s += ",\"comm_ok\":" + String(commOk);
  s += ",\"dark\":" + String(light);
  s += ",\"filter_1\":" + String(filter1);
  s += ",\"filter_2\":" + String(filter2);
  s += ",\"filter\":" + String(filterAlarm());
  s += ",\"humidity_alarm\":" + String(moist);
  s += ",\"rssi\":" + String((int)WiFi.RSSI());
  s += ",\"uptime_s\":" + String(now / 1000);
  s += "}";
  return s;
}

// Zeilenformat für die Loxone-Befehlserkennung, z. B. "filter=\v".
String loxoneText() {
  String s;
  s += "mode=" + String((int)mode) + "\n";
  s += "level=" + String((int)level) + "\n";
  s += "humidity_level=" + String((int)humidity) + "\n";
  s += "night=" + String(night ? 1 : 0) + "\n";
  s += "enabled=" + String(enabled ? 1 : 0) + "\n";
  s += "direction=" + String(directionCode()) + "\n";
  s += "boost_remaining=" + String(boostRemainingS()) + "\n";
  s += "dark=" + String(light) + "\n";
  s += "filter=" + String(filterAlarm()) + "\n";
  s += "humidity_alarm=" + String(moist) + "\n";
  s += "comm_ok=" + String(commOk) + "\n";
  s += "status_age=" + (hasReply ? String(uint32_t(millis() - lastReply) / 1000) : String(-1)) + "\n";
  s += "rssi=" + String((int)WiFi.RSSI()) + "\n";
  s += "uptime=" + String(millis() / 1000) + "\n";
  return s;
}

String settingsJson() {
  String s = "{";
  s += "\"version\":\"" FW_VERSION "\"";
  s += ",\"hostname\":\"" + hostName + "\"";
  s += ",\"ap_name\":\"" + apName() + "\"";
  s += ",\"ap_active\":" + String(apActive ? "true" : "false");
  s += ",\"has_password\":" + String(devicePassword.isEmpty() ? "false" : "true");
  s += ",\"ssid\":\"" + jsonEscape(wifiSsid) + "\"";
  s += ",\"ipmode\":\"" + prefs.getString("ipmode", "dhcp") + "\"";
  s += ",\"ip\":\"" + prefs.getString("ip", "") + "\"";
  s += ",\"mask\":\"" + prefs.getString("mask", "255.255.255.0") + "\"";
  s += ",\"gw\":\"" + prefs.getString("gw", "") + "\"";
  s += ",\"dns\":\"" + prefs.getString("dns", "") + "\"";
  s += ",\"txlow\":" + String(prefs.getBool("txlow", false) ? "true" : "false");
  s += ",\"resume\":" + String(resumeAfterBoot ? "true" : "false");
  s += ",\"autoquery\":" + String(autoQuery ? "true" : "false");
  s += "}";
  return s;
}

// ================================================================ HTTP-Handler
bool keyValid() { return !apiKey.isEmpty() && (web.arg("key") == apiKey || web.header("X-API-Key") == apiKey); }

bool apiAuth() {
  if (keyValid()) return true;
  web.send(401, "text/plain", "API key required");
  return false;
}

// Die Bedienseite enthält den API-Schlüssel und ist deshalb mit dem Gerätepasswort geschützt.
// Solange noch kein Gerätepasswort festgelegt ist (Ersteinrichtung), ist sie offen.
bool webAuth() {
  if (devicePassword.isEmpty() || web.authenticate(WEB_USER, devicePassword.c_str())) return true;
  web.requestAuthentication();
  return false;
}

void sendStatus() { web.send(200, "application/json", statusJson()); }

void handleRoot() {
  if (!webAuth()) return;
  String html = UI_PAGE;
  html.replace("{{KEY}}", apiKey);
  html.replace("{{VERSION}}", FW_VERSION);
  web.send(200, "text/html; charset=utf-8", html);
}

void handleControl() {
  if (!apiAuth()) return;
  Mode next = mode;
  String m = numArg("mode");
  bool modeGiven = !m.isEmpty();
  if (modeGiven && !parseMode(m, next)) {
    web.send(400, "text/plain", "mode: off|0, wrg|1, auto|2, supply|3, extract|4, pause|5, boost|6");
    return;
  }
  int l = optionalDigit("level", 0, 3), h = optionalDigit("humidity", 2, 3), n = optionalDigit("night", 0, 1);
  if (l == -2 || h == -2 || n == -2) {
    web.send(400, "text/plain", "Invalid level (0-3) / humidity (2-3) / night (0-1)");
    return;
  }
  if (!modeGiven && l < 0 && h < 0 && n < 0) {
    web.send(400, "text/plain", "mode, level, humidity or night required");
    return;
  }
  if (!enabled) {
    web.send(409, "text/plain", "Enable bus after checking wiring first");
    return;
  }

  if (next == BOOST) {
    if (mode == BOOST && !filterResetActive) {
      // Wiederholter Boost verlängert nicht. Nur Werte ohne Betriebsart (z. B. Loxone-Stufe)
      // werden für die Zeit nach dem Boost übernommen.
      if (!modeGiven) {
        if (l >= 0) beforeBoost.level = l;
        if (h >= 0) beforeBoost.humidity = h;
        if (n >= 0) beforeBoost.night = n;
        saveState();
      }
    } else {
      beforeBoost = persistentSettings();  // auch „Aus“ wird nach dem Boost wiederhergestellt
      setMode(BOOST);
    }
    sendStatus();
    return;
  }

  bool settingsChanged = (l >= 0 && l != level) || (h >= 0 && h != humidity) || (n >= 0 && n != night);
  if (l >= 0) level = l;
  if (h >= 0) humidity = h;
  if (n >= 0) night = n;
  // Gleiche Betriebsart, nur andere Stufe: Richtung bleibt, daher keine Motorpause und kein Zyklusneustart.
  if (next != mode || filterResetActive) setMode(next);
  else if (settingsChanged) saveState();
  sendStatus();
}

void handleEnable() {
  if (!apiAuth()) return;
  String e = web.arg("enabled");
  if (e != "0" && e != "1") {
    web.send(400, "text/plain", "enabled=0|1");
    return;
  }
  bool want = e == "1";
  if (want == enabled) {
    web.send(200, "text/plain", "OK");
    return;
  }
  if (!want && (mode != OFF || !offSent)) {
    web.send(409, "text/plain", "Bitte zuerst Aus wählen und mindestens eine Sekunde warten.");
    return;
  }
  enabled = want;
  prefs.putBool("enabled", enabled);
  setMode(OFF);
  web.send(200, "text/plain", "OK");
}

void handleQuery() {
  if (!apiAuth()) return;
  if (!enabled) {
    web.send(409, "text/plain", "Bus gesperrt");
    return;
  }
  queryRequested = true;
  web.send(202, "text/plain", "Abfrage wird gesendet (nach laufender Motorpause); danach 3 s Sendepause");
}

void handleFilterReset() {
  if (!apiAuth()) return;
  if (!enabled) {
    web.send(409, "text/plain", "Bus gesperrt");
    return;
  }
  if (!filterResetActive) afterFilterReset = persistentSettings();
  filterResetActive = true;
  filterResetFrames = 0;
  waiting = false;
  pauseSent = false;
  lastSend = millis() - FRAME_INTERVAL_MS;
  sendStatus();
}

void handleWifi() {
  if (!apiAuth()) return;
  String s = web.arg("ssid"), p = web.arg("password");
  if (s.isEmpty() || s.length() > 32 || p.length() > 63 || (p.length() > 0 && p.length() < 8)) {
    web.send(400, "text/plain", "WLAN-Name 1–32 Zeichen, Passwort leer oder 8–63 Zeichen");
    return;
  }
  prefs.putString("ssid", s);
  prefs.putString("pass", p);
  wifiSsid = s;
  WiFi.begin(s.c_str(), p.c_str());
  web.send(200, "text/plain", "WLAN gespeichert; Verbindung wird aufgebaut");
}

void handlePassword() {
  if (!apiAuth()) return;
  String p = web.arg("password");
  if (!validSecret(p, 8, 63, false)) {
    web.send(400, "text/plain", "Passwort: 8–63 Zeichen, ohne Leerzeichen und Umlaute");
    return;
  }
  devicePassword = p;
  prefs.putString("devpw", p);
  web.send(200, "text/plain", "Gerätepasswort gespeichert");
}

void handleApiKey() {
  if (!apiAuth()) return;
  String k = web.arg("newkey");
  if (!validSecret(k, 16, 64, true)) {
    web.send(400, "text/plain", "Schlüssel: 16–64 Zeichen, nur A–Z, a–z, 0–9, - und _");
    return;
  }
  apiKey = k;
  prefs.putString("key", k);
  web.send(200, "text/plain", "API-Schlüssel gespeichert");
}

void handleSettingsGet() {
  if (!apiAuth()) return;
  web.send(200, "application/json", settingsJson());
}

void handleSettingsPost() {
  if (!apiAuth()) return;
  String r = web.arg("resume"), q = web.arg("autoquery");
  if ((r.length() && r != "0" && r != "1") || (q.length() && q != "0" && q != "1")) {
    web.send(400, "text/plain", "resume/autoquery: 0|1");
    return;
  }
  if (r.length()) { resumeAfterBoot = r == "1"; prefs.putBool("resume", resumeAfterBoot); }
  if (q.length()) { autoQuery = q == "1"; prefs.putBool("autoq", autoQuery); }
  web.send(200, "application/json", settingsJson());
}

void handleNetwork() {
  if (!apiAuth()) return;
  String name = web.arg("hostname"), ipmode = web.arg("ipmode"), tx = web.arg("txlow");
  name.toLowerCase();
  if (!validHostName(name)) {
    web.send(400, "text/plain", "Gerätename: 1–32 Zeichen a–z, 0–9 und -");
    return;
  }
  if (ipmode != "dhcp" && ipmode != "static") {
    web.send(400, "text/plain", "ipmode: dhcp|static");
    return;
  }
  if (ipmode == "static") {
    IPAddress ip, mask, gw, dns;
    String d = web.arg("dns");
    if (d.isEmpty()) d = web.arg("gw");
    if (!ip.fromString(web.arg("ip")) || !mask.fromString(web.arg("mask")) || !gw.fromString(web.arg("gw")) ||
        !dns.fromString(d)) {
      web.send(400, "text/plain", "Ungültige IP-Adresse, Subnetzmaske, Gateway oder DNS");
      return;
    }
    prefs.putString("ip", web.arg("ip"));
    prefs.putString("mask", web.arg("mask"));
    prefs.putString("gw", web.arg("gw"));
    prefs.putString("dns", d);
  }
  prefs.putString("ipmode", ipmode);
  prefs.putString("host", name);
  prefs.putBool("txlow", tx == "1");
  web.send(200, "text/plain", "Gespeichert. Neustart …");
  scheduleRestart();
}

void handleRestart() {
  if (!apiAuth()) return;
  web.send(200, "text/plain", "Neustart …");
  scheduleRestart();
}

// Firmware-Update über den Browser: nur die OTA-Datei (Anwendung), nicht das Komplettimage.
void handleUpdateUpload() {
  HTTPUpload &up = web.upload();
  if (up.status == UPLOAD_FILE_START) {
    updateAccepted = false;
    updateOffset = 0;
    updateError = "";
    if (!keyValid()) { updateError = "API key required"; return; }
    if (enabled && !(mode == OFF && offSent)) {
      updateError = "Bitte zuerst „Aus“ wählen und eine Sekunde warten.";
      return;
    }
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { updateError = String("Update-Start fehlgeschlagen: ") + Update.errorString(); return; }
    updateAccepted = true;
  } else if (up.status == UPLOAD_FILE_WRITE) {
    if (!updateAccepted) return;
    // Komplettimage (für 0x0) erkennen: dort liegt bei 0x8000 die Partitionstabelle (Magic AA 50).
    for (size_t i = 0; updateOffset <= 0x8001 && i < up.currentSize; i++) {
      uint32_t pos = updateOffset + i;
      if (pos == 0x8000) updateMagic0 = up.buf[i];
      if (pos == 0x8001 && updateMagic0 == 0xAA && up.buf[i] == 0x50) {
        Update.abort();
        updateAccepted = false;
        updateError = "Das ist das Komplettimage für USB (0x0). Bitte die Datei …-ota.bin verwenden.";
        return;
      }
      if (pos > 0x8001) break;
    }
    if (Update.write(up.buf, up.currentSize) != up.currentSize) {
      updateError = String("Schreibfehler: ") + Update.errorString();
      Update.abort();
      updateAccepted = false;
      return;
    }
    updateOffset += up.currentSize;
  } else if (up.status == UPLOAD_FILE_END) {
    if (updateAccepted && !Update.end(true)) {
      updateError = String("Update ungültig: ") + Update.errorString();
      updateAccepted = false;
    }
  } else if (up.status == UPLOAD_FILE_ABORTED) {
    if (updateAccepted) Update.abort();
    updateAccepted = false;
    updateError = "Upload abgebrochen";
  }
}

void handleUpdateDone() {
  if (!updateAccepted) {
    int code = updateError == "API key required" ? 401 : 400;
    web.send(code, "text/plain", updateError.isEmpty() ? String("Keine Firmware-Datei empfangen") : updateError);
    return;
  }
  web.send(200, "text/plain", "Update erfolgreich. Der ESP startet neu.");
  scheduleRestart();
}

void handleNetworkStatus() {
  String s = "{\"connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false");
  s += ",\"ip\":\"" + WiFi.localIP().toString() + "\"";
  s += ",\"hostname\":\"" + hostName + "\"}";
  web.send(200, "application/json", s);
}

void setupRoutes() {
  const char *headers[] = {"X-API-Key"};
  web.collectHeaders(headers, 1);
  web.on("/", HTTP_GET, handleRoot);
  web.on("/network", HTTP_GET, handleNetworkStatus);
  web.on("/api/status", HTTP_GET, sendStatus);
  web.on("/api/loxone", HTTP_GET, [] { web.send(200, "text/plain", loxoneText()); });
  web.on("/api/control", HTTP_GET, handleControl);
  web.on("/api/control", HTTP_POST, handleControl);
  web.on("/api/filterreset", HTTP_GET, handleFilterReset);
  web.on("/api/filterreset", HTTP_POST, handleFilterReset);
  web.on("/api/query", HTTP_GET, handleQuery);
  web.on("/api/query", HTTP_POST, handleQuery);
  web.on("/enable", HTTP_POST, handleEnable);
  web.on("/wifi", HTTP_POST, handleWifi);
  web.on("/api/password", HTTP_POST, handlePassword);
  web.on("/api/apikey", HTTP_POST, handleApiKey);
  web.on("/api/settings", HTTP_GET, handleSettingsGet);
  web.on("/api/settings", HTTP_POST, handleSettingsPost);
  web.on("/api/network", HTTP_POST, handleNetwork);
  web.on("/api/restart", HTTP_POST, handleRestart);
  web.on("/update", HTTP_POST, handleUpdateDone, handleUpdateUpload);
}

// ================================================================ WLAN
void startAP() {
  const char *pw = devicePassword.isEmpty() ? SETUP_PASSWORD : devicePassword.c_str();
  WiFi.softAP(apName().c_str(), pw);
  apActive = true;
  apStarted = millis();
}

void stopAP() {
  WiFi.softAPdisconnect(true);
  apActive = false;
}

void setupWifi() {
  WiFi.setHostname(hostName.c_str());
  WiFi.mode(WIFI_STA);
  bool txLow = prefs.getBool("txlow", false);
  if (txLow) WiFi.setTxPower(WIFI_POWER_8_5dBm);  // häufige Abhilfe bei WLAN-Problemen des Super Mini
  if (prefs.getString("ipmode", "dhcp") == "static") {
    IPAddress ip, mask, gw, dns;
    if (ip.fromString(prefs.getString("ip", "")) && mask.fromString(prefs.getString("mask", "")) &&
        gw.fromString(prefs.getString("gw", "")) && dns.fromString(prefs.getString("dns", "")))
      WiFi.config(ip, gw, mask, dns);
  }
  wifiSsid = prefs.getString("ssid", "");
  if (wifiSsid.length()) {
    WiFi.begin(wifiSsid.c_str(), prefs.getString("pass", "").c_str());
    if (txLow) WiFi.setTxPower(WIFI_POWER_8_5dBm);
  } else {
    startAP();  // Ersteinrichtung
  }
  disconnectedSince = millis();
}

void wifiLoop(uint32_t now) {
  if (WiFi.status() == WL_CONNECTED) {
    disconnectedSince = 0;
    if (!connectedSince) connectedSince = now ? now : 1;
    if (!mdnsStarted && MDNS.begin(hostName.c_str())) {
      MDNS.addService("http", "tcp", 80);
      mdnsStarted = true;
    }
    // Einrichtungs-WLAN kurz offen lassen, damit die Seite die neue Adresse anzeigen kann.
    if (apActive && elapsed(now, connectedSince, AP_LINGER_MS)) stopAP();
  } else {
    connectedSince = 0;
    if (!disconnectedSince) disconnectedSince = now ? now : 1;
    bool needed = wifiSsid.isEmpty() || elapsed(now, disconnectedSince, AP_GRACE_MS);
    if (needed && !apActive && !apWindowExpired) startAP();
  }
  // Mit dem öffentlich bekannten Standardpasswort höchstens 10 Minuten je Neustart.
  if (apActive && devicePassword.isEmpty() && wifiSsid.length() && elapsed(now, apStarted, AP_DEFAULT_PW_WINDOW_MS)) {
    stopAP();
    apWindowExpired = true;
  }
}

// ================================================================ System
void factoryResetNetwork() {
  const char *keys[] = {"ssid", "pass", "devpw", "ipmode", "ip", "mask", "gw", "dns", "host", "txlow"};
  for (const char *k : keys) prefs.remove(k);
  Serial.println("BOOT-Taster: WLAN, Gerätepasswort und Netzwerkeinstellungen gelöscht. Neustart.");
  scheduleRestart();
}

void buttonLoop(uint32_t now) {
  if (restartPending) return;
  if (digitalRead(BOOT_PIN) == LOW) {
    if (!bootPressed) { bootPressed = true; bootPressedSince = now; }
    else if (elapsed(now, bootPressedSince, FACTORY_RESET_HOLD_MS)) factoryResetNetwork();
  } else {
    bootPressed = false;
  }
}

void printInfo() {
  Serial.println("Ambientika-Steuerung " FW_VERSION);
  Serial.println("Gerätename: " + hostName + " (http://" + hostName + ".local)");
  Serial.println("Einrichtungs-WLAN: " + apName());
  if (devicePassword.isEmpty()) Serial.println("WLAN-Passwort: " + String(SETUP_PASSWORD) + " (Standard)");
  else Serial.println("WLAN-/Web-Passwort: Gerätepasswort (admin)");
  Serial.println("API-Schlüssel: " + apiKey);
  Serial.println("IP: " + WiFi.localIP().toString());
}

void loadSettings() {
  prefs.begin("ambientika", false);
  apiKey = prefs.getString("key", "");
  if (apiKey.isEmpty()) {
    char key[33];
    for (int i = 0; i < 4; i++) snprintf(key + 8 * i, 9, "%08lx", (unsigned long)esp_random());
    apiKey = key;
    prefs.putString("key", apiKey);
  }
  devicePassword = prefs.getString("devpw", "");
  hostName = prefs.getString("host", "");
  if (!validHostName(hostName)) hostName = "ambientika-" + chipSuffix();
  enabled = prefs.getBool("enabled", false);
  resumeAfterBoot = prefs.getBool("resume", true);
  autoQuery = prefs.getBool("autoq", true);

  uint8_t m = prefs.getUChar("mode", OFF), l = prefs.getUChar("level", 1), h = prefs.getUChar("hum", 2);
  savedState = {m < MODE_COUNT && m != BOOST ? Mode(m) : OFF, l <= 3 ? l : uint8_t(1), (h == 2 || h == 3) ? h : uint8_t(2),
                prefs.getBool("night", false)};
  savedStateValid = true;
}

void setup() {
  Serial.begin(115200);
  uint32_t usbWait = millis();
  while (!Serial && !elapsed(millis(), usbWait, 1500)) delay(10);
  pinMode(BOOT_PIN, INPUT_PULLUP);
  bus.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  loadSettings();
  setupWifi();
  setupRoutes();
  web.begin();
  lastQuery = millis() - AUTO_QUERY_MS + FIRST_QUERY_MS;

  // Nach Stromausfall/Neustart: letzte Betriebsart (nie Boost) über eine Motorpause wieder anfahren.
  if (enabled && resumeAfterBoot && savedState.mode != OFF) applySettings(savedState);
  else {
    level = savedState.level;
    humidity = savedState.humidity;
    night = savedState.night;
    mode = OFF;
  }
  printInfo();
}

void loop() {
  web.handleClient();
  uint32_t now = millis();
  if (Serial.available() && Serial.read() == 'i') printInfo();
  wifiLoop(now);
  buttonLoop(now);
  busLoop(now);
  if (restartPending && elapsed(now, restartRequested, RESTART_DELAY_MS)) ESP.restart();
}
