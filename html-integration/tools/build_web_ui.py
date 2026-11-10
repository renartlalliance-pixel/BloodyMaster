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
output=pathlib.Path(sys.argv[1]);output.mkdir(parents=True,exist_ok=True)
(output/'index.html').write_text(html,encoding='utf-8')
print('Runtime HTML generated; approved HTML unchanged.')
