#pragma once
// Bedienseite (statisch; den API-Schlüssel holt die Seite nach der Anmeldung über /api/session).
// Gestaltung angelehnt an das Lattice-Designsystem (UXKIN): warmer Papierton, Tannengrün als einzige
// dunkle Strukturfarbe, Pastellflächen als Zustandsfarben, runde Kacheln und Pillen-Schaltflächen.
const char UI_PAGE[] = R"HTML(<!doctype html><html lang="de"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#f7f6f2"><title>Lüftung</title>
<style>
:root{--bg:#f7f6f2;--card:#fff;--ink:#001f1f;--muted:#5c7070;--soft:#889494;--line:rgba(0,31,31,.12);--meadow:#cdface;
--mint:#e4f7f9;--lav:#e1e1fa;--butter:#fff3c2;--petal:#fde5ff;--sage:#eff5ce;--off:#efeee8;--danger:#9b2c2c;--dangerbg:#fde8e6;--ok:#2a4e1c;
--shadow:0 12px 12px -6px rgba(0,31,31,.04),0 6px 6px -3px rgba(0,31,31,.04),0 1px 1px -.5px rgba(0,31,31,.08),0 0 0 1px rgba(0,31,31,.05);
font-family:ui-sans-serif,system-ui,-apple-system,"Segoe UI",Roboto,Inter,sans-serif;color-scheme:light}
@media (prefers-color-scheme:dark){:root{--bg:#0d1716;--card:#15211f;--ink:#e8f0ee;--muted:#a3b5b2;--soft:#7f918e;--line:rgba(232,240,238,.14);
--mint:#16343a;--lav:#26264a;--butter:#3b3418;--petal:#3d2440;--sage:#2b3418;--off:#1c2725;--dangerbg:#3a1d1b;--danger:#ffb4ab;--ok:#a9e0a0;
--shadow:0 0 0 1px rgba(232,240,238,.08);color-scheme:dark}}
*{box-sizing:border-box}html{-webkit-text-size-adjust:100%}
body{margin:0;background:var(--bg);color:var(--ink);font-size:16px;line-height:1.4;padding-bottom:calc(88px + env(safe-area-inset-bottom))}
main,header.top{max-width:860px;margin:0 auto;padding:0 16px}
header.top{padding-top:20px}
.eyebrow{font-size:11px;letter-spacing:.08em;text-transform:uppercase;color:var(--muted);font-weight:600}
h1{font-size:30px;line-height:1.1;letter-spacing:-.02em;margin:6px 0 2px;font-weight:600}
h2{font-size:22px;line-height:1.2;margin:0 0 6px;font-weight:600;letter-spacing:-.01em}
h3.label{font-size:11px;letter-spacing:.08em;text-transform:uppercase;color:var(--muted);margin:24px 0 10px;font-weight:600}
p{margin:6px 0}.muted{color:var(--muted)}small,.small{font-size:13px;color:var(--muted)}
a{color:inherit}code{font-family:ui-monospace,SFMono-Regular,Menlo,monospace;font-size:13px;background:var(--off);padding:2px 6px;border-radius:6px;overflow-wrap:anywhere}
.card{background:var(--card);border-radius:18px;padding:20px;box-shadow:var(--shadow);margin:12px 0}
.notice{border-radius:14px;padding:14px 16px;margin:12px 0;background:var(--butter)}
.notice.danger{background:var(--dangerbg);color:var(--danger)}
/* Navigation */
nav.tabs{position:fixed;left:0;right:0;bottom:0;z-index:5;display:flex;justify-content:center;gap:2px;background:color-mix(in srgb,var(--bg) 92%,transparent);
backdrop-filter:blur(12px);-webkit-backdrop-filter:blur(12px);border-top:1px solid var(--line);padding:6px 6px calc(6px + env(safe-area-inset-bottom))}
nav.tabs button{flex:1;max-width:120px;background:none;border:0;color:var(--muted);font:inherit;font-size:11px;padding:6px 2px;border-radius:12px;display:flex;flex-direction:column;align-items:center;gap:3px;cursor:pointer}
nav.tabs button svg{width:22px;height:22px}
nav.tabs button[aria-selected=true]{color:var(--ink);background:var(--meadow)}
@media (prefers-color-scheme:dark){nav.tabs button[aria-selected=true]{color:#001f1f}}
@media (min-width:760px){body{padding-bottom:24px}nav.tabs{position:sticky;top:0;bottom:auto;border-top:0;border-bottom:1px solid var(--line);margin-top:12px}
nav.tabs button{flex-direction:row;font-size:14px;max-width:170px;padding:10px 14px}}
.tab{display:none}.tab.active{display:block;animation:fade .2s ease}
@keyframes fade{from{opacity:0;transform:translateY(4px)}to{opacity:1;transform:none}}
/* Buttons */
button,.btn{font:inherit;font-weight:600;font-size:15px;border-radius:29px;padding:12px 20px;border:1.5px solid var(--ink);background:var(--ink);color:var(--bg);cursor:pointer;
display:inline-flex;align-items:center;justify-content:center;gap:8px;text-decoration:none;min-height:46px}
button.secondary,.btn.secondary{background:transparent;color:var(--ink)}
button.danger{background:transparent;color:var(--danger);border-color:var(--danger)}
button:disabled{opacity:.4;cursor:not-allowed}
button:focus-visible,input:focus-visible,select:focus-visible{outline:3px solid #67e2eb;outline-offset:2px}
.row{display:flex;flex-wrap:wrap;gap:10px;margin-top:12px}.row>*{flex:1 1 auto}
/* Formulare */
label{display:block;margin:12px 0 0;font-size:14px;font-weight:600}
input,select{display:block;width:100%;margin-top:6px;font:inherit;font-weight:400;padding:12px 14px;border-radius:12px;border:1px solid var(--line);background:var(--bg);color:var(--ink);min-height:46px}
label.check{display:flex;gap:12px;align-items:flex-start;font-weight:400;margin:14px 0 0}
label.check input{width:22px;height:22px;min-height:0;margin:1px 0 0;flex:none;accent-color:#001f1f}
.grid2{display:grid;grid-template-columns:1fr 1fr;gap:0 12px}@media(max-width:520px){.grid2{grid-template-columns:1fr}}
/* Hero */
.hero{display:flex;gap:18px;align-items:center;transition:background .4s}
#heroMode{font-size:clamp(19px,5.4vw,24px);hyphens:auto;-webkit-hyphens:auto;overflow-wrap:anywhere}
@media(max-width:380px){.hero{gap:12px;padding:16px}.fanwrap{width:76px;height:76px}.fan{width:50px;height:50px}}
.hero[data-phase=supply]{background:var(--mint)}.hero[data-phase=extract]{background:var(--lav)}.hero[data-phase=pause]{background:var(--butter)}
.hero[data-phase=boost]{background:var(--petal)}.hero[data-phase=off],.hero[data-phase=disabled]{background:var(--off)}.hero[data-phase=filter_reset]{background:var(--sage)}
.fanwrap{position:relative;width:96px;height:96px;flex:none;border-radius:50%;background:var(--card);display:grid;place-items:center;box-shadow:var(--shadow)}
.fan{width:64px;height:64px}.fan.spin{animation:spin 1.6s linear infinite}.fan.spin.fast{animation-duration:.7s}.fan.spin.rev{animation-direction:reverse}
@keyframes spin{to{transform:rotate(360deg)}}
@media (prefers-reduced-motion:reduce){.fan.spin{animation:none}}
.dir{position:absolute;right:-6px;bottom:-6px;width:36px;height:36px;border-radius:50%;background:var(--ink);color:var(--bg);display:grid;place-items:center}
.dir svg{width:20px;height:20px}
.pill{display:inline-flex;align-items:center;gap:6px;border-radius:22px;padding:5px 11px;font-size:13px;font-weight:600;background:var(--card);border:1px solid var(--line)}
.dot{width:8px;height:8px;border-radius:50%;background:var(--soft)}.dot.ok{background:#3c9a4a}.dot.warn{background:#d9a21b}.dot.bad{background:#c9483b}
.bar{height:6px;border-radius:6px;background:rgba(0,31,31,.08);overflow:hidden;margin-top:12px}.bar i{display:block;height:100%;background:var(--ink);width:0;transition:width 1s linear}
/* Kacheln */
.modes{display:grid;grid-template-columns:repeat(3,1fr);gap:10px}@media(max-width:420px){.modes{grid-template-columns:repeat(2,1fr)}}
.tile{background:var(--card);color:var(--ink);border:1.5px solid transparent;box-shadow:var(--shadow);border-radius:18px;padding:14px 12px;min-height:96px;
display:flex;flex-direction:column;align-items:flex-start;justify-content:space-between;text-align:left;font-weight:600;gap:8px}
.tile svg{width:26px;height:26px}.tile span{font-size:15px}.tile small{font-weight:400;font-size:12px}
.tile[aria-pressed=true]{background:var(--meadow);border-color:var(--ink);color:#001f1f}.tile[aria-pressed=true] small{color:#3d5555}
.seg{display:flex;background:var(--off);border-radius:29px;padding:4px;gap:4px}
.seg button{flex:1;background:transparent;border:0;color:var(--ink);min-height:42px;padding:8px 10px;font-weight:600}
.seg button[aria-pressed=true]{background:var(--card);box-shadow:var(--shadow)}
.boost{width:100%;margin-top:12px;background:var(--card);color:var(--ink);border-color:var(--line);justify-content:space-between;padding:14px 20px}
.boost[aria-pressed=true]{background:var(--petal);border-color:var(--ink)}
.chips{display:flex;flex-wrap:wrap;gap:8px}
.chip{display:inline-flex;align-items:center;gap:7px;border-radius:22px;padding:8px 12px;font-size:13px;background:var(--card);box-shadow:var(--shadow)}
/* Schritte */
.progress{display:flex;align-items:center;gap:12px;margin:8px 0 4px}.progress .bar{flex:1;margin:0}
.step{position:relative;padding-left:64px}
.step .num{position:absolute;left:18px;top:20px;width:32px;height:32px;border-radius:50%;display:grid;place-items:center;font-weight:700;font-size:14px;
background:var(--off);color:var(--ink)}
.step.done .num{background:var(--meadow);color:#001f1f}
.step .state{float:right;font-size:12px;font-weight:600;color:var(--muted)}
.step.done .state{color:var(--ok)}
details.step>summary{list-style:none;cursor:pointer}details.step>summary::-webkit-details-marker{display:none}
details.step>summary h2{margin:3px 0 0;font-size:19px}details.step>summary p{margin:2px 0 0}
details.step[open]>summary{margin-bottom:6px}
ol.steps{padding-left:20px;margin:8px 0}ol.steps li{margin:6px 0}
.radio-cards{display:grid;gap:10px;margin-top:10px}
.radio-cards label{margin:0;border-radius:14px;padding:14px;border:1.5px solid var(--line);display:flex;gap:12px;align-items:flex-start;font-weight:400;cursor:pointer}
.radio-cards input{width:20px;height:20px;min-height:0;margin:2px 0 0;flex:none;accent-color:#001f1f}
.radio-cards label:has(input:checked){border-color:var(--ink);background:var(--meadow);color:#001f1f}
.kv{display:grid;grid-template-columns:auto 1fr;gap:6px 16px;font-size:14px}.kv dt{color:var(--muted)}.kv dd{margin:0;overflow-wrap:anywhere}
.copyrow{display:flex;gap:8px;align-items:center;margin-top:8px}.copyrow code{flex:1;padding:10px;display:block}
.copyrow button{min-height:40px;padding:8px 14px;font-size:13px}
table{width:100%;border-collapse:collapse;font-size:14px}td,th{text-align:left;padding:8px 6px;border-bottom:1px solid var(--line);vertical-align:top}
/* Übersicht */
.fleet{display:grid;grid-template-columns:repeat(auto-fill,minmax(220px,1fr));gap:12px}
.unit{border-radius:18px;padding:16px;box-shadow:var(--shadow);background:var(--card);display:flex;flex-direction:column;gap:8px;min-height:150px}
.unit[data-phase=supply]{background:var(--mint)}.unit[data-phase=extract]{background:var(--lav)}.unit[data-phase=pause]{background:var(--butter)}.unit[data-phase=boost]{background:var(--petal)}
.unit h2{font-size:18px;margin:0}.unit .meta{font-size:12px;color:var(--muted)}.unit .big{font-size:20px;font-weight:600}
.unit a{font-size:14px;font-weight:600;margin-top:auto}
pre{background:#0f2230;color:#d8e6ee;padding:12px;border-radius:12px;max-height:420px;overflow:auto;font-size:12px;white-space:pre-wrap;overflow-wrap:anywhere}
progress{width:100%;height:10px;margin-top:10px;accent-color:#001f1f}
#toast{position:fixed;left:50%;transform:translateX(-50%) translateY(20px);bottom:calc(92px + env(safe-area-inset-bottom));z-index:9;background:var(--ink);color:var(--bg);
padding:12px 18px;border-radius:14px;max-width:min(560px,calc(100% - 32px));box-shadow:0 10px 30px rgba(0,0,0,.2);opacity:0;pointer-events:none;transition:.25s;font-size:14px}
#toast.show{opacity:1;transform:translateX(-50%)}#toast.err{background:var(--danger);color:#fff}
@media (min-width:760px){#toast{bottom:24px}}
.hidden{display:none!important}
</style></head><body>
<svg width="0" height="0" style="position:absolute" aria-hidden="true">
<symbol id="i-fan" viewBox="0 0 24 24"><path fill="currentColor" d="M12 10.6a1.4 1.4 0 1 1 0 2.8 1.4 1.4 0 0 1 0-2.8Zm.6-1.5c.3-2.6 1.2-6.1 4.3-6.1 2.4 0 3.3 2.7 1.8 4.6-1.2 1.4-3.3 1.9-6.1 1.5Zm1.8 3.9c2.4 1 5.6 2.8 4.7 5.8-.7 2.3-3.5 2.4-4.9.4-1.1-1.5-1-3.6.2-6.2ZM10.8 14c-1.9 1.7-4.8 3.8-7 1.6-1.7-1.7-.5-4.3 1.9-4.4 1.8-.1 3.6 1 5.1 2.8Z"/></symbol>
<symbol id="i-swap" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" d="M7 4v15m0 0-3-3m3 3 3-3M17 20V5m0 0-3 3m3-3 3 3"/></symbol>
<symbol id="i-auto" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" d="M12 3s6 6.4 6 10.5A6 6 0 0 1 6 13.5C6 9.4 12 3 12 3Zm-2.5 11.5h5"/></symbol>
<symbol id="i-in" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" d="M12 3v14m0 0-5-5m5 5 5-5M5 21h14"/></symbol>
<symbol id="i-out" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" d="M12 21V7m0 0-5 5m5-5 5 5M5 3h14"/></symbol>
<symbol id="i-pause" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" d="M9 6v12M15 6v12"/></symbol>
<symbol id="i-off" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" d="M12 3v8m5.7-4.7a8 8 0 1 1-11.4 0"/></symbol>
<symbol id="i-bolt" viewBox="0 0 24 24"><path fill="currentColor" d="M13 2 4 14h7l-1 8 9-12h-7l1-8Z"/></symbol>
<symbol id="i-grid" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round" d="M4 4h7v7H4zM13 4h7v7h-7zM4 13h7v7H4zM13 13h7v7h-7z"/></symbol>
<symbol id="i-check" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" d="M4 12.5 9.5 18 20 6"/></symbol>
<symbol id="i-link" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" d="M10 14a4.5 4.5 0 0 0 6.4 0l3-3a4.5 4.5 0 0 0-6.4-6.4l-1 1M14 10a4.5 4.5 0 0 0-6.4 0l-3 3a4.5 4.5 0 0 0 6.4 6.4l1-1"/></symbol>
<symbol id="i-gear" viewBox="0 0 24 24"><path fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round" d="M12 8.5a3.5 3.5 0 1 1 0 7 3.5 3.5 0 0 1 0-7Zm7.4 4.9.1-1.4-.1-1.4 2-1.6-2-3.4-2.4.9a7.6 7.6 0 0 0-2.4-1.4L14.2 2h-4.4l-.4 2.6A7.6 7.6 0 0 0 7 6L4.6 5l-2 3.4 2 1.6-.1 1.4.1 1.4-2 1.6 2 3.4 2.4-.9a7.6 7.6 0 0 0 2.4 1.4l.4 2.7h4.4l.4-2.6a7.6 7.6 0 0 0 2.4-1.4l2.4.9 2-3.4-2-1.7Z"/></symbol>
</svg>

<header class="top">
 <div class="eyebrow">Ambientika · Lüftung</div>
 <h1 id="title">Lüfter</h1>
 <p class="muted" id="subtitle">Verbindung wird aufgebaut …</p>
</header>
<nav class="tabs" role="tablist" aria-label="Bereiche">
 <button role="tab" data-tab="fan" aria-selected="true"><svg><use href="#i-fan"/></svg>Lüfter</button>
 <button role="tab" data-tab="all" aria-selected="false"><svg><use href="#i-grid"/></svg>Alle</button>
 <button role="tab" data-tab="setup" aria-selected="false"><svg><use href="#i-check"/></svg>Einrichten</button>
 <button role="tab" data-tab="loxone" aria-selected="false"><svg><use href="#i-link"/></svg>Loxone</button>
 <button role="tab" data-tab="system" aria-selected="false"><svg><use href="#i-gear"/></svg>System</button>
</nav>

<main>
<!-- ======================= Lüfter ======================= -->
<section class="tab active" id="tab-fan" role="tabpanel">
 <div class="notice hidden" id="enableHint"><b>Lüfter noch nicht freigegeben.</b> Erst die Einrichtung abschließen (Schritt 5), dann kann hier bedient werden. <a href="#setup">Zur Einrichtung</a></div>
 <article class="card hero" id="hero" data-phase="off">
  <div class="fanwrap"><svg class="fan" id="fanIcon"><use href="#i-fan"/></svg><div class="dir" id="dirIcon"><svg><use href="#i-pause"/></svg></div></div>
  <div style="flex:1;min-width:0">
   <span class="pill" id="phasePill"><i class="dot"></i>—</span>
   <h2 id="heroMode" style="margin-top:8px">—</h2>
   <p class="muted" id="heroDetail">—</p>
   <div class="bar"><i id="phaseBar"></i></div>
  </div>
 </article>
 <h3 class="label">Betriebsart</h3>
 <div class="modes" id="modes">
  <button class="tile" data-mode="wrg"><svg><use href="#i-swap"/></svg><span>Wärme&shy;rückgewinnung<br><small>60 s rein, 60 s raus</small></span></button>
  <button class="tile" data-mode="auto"><svg><use href="#i-auto"/></svg><span>Automatik<br><small>nach Feuchte</small></span></button>
  <button class="tile" data-mode="supply"><svg><use href="#i-in"/></svg><span>Zuluft<br><small>dauerhaft rein</small></span></button>
  <button class="tile" data-mode="extract"><svg><use href="#i-out"/></svg><span>Abluft<br><small>dauerhaft raus</small></span></button>
  <button class="tile" data-mode="pause"><svg><use href="#i-pause"/></svg><span>Pause<br><small>Klappe offen</small></span></button>
  <button class="tile" data-mode="off"><svg><use href="#i-off"/></svg><span>Aus<br><small>Klappe zu</small></span></button>
 </div>
 <button class="boost" id="boostBtn" aria-pressed="false"><span style="display:flex;gap:10px;align-items:center"><svg width="22" height="22"><use href="#i-bolt"/></svg>Boost · 20 Minuten Abluft</span><span id="boostRest" class="small"></span></button>
 <div id="levelBox"><h3 class="label">Stufe</h3>
  <div class="seg" id="levels"><button data-level="1">1 · leise</button><button data-level="2">2 · mittel</button><button data-level="3">3 · stark</button></div>
  <p class="small">Eine Stufenänderung wird ohne Motorpause übernommen. <a href="#" id="level0">Stufe 0</a> ist eine gerätespezifische Minimalstufe.</p></div>
 <div id="autoBox" class="hidden"><h3 class="label">Automatik</h3>
  <div class="seg" id="humSeg"><button data-hum="2">Feuchtestufe 2</button><button data-hum="3">Feuchtestufe 3</button></div>
  <div class="seg" id="nightSeg" style="margin-top:8px"><button data-night="0">Tag</button><button data-night="1">Nacht</button></div></div>
 <h3 class="label">Zustand</h3>
 <div class="chips" id="chips"></div>
 <div class="notice hidden" id="filterCard"><b>Filterwechsel fällig.</b> Filter reinigen bzw. tauschen und danach den Alarm zurücksetzen.
  <div class="row"><button class="secondary" id="filterReset">Filteralarm zurücksetzen</button></div></div>
 <div class="row"><button class="secondary" id="queryBtn">Sensorstatus jetzt abfragen</button></div>
</section>

<!-- ======================= Alle Lüfter ======================= -->
<section class="tab" id="tab-all" role="tabpanel">
 <h3 class="label">Alle Lüfter im Netzwerk</h3>
 <p class="muted">Jeder Lüfter mit dieser Firmware meldet sich automatisch im Heimnetz. Tippe auf einen Lüfter, um seine Seite zu öffnen.</p>
 <div class="row"><button class="secondary" id="scanBtn">Neu suchen</button></div>
 <div class="card"><b>An alle Lüfter senden</b><p class="small">Funktioniert für Lüfter mit dem gleichen API-Schlüssel (Einrichtung, Schritt 7).</p>
  <div class="row"><button class="secondary" data-all="wrg">WRG</button><button class="secondary" data-all="auto">Automatik</button><button class="secondary" data-all="boost">Boost</button><button class="danger" data-all="off">Alle aus</button></div></div>
 <div class="fleet" id="fleet"></div>
 <p class="small" id="fleetInfo"></p>
</section>

<!-- ======================= Einrichtung ======================= -->
<section class="tab" id="tab-setup" role="tabpanel">
 <h3 class="label">Einrichtung dieses Lüfters</h3>
 <div class="progress"><div class="bar"><i id="setupBar"></i></div><b id="setupCount">0/6</b></div>
 <p class="small">Schritte von oben nach unten erledigen. Erledigte Schritte sind grün markiert und können jederzeit wieder geöffnet werden.</p>

 <details class="card step" id="st-pw"><summary><span class="num">1</span><span class="state"></span><p class="eyebrow">Sicherheit</p><h2>Gerätepasswort festlegen</h2></summary>
  <p>Schützt diese Seite (Anmeldung mit Benutzer <b>admin</b>) und Updates. Sobald das Heim-WLAN eingerichtet ist, gilt es auch für das Einrichtungs-WLAN – bis dahin bleibt dort <b>Lueftung-Start</b>. Tipp: für alle Lüfter dasselbe Passwort verwenden.</p>
  <form id="pwForm"><div class="grid2"><label>Neues Passwort<input name="password" type="password" minlength="8" maxlength="63" autocomplete="new-password" required></label>
  <label>Wiederholen<input name="repeat" type="password" maxlength="63" autocomplete="new-password" required></label></div>
  <p class="small">8–63 Zeichen, ohne Leerzeichen und Umlaute.</p><div class="row"><button>Passwort speichern</button></div></form></details>

 <details class="card step" id="st-name"><summary><span class="num">2</span><span class="state"></span><p class="eyebrow">Name</p><h2>Raum und Gerätenamen vergeben</h2></summary>
  <p>Der <b>Anzeigename</b> erscheint hier, in der Übersicht und in Loxone. Der <b>Gerätename</b> erscheint im Router und ergibt die Adresse <code>http://gerätename.local</code>.</p>
  <form id="nameForm"><label>Anzeigename (z. B. „Schlafzimmer“)<input name="dname" maxlength="40" required></label>
  <label>Gerätename im Netzwerk<input name="hostname" maxlength="32" pattern="[a-z0-9]([a-z0-9\-]*[a-z0-9])?" required></label>
  <p class="small">Nur a–z, 0–9 und Bindestrich, z. B. <code>lueftung-schlafen</code>. Ist der ESP schon mit dem Heim-WLAN verbunden, startet er nach einer Änderung neu.</p>
  <div class="row"><button>Namen speichern</button></div></form></details>

 <details class="card step" id="st-wifi"><summary><span class="num">3</span><span class="state"></span><p class="eyebrow">Netzwerk</p><h2>Mit dem Heim-WLAN verbinden</h2></summary>
  <p>Nur <b>2,4-GHz</b>-WLAN. Nach dem Speichern bleibt diese Seite noch etwa eine Minute im Einrichtungs-WLAN erreichbar und zeigt die neue Adresse an.</p>
  <form id="wifiForm"><label>WLAN-Name (SSID)<input name="ssid" maxlength="32" required autocomplete="off"></label>
  <label>WLAN-Passwort<input name="password" type="password" maxlength="63" autocomplete="new-password"></label>
  <div class="row"><button>WLAN verbinden</button></div></form>
  <div class="notice hidden" id="wifiResult"></div></details>

 <details class="card step" id="st-ip"><summary><span class="num">4</span><span class="state"></span><p class="eyebrow">Netzwerk</p><h2>Feste IP-Adresse vergeben</h2></summary>
  <p>Loxone erreicht den Lüfter über seine IP-Adresse – sie darf sich nicht ändern. <b>Empfohlen:</b> im Router „immer die gleiche IP-Adresse zuweisen“.</p>
  <ol class="steps"><li>Router öffnen (FRITZ!Box: <code>http://fritz.box</code>).</li>
  <li><b>Heimnetz → Netzwerk → Netzwerkverbindungen</b>, Gerät <b id="ipHost">…</b> suchen, auf den Stift klicken.</li>
  <li>Haken bei <b>„Diesem Netzwerkgerät immer die gleiche IPv4-Adresse zuweisen“</b> setzen, speichern.</li>
  <li>Unten „Erledigt“ anhaken. Aktuelle Adresse: <b id="ipNow">…</b></li></ol>
  <label class="check"><input type="checkbox" id="ipDone"><span>Feste Zuordnung im Router ist eingerichtet</span></label>
  <details style="margin-top:14px"><summary class="small">Alternative: statische IP direkt im ESP eintragen</summary>
  <form id="netForm"><label>IP-Adresse beziehen<select name="ipmode"><option value="dhcp">Automatisch vom Router (DHCP) – empfohlen</option><option value="static">Statisch im ESP</option></select></label>
  <div class="grid2" id="staticFields"><label>IP-Adresse<input name="ip" inputmode="decimal" placeholder="192.168.178.210"></label><label>Subnetzmaske<input name="mask" inputmode="decimal" placeholder="255.255.255.0"></label>
  <label>Gateway (Router)<input name="gw" inputmode="decimal" placeholder="192.168.178.1"></label><label>DNS (leer = Gateway)<input name="dns" inputmode="decimal"></label></div>
  <p class="small">Adresse außerhalb des DHCP-Bereichs wählen (FRITZ!Box ab Werk: .20–.200). Falsch eingetragen? BOOT-Taster am ESP 10 s halten.</p>
  <label class="check"><input type="checkbox" name="txlow" value="1"><span>WLAN-Sendeleistung reduzieren (hilft beim ESP32-C3 Super Mini oft bei Verbindungsabbrüchen)</span></label>
  <div class="row"><button>Speichern und neu starten</button></div></form></details></details>

 <details class="card step" id="st-enable"><summary><span class="num">5</span><span class="state"></span><p class="eyebrow">Lüfter</p><h2>Lüfter freigeben</h2></summary>
  <p>Erst danach sendet der ESP Befehle an den Lüfter. Bitte bestätigen:</p>
  <label class="check"><input type="checkbox" class="pre"><span>Verdrahtung spannungsfrei geprüft (A → Klemme 4, B → Klemme 3, GND → Klemme 5)</span></label>
  <label class="check"><input type="checkbox" class="pre"><span>DIP-Schalter des Lüfters auf <b>„Master mit Wandpanel“</b></span></label>
  <label class="check"><input type="checkbox" class="pre"><span>Kein Wandpanel und kein weiterer Lüfter an derselben RS485-Leitung</span></label>
  <div class="row"><button id="enableBtn" disabled>Lüfter freigeben</button><button class="secondary" id="disableBtn">Busausgabe sperren</button></div>
  <p class="small">Sperren geht nur, wenn vorher „Aus“ gewählt wurde.</p></details>

 <details class="card step" id="st-group"><summary><span class="num">6</span><span class="state"></span><p class="eyebrow">Mehrere Lüfter</p><h2>Gegenläufigen Betrieb einstellen</h2></summary>
  <p>Lüfter arbeiten am besten paarweise: Während einer Zuluft macht, macht der andere Abluft. Lüfter derselben Gruppe laufen im Gleichtakt, Gruppe A und B immer gegenläufig – auch über 10 Lüfter hinweg. Der Takt kommt aus der Uhrzeit (Internet bzw. Router), Kabel zwischen den ESPs sind nicht nötig.</p>
  <form id="groupForm" class="radio-cards">
   <label><input type="radio" name="group" value="1"><span><b>Gruppe A</b><br><small>z. B. Wohnzimmer, Schlafzimmer</small></span></label>
   <label><input type="radio" name="group" value="2"><span><b>Gruppe B</b><br><small>der jeweilige Partnerraum, z. B. Flur, Kinderzimmer</small></span></label>
   <label><input type="radio" name="group" value="0"><span><b>Eigener Takt</b><br><small>einzelner Lüfter ohne Partner</small></span></label>
  </form><p class="small" id="groupInfo"></p></details>

 <details class="card step" id="st-copy"><summary><span class="num">7</span><span class="state">optional</span><p class="eyebrow">Mehrere Lüfter</p><h2>Weitere Lüfter schneller einrichten</h2></summary>
  <p><b>Am ersten Lüfter:</b> Vorlage herunterladen. Sie enthält WLAN, Gerätepasswort und Optionen – <b>wie ein Passwort aufbewahren</b>.</p>
  <label class="check"><input type="checkbox" id="withKey" checked><span>Gleichen API-Schlüssel für alle Lüfter verwenden (empfohlen: eine Loxone-Vorlage für alle, „Alle Lüfter“-Steuerung möglich)</span></label>
  <div class="row"><button class="secondary" id="exportBtn">Vorlage herunterladen</button></div>
  <p style="margin-top:18px"><b>An jedem weiteren Lüfter:</b> mit dessen Einrichtungs-WLAN verbinden, diese Seite öffnen und die Vorlage laden. Danach nur noch Name (Schritt 2), feste IP (4), Freigabe (5) und Gruppe (6).</p>
  <label>Vorlage laden<input type="file" id="importFile" accept=".json,application/json"></label></details>
</section>

<!-- ======================= Loxone ======================= -->
<section class="tab" id="tab-loxone" role="tabpanel">
 <h3 class="label">Loxone verbinden</h3>
 <div class="card step"><span class="num">1</span><h2>Vorlagen herunterladen</h2>
  <p>Zwei fertige Dateien für diesen Lüfter – mit Adresse und Schlüssel bereits ausgefüllt.</p>
  <div class="row"><button id="voBtn">Ausgang (Steuerung)</button><button class="secondary" id="viBtn">Eingang (Rückmeldung)</button></div></div>
 <div class="card step"><span class="num">2</span><h2>In Loxone Config importieren</h2>
  <ol class="steps"><li>Peripheriebaum: <b>Virtuelle Ausgänge</b> markieren → Menüleiste <b>Vordefinierte Geräte → Vorlage importieren …</b> → Datei <code>VO_…xml</code>.</li>
  <li><b>Virtuelle Eingänge</b> markieren → ebenso <code>VIH_…xml</code> importieren.</li>
  <li>Für jeden weiteren Lüfter auf dessen Seite wiederholen.</li></ol>
  <p class="small">Menünamen können je nach Config-Version leicht abweichen. Ohne Import-Menü: Befehle unten manuell anlegen.</p></div>
 <div class="card step"><span class="num">3</span><h2>Bausteine verbinden</h2>
  <ol class="steps"><li>Baustein <b>Radiotasten</b> anlegen, „Alle aus erlaubt“ aktivieren, Einträge: 1 WRG, 2 Automatik, 3 Zuluft, 4 Abluft, 5 Pause. Ausgang <b>AQ</b> → Befehl <b>Betriebsart</b>.</li>
  <li>Zweite <b>Radiotasten</b> (Stufe 1–3) → Befehl <b>Stufe</b>. Taster → <b>Boost</b>.</li>
  <li>Eingang <b>Filteralarm</b> an eine Benachrichtigung, <b>Betriebsart Ist</b> an einen Statusbaustein.</li>
  <li>Raum und Kategorie setzen, in den Miniserver speichern – fertig für die App.</li></ol></div>
 <div class="card"><b>Manuell einrichten</b>
  <p class="small">Adresse des Virtuellen Ausgangs:</p><div class="copyrow"><code id="baseUrl"></code><button class="secondary" data-copy="baseUrl">Kopieren</button></div>
  <table id="cmdTable"><thead><tr><th>Befehl</th><th>Befehl bei EIN</th></tr></thead><tbody></tbody></table>
  <p class="small">Analogbefehle (<code>&lt;v&gt;</code>): Haken „Als Digitalausgang verwenden“ entfernen. Rückmeldung: Virtueller HTTP Eingang <code id="statusUrl"></code>, Befehlserkennung z. B. <code>filter=\v</code>.</p>
  <table><thead><tr><th>Wert</th><th>Betriebsart</th></tr></thead><tbody><tr><td>0</td><td>Aus</td></tr><tr><td>1</td><td>Wärmerückgewinnung</td></tr><tr><td>2</td><td>Automatik</td></tr><tr><td>3</td><td>Zuluft</td></tr><tr><td>4</td><td>Abluft</td></tr><tr><td>5</td><td>Pause</td></tr><tr><td>6</td><td>Boost</td></tr></tbody></table></div>
</section>

<!-- ======================= System ======================= -->
<section class="tab" id="tab-system" role="tabpanel">
 <h3 class="label">Gerät</h3>
 <div class="card"><dl class="kv" id="info"></dl></div>
 <h3 class="label">Optionen</h3>
 <div class="card">
  <label class="check"><input type="checkbox" id="optResume"><span><b>Nach Stromausfall weiterlaufen</b><br><small>Letzte Betriebsart wiederherstellen (Boost ausgenommen)</small></span></label>
  <label class="check"><input type="checkbox" id="optQuery"><span><b>Sensorstatus automatisch abfragen</b><br><small>alle 5 Minuten Filter, Feuchte und Dämmerung lesen</small></span></label></div>
 <h3 class="label">Firmware-Update</h3>
 <div class="card"><p>Datei <code>ambientika-c3-v2-ota.bin</code> wählen. Vorher „Aus“ wählen. Einstellungen bleiben erhalten.</p>
  <form id="updForm"><input type="file" name="firmware" accept=".bin" required><progress id="updProg" max="100" value="0" class="hidden"></progress><div class="row"><button>Firmware hochladen</button></div></form></div>
 <h3 class="label">Protokoll</h3>
 <div class="card"><p>Starts, WLAN, Befehle und Lüfterantworten. Bei Problemen herunterladen und weitergeben – ohne Passwörter und Schlüssel.</p>
  <div class="row"><button class="secondary" id="logShow">Anzeigen</button><button class="secondary" id="logDl">Herunterladen</button><button class="secondary" id="logCopy">Kopieren</button><button class="danger" id="logClear">Löschen</button></div>
  <pre id="log" class="hidden"></pre></div>
 <h3 class="label">Sicherheit</h3>
 <div class="card"><p>API-Schlüssel (für Loxone): <code id="keyView">••••••••</code> <a href="#" id="keyToggle">anzeigen</a></p>
  <form id="keyForm"><label>Eigenen oder alten Schlüssel setzen<input name="newkey" minlength="16" maxlength="64" pattern="[A-Za-z0-9_\-]{16,64}" required autocomplete="off"></label>
  <div class="row"><button class="secondary">Schlüssel speichern</button></div></form>
  <div class="row"><button class="secondary" id="restartBtn">ESP neu starten</button></div></div>
</section>
</main>
<div id="toast" role="status" aria-live="polite"></div>
<script>
'use strict';
const $=s=>document.querySelector(s),$$=s=>[...document.querySelectorAll(s)];
let KEY='',ST=null,SET=null,busy=false;
const NAMES={wrg:'Wärmerückgewinnung',auto:'Automatik',supply:'Zuluft',extract:'Abluft',pause:'Pause',off:'Aus',boost:'Boost'};
const PHASE={supply:'Zuluft',extract:'Abluft',pause:'Motorpause',off:'Aus',disabled:'Bus gesperrt',filter_reset:'Filter-Reset'};
function toast(m,err){const t=$('#toast');t.textContent=m;t.className='show'+(err?' err':'');clearTimeout(t._h);t._h=setTimeout(()=>t.className='',err?6000:3200)}
async function api(path,params={},method='POST'){const r=await fetch(path+'?'+new URLSearchParams({...params,key:KEY}),{method});const t=await r.text();if(!r.ok)throw Error(t||('HTTP '+r.status));return t}
async function json(path){const r=await fetch(path);if(!r.ok)throw Error('HTTP '+r.status);return r.json()}
function act(fn){return async e=>{if(e)e.preventDefault();if(busy)return;busy=true;try{await fn(e)}catch(err){toast(err.message,1)}finally{busy=false}}}
function save(name,blob){const a=document.createElement('a');a.href=URL.createObjectURL(blob);a.download=name;document.body.appendChild(a);a.click();a.remove()}
function slug(s){return s.toLowerCase().replace(/ä/g,'ae').replace(/ö/g,'oe').replace(/ü/g,'ue').replace(/ß/g,'ss').replace(/[^a-z0-9]+/g,'-').replace(/^-+|-+$/g,'')}
function fmt(s){s=Math.max(0,s|0);return s>=60?Math.floor(s/60)+' min '+(s%60?s%60+' s':''):s+' s'}
const ls={get(k){try{return localStorage.getItem(k)}catch(e){return null}},set(k,v){try{localStorage.setItem(k,v)}catch(e){}}};

/* ---------- Tabs ---------- */
function show(tab){if(!$('#tab-'+tab))tab='fan';$$('.tab').forEach(s=>s.classList.toggle('active',s.id==='tab-'+tab));
 $$('nav.tabs button').forEach(b=>b.setAttribute('aria-selected',b.dataset.tab===tab));if(location.hash!=='#'+tab)history.replaceState(null,'','#'+tab);
 if(tab==='all')loadFleet(false);if(tab==='loxone')renderLoxone();window.scrollTo(0,0)}
$$('nav.tabs button').forEach(b=>b.onclick=()=>show(b.dataset.tab));
window.addEventListener('hashchange',()=>show(location.hash.slice(1)));

/* ---------- Lüfter bedienen ---------- */
function render(s){ST=s;const run=s.phase==='supply'||s.phase==='extract';const phase=s.mode==='boost'&&run?'boost':s.phase;
 $('#hero').dataset.phase=phase;$('#title').textContent=s.name||'Lüfter';
 const fan=$('#fanIcon');fan.classList.toggle('spin',run);fan.classList.toggle('rev',s.phase==='extract');fan.classList.toggle('fast',s.mode==='boost'||s.level===3);
 $('#dirIcon use').setAttribute('href',s.phase==='supply'?'#i-in':s.phase==='extract'?'#i-out':s.phase==='off'||s.phase==='disabled'?'#i-off':'#i-pause');
 const dot=s.phase==='disabled'?'bad':run?'ok':'warn';$('#phasePill').innerHTML='<i class="dot '+dot+'"></i>'+(PHASE[s.phase]||s.phase);
 $('#heroMode').textContent=s.enabled?NAMES[s.mode]:'Noch nicht freigegeben';
 let d=[];if(!s.enabled)d.push('Einrichtung abschließen, dann Lüfter freigeben');
 else if(s.phase==='off')d.push('Motor aus, Klappe geschlossen');else if(s.mode==='pause')d.push('Motor steht, Klappe offen');
 else{if(s.phase==='pause')d.push('Motorpause vor dem '+(ST.mode==='boost'?'Boost':'Start bzw. Richtungswechsel'));else d.push(PHASE[s.phase]);
  if(s.mode==='auto')d.push('Feuchte '+s.humidity_level+(s.night?' · Nacht':' · Tag'));else if(s.mode!=='boost'&&run)d.push('Stufe '+s.level);
  if(s.mode==='boost'&&run)d.push('noch '+fmt(s.boost_remaining_s));else if(s.phase_remaining_s&&(s.mode==='wrg'||s.mode==='auto'||s.phase==='pause'))d.push((s.phase==='pause'?'weiter in ':'Wechsel in ')+fmt(s.phase_remaining_s));
  if(s.group_clock)d.push('Gegenlauf '+(s.sync_group===1?'A':'B'))}
 $('#heroDetail').textContent=d.join(' · ');
 const total=s.phase==='pause'?10:s.mode==='boost'?1200:(s.mode==='wrg'||s.mode==='auto')?60:0;
 $('#phaseBar').style.width=total&&s.phase_remaining_s?Math.min(100,100-100*s.phase_remaining_s/total)+'%':(run?'100%':'0');
 $$('#modes .tile').forEach(b=>{b.setAttribute('aria-pressed',b.dataset.mode===s.mode);b.disabled=!s.enabled});
 const bb=$('#boostBtn');bb.setAttribute('aria-pressed',s.mode==='boost');bb.disabled=!s.enabled;$('#boostRest').textContent=s.mode==='boost'?'noch '+fmt(s.boost_remaining_s):'';
 $$('#levels button').forEach(b=>{b.setAttribute('aria-pressed',+b.dataset.level===s.level);b.disabled=!s.enabled});
 $('#levelBox').classList.toggle('hidden',s.mode==='auto'||s.mode==='off'||s.mode==='pause'||s.mode==='boost');
 $('#autoBox').classList.toggle('hidden',s.mode!=='auto');
 $$('#humSeg button').forEach(b=>b.setAttribute('aria-pressed',+b.dataset.hum===s.humidity_level));
 $$('#nightSeg button').forEach(b=>b.setAttribute('aria-pressed',+b.dataset.night===(s.night?1:0)));
 $('#enableHint').classList.toggle('hidden',s.enabled);
 const f=(v,ok,bad,unk)=>v<0?['',unk]:v?['bad',bad]:['ok',ok];
 const chips=[f(s.filter,'Filter in Ordnung','Filterwechsel fällig','Filter: noch unbekannt'),f(s.humidity_alarm,'Feuchte normal','Feuchtealarm','Feuchte: unbekannt'),
  s.dark<0?['','Dämmerung: unbekannt']:['',s.dark?'Dunkel':'Hell'],s.comm_ok<0?['','Lüfterantwort: ausstehend']:s.comm_ok?['ok','Lüfter antwortet']:['bad','Lüfter antwortet nicht'],
  [s.rssi>-67?'ok':s.rssi>-80?'warn':'bad','WLAN '+s.rssi+' dBm']];
 if(s.status_raw&&!s.status_decoded)chips.push(['','Rohdaten '+s.status_raw+' (Sensoren noch nicht ausgewertet)']);
 if(s.sync_group)chips.push([s.group_clock?'ok':'warn','Gegenlauf '+(s.sync_group===1?'A':'B')+(s.group_clock?'':' (wartet auf Uhrzeit)')]);
 $('#chips').innerHTML=chips.map(c=>'<span class="chip"><i class="dot '+c[0]+'"></i>'+c[1]+'</span>').join('');
 $('#filterCard').classList.toggle('hidden',s.filter!==1);$('#queryBtn').disabled=!s.enabled;
 $('#subtitle').textContent=(SET?SET.hostname+' · ':'')+(s.enabled?'Lüfter freigegeben':'Bus gesperrt')+' · Firmware '+s.version;
 renderSetup()}
async function refresh(){try{render(await json('/api/status'))}catch(e){$('#subtitle').textContent='Keine Verbindung zum ESP – Seite neu laden oder Adresse prüfen'}}
async function control(p,msg){render(JSON.parse(await api('/api/control',p)));if(msg)toast(msg)}
$$('#modes .tile').forEach(b=>b.onclick=act(()=>control({mode:b.dataset.mode},b.dataset.mode==='off'?'Aus – Klappe schließt':b.dataset.mode==='pause'?'Pause':'Startet nach 10 s Motorpause')));
$('#boostBtn').onclick=act(()=>control({mode:'boost'},'Boost läuft 20 Minuten, danach zurück zur vorherigen Betriebsart'));
$$('#levels button').forEach(b=>b.onclick=act(()=>control({level:b.dataset.level})));
$('#level0').onclick=act(()=>control({level:0},'Stufe 0 gewählt'));
$$('#humSeg button').forEach(b=>b.onclick=act(()=>control({humidity:b.dataset.hum})));
$$('#nightSeg button').forEach(b=>b.onclick=act(()=>control({night:b.dataset.night})));
$('#filterReset').onclick=act(async()=>{if(!confirm('Filteralarm zurücksetzen? Nur nach Reinigung oder Tausch des Filters. Der Motor stoppt dafür kurz.'))return;await api('/api/filterreset');toast('Filter-Reset gesendet')});
$('#queryBtn').onclick=act(async()=>{await api('/api/query');toast('Abfrage läuft – Ergebnis in wenigen Sekunden')});

/* ---------- Einrichtung ---------- */
function setStep(id,done,openFirst){const el=$(id);el.classList.toggle('done',!!done);el.querySelector('.state').textContent=done?'✓ erledigt':(id==='#st-copy'?'optional':'offen');return done}
function renderSetup(){if(!SET||!ST)return;const defName=SET.name.startsWith('Lüfter ')&&SET.hostname.startsWith('ambientika-');
 const steps=[setStep('#st-pw',SET.has_password),setStep('#st-name',!defName),setStep('#st-wifi',SET.connected),
  setStep('#st-ip',SET.ipmode==='static'||ls.get('ipdone')==='1'),setStep('#st-enable',ST.enabled),setStep('#st-group',ls.get('groupdone')==='1'||SET.sync_group>0)];
 setStep('#st-copy',false);const n=steps.filter(Boolean).length;$('#setupCount').textContent=n+'/6';$('#setupBar').style.width=(n*100/6)+'%';
 if(!renderSetup.opened){renderSetup.opened=1;const first=['#st-pw','#st-name','#st-wifi','#st-ip','#st-enable','#st-group'][steps.indexOf(false)];if(first)$(first).open=true}}
async function loadSettings(){try{SET=JSON.parse(await api('/api/settings',{},'GET'));
 const nf=$('#nameForm');if(document.activeElement.form!==nf){nf.dname.value=SET.name;nf.hostname.value=SET.hostname}
 const wf=$('#wifiForm');if(!wf.ssid.value)wf.ssid.value=SET.ssid;
 const n=$('#netForm');if(document.activeElement.form!==n){n.ipmode.value=SET.ipmode;n.ip.value=SET.static_ip;n.mask.value=SET.mask;n.gw.value=SET.gw;n.dns.value=SET.dns;n.txlow.checked=SET.txlow;staticFields()}
 $('#ipHost').textContent=SET.hostname;$('#ipNow').textContent=SET.connected?SET.ip:'noch nicht verbunden';$('#ipDone').checked=ls.get('ipdone')==='1';
 $$('#groupForm input').forEach(i=>i.checked=+i.value===SET.sync_group);
 $('#groupInfo').textContent=SET.time?'Uhrzeit: '+SET.time+' – Takt verfügbar.':'Uhrzeit noch nicht verfügbar (Internet/Router-Zeitserver nötig). Bis dahin eigener Takt.';
 $('#optResume').checked=SET.resume;$('#optQuery').checked=SET.autoquery;
 if(SET.connected&&SET.ap_active){const r=$('#wifiResult');r.classList.remove('hidden');r.innerHTML='<b>Verbunden!</b> Neue Adresse: <b>http://'+SET.ip+'</b>. Jetzt ins Heim-WLAN wechseln und diese Adresse öffnen.'}
 $('#info').innerHTML=[['Anzeigename',SET.name],['Gerätename',SET.hostname+'.local'],['IP-Adresse',SET.connected?SET.ip:'nicht verbunden'],['WLAN',SET.ssid||'–'],
  ['Einrichtungs-WLAN',SET.ap_name+(SET.ap_active?' (offen)':' (geschlossen)')],['Uhrzeit',SET.time||'nicht synchronisiert'],['Laufzeit',fmt(SET.uptime_s)],
  ['Start',('#'+SET.boot+' · '+SET.reset_reason)],['Firmware',SET.version]].map(r=>'<dt>'+r[0]+'</dt><dd>'+String(r[1]).replace(/</g,'&lt;')+'</dd>').join('');
 renderSetup()}catch(e){}}
function staticFields(){$('#staticFields').classList.toggle('hidden',$('#netForm').ipmode.value!=='static')}
$('#netForm').ipmode.onchange=staticFields;
$('#pwForm').onsubmit=act(async e=>{const f=e.target;if(f.password.value!==f.repeat.value)throw Error('Die Passwörter stimmen nicht überein');
 await api('/api/password',{password:f.password.value});f.reset();toast('Gespeichert. Beim nächsten Öffnen der Seite: Benutzer „admin“ + dieses Passwort');loadSettings()});
$('#nameForm').dname.oninput=e=>{const h=$('#nameForm').hostname;if(!h.dataset.touched)h.value=('lueftung-'+slug(e.target.value)).slice(0,32).replace(/-+$/,'')};
$('#nameForm').hostname.oninput=e=>e.target.dataset.touched=1;
$('#nameForm').onsubmit=act(async e=>{const f=e.target;await api('/api/settings',{name:f.dname.value.trim()});
 if(f.hostname.value!==SET.hostname){
  const t=await api('/api/network',{hostname:f.hostname.value,ipmode:SET.ipmode,ip:SET.static_ip,mask:SET.mask,gw:SET.gw,dns:SET.dns,txlow:SET.txlow?1:0});
  if(/Neustart/.test(t)){toast('Gespeichert – der ESP startet neu, Seite lädt in 20 s neu');setTimeout(()=>location.reload(),20000)}else toast('Name gespeichert')}else toast('Name gespeichert');loadSettings();refresh()});
$('#wifiForm').onsubmit=act(async e=>{const f=e.target;if(f.password.value&&f.password.value.length<8)throw Error('WLAN-Passwort: mindestens 8 Zeichen');
 await api('/wifi',{ssid:f.ssid.value,password:f.password.value});const r=$('#wifiResult');r.classList.remove('hidden');r.textContent='Verbindung wird aufgebaut … das kann bis zu 30 Sekunden dauern.';
 let n=0;const t=setInterval(async()=>{n++;await loadSettings();if(SET.connected||n>15){clearInterval(t);if(!SET.connected)r.textContent='Noch keine Verbindung. WLAN-Name und Passwort prüfen (2,4 GHz). Details im Protokoll (System).'}},3000)});
$('#ipDone').onchange=e=>{ls.set('ipdone',e.target.checked?'1':'0');renderSetup()};
$('#netForm').onsubmit=act(async e=>{const f=e.target;await api('/api/network',{hostname:SET.hostname,ipmode:f.ipmode.value,ip:f.ip.value,mask:f.mask.value,gw:f.gw.value,dns:f.dns.value,txlow:f.txlow.checked?1:0});
 toast('Gespeichert – der ESP startet neu und ist danach unter '+(f.ipmode.value==='static'?f.ip.value:'der Router-Adresse')+' erreichbar',0)});
$$('.pre').forEach(c=>c.onchange=()=>$('#enableBtn').disabled=!$$('.pre').every(x=>x.checked));
$('#enableBtn').onclick=act(async()=>{await api('/enable',{enabled:1});toast('Lüfter freigegeben – jetzt im Tab „Lüfter“ eine Betriebsart wählen');refresh()});
$('#disableBtn').onclick=act(async()=>{await api('/enable',{enabled:0});toast('Busausgabe gesperrt');refresh()});
$$('#groupForm input').forEach(i=>i.onchange=act(async()=>{await api('/api/settings',{group:i.value});ls.set('groupdone','1');toast(i.value==='0'?'Eigener Takt':'Gruppe '+(i.value==='1'?'A':'B')+' – gilt ab dem nächsten Richtungswechsel');loadSettings()}));
$('#exportBtn').onclick=act(async()=>{const t=await api('/api/config/export',{withkey:$('#withKey').checked?1:0},'GET');save('ambientika-vorlage.json',new Blob([t],{type:'application/json'}));toast('Vorlage gespeichert – sicher aufbewahren')});
$('#importFile').onchange=act(async e=>{const file=e.target.files[0];if(!file)return;let j;try{j=JSON.parse(await file.text())}catch(x){throw Error('Datei ist keine gültige Vorlage')}
 if(j.type!=='ambientika-vorlage')throw Error('Datei ist keine Ambientika-Vorlage');
 if(!confirm('Vorlage übernehmen? WLAN „'+j.ssid+'“, Gerätepasswort'+(j.key?' und gemeinsamer API-Schlüssel':'')+' werden gesetzt.'))return;
 await api('/api/config/import',{ssid:j.ssid||'',pass:j.pass||'',devpw:j.devpw||'',newkey:j.key||'',resume:j.resume,autoquery:j.autoquery,txlow:j.txlow});
 if(j.key)KEY=j.key;e.target.value='';toast('Vorlage übernommen – der ESP verbindet sich mit „'+j.ssid+'“ …');
 let n=0;const t=setInterval(async()=>{n++;await loadSettings();if(SET.connected||n>15){clearInterval(t);
  toast(SET.connected?'Verbunden! Neue Adresse: http://'+SET.ip+' – ins Heim-WLAN wechseln und öffnen':'Noch keine WLAN-Verbindung – Protokoll unter System prüfen',!SET.connected)}},3000)});

/* ---------- Alle Lüfter ---------- */
async function loadFleet(scan){try{const p=await json('/api/peers'+(scan?'?scan=1':''));const fleet=$('#fleet');
 fleet.innerHTML=p.devices.map((d,i)=>'<article class="unit" id="u'+i+'" data-phase="off"><div class="meta">'+(d.self?'Dieser Lüfter · ':'')+d.ip+'</div><h2>'+(d.name||d.host).replace(/</g,'&lt;')+'</h2><div class="big">…</div><div class="meta"></div>'+
  (d.self?'':'<a href="http://'+d.ip+'/" target="_blank" rel="noopener">Öffnen ↗</a>')+'</article>').join('');
 $('#fleetInfo').textContent=p.devices.length+' Lüfter gefunden'+(p.scanning?' – Suche läuft …':'')+'. Fehlt einer? Er muss eingeschaltet, im WLAN und auf Firmware 2.2 oder neuer sein.';
 loadFleet.devs=p.devices;p.devices.forEach((d,i)=>fetch(d.self?'/api/status':'http://'+d.ip+'/api/status').then(r=>r.json()).then(s=>{const u=$('#u'+i);if(!u)return;
  const run=s.phase==='supply'||s.phase==='extract';u.dataset.phase=s.mode==='boost'&&run?'boost':s.phase;
  u.querySelector('.big').textContent=s.enabled?NAMES[s.mode]:'gesperrt';
  u.querySelectorAll('.meta')[1].textContent=[PHASE[s.phase]||s.phase,s.filter===1?'⚠ Filter':'',s.comm_ok===0?'⚠ keine Antwort':'',s.sync_group?'Gruppe '+(s.sync_group===1?'A':'B'):''].filter(Boolean).join(' · ')}).catch(()=>{const u=$('#u'+i);if(u)u.querySelector('.big').textContent='nicht erreichbar'}));
 if(p.scanning)setTimeout(()=>loadFleet(false),3500)}catch(e){toast(e.message,1)}}
$('#scanBtn').onclick=()=>loadFleet(true);
$$('[data-all]').forEach(b=>b.onclick=act(async()=>{const m=b.dataset.all;if(!confirm(NAMES[m]+' an alle gefundenen Lüfter senden?'))return;
 const devs=loadFleet.devs||[];let ok=0,fail=[];await Promise.all(devs.map(d=>fetch((d.self?'':'http://'+d.ip)+'/api/control?'+new URLSearchParams({mode:m,key:KEY}),{method:'POST'})
  .then(r=>{if(r.ok)ok++;else fail.push(d.name+(r.status===401?' (anderer Schlüssel)':''))}).catch(()=>fail.push(d.name))));
 toast(ok+' von '+devs.length+' Lüftern: '+NAMES[m]+(fail.length?' – nicht erreicht: '+fail.join(', '):''),fail.length>0);setTimeout(()=>loadFleet(false),800)}));

/* ---------- Loxone ---------- */
function renderLoxone(){const base=location.origin;$('#baseUrl').textContent=base;$('#statusUrl').textContent=base+'/api/loxone';
 const k='/api/control?key='+KEY;const rows=[['Betriebsart (analog)',k+'&mode=<v>'],['Stufe (analog)',k+'&level=<v>'],['Nachtbetrieb (EIN / AUS)',k+'&night=1  /  '+k+'&night=0'],
  ['Boost (Taster)',k+'&mode=boost'],['Aus (Taster)',k+'&mode=off'],['Filter-Reset (Taster)','/api/filterreset?key='+KEY]];
 $('#cmdTable tbody').innerHTML=rows.map(r=>'<tr><td>'+r[0]+'</td><td><code>'+r[1].replace(/&/g,'&amp;').replace(/</g,'&lt;')+'</code></td></tr>').join('')}
async function dlXml(path,prefix){const t=await api(path,{},'GET');save(prefix+'_Ambientika_'+(slug(ST?ST.name:'luefter')||'luefter')+'.xml',new Blob([t],{type:'application/xml'}))}
$('#voBtn').onclick=act(()=>dlXml('/api/loxone/output.xml','VO'));$('#viBtn').onclick=act(()=>dlXml('/api/loxone/input.xml','VIH'));
$$('[data-copy]').forEach(b=>b.onclick=()=>copy($('#'+b.dataset.copy).textContent));
async function copy(t){try{await navigator.clipboard.writeText(t);toast('Kopiert')}catch(e){prompt('Zum Kopieren markieren:',t)}}

/* ---------- System ---------- */
$('#optResume').onchange=act(async e=>{await api('/api/settings',{resume:e.target.checked?1:0});toast('Gespeichert')});
$('#optQuery').onchange=act(async e=>{await api('/api/settings',{autoquery:e.target.checked?1:0});toast('Gespeichert')});
$('#updForm').onsubmit=e=>{e.preventDefault();const f=e.target.firmware.files[0];if(!f)return;
 if(!/ota\.bin$/i.test(f.name)&&!confirm('Der Dateiname endet nicht auf „ota.bin“. Trotzdem hochladen?'))return;
 const x=new XMLHttpRequest(),fd=new FormData(),p=$('#updProg');fd.append('firmware',f,f.name);x.open('POST','/update?key='+encodeURIComponent(KEY));p.classList.remove('hidden');p.value=0;
 x.upload.onprogress=ev=>{if(ev.lengthComputable)p.value=Math.round(ev.loaded*100/ev.total)};
 x.onload=()=>{if(x.status===200){toast('Update erfolgreich – der ESP startet neu, Seite lädt in 20 s neu');setTimeout(()=>location.reload(),20000)}else toast('Update fehlgeschlagen: '+x.responseText,1)};
 x.onerror=()=>toast('Upload unterbrochen',1);toast('Firmware wird hochgeladen …');x.send(fd)};
const getLog=()=>api('/api/log',{},'GET');
$('#logShow').onclick=act(async()=>{const l=$('#log');l.textContent=await getLog();l.classList.remove('hidden');l.scrollTop=l.scrollHeight});
$('#logDl').onclick=act(async()=>save('ambientika-'+(SET?SET.hostname:'esp')+'-protokoll-'+new Date().toISOString().slice(0,16).replace(/[:T]/g,'-')+'.txt',new Blob([await getLog()],{type:'text/plain'})));
$('#logCopy').onclick=act(async()=>copy(await getLog()));
$('#logClear').onclick=act(async()=>{if(!confirm('Protokoll wirklich löschen?'))return;await api('/api/log/clear');$('#log').classList.add('hidden');toast('Protokoll gelöscht')});
$('#keyToggle').onclick=e=>{e.preventDefault();const v=$('#keyView');v.textContent=v.textContent===KEY?'••••••••':KEY};
$('#keyForm').onsubmit=act(async e=>{await api('/api/apikey',{newkey:e.target.newkey.value});KEY=e.target.newkey.value;e.target.reset();toast('Schlüssel gespeichert – in Loxone anpassen')});
$('#restartBtn').onclick=act(async()=>{if(!confirm('ESP neu starten?'))return;await api('/api/restart');toast('Neustart … Seite lädt in 15 s neu');setTimeout(()=>location.reload(),15000)});

/* ---------- Start ---------- */
(async()=>{try{const s=await json('/api/session');KEY=s.key}catch(e){toast('Anmeldung fehlgeschlagen – Seite neu laden',1);return}
 await Promise.all([refresh(),loadSettings()]);
 const want=location.hash.slice(1);show(want||((SET&&SET.has_password&&ST&&ST.enabled&&SET.connected)?'fan':'setup'));
 setInterval(refresh,2000);setInterval(loadSettings,15000)})();
</script></body></html>)HTML";
