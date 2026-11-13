"""Generate standalone manual HTML from its Markdown source.
Requires Markdown 3.10: python -m pip install Markdown==3.10
Usage: python tools/build_manual.py [output.html]
The PDF can be printed from this HTML with a Chromium-based browser.
"""
from pathlib import Path
import sys
import markdown
root=Path(__file__).resolve().parents[1]
source=root/'docs/NOTICE_BLOODYMASTER_FR.md'
output=Path(sys.argv[1]) if len(sys.argv)>1 else source.with_suffix('.html')
css='''
@page { size:A4; margin:18mm 16mm 19mm; }
* { box-sizing:border-box; }
body { max-width:950px; margin:40px auto; padding:0 24px; color:#202326; font:15px/1.55 Arial,sans-serif; }
h1 { font-size:34px; line-height:1.15; border-bottom:5px solid #b99743; padding-bottom:18px; margin-bottom:12px; }
h2 { font-size:23px; line-height:1.25; color:#202326; border-bottom:1px solid #c9b679; margin-top:34px; padding-bottom:6px; }
h3 { font-size:18px; margin:23px 0 10px; }
a { color:#755a11; text-decoration:underline; }
p,li { orphans:3; widows:3; }
li { margin:7px 0; }
table { width:100%; border-collapse:collapse; margin:16px 0 20px; font-size:13px; line-height:1.45; }
th { background:#efe8d4; text-align:left; }
th,td { border:1px solid #d6d2c5; padding:8px; vertical-align:top; }
tr { break-inside:avoid; }
thead { display:table-header-group; }
h1,h2,h3 { break-after:avoid; }
code { font-size:90%; overflow-wrap:anywhere; }
.toc { background:#f6f3eb; padding:14px 20px; margin:24px 0; }
.toc ul { list-style:none; padding-left:0; }
.toc li { margin:4px 0; }
@media print { body { max-width:none; padding:0; margin:0; font-size:10.5pt; line-height:1.45; } h1 {font-size:28pt;} h2 {font-size:17pt;} h3 {font-size:13pt;} table {font-size:9pt;} th,td {padding:6px;} .toc {break-after:page;} a {color:#59420a;} }
'''
# Keep the summary after the introductory paragraph.
intro='Cette notice décrit'
md=markdown.Markdown(extensions=['tables','toc'],extension_configs={'toc':{'toc_depth':'2'}})
body=md.convert(source.read_text(encoding='utf-8'))
pos=body.index('</p>',body.index(intro))+4
body=body[:pos]+'<nav aria-label="Sommaire"><h2>Sommaire</h2>'+md.toc+'</nav>'+body[pos:]
output.write_text('<!doctype html><html lang="fr"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Bloody Master 0.3.9 — Notice française</title><style>'+css+'</style><main>'+body+'</main></html>',encoding='utf-8')
print(output)
