#pragma once
// Bedienseite. {{KEY}} und {{VERSION}} werden beim Ausliefern ersetzt.
const char UI_PAGE[] = R"HTML(<!doctype html><html lang="de"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Meine Lüftung</title>
<style>
body{font:17px system-ui;background:#f2f5f7;color:#172b3a;max-width:800px;margin:auto;padding:16px}
section{background:#fff;border-radius:16px;padding:20px;margin:18px 0}
button,input,select{font:inherit;padding:12px;border:1px solid #b5c4cd;border-radius:8px;max-width:100%;box-sizing:border-box}
button{background:#176958;color:#fff;cursor:pointer;margin:6px 6px 6px 0}button:disabled{opacity:.45;cursor:default}
label{display:block;margin:12px 0}input,select{display:block;width:100%;margin-top:6px}
label.check{display:flex;gap:10px;align-items:center}label.check input{width:auto;margin:0}
code{overflow-wrap:anywhere;background:#eef3f5;padding:2px 5px;border-radius:4px}
#message{position:sticky;top:0;background:#e3f3eb;padding:14px;white-space:pre-wrap;border-radius:8px;z-index:1}
small{color:#4c6372}h1{margin-bottom:0}.danger{background:#a03838}.secondary{background:#4c6372}
.warn{background:#fdf0d5;border-left:5px solid #d89614;padding:12px;border-radius:8px}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:0 12px}@media(max-width:560px){.grid{grid-template-columns:1fr}}
progress{width:100%;height:18px}
pre{background:#0f2230;color:#d8e6ee;padding:12px;border-radius:8px;max-height:420px;overflow:auto;font-size:12px;white-space:pre-wrap;overflow-wrap:anywhere}
</style>
<h1>Meine Lüftung</h1><p id="sub">Ambientika · ein Lüfter · Firmware {{VERSION}}</p>
<div id="message" role="status" aria-live="polite">Verbindung wird geprüft …</div>

<section><h2>1 · Zugang und Heim-WLAN</h2>
<div id="pwwarn" class="warn" hidden><b>Noch kein Gerätepasswort festgelegt.</b> Diese Seite ist offen und das Einrichtungs-WLAN nutzt das Standardpasswort. Bitte jetzt ein eigenes Passwort festlegen.</div>
<form id="password"><label>Gerätepasswort (8–63 Zeichen, ohne Leerzeichen)<input name="password" type="password" minlength="8" maxlength="63" autocomplete="new-password" required></label><label>Passwort wiederholen<input name="repeat" type="password" maxlength="63" autocomplete="new-password" required></label><button>Gerätepasswort speichern</button></form>
<small>Das Gerätepasswort schützt diese Seite (Benutzername <b>admin</b>), das Einrichtungs-WLAN und Firmware-Updates. Vergessen? BOOT-Taster am ESP 10 s gedrückt halten: WLAN, Passwort und Netzwerkeinstellungen werden gelöscht, API-Schlüssel und Lüfterfreigabe bleiben.</small>
<form id="wifi"><h3>Heim-WLAN</h3><p>Nur 2,4-GHz-WLAN. Nach dem Speichern bleibt diese Seite zunächst erreichbar.</p><label>WLAN-Name<input name="ssid" maxlength="32" required autocomplete="off"></label><label>WLAN-Passwort<input name="password" type="password" maxlength="63" autocomplete="new-password"></label><button>WLAN verbinden</button></form><p id="network"></p></section>

<section><h2>2 · Gerätename und feste IP-Adresse</h2>
<p>Empfohlen: <b>DHCP</b> lassen und im Router eine feste Zuordnung für diesen ESP anlegen (siehe Anleitung). Alternativ hier eine statische Adresse eintragen, die <b>außerhalb</b> des DHCP-Bereichs des Routers liegt.</p>
<form id="net"><label>Gerätename im Netzwerk (a–z, 0–9, -)<input name="hostname" maxlength="32" pattern="[a-z0-9]([a-z0-9-]*[a-z0-9])?" required></label>
<label>IP-Adresse beziehen<select name="ipmode"><option value="dhcp">Automatisch (DHCP) – empfohlen</option><option value="static">Statisch im ESP festlegen</option></select></label>
<div id="static" class="grid"><label>IP-Adresse<input name="ip" placeholder="192.168.178.80" inputmode="decimal"></label><label>Subnetzmaske<input name="mask" placeholder="255.255.255.0" inputmode="decimal"></label><label>Gateway (Router)<input name="gw" placeholder="192.168.178.1" inputmode="decimal"></label><label>DNS (leer = Gateway)<input name="dns" inputmode="decimal"></label></div>
<label class="check"><input type="checkbox" name="txlow" value="1">WLAN-Sendeleistung reduzieren (hilft beim ESP32-C3 Super Mini oft bei Verbindungsabbrüchen)</label>
<button>Speichern und neu starten</button></form><small id="netinfo"></small></section>

<section><h2>3 · Lüfter freigeben</h2><p>Vorher spannungsfrei verdrahten und den Lüfter auf „Master mit Wandpanel“ einstellen.</p><p id="bus"></p><button id="enable">Verdrahtung geprüft – freigeben</button><button id="disable" class="secondary">Busausgabe sperren</button><p><small>Zum Sperren zuerst „Aus“ wählen. Sperren beendet nur die Befehlsübertragung.</small></p></section>

<section><h2>4 · Lüftung bedienen</h2><p id="state">Noch kein Zustand verfügbar.</p>
<form id="control"><label>Betriebsart<select name="mode"><option value="wrg">Wärmerückgewinnung – Standard</option><option value="auto">Automatik mit Feuchtesensor</option><option value="boost">Boost – 20 Minuten Abluft Stufe 3</option><option value="supply">Dauerhafte Zuluft</option><option value="extract">Dauerhafte Abluft</option><option value="pause">Pause – Klappe offen</option><option value="off">Aus – Klappe geschlossen</option></select></label><label>Lüfterstufe<select name="level"><option value="1">1 – niedrig</option><option value="2">2 – mittel</option><option value="3">3 – hoch</option><option value="0">0 – gerätespezifische Stufe</option></select></label><label>Feuchtestufe für Automatik<select name="humidity"><option value="2">2</option><option value="3">3</option></select></label><label>Nachtbetrieb für Automatik<select name="night"><option value="0">Nein</option><option value="1">Ja</option></select></label><button>Übernehmen</button></form>
<button class="danger" id="off">Aus wählen</button><button id="query" class="secondary">Sensorstatus jetzt abfragen</button><button id="filterreset" class="secondary">Filteralarm zurücksetzen</button>
<p id="sensors">Noch keine bestätigte Antwort vom Lüfter.</p>
<label class="check"><input type="checkbox" id="resume">Nach Stromausfall/Neustart letzte Betriebsart wiederherstellen (Boost ausgenommen)</label>
<label class="check"><input type="checkbox" id="autoquery">Sensorstatus alle 5 Minuten automatisch abfragen</label>
<small>Die Anzeige beschreibt gesendete Befehle, keine gemessene Drehzahl. Richtungswechsel enthalten 10 s Motorpause. WRG und Automatik wechseln alle 60 s die Richtung. Eine reine Stufenänderung wird ohne Pause übernommen. Filter-Reset: 2 s „Motor aus, Reset Filterwechsel“, danach weiter in der vorherigen Betriebsart.</small></section>

<section><h2>5 · Mit Loxone verbinden</h2>
<p>In Loxone Config: <b>Virtueller Ausgang</b> mit der Adresse unten, darunter je Befehl ein <b>Virtueller Ausgangsbefehl</b>. Ausführlich in <code>docs/LOXONE.md</code>.</p>
<label>Adresse für den Virtuellen Ausgang<input id="base" readonly></label>
<label>Befehl auswählen<select id="loxmode">
<option value="/api/control?key=KEY&mode=<v>|analog">Betriebsart per Radiotasten (Analogwert 0–6)</option>
<option value="/api/control?key=KEY&level=<v>|analog">Lüfterstufe per Analogwert (0–3)</option>
<option value="/api/control?key=KEY&mode=wrg&level=1|digital">WRG Stufe 1 (Taster)</option>
<option value="/api/control?key=KEY&mode=auto&humidity=2&night=0|digital">Automatik Tag (Taster)</option>
<option value="/api/control?key=KEY&mode=auto&humidity=2&night=1|digital">Automatik Nacht (Taster)</option>
<option value="/api/control?key=KEY&mode=boost|digital">Boost 20 min (Taster)</option>
<option value="/api/control?key=KEY&mode=off|digital">Aus (Taster)</option>
<option value="/api/filterreset?key=KEY|digital">Filteralarm zurücksetzen (Taster)</option>
</select></label>
<label>„Befehl bei EIN“<input id="cmd" readonly></label><p id="cmdhint"></p>
<button id="copy">Befehl kopieren</button>
<p>Rückmeldung: <b>Virtueller HTTP Eingang</b> mit URL <code id="statusurl"></code>, Befehlserkennungen z. B. <code>mode=\v</code>, <code>filter=\v</code>, <code>comm_ok=\v</code>.</p>
<details><summary>API-Schlüssel</summary><p><code id="key"></code></p><p><small>Nur im vertrauenswürdigen Heimnetz verwenden, keine Portfreigabe im Router. Nach einem Neuflashen per Komplettimage entsteht ein neuer Schlüssel; hier kann der alte wieder eingetragen werden, damit Loxone nicht angepasst werden muss.</small></p>
<form id="apikey"><label>Eigenen/alten API-Schlüssel setzen (16–64 Zeichen A–Z, a–z, 0–9, - _)<input name="newkey" minlength="16" maxlength="64" pattern="[A-Za-z0-9_-]{16,64}" required autocomplete="off"></label><button class="secondary">Schlüssel speichern</button></form></details></section>

<section><h2>6 · Firmware-Update</h2><p>Datei <code>ambientika-c3-…-ota.bin</code> auswählen. Vorher „Aus“ wählen. Einstellungen, WLAN und API-Schlüssel bleiben erhalten.</p>
<form id="update"><input type="file" name="firmware" accept=".bin" required><progress id="progress" max="100" value="0" hidden></progress><button>Firmware hochladen</button></form>
<button id="restart" class="secondary">ESP neu starten</button></section>

<section><h2>7 · Protokoll</h2><p>Der ESP protokolliert Starts (mit Grund, z. B. Stromausfall oder Absturz), WLAN-Verbindungen, Befehle von Loxone und Webseite, abgelehnte Befehle, Antworten des Lüfters und Updates. Bei Problemen das Protokoll herunterladen oder kopieren und zur Fehlersuche weitergeben – Passwörter und API-Schlüssel sind nicht enthalten.</p>
<button id="logshow">Protokoll anzeigen</button><button id="logdl" class="secondary">Herunterladen</button><button id="logcopy" class="secondary">Kopieren</button><button id="logclear" class="danger">Löschen</button>
<pre id="log" hidden></pre><small>Die letzten 200 Einträge überstehen Neustarts und Updates, aber keinen Stromausfall. Wichtige Ereignisse (Starts, Fehler, Updates, Einstellungsänderungen) werden zusätzlich dauerhaft gespeichert.</small></section>
<script>
const key='{{KEY}}',$=s=>document.querySelector(s);let initialized=false,settings=null;
$('#key').textContent=key;
const names={wrg:'Wärmerückgewinnung',auto:'Automatik',boost:'Boost',off:'Aus',pause:'Pause',supply:'Zuluft',extract:'Abluft',disabled:'Bus gesperrt',filter_reset:'Filter-Reset'};
const flag=v=>v<0?'unbekannt':v?'aktiv':'inaktiv';
function message(s){$('#message').textContent=s}
async function command(path,params={}){let r=await fetch(path+'?'+new URLSearchParams({...params,key}),{method:'POST'});let t=await r.text();if(!r.ok)throw Error(t||('HTTP '+r.status));return t}
function bind(id,fn){$(id).onclick=async()=>{try{await fn();await refresh()}catch(e){message('Aktion fehlgeschlagen: '+e.message)}}}
function form(id,fn){$(id).onsubmit=async e=>{e.preventDefault();try{await fn(Object.fromEntries(new FormData(e.target)),e.target)}catch(err){message('Fehler: '+err.message)}}}
form('#password',async d=>{if(d.password!==d.repeat){message('Die Passwörter stimmen nicht überein.');return}await command('/api/password',{password:d.password});message('Gerätepasswort gespeichert. Beim nächsten Öffnen der Seite: Benutzer „admin“ und dieses Passwort. Es gilt ab dem nächsten Start auch für das Einrichtungs-WLAN.');$('#password').reset();loadSettings()});
form('#wifi',async d=>{if(d.password.length>0&&d.password.length<8){message('Das WLAN-Passwort muss mindestens 8 Zeichen haben.');return}await command('/wifi',d);message('WLAN gespeichert. Verbindung wird aufgebaut. Bitte bis zu 60 Sekunden warten.')});
form('#net',async d=>{d.txlow=d.txlow?'1':'0';await command('/api/network',d);let addr=d.ipmode==='static'?d.ip:d.hostname+'.local';message('Gespeichert, der ESP startet neu. Danach erreichbar unter http://'+addr+(d.ipmode==='static'?'':' bzw. unter der im Router zugeordneten IP')+'.')});
form('#control',async d=>{await command('/api/control',d);message('Betriebsart übernommen. Eine Motorpause vor dem Start ist normal.');await refresh()});
form('#apikey',async d=>{await command('/api/apikey',d);message('API-Schlüssel gespeichert. Seite wird neu geladen.');setTimeout(()=>location.reload(),1500)});
$('#update').onsubmit=e=>{e.preventDefault();let f=e.target.firmware.files[0];if(!f)return;if(!/ota\.bin$/i.test(f.name)&&!confirm('Der Dateiname endet nicht auf „ota.bin“. Trotzdem hochladen?'))return;
 let x=new XMLHttpRequest(),fd=new FormData();fd.append('firmware',f,f.name);x.open('POST','/update?key='+encodeURIComponent(key));let p=$('#progress');p.hidden=false;p.value=0;
 x.upload.onprogress=ev=>{if(ev.lengthComputable)p.value=Math.round(ev.loaded*100/ev.total)};
 x.onload=()=>{message(x.status===200?'Update erfolgreich. Der ESP startet neu, Seite in ca. 20 s neu laden.':'Update fehlgeschlagen: '+x.responseText);if(x.status===200)setTimeout(()=>location.reload(),20000)};
 x.onerror=()=>message('Upload unterbrochen.');message('Firmware wird hochgeladen …');x.send(fd)};
bind('#enable',async()=>{await command('/enable',{enabled:1});message('Bus freigegeben. Jetzt z. B. WRG Stufe 1 wählen.')});
bind('#disable',async()=>{await command('/enable',{enabled:0});message('Busausgabe gesperrt.')});
bind('#off',async()=>{await command('/api/control',{mode:'off'});message('Aus beauftragt.')});
bind('#query',async()=>{await command('/api/query');message('Status angefordert. Drei Sekunden Sendepause abwarten.')});
bind('#filterreset',async()=>{if(!confirm('Filteralarm zurücksetzen? Nur nach Filtertausch/-reinigung. Der Motor stoppt dafür 2 Sekunden.'))return;await command('/api/filterreset');message('Filter-Reset gesendet. Der Lüfter läuft danach in der vorherigen Betriebsart weiter.')});
async function getLog(){let r=await fetch('/api/log?key='+encodeURIComponent(key));if(!r.ok)throw Error(await r.text());return r.text()}
bind('#logshow',async()=>{let t=await getLog(),l=$('#log');l.textContent=t;l.hidden=false;l.scrollTop=l.scrollHeight;message('Protokoll geladen.')});
bind('#logdl',async()=>{let t=await getLog(),a=document.createElement('a');a.href=URL.createObjectURL(new Blob([t],{type:'text/plain'}));a.download='ambientika-'+(settings?settings.hostname:'esp')+'-protokoll-'+new Date().toISOString().slice(0,16).replace(/[:T]/g,'-')+'.txt';document.body.appendChild(a);a.click();a.remove();message('Protokoll heruntergeladen.')});
bind('#logcopy',async()=>{let t=await getLog();try{await navigator.clipboard.writeText(t);message('Protokoll in die Zwischenablage kopiert.')}catch(e){let l=$('#log');l.textContent=t;l.hidden=false;let r=document.createRange();r.selectNodeContents(l);let sel=getSelection();sel.removeAllRanges();sel.addRange(r);message('Protokoll markiert – mit Strg+C kopieren.')}});
bind('#logclear',async()=>{if(!confirm('Protokoll wirklich löschen?'))return;await command('/api/log/clear');$('#log').hidden=true;message('Protokoll gelöscht.')});
bind('#restart',async()=>{await command('/api/restart');message('ESP startet neu …')});
for(let id of ['resume','autoquery'])$('#'+id).onchange=async e=>{try{await command('/api/settings',{[id]:e.target.checked?1:0});message('Einstellung gespeichert.')}catch(err){message('Fehler: '+err.message)}};
function staticVisible(){$('#static').hidden=$('#net [name=ipmode]').value!=='static'}$('#net [name=ipmode]').onchange=staticVisible;
function lox(){let [path,kind]=$('#loxmode').value.split('|');$('#base').value=location.origin;$('#cmd').value=path.replace('KEY',key);
 $('#cmdhint').innerHTML=kind==='analog'?'<small>Haken bei „Als Digitalausgang verwenden“ <b>entfernen</b>. Loxone setzt für &lt;v&gt; den Wert des angeschlossenen Bausteins ein.</small>':'<small>„Als Digitalausgang verwenden“ aktiv lassen, „Befehl bei AUS“ leer lassen.</small>';
 $('#statusurl').textContent=location.origin+'/api/loxone'}
$('#loxmode').onchange=lox;lox();
$('#copy').onclick=async()=>{try{await navigator.clipboard.writeText($('#cmd').value);message('Befehl kopiert.')}catch(e){$('#cmd').focus();$('#cmd').select();message('Befehl markiert – mit Strg+C kopieren.')}};
async function loadSettings(){try{settings=JSON.parse(await (await fetch('/api/settings?key='+encodeURIComponent(key))).text());
 $('#pwwarn').hidden=settings.has_password;let n=$('#net');for(let f of ['hostname','ipmode','ip','mask','gw','dns'])n[f].value=settings[f]||'';n.txlow.checked=settings.txlow;staticVisible();
 $('#resume').checked=settings.resume;$('#autoquery').checked=settings.autoquery;$('#sub').textContent='Ambientika · '+settings.hostname+' · Firmware '+settings.version;
 $('#netinfo').textContent='Einrichtungs-WLAN: '+settings.ap_name+(settings.ap_active?' (aktiv)':' (geschlossen)')+' · mDNS: http://'+settings.hostname+'.local';
 if(settings.ssid&&!$('#wifi').ssid.value)$('#wifi').ssid.value=settings.ssid}catch(e){}}
async function refresh(){try{let r=await fetch('/api/status');if(!r.ok)throw Error('HTTP '+r.status);let s=await r.json();
 $('#bus').textContent=s.enabled?'Bus ist freigegeben.':'Bus ist gesperrt.';
 let st=(names[s.mode]||s.mode)+' · '+(names[s.phase]||s.phase)+' · Stufe '+s.level;if(s.mode==='boost')st+=' · noch '+Math.ceil(s.boost_remaining_s/60)+' min';$('#state').textContent=st;
 for(let b of document.querySelectorAll('#control button,#off,#query,#filterreset'))b.disabled=!s.enabled;$('#enable').disabled=s.enabled;$('#disable').disabled=!s.enabled;
 $('#sensors').textContent=(s.device_status_received?'Letzte Geräteantwort vor '+Math.floor(s.status_age_ms/1000)+' s':'Noch keine Geräteantwort')+(s.comm_ok===0?' · ⚠ letzte Abfrage ohne Antwort (Verdrahtung/A-B prüfen)':'')+' · Filteralarm: '+flag(s.filter)+' · Feuchtealarm: '+flag(s.humidity_alarm)+' · Dämmerung: '+flag(s.dark)+' · WLAN '+s.rssi+' dBm';
 if(!initialized){let c=$('#control');c.mode.value=s.mode;c.level.value=s.level;c.humidity.value=s.humidity_level;c.night.value=Number(s.night);initialized=true;message('Steuerung erreichbar.')}
 let n=await(await fetch('/network')).json();$('#network').textContent=n.connected?'Heim-WLAN verbunden. Adresse: http://'+n.ip+' bzw. http://'+n.hostname+'.local':'Noch keine Heim-WLAN-Verbindung.'}
 catch(e){message('Verbindung unterbrochen. Nach WLAN-Einrichtung ins Heim-WLAN wechseln und die ESP-Adresse öffnen.')}}
loadSettings();refresh();setInterval(refresh,2500);
</script></html>)HTML";
