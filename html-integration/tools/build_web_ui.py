"""Build a runtime copy; never modify the approved HTML or its reference copy."""
import hashlib,pathlib,re,sys
root=pathlib.Path(__file__).resolve().parents[1]
original=(root/'Resources/UI/index.html').read_bytes()
expected='c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff'
if hashlib.sha256(original).hexdigest()!=expected:raise SystemExit('Approved HTML hash changed; refusing to overwrite the reference.')
if original!=(root/'Resources/Reference/index.html').read_bytes():raise SystemExit('HTML reference differs.')
html=original.decode('utf-8')
html=html.replace('<script src="https://www.gstatic.com/antigravity/web/dev/tailwindcss.min.js"></script>','')
html=html.replace('</head>','<link rel="stylesheet" href="tailwind.css">\n</head>')
html,count=re.subn(r'<script>.*?</script>','<script src="bridge.js"></script>',html,flags=re.S)
if count!=1:raise SystemExit('Unexpected reference script structure')
# User-approved extension: independent amount and activation for both traps.
for name,parameter in [('lowres','mud'),('hires','ring')]:
    controls=f'''<div class="flex items-center justify-center gap-1 mt-1">
<button id="{parameter}ActiveBtn" class="tascam-btn text-[8px] px-1 py-0.5" title="Activer ce nettoyage">ON</button>
<div class="knob-base knob-orange w-6 h-6" id="knob-{parameter}-amount" data-param="{parameter}_amount" data-min="0" data-max="100" data-val="25" data-unit="%"><div class="knob-cap"><div class="knob-indicator"></div></div></div>
</div><div id="val-{parameter}_amount" class="text-[8px] font-mono text-neutral-400">25%</div>'''
    html,count=re.subn(r'(<div[^>]*id="val-'+name+r'"[^>]*>.*?</div>)',lambda m:m[0]+controls,html,flags=re.S)
    if count!=1:raise SystemExit('Trap placeholder not found: '+name)
