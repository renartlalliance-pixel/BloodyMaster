"""Independent musical fixture, BS.1770 gating and 32x peak checks.
Usage: python validate_loudness_gain.py /path/to/BloodyMasterTest /temporary/output
Requires numpy, scipy; ffmpeg additionally records AAC/MP3 reconstruction.
Synthetic fixture only: does not predict loudness/transparency of a user's track.
"""
import numpy as np,pathlib,subprocess,json,sys,hashlib,shutil
from scipy.signal import resample_poly
from scipy.signal import lfilter
from scipy.io import wavfile
exe=str(pathlib.Path(sys.argv[1]).resolve());root=pathlib.Path(sys.argv[2]);root.mkdir(parents=True,exist_ok=True)
fs=48000;t=np.arange(fs*8)/fs;rng=np.random.default_rng(20261113)
noise=rng.normal(size=(len(t),2));bed=lfilter([.08],[1,-.92],noise,axis=0)
kick=np.exp(-np.mod(t,.5)/.045)*np.sin(2*np.pi*55*t)
snare=np.exp(-np.mod(t+.25,.5)/.018)[:,None]*noise
x=(.08*np.sin(2*np.pi*61*t)[:,None]+.6*kick[:,None]+.12*snare+.08*bed+.015*np.column_stack([np.sin(2*np.pi*420*t),np.sin(2*np.pi*620*t)]))
def measure(y):
 z=lfilter([1.53512485958697,-2.69169618940638,1.19839281085285],[1,-1.69065929318241,.73248077421585],y,axis=0)
 z=lfilter([1,-2,1],[1,-1.99004745483398,.99007225036621],z,axis=0);power=np.sum(z*z,axis=1)
 w=np.array([power[i:i+19200].mean() for i in range(0,len(power)-19199,4800)]);w=w[w>=10**((-70+.691)/10)];cut=max(10**((-70+.691)/10),w.mean()*.1)
 return float(-.691+10*np.log10(w[w>=cut].mean()))
x*=10**((-22-measure(x))/20);src=root/'beat.wav';wavfile.write(src,fs,x.astype('float32'))
before=hashlib.sha256(src.read_bytes()).hexdigest();results=[]
for war in [0,6,12,18]:
 run=subprocess.run([exe,str(src),'--preset','clean','--drive','12','--push','6','--clip','1.8','--ceiling','-3','--loudness-war',str(war)],capture_output=True,text=True);assert run.returncode==0,run.stderr
 out=max(root.glob('beat_BloodyMaster_*.wav'),key=lambda p:p.stat().st_mtime_ns);rate,y=wavfile.read(out)
 assert rate==fs and y.shape==x.shape and np.isfinite(y).all()
 peak=float(20*np.log10(np.max(np.abs(resample_poly(y.astype(float),32,1,axis=0,window=('kaiser',12))))))
 row={'loudnessWarDb':war,'integratedLUFS':measure(y),'truePeak32xDb':peak}
 assert peak<=-3+.05,row
 assert row['integratedLUFS']<=-5,row
 if results:assert row['integratedLUFS']>results[-1]['integratedLUFS']+.5,row
 if war==12:
  assert -10<=row['integratedLUFS']<=-7,row
  if shutil.which('ffmpeg'):
   for codec,args,suffix in [('aac',['-c:a','aac','-b:a','256k'],'m4a'),('mp3',['-c:a','libmp3lame','-b:a','320k'],'mp3')]:
    encoded=root/('musical.'+suffix);decoded=root/('decoded-'+codec+'.wav')
    subprocess.run(['ffmpeg','-v','error','-y','-i',str(out),*args,str(encoded)],check=True)
    subprocess.run(['ffmpeg','-v','error','-y','-i',str(encoded),'-c:a','pcm_f32le',str(decoded)],check=True)
    _,audio=wavfile.read(decoded)
    row[codec+'DecodedPeak32xDb']=float(20*np.log10(np.max(np.abs(resample_poly(audio.astype(float),32,1,axis=0,window=('kaiser',12))))))
    row[codec+'DecodedClips']=row[codec+'DecodedPeak32xDb']>=0
 results.append(row);print(row,flush=True)
assert hashlib.sha256(src.read_bytes()).hexdigest()==before
(root/'report.json').write_text(json.dumps({'inputLUFS':measure(x),'sourceIntact':True,'fixture':'Deterministic 8 s stereo bass/drums/bed; Drive 12, Hammer 6, ceiling -3','results':results},indent=2)+'\n')
