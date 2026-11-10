"""Independent reconstructed peaks for factory presets; numpy/scipy required."""
import hashlib
import json
import pathlib
import subprocess
import sys
import numpy as np
from scipy.io import wavfile
from scipy.signal import resample_poly
exe=str(pathlib.Path(sys.argv[1]).resolve())
root=pathlib.Path(sys.argv[2]);root.mkdir(parents=True,exist_ok=True)
bank=json.loads(subprocess.check_output([exe,'--presets-json'],text=True))
results=[]
for fs in (44100,48000,96000,192000):
    t=np.arange(int(fs*.3))/fs
    rng=np.random.default_rng(127)
    for vector in ('rhythmic','overload'):
        if vector=='rhythmic':
            env=np.exp(-35*(t%(.125)))
            left=.6*env*np.sin(2*np.pi*55*t)+.12*np.sin(2*np.pi*1300*t)+.04*rng.normal(size=len(t))
            right=.6*env*np.sin(2*np.pi*55*t)+.1*np.sin(2*np.pi*2200*t)+.04*rng.normal(size=len(t))
        else:
            left=1.4*np.sin(2*np.pi*(fs/4)*t+np.pi/4)
            right=.8*np.sin(2*np.pi*47*t)+.8*np.sin(2*np.pi*7100*t)
        x=np.column_stack((left,right)).astype(np.float32)
        for preset in bank:
            name=f'{preset["slug"]}_{vector}_{fs}'
            inp=root/(name+'.wav');wavfile.write(inp,fs,x)
            before=hashlib.sha256(inp.read_bytes()).digest()
            subprocess.run([exe,str(inp),'--preset',preset['slug']],check=True,capture_output=True)
            output=max(root.glob(name+'_BloodyMaster_*.wav'),key=lambda p:p.stat().st_mtime_ns)
            rate,y=wavfile.read(output)
            assert rate==fs and y.shape==x.shape and np.isfinite(y).all()
            assert hashlib.sha256(inp.read_bytes()).digest()==before
            peak=float(np.max(np.abs(resample_poly(y.astype(np.float64),32,1,axis=0,window=('kaiser',10)))))
            db=20*np.log10(max(peak,1e-12))
            ceiling=preset['parameters']['bm_trim']
            assert db<=ceiling+.05,(name,db,ceiling)
            results.append({'preset':preset['slug'],'vector':vector,'rate':fs,'peak_32x_dbtp':db})
report={'note':'Synthetic vectors only; no listening test, hardware matching or LUFS target validation.','renders':results}
(root/'preset_validation.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({'presets':len(bank),'renders_passed':len(results),'worst_peak_32x_dbtp':max(r['peak_32x_dbtp'] for r in results)},indent=2))