# User-approved harmonic controls, added only to the generated runtime page.
color_controls='''<div class="flex items-center justify-end gap-3 mt-1" style="min-height:32px">
<span class="text-[8px] text-red-400 font-bold">XL MODE</span>
<div class="knob-base knob-red w-6 h-6" id="knob-xl-mode" data-param="xl_mode" data-min="0" data-max="2" data-step="1" data-val="0"><div class="knob-cap"><div class="knob-indicator"></div></div></div>
<span id="val-xl_mode" class="text-[8px] font-mono text-neutral-400" style="min-width:52px">Densité</span>
<button id="thdColorBtn" class="tascam-btn text-[8px] px-1 py-0.5">THD : Warm</button>
</div>'''
html,count=re.subn(r'(<span[^>]*id="val-thd"[^>]*>.*?</span>\s*</div>\s*</div>\s*</div>)',lambda m:m[0]+color_controls,html,flags=re.S)
if count!=1:raise SystemExit('Harmonic controls placeholder not found')
# The original footer overflows its HUD at the plugin's normal width. Keep
# these controls inside the HUD when adding the authorised mode controls.
html,count=re.subn(r'(<div class="[^"]*")(>\s*<div class="[^"]*">\s*<span[^>]*>TARGET:)',r'\1 style="flex-wrap:wrap;row-gap:8px"\2',html,count=1)
if count!=1:raise SystemExit('HUD target row not found')
tape_controls='''<div class="flex items-center justify-end gap-3 mt-1">
<span class="text-[8px] font-bold text-teal-300">TAPE COLOR</span>
<div class="knob-base knob-green w-6 h-6" id="knob-tape-color" data-param="tape_color" data-min="0" data-max="100" data-val="0" data-unit="%"><div class="knob-cap"><div class="knob-indicator"></div></div></div>
<span id="val-tape_color" class="text-[8px] font-mono text-neutral-400">0%</span>
<button id="tapeModeBtn" class="tascam-btn text-[8px] px-1 py-0.5">Discret</button></div>'''
html,count=re.subn(r'(<div[^>]*id="val-clipper"[^>]*>.*?</div>\s*</div>\s*</div>)',lambda m:m[0]+tape_controls,html,flags=re.S)
if count!=1:raise SystemExit('Tape controls placeholder not found')
# Real output meters and manual reset, only in generated runtime HTML.
meter_controls='<div class="flex items-center justify-center gap-3" style="position:absolute;left:6px;right:6px;bottom:6px;flex-wrap:wrap;row-gap:2px"><span id="loudnessReadout" style="flex-basis:100%;text-align:center" class="text-[8px] font-mono text-neutral-400">I — LUFS / S — LUFS / TP — dBTP</span><button id="monitorMatchBtn" class="tascam-btn text-[8px] px-1 py-0.5">Comp. écoute OFF</button><span id="outputSafetyReadout" class="text-[8px] font-mono text-neutral-400" title="Protection globale : crêtes et énergie pondérée K ; active aussi en bypass">Protection sortie : −0,0 dB</span><button id="resetMetersBtn" class="tascam-btn text-[8px] px-1 py-0.5">RAZ mesures</button></div>'
canvas='<canvas id="precisionMeterCanvas" class="w-full h-full block"></canvas>'
if html.count(canvas)!=1:raise SystemExit('Meter canvas placeholder not found')
html=html.replace(canvas,canvas.replace('class=', 'style="height:calc(100% - 62px)" class=')+meter_controls)
# User presets: controls only in the generated page; approved HTML unchanged.
preset_controls='<button id="saveUserPresetBtn" class="tascam-btn px-2 py-1 text-xs" title="Enregistrer les réglages dans User">Sauver</button><button id="refreshUserPresetsBtn" class="tascam-btn px-2 py-1 text-xs" title="Actualiser les presets User" aria-label="Actualiser User">↻</button>'
html,count=re.subn(r'(<select id="presetSelector".*?</select>)',lambda m:m[0]+preset_controls,html,count=1,flags=re.S)
if count!=1:raise SystemExit('Preset selector placeholder not found')
html=html.replace('id="presetSelector"','id="presetSelector" style="max-width:180px"')
dialog='''<dialog id="userPresetDialog" style="background:#151718;color:#ddd;border:1px solid #555;border-radius:8px;padding:20px;width:360px;max-width:90vw">
<form id="userPresetForm"><label for="userPresetName">Enregistrer dans User</label>
<input id="userPresetName" name="name" type="text" maxlength="80" required autocomplete="off" placeholder="Nom du preset" style="display:block;width:100%;margin:12px 0;background:#090909;color:#eee;border:1px solid #555;padding:8px">
<p id="userPresetError" role="alert" style="font-size:12px;color:#fca5a5"></p>
<div style="display:flex;justify-content:flex-end;gap:8px;margin-top:12px"><button id="cancelUserPresetBtn" type="button" class="tascam-btn px-2 py-1">Annuler</button><button id="confirmUserPresetBtn" type="submit" class="tascam-btn px-2 py-1">Enregistrer</button></div>
<p style="font-size:11px;color:#aaa;margin-top:12px">Un nom existant crée une copie numérotée.</p></form></dialog>
<div id="presetNotice" role="status" aria-live="polite" hidden style="position:fixed;bottom:12px;left:50%;transform:translateX(-50%);background:#151718;border:1px solid #555;border-radius:4px;padding:8px 12px;z-index:1000;font-size:12px;max-width:90vw"></div>'''
html=html.replace('<script src="bridge.js"></script>',dialog+'<script src="bridge.js"></script>')
# Transient global Delta audition, separate from the existing cleanup solo.
delta_button='<button id="deltaBtn" class="tascam-btn px-2 py-1 text-xs font-bold" aria-pressed="false" title="Écouter la différence entre le signal original et le signal traité.">DELTA</button>'
html,count=re.subn(r'(<button id="abBtn".*?</button>)',lambda m:m[0]+delta_button,html,count=1,flags=re.S)
if count!=1:raise SystemExit('A/B button placeholder not found for Delta')
html=html.replace('</head>','<style>#deltaBtn[aria-pressed="true"]{background:#78350f;color:#fef3c7;border-color:#f59e0b;box-shadow:0 0 8px #f59e0b88}</style></head>')
output=pathlib.Path(sys.argv[1]);output.mkdir(parents=True,exist_ok=True)
(output/'index.html').write_text(html,encoding='utf-8')
print('Runtime HTML generated; approved HTML unchanged.')
