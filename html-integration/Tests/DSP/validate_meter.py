"""Independent 48 kHz loudness reference and SciPy 32x peak reconstruction.
Run: python validate_meter.py /path/to/bm_test_meter /scratch/directory
"""
import json, pathlib, subprocess, sys
import numpy as np
from scipy.signal import lfilter, resample_poly
exe=str(pathlib.Path(sys.argv[1]).resolve()); folder=pathlib.Path(sys.argv[2]);folder.mkdir(parents=True,exist_ok=True)
fs=48000
# Published BS.1770 48 kHz K-weighting coefficients (not imported from the C++ design).
b1=[1.53512485958697,-2.69169618940638,1.19839281085285];a1=[1,-1.69065929318241,.73248077421585]
b2=[1,-2,1];a2=[1,-1.99004745483398,.99007225036621]
def loudness(x):
    y=lfilter(b2,a2,lfilter(b1,a1,x,axis=0),axis=0)
    power=np.sum(y*y,axis=1);hop=4800
    windows=np.array([np.mean(power[i:i+4*hop]) for i in range(0,len(power)-4*hop+1,hop)])
    def lu(p):return -.691+10*np.log10(max(float(p),1e-30))
    passed=windows[[lu(p)>=-70 for p in windows]]
    threshold=max(-70,lu(passed.mean())-10)
    gated=passed[[lu(p)>=threshold for p in passed]]
    return lu(gated.mean()),lu(np.mean(power[-3*fs:]))
def measure(x,name):
    path=folder/(name+'.f64');np.asarray(x,dtype='<f8').tofile(path)
    return list(map(float,subprocess.check_output([exe,'--raw',str(fs),str(x.shape[1]),str(path)],text=True).split()))
reports=[]
t=np.arange(6*fs)/fs
for channels in [1,2]:
    for name,hz in [('bass',60),('mid',1000),('treble',10000)]:
        x=np.tile((.1*np.sin(2*np.pi*hz*t))[:,None],(1,channels))
        # Distinct gain segments exercise the relative gate and trailing 3 s.
        x[:fs]*=.001;x[fs:2*fs]*=.02;x[2*fs:3*fs]*=.1
        result=measure(x,f'{name}-{channels}');reference=loudness(x)
        error=max(abs(result[i]-reference[i]) for i in range(2))
        assert error<.03,(name,channels,result,reference)
        reports.append(dict(name=f'{name}-{channels}',integrated=result[0],shortTerm=result[1],maxLoudnessError=error))
rng=np.random.default_rng(43433424498)
x=rng.standard_normal((10*fs,2))
for i,db in enumerate([-80,-55,-35,-20,-8,-17,-26,-18,-55,-80]):
    x[i*fs:(i+1)*fs]*=10**(db/20)*.15
result=measure(x,'gated-noise');reference=loudness(x)
error=max(abs(result[i]-reference[i]) for i in range(2));assert error<.03,(result,reference)
reports.append(dict(name='gated-noise',integrated=result[0],shortTerm=result[1],maxLoudnessError=error))
for hz in [6000,12000,18000]:
    t=np.arange(fs)/fs;x=.8*np.sin(2*np.pi*hz*t+.7)
    # Fade endpoints to avoid attributing truncation ringing to a steady tone.
    x[:1000]*=np.linspace(0,1,1000);x[-1000:]*=np.linspace(1,0,1000)
    x=np.column_stack([x,-x]); measured=measure(x,f'peak-{hz}')[2]
    reference=float(np.max(np.abs(resample_poly(x,32,1,axis=0,window=('kaiser',10)))))
    error=abs(20*np.log10(measured/reference));assert error<.15,(hz,measured,reference,error)
    reports.append(dict(name=f'peak-{hz}',measuredPeak=measured,referencePeak32x=reference,errorDb=error))
(folder/'meter-validation.json').write_text(json.dumps(reports,indent=2)+'\n')
print(f'PASS {len(reports)} independent loudness/peak cases; report: {folder}/meter-validation.json')
