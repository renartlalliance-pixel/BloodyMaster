"""Integration test in an isolated Wine prefix; never targets a real Windows install.
Usage: python test_installer_wine.py WINE_WRAPPER PREFIX SETUP VST3_BINARY SMOKE_EXE
The wrapper must preserve Windows options such as /S and translate Unix file paths.
"""
from pathlib import Path
import hashlib,json,subprocess,sys,time
wrapper,prefix,setup,source,smoke=[Path(a).resolve() for a in sys.argv[1:]]
plugin=prefix/'drive_c/Program Files/Common Files/VST3/Bloody Master.vst3/Contents/x86_64-win/Bloody Master.vst3'
app=prefix/'drive_c/Program Files/BRI Audio Systems/BloodyMaster'
sentinel=plugin.parents[3]/'BloodyMaster-Installer-Test-Sentinel.txt'
sentinel.parent.mkdir(parents=True,exist_ok=True);sentinel.write_text('preserve me')
def run(*args):
    return subprocess.run([str(wrapper),*[str(a) for a in args]],check=True,capture_output=True,timeout=45)
run(setup,'/S');assert plugin.read_bytes()==source.read_bytes()
run(smoke,plugin)
run(setup,'/S');assert plugin.read_bytes()==source.read_bytes()
run(app/'Desinstaller.exe','/S')
end=time.monotonic()+15
while plugin.exists() and time.monotonic()<end:time.sleep(.1)
assert not plugin.exists(),'Plugin remains after uninstall'
assert sentinel.read_text()=='preserve me'
run(setup,'/S');assert plugin.read_bytes()==source.read_bytes()
report={'installer':setup.name,'bytes':setup.stat().st_size,'sha256':hashlib.sha256(setup.read_bytes()).hexdigest(),'architecture':'PE x64','install_hash_matches':True,'installed_vst3_stereo_smoke':True,'repeat_install':True,'uninstall_removes_plugin':True,'unrelated_file_preserved':True,'reinstall_hash_matches':True,'environment':'Wine 10 on Linux, not native Windows or FL Studio'}
(setup.parent/'installer-validation.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
