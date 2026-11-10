"""Build a runtime copy; never modify the approved HTML or its reference copy."""
import hashlib,pathlib,re,sys
root=pathlib.Path(__file__).resolve().parents[1]
original=(root/'Resources/UI/index.html').read_bytes()
expected='c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff'
if hashlib.sha256(original).hexdigest()!=expected:raise SystemExit('Approved HTML hash changed; refusing to overwrite the reference.')
if original!=(root/'Resources/Reference/index.html').read_bytes():raise SystemExit('HTML reference differs.')
html=original.decode('utf-8')
html=html.replace('<script src="https://www.gstatic.com/antigravity/web/dev/tailwindcss.min.js"></script>','<link rel="stylesheet" href="tailwind.css">')
html,count=re.subn(r'<script>.*?</script>','<script src="bridge.js"></script>',html,flags=re.S)
if count!=1:raise SystemExit('Unexpected reference script structure')
output=pathlib.Path(sys.argv[1]);output.mkdir(parents=True,exist_ok=True)
(output/'index.html').write_text(html,encoding='utf-8')
print('Runtime HTML generated; approved HTML unchanged.')
