# Build und Tests

```sh
python -m venv /workspace/ambientika-tools
/workspace/ambientika-tools/bin/pip install platformio==6.1.18
bash scripts/build.sh
g++ -std=c++17 -Wall -Wextra -Wno-misleading-indentation -fsanitize=address,undefined -fno-omit-frame-pointer -I tests/native tests/native/test.cpp -o /tmp/ambientika-native-tests
/tmp/ambientika-native-tests
```

PlatformIO ESP32-Plattform 6.10.0, Arduino 2.0.17. Das Buildscript erzeugt das
Gesamtimage, überprüft dessen Bestandteile und schreibt `dist/SHA256SUMS`.

Native Tests führen Firmware-Handler und Steuerungsschleife mit simulierten
UART-/Web-/WLAN-Schnittstellen aus. Abgedeckt sind Busfreigabe, Parameter,
Authentifizierung, WRG-Zyklus, Motorpausen, wiederholte Befehle, Boost-Ende,
Einstellungswiederherstellung, Statusparser, Drei-Sekunden-Sendesperre,
Zeitüberlauf und WLAN-Wiederherstellung.

Build, Imageprüfung, native Tests mit AddressSanitizer/UndefinedBehaviorSanitizer
und JavaScript-Syntaxprüfung wurden erfolgreich ausgeführt. Physische
ESP-/WLAN-/Lüftertests sind bisher nicht ausgeführt worden.

Pro ESP ist eine UART-/RS485-Verbindung implementiert. Mehrere Geräte verwenden
dieselbe Firmware mit getrennt gespeicherten WLAN-Einstellungen und API-Schlüsseln.
Ein zentrales Mehrkanal-Gateway und eine Synchronisation der Wechselzyklen sind
nicht implementiert.
