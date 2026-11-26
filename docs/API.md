# HTTP-API (Firmware v2, Stand 2.2.0)

`KEY` = API-Schlüssel des jeweiligen ESP (Bedienseite, Tab Loxone oder System). Er kann als Parameter
`key=…` oder als Header `X-API-Key` übergeben werden. Befehle per GET oder POST.

## Steuern

| Aufruf | Wirkung |
|---|---|
| `/api/control?key=KEY&mode=M` | Betriebsart setzen |
| `/api/control?key=KEY&mode=M&level=L&humidity=H&night=N` | mit Einstellungen |
| `/api/control?key=KEY&level=L` | nur Stufe (bzw. `humidity`, `night`) – Betriebsart bleibt |
| `/api/filterreset?key=KEY` | Filteralarm zurücksetzen (2 s Motor aus, danach vorherige Betriebsart) |
| `/api/query?key=KEY` | Sensorstatus sofort abfragen (sonst automatisch alle 5 min) |

| Parameter | Werte |
|---|---|
| `mode` | `off`/`0`, `wrg`/`1`, `auto`/`2`, `supply`/`3`, `extract`/`4`, `pause`/`5`, `boost`/`6` |
| `level` | `0`–`3` (Stufe für WRG, Zuluft, Abluft) |
| `humidity` | `2` oder `3` (Feuchtestufe Automatik) |
| `night` | `0` Tag, `1` Nacht (Automatik) |

Zahlen dürfen auch als `2.0` kommen (Loxone-Analogwerte). Antwort: Status-JSON (siehe unten).

| HTTP-Code | Bedeutung |
|---|---|
| 200 | angenommen (bestätigt nur die Annahme durch den ESP) |
| 400 | ungültiger Parameter |
| 401 | API-Schlüssel fehlt/falsch |
| 409 | Busausgabe noch nicht freigegeben |

Verhalten:
- Ein wiederholter identischer Befehl startet keinen Zyklus neu und verlängert keinen Boost.
- Jeder Wechsel der Betriebsart startet mit 10 s Motorpause; eine reine Stufenänderung nicht.
- Boost: 20 min Abluft Stufe 3, danach die vorherige Betriebsart samt Einstellungen
  (auch „Aus“). `level`/`humidity`/`night` **ohne** `mode` während des Boosts gelten für die
  Zeit danach.

## Lesen (ohne Schlüssel)

- `/api/status` – JSON:
  `version, enabled, mode, mode_code, phase, direction, level, night, humidity_level,
  boost_remaining_s, device_status_received, status_age_ms, comm_ok, dark, filter_1,
  filter_2, filter, humidity_alarm, rssi, uptime_s`
- `/api/loxone` – dieselben Werte als `name=wert`-Zeilen für die Loxone-Befehlserkennung
  (Beschreibung in [LOXONE.md](LOXONE.md#7-rückmeldungen)).
- `/network` – WLAN-Verbindung, IP, Gerätename.

`phase`: `disabled`, `off`, `pause`, `supply`, `extract`, `filter_reset`.
Sensorwerte: −1 unbekannt, 0 inaktiv, 1 aktiv.

## Verwaltung (mit Schlüssel, POST)

| Aufruf | Parameter |
|---|---|
| `/enable` | `enabled=0|1` (Sperren nur nach „Aus“) |
| `/wifi` | `ssid`, `password` |
| `/api/password` | `password` (8–63 Zeichen) – Gerätepasswort |
| `/api/apikey` | `newkey` (16–64 Zeichen `A–Z a–z 0–9 - _`) |
| `/api/settings` (GET/POST) | `resume=0|1`, `autoquery=0|1` |
| `/api/network` | `hostname`, `ipmode=dhcp|static`, `ip`, `mask`, `gw`, `dns`, `txlow=0|1` → Neustart |
| `/api/restart` | – |
| `/update` | Multipart-Upload der `…-ota.bin`, nur im Zustand „Aus“ |
| `/api/log/clear` | Protokoll löschen |

Protokoll lesen: `GET /api/log?key=KEY` → Text ([FEHLERSUCHE.md](FEHLERSUCHE.md)).

## Mehrere Lüfter, Loxone-Vorlagen, Einrichtung (ab 2.2.0)

| Aufruf | Schlüssel | Wirkung |
|---|---|---|
| `GET /api/peers` (`?scan=1` startet eine neue Suche) | nein | gefundene Lüfter: `name`, `host`, `ip`, `self` |
| `GET /api/loxone/output.xml` | ja | Loxone-Vorlage Virtueller Ausgang |
| `GET /api/loxone/input.xml` | ja | Loxone-Vorlage Virtueller HTTP-Eingang |
| `GET /api/config/export` (`withkey=1`) | ja | Vorlage für weitere Lüfter (JSON, enthält WLAN- und Gerätepasswort!) |
| `POST /api/config/import` | ja | `ssid`, `pass`, `devpw`, `newkey`, `resume`, `autoquery`, `txlow` |
| `POST /api/settings` | ja | zusätzlich `name` (Anzeigename, 1–40 Zeichen) und `group` (0 eigener Takt, 1 A, 2 B) |
| `GET /api/session` | Gerätepasswort | liefert den API-Schlüssel für die Bedienseite |

`/api/status` enthält ab 2.2.0 zusätzlich `name`, `hostname`, `phase_remaining_s`,
`sync_group`, `group_clock` (Gruppentakt aktiv) und `time_synced`, ab 2.2.2 außerdem
`status_raw` (Inhalt der letzten Lüfterantwort) und `status_decoded` (true = Format laut
Hersteller-PDF, Sensorwerte ausgewertet). `/api/status`,
`/api/loxone`, `/api/peers` und die Steuerbefehle senden
`Access-Control-Allow-Origin: *`, damit die Übersichtsseite eines Lüfters die anderen
abfragen kann. Die Steuerbefehle bleiben durch den Schlüssel geschützt.

Die Bedienseite `/` erfordert nach dem Festlegen des Gerätepassworts HTTP-Basic-Auth
(Benutzer `admin`).
