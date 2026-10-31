# Flashen mit grafischer Oberfläche

Du brauchst keinen Python-Befehl. Am einfachsten ist Espressifs grafischer
Browser-Flasher auf einem PC mit Chrome oder Edge und WebSerial-Unterstützung:
https://espressif.github.io/esptool-js/

Der Flasher läuft als Webseite, greift aber auf den per USB angeschlossenen ESP
zu. Die spätere Lüfter-Webseite läuft unabhängig davon auf deinem ESP.

1. [Binary herunterladen](../dist/ambientika-c3-single-fan.bin). Auf GitHub die
   Dateiseite öffnen und „Download raw file“ nutzen oder das [ZIP](../dist/ambientika-c3-testpaket.zip)
   herunterladen und entpacken. Nicht die GitHub-HTML-Seite als `.bin` speichern.
2. ESP mit USB-Datenkabel am PC anschließen. Für das erste Flashen kann der
   Lüfter abgeklemmt bleiben. Andere serielle Programme schließen.
3. Den Browser-Flasher öffnen. Baudrate zunächst **115200** wählen.
4. **Connect** drücken und den USB-Port des ESP auswählen. Chiptyp muss
   **ESP32-C3** sein, Flashgröße **4 MB**. Bei abweichender Hardware stoppen.
5. Wenn die Verbindung nicht klappt: BOOT gedrückt halten, RESET kurz drücken,
   BOOT loslassen und Connect erneut versuchen. USB-Port kann sich dabei ändern.
6. Eine Datei hinzufügen, falls nötig über **Add File**. Nur eine Zeile verwenden:

   | Feld | Einstellung |
   |---|---|
   | Flash Address | **0x0** |
   | File | `ambientika-c3-single-fan.bin` |
   | Flash Mode | `keep` oder `dio` |
   | Flash Frequency | `keep` (Image enthält 40 MHz) |
   | Flash Size | `keep` (Image enthält 4 MB) |
   | Erase all | für den normalen Ablauf nicht erforderlich |

7. **Program** drücken. USB-Kabel nicht trennen. Erfolgreiches Ende abwarten;
   bei Fehlern nicht von einem vollständigen Flash ausgehen.
8. **Disconnect** drücken. RESET kurz drücken, ohne BOOT gedrückt zu halten.
9. Mit der [WLAN-Einrichtung](../README.md#3-einrichten--ohne-usb-terminal) fortfahren.

Das Gesamtimage enthält Bootloader und Anwendung; es gehört an **0x0**, nicht
an 0x10000. Erneutes Flashen setzt WLAN-Einstellungen und API-Schlüssel zurück.
Ein vollständiges „Erase Flash“ löscht ebenfalls sämtliche Geräteeinstellungen.

Falls der Browser keinen USB-Port anbietet: aktuellen Chrome/Edge auf dem PC
verwenden, Datenkabel prüfen, serielle Programme schließen und USB-Bootmodus
versuchen. Smartphones/Tablets sind für diesen Ablauf nicht vorgesehen.

Als installiertes Windows-Programm ist auch Espressifs **Flash Download Tools**
geeignet (offizielle Bezugsquelle https://www.espressif.com/en/support/download/other-tools).
ESP32-C3 / UART, eine BIN an 0x0, DIO, 40 MHz, 4 MB bzw. 32 Mbit wählen und den
passenden COM-Port einstellen. Menübezeichnungen unterscheiden sich je Version.
Der Browser-Flasher ist hier der dokumentierte Standardweg.
