# Fehlersuche mit dem Protokoll (ab Firmware 2.1.0)

Der ESP führt ein Ereignisprotokoll. Wenn etwas nicht funktioniert, lade es herunter
und gib es zur Analyse weiter, z. B. in einen Chat mit Claude kopieren.
**Passwörter und API-Schlüssel stehen nicht im Protokoll.**

## Protokoll holen

**Über die Webseite (am einfachsten):** ESP-Seite öffnen → Tab **System → Protokoll** →
**Herunterladen** (Textdatei) oder **Kopieren** (Zwischenablage). Mit **Protokoll anzeigen**
siehst du es direkt auf der Seite.

**Direkt per Adresse:** `http://ESP-IP/api/log?key=API-SCHLUESSEL` im Browser öffnen.

**Per USB, falls der ESP nicht ins WLAN kommt:** Jede Protokollzeile wird auch auf der
seriellen Schnittstelle ausgegeben (115200 Baud). Zum Beispiel mit dem Browser-Flasher
https://espressif.github.io/esptool-js/ → **Connect** → Bereich „Console“, oder mit dem
seriellen Monitor der Arduino-IDE. Die Taste `i` gibt die Grunddaten aus (Version, Name,
IP, API-Schlüssel). Achtung: Das ist die einzige Ausgabe, die den Schlüssel zeigt. Sie
gehört nicht in Chats oder Foren.

## Was steht drin?

Kopfbereich (aktueller Zustand): Firmware, Gerätename, Uhrzeit, Startnummer und Grund des
letzten Starts, WLAN-Status und Empfang, Netzwerkmodus, freier Speicher, Bus und
Betriebsart, Optionen, letzte Lüfterantwort und Sensorwerte.

Danach zwei Listen, jeweils älteste Einträge zuerst:

| Liste | Inhalt | Übersteht |
|---|---|---|
| **Wichtige Ereignisse** (16 Einträge) | Starts mit Grund, Abstürze, Updates, Busfreigabe, Einstellungsänderungen, „Lüfter antwortet nicht“, länger anhaltender WLAN-Ausfall, Filteralarm | alles, auch Stromausfall und Updates |
| **Ereignisprotokoll** (200 Einträge) | zusätzlich: jeder Befehl mit Absender-IP (Loxone/Webseite), abgelehnte Befehle mit Grund, Wechsel der Betriebsart, Boost-Ende, WLAN verbunden/getrennt mit Grund, Einrichtungs-WLAN, Sensoränderungen mit Rohdaten, unerwartete Busdaten, Lebenszeichen alle 6 h | Neustarts, Abstürze, Updates – **nicht** Stromausfall |

Zeilenformat:

```
2026-11-22 14:03:12  [#12 +00:05:31]  Befehl von 192.168.178.10 (GET): mode=1
```

Datum und Uhrzeit (per NTP aus dem Internet bzw. vom Router; Einträge vor der
Synchronisation werden nachträglich datiert), `#12` = zwölfter Start des ESP, `+00:05:31` =
Laufzeit seit diesem Start, dann die Meldung.

Damit das Protokoll nicht überläuft, wird nicht jeder Vorgang einzeln aufgezeichnet: Die
regelmäßigen Statusabfragen erscheinen nur, wenn sich etwas ändert, die Richtungswechsel
im WRG-Takt gar nicht.

## Typische Meldungen und was sie bedeuten

