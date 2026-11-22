# Build und Tests

```sh
python -m venv /workspace/ambientika-tools
/workspace/ambientika-tools/bin/pip install platformio==6.1.18
bash scripts/build.sh
g++ -std=c++17 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -I tests/native tests/native/test.cpp -o /tmp/ambientika-native-tests
/tmp/ambientika-native-tests
```

Pfade lassen sich über `AMBIENTIKA_TOOLS` (venv mit PlatformIO) und `PLATFORMIO_CORE_DIR`
anpassen. PlatformIO ESP32-Plattform 6.10.0, Arduino 2.0.17, Standard-Partitionstabelle
(zwei OTA-Partitionen à 1,25 MB). Das Buildscript erzeugt in `dist/v2` Komplettimage,
Update-Image (ab 0xE000) und OTA-Datei, prüft deren Inhalt und schreibt `SHA256SUMS`.

Die v1-Dateien in `dist/v1` sind aus der ursprünglich veröffentlichten v1-Binary
geschnitten (Update-Image = Komplettimage ab 0xE000, OTA = ab 0x10000), damit sie
byte-identisch zur geflashten v1 sind.

## Aufbau der Firmware (`src/main.cpp`)

| Teil | Inhalt |
|---|---|
| Konstanten | Pins, Zeiten laut Herstelleranleitung, Telegramme |
| Lüfterlogik | `setMode`, `busLoop` (500-ms-Takt, Motorpausen, WRG-Zyklus, Boost, Filter-Reset, Statusabfrage mit 3 s Sendesperre), Telegramm-Encoder |
| Persistenz | `saveState` schreibt Betriebsart/Einstellungen nur bei Änderung in den NVS |
| HTTP | Handler für API, Bedienseite (`ui.h`), OTA-Update |
| WLAN/System | Einrichtungs-WLAN, mDNS, statische IP, BOOT-Taster-Reset |
| Protokoll | `logMsg`/`LOG`/`LOG_IMPORTANT`: RAM-Ring (200 × 102 Byte im `.noinit`-Bereich, übersteht Software-Neustarts, mit Magic/Prüfwert) und 16 wichtige Einträge als NVS-Blob; Ausgabe über `/api/log` |

## Tests

Die nativen Tests binden `src/main.cpp` direkt ein und ersetzen UART, Webserver, WLAN, NVS,
Update und mDNS durch Attrappen (`tests/native/*.h`). Abgedeckt: Busfreigabe,
Authentifizierung, Parameterprüfung (auch Loxone-Analogwerte), WRG-Zyklus mit vollständiger
Motorpause, Stufenänderung ohne Pause, Boost inkl. Rückkehr zu „Aus“, manuelle und
automatische Statusabfrage mit Sendesperre, `comm_ok`, Filter-Reset, Telegramm-Encoder für
alle Hand-/Automatiktelegramme, Wiederherstellung nach Neustart, Passwortschutz,
API-Schlüsselwechsel, Einrichtungs-WLAN-Regeln, Protokoll (Inhalt, keine Geheimnisse,
Ringpuffer, Kürzung, Verhalten bei Absturz/Stromausfall/beschädigtem Speicher, WLAN-Gründe,
unerwartete Busdaten), statische IP, OTA (inkl. Ablehnung des
Komplettimages und Sperre bei laufendem Lüfter), BOOT-Reset, `millis()`-Überlauf.
Zusätzlich wurde die Weboberfläche mit Playwright gegen einen Mock-Server geprüft.

Physische Tests mit ESP, WLAN und Lüfter stehen für v2 noch aus (siehe Testliste in
[LOXONE.md](LOXONE.md#7-testen-und-fehlersuche)).
