# Flashen

Pro Version gibt es drei Dateien. Welche du brauchst, hängt davon ab, ob die
Einstellungen (WLAN, API-Schlüssel, Freigabe) erhalten bleiben sollen:

| Datei | Wie | Wohin | Einstellungen |
|---|---|---|---|
| `ambientika-c3-v2-komplett-0x0.bin` | USB | **0x0** | werden **gelöscht** (neuer API-Schlüssel) – für neue ESPs |
| `ambientika-c3-v2-update-0xE000.bin` | USB | **0xE000** | bleiben erhalten – für ESPs, auf denen schon v1/v2 läuft |
| `ambientika-c3-v2-ota.bin` | ESP-Webseite, Tab System | – | bleiben erhalten – nur wenn bereits v2 läuft |

Dateien: [v2](../dist/v2/) · [v1](../dist/v1/) · Wechsel zwischen den Versionen: [VERSIONEN.md](VERSIONEN.md)

Auf GitHub die Dateiseite öffnen und **„Download raw file“** nutzen – nicht die
GitHub-HTML-Seite als `.bin` speichern.

## Per USB mit dem Browser-Flasher

Espressifs Browser-Flasher funktioniert in Chrome oder Edge am PC:
https://espressif.github.io/esptool-js/

1. ESP mit USB-Datenkabel am PC anschließen. Der Lüfter kann angeschlossen bleiben,
   sicherer ist es, vorher auf der ESP-Seite „Aus“ zu wählen.
2. Den Browser-Flasher öffnen, Baudrate **115200**, **Connect** drücken und den USB-Port
   des ESP wählen. Chiptyp muss **ESP32-C3** sein.
3. Klappt die Verbindung nicht: BOOT gedrückt halten, RESET kurz drücken, BOOT loslassen,
   erneut Connect.
4. **Eine** Zeile ausfüllen:

   | Feld | Neuer ESP | Update (Einstellungen behalten) |
   |---|---|---|
   | Flash Address | `0x0` | `0xE000` |
   | File | `…-komplett-0x0.bin` | `…-update-0xE000.bin` |

   Flash Mode/Frequency/Size auf `keep` lassen. **Kein „Erase Flash“** – das löscht
   alle Einstellungen.
5. **Program** drücken, Abschluss abwarten, **Disconnect**, am ESP kurz **RESET** drücken
   (ohne BOOT).

Die Adresse muss zur Datei passen: Das Komplettimage gehört an 0x0, das Update-Image an
0xE000. Das Update-Image überschreibt den Einstellungsspeicher (0x9000–0xDFFF) nicht.

Als installiertes Windows-Programm eignet sich auch Espressifs **Flash Download Tools**
(https://www.espressif.com/en/support/download/other-tools): ESP32-C3 / UART, eine Datei
mit der passenden Adresse, DIO, 40 MHz, 4 MB.

## Über die Weboberfläche (ab v2)

1. ESP-Seite öffnen, im Tab **Lüfter** „Aus“ wählen und eine Sekunde warten
   (bei laufendem Lüfter lehnt der ESP das Update ab).
2. Tab **System → Firmware-Update**: Datei `…-ota.bin` wählen, **Firmware hochladen**.
3. Nach ca. 20 s startet der ESP neu. Mit aktivierter Wiederherstellung läuft die
   Lüftung danach in der zuletzt gespeicherten Betriebsart.

Das Komplettimage wird hier erkannt und abgelehnt. Schlägt ein Update fehl, läuft die
bisherige Firmware weiter.

## Nach dem ersten Flashen (neuer ESP)

Weiter mit der [Schritt-für-Schritt-Anleitung](ANLEITUNG.md#5-ersten-lüfter-einrichten).
