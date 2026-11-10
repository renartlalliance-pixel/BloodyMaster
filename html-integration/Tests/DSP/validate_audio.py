"""Independent rendered-audio checks. Requires numpy, scipy.
Usage: python validate_audio.py /path/to/BloodyMasterTest /temporary/output
The DSP under test is C++; true-peak estimation here is a different 32x FIR.
"""
import sys, pathlib, subprocess, json, hashlib
import numpy as np
from scipy.io import wavfile
from scipy.signal import resample_poly
exe=str(pathlib.Path(sys.argv[1]).resolve()); root=pathlib.Path(sys.argv[2]); root.mkdir(parents=True,exist_ok=True)
results=[]
def render(name,fs,x,options):
    inp=root/(name+'.wav'); wavfile.write(inp,fs,x.astype(np.float32))
    before=hashlib.sha256(inp.read_bytes()).hexdigest()
    p=subprocess.run([exe,str(inp),*options],capture_output=True,text=True)
    if p.returncode: raise AssertionError(p.stderr+p.stdout)
    candidates=list(root.glob(name+'_BloodyMaster_*.wav')); out=max(candidates,key=lambda x:x.stat().st_mtime_ns)
    rate,y=wavfile.read(out)
    y=y.astype(np.float64)
    if y.ndim==1: y=y[:,None]
    assert rate==fs and len(y)==len(x) and y.shape[1]==(1 if x.ndim==1 else x.shape[1])
    assert hashlib.sha256(inp.read_bytes()).hexdigest()==before
    assert np.isfinite(y).all()
    return y,out
for fs in (44100,48000,96000,192000):
    t=np.arange(int(fs*.35))/fs; n=len(t)
    rng=np.random.default_rng(715)
    vectors={
       'interpeak':1.4*np.sin(2*np.pi*(fs/4)*t+np.pi/4),
       'impulse':np.where(np.arange(n)==n//3,5.,0.),
       'bass_bursts':1.3*np.sin(2*np.pi*40*t)+.7*np.sin(2*np.pi*7000*t)*(np.sin(2*np.pi*12*t)>0),
       'noise':rng.normal(0,.9,n),
       'dc_step':np.where((t>.08)&(t<.2),2.,0.)}
    for os in (2,4,8):
        for name,x in vectors.items():
            y,out=render(f'{name}_{fs}_{os}',fs,x,['--dynamics-off','1','--clip','2','--ceiling','-1','--os',str(os)])
            tp=float(np.max(np.abs(resample_poly(y,32,1,axis=0,window=('kaiser',10.0)))))
            db=20*np.log10(max(tp,1e-12));sp=20*np.log10(max(float(np.max(np.abs(y))),1e-12))
            assert db<=-1+.05, (name,fs,db)
            results.append(dict(vector=name,rate=fs,oversampling=os,sample_peak_dbfs=sp,peak_32x_dbtp=db))
    y,_=render(f'bypass_{fs}',fs,np.column_stack([vectors['noise'],vectors['noise']*.4]),['--preset','bypass'])
    expected=np.column_stack([vectors['noise'],vectors['noise']*.4]).astype(np.float32)
    assert np.array_equal(y,expected)
# Aliasing comparison: 7 kHz sine, 5th harmonic folding to 13 kHz without OS.
fs=48000;t=np.arange(fs)/fs;x=.9*np.sin(2*np.pi*7000*t)
y,_=render('aliasing',fs,x,['--dynamics-off','1','--limiter-off','1','--clip','6','--os','8'])
drive=10**(6/20);a=np.abs(x*drive);u=(a-.75)/.5
naive=np.sign(x)*np.where(a<=.75,a,np.where(a>=1.25,1,.75+.5*(u-.5*u*u)))/drive
# Use only settled material, same window and amplitude normalization.
def amplitude(v,hz):
    v=v[2000:-2000]; w=np.hanning(len(v));phase=np.exp(-2j*np.pi*hz*np.arange(len(v))/fs)
    return float(2*abs(np.sum(v*w*phase))/sum(w))
a0=amplitude(naive,13000);a1=amplitude(y[:,0],13000)
improvement=20*np.log10(max(a0,1e-15)/max(a1,1e-15))
assert improvement>25,improvement
report={'true_peak_estimator':'independent scipy resample_poly, 32x Kaiser beta10; finite vector validation, not certification',
        'vectors':results,'alias_13khz_reduction_db':improvement,'bypass_exact_and_file_integrity':True}
(root/'audio_validation.json').write_text(json.dumps(report,indent=2))
print(json.dumps({'vectors_passed':len(results),'worst_peak_32x_dbtp':max(r['peak_32x_dbtp'] for r in results),'alias_reduction_db':improvement},indent=2))