| Meldung | Bedeutung / Abhilfe |
|---|---|
| `Grund: Einschalten/Stromausfall` | Normaler Start nach Spannungsausfall. |
| `Grund: ABSTURZ (…)` | Firmware ist abgestürzt und hat neu gestartet. Protokoll bitte melden. |
| `Grund: UNTERSPANNUNG (Brownout)` | Versorgung zu schwach: USB-Netzteil/Kabel tauschen. |
| `WLAN Verbindung fehlgeschlagen: Grund 15/202 Anmeldung fehlgeschlagen` | WLAN-Passwort falsch. |
| `Grund 201 WLAN nicht gefunden` | Name falsch, 5-GHz-Netz oder außer Reichweite. |
| `Grund 200 Funkverbindung verloren` / Empfang unter −80 dBm | Schwacher Empfang. Repeater oder auf der ESP-Seite „WLAN-Sendeleistung reduzieren“ testen. |
| `Abgelehnt 401 (API-Schluessel fehlt/falsch) … von 192.168.178.x` | Loxone (IP des Miniservers) nutzt einen falschen Schlüssel. |
| `Abgelehnt 400 (ungueltiger Wert) … mode=7` | Loxone sendet einen ungültigen Wert, z. B. bei Analogbefehlen „Als Digitalausgang verwenden“ nicht abgeschaltet. |
| `Abgelehnt 409 (Bus gesperrt)` | Lüfter auf der ESP-Seite noch nicht freigegeben. |
| `Luefter antwortet nicht auf Statusabfrage` | Keine Antwort über RS485: Verdrahtung, A/B vertauscht, DIP-Schalter „Master mit Wandpanel“, Versorgung des Wandlers prüfen. |
| `Statusabfrage: ungueltige Antwort '…'` | Es kommen Daten, aber gestört: Masse/Abschlusswiderstand prüfen, A/B tauschen. |
| `Unerwartete Daten auf dem Bus` | Jemand anderes sendet: Wandpanel noch angeschlossen, zweiter Master oder Echo des Wandlers. |
| Keine `Befehl von …`-Zeilen, obwohl Loxone schalten sollte | Befehl kommt nicht an: IP/Adresse im virtuellen Ausgang prüfen, Programm im Miniserver (nicht Simulation). |

## Protokoll löschen

Tab System → **Löschen** (oder `POST /api/log/clear?key=…`). Das leert beide Listen. Sinnvoll
nach abgeschlossener Fehlersuche, damit neue Ereignisse leichter zu finden sind.

## Häufige Fragen bei mehreren Lüftern

| Beobachtung | Ursache / Abhilfe |
|---|---|
| Ein Lüfter fehlt im Tab **Alle** | Er muss eingeschaltet, im selben WLAN und auf Firmware 2.2 oder neuer sein. „Neu suchen“ drücken. Manche Router (Gastnetz, „WLAN-Isolierung“, Mesh mit Multicast-Filter) blockieren mDNS. Dann jeden Lüfter über seine IP öffnen; Loxone ist davon nicht betroffen. |
| Kachel zeigt „nicht erreichbar“ | Lüfter aus oder IP geändert. Feste Zuordnung im Router prüfen. |
| „An alle senden“: „anderer Schlüssel“ | Dieser Lüfter hat einen anderen API-Schlüssel. Unter System den gemeinsamen Schlüssel eintragen oder die Vorlage erneut laden. |
| „Gegenlauf (wartet auf Uhrzeit)“ | Der ESP hat noch keine Uhrzeit. Er braucht Internet oder einen Zeitserver im Router (FRITZ!Box: Heimnetz → Netzwerk → Netzwerkeinstellungen → Zeitsynchronisation). Bis dahin läuft er im eigenen Takt. |
| Partnerlüfter laufen nicht gegenläufig | Beide in derselben Gruppe? Einer muss A, der andere B sein. Die Änderung greift beim nächsten Richtungswechsel. |
| Handy öffnet die Einrichtungsseite nicht automatisch | `http://192.168.4.1` im Browser eingeben. Bei „Kein Internet“ verbunden bleiben. |
| Im automatisch geöffneten Fenster lässt sich die Vorlage nicht auswählen | `http://192.168.4.1` im normalen Browser (Safari/Chrome) öffnen. |
| Einrichtungs-WLAN „Ambientika-…“ ist nicht zu sehen | Es öffnet sich nur ohne gespeichertes WLAN oder nach 2 Minuten ohne Heim-WLAN. Zum Zurücksetzen den BOOT-Taster 10 s halten. |
| Passwort des Einrichtungs-WLANs passt nicht | Nach dem Festlegen des Gerätepassworts gilt dieses statt `Lueftung-Start`. |
