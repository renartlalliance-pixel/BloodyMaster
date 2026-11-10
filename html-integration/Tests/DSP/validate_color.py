"""Independent FFT/reconstruction checks for the original harmonic stages."""
import json, pathlib, subprocess, sys
import numpy as np
from scipy.signal import resample_poly

binary, directory = sys.argv[1:3]
directory = pathlib.Path(directory)
subprocess.run([binary, '--export', str(directory)], check=True)
rows = []
for mode in range(6):
    frequency = 14000 if mode in (0, 5) else 7000
    # Even stages: second harmonic folds to 20k; odd stages: fifth to 13k.
    alias = 20000 if frequency == 14000 else 13000
    reference = np.fromfile(directory / f'color-{mode}-1.f64', dtype='<f8')
    reference_fft = np.abs(np.fft.rfft(reference)) * 2 / len(reference)
    for factor in (2, 4, 8):
        audio = np.fromfile(directory / f'color-{mode}-{factor}.f64', dtype='<f8')
        assert len(audio) == 48000 and np.all(np.isfinite(audio))
        spectrum = np.abs(np.fft.rfft(audio)) * 2 / len(audio)
        reduction = 20 * np.log10(max(reference_fft[alias], 1e-15) / max(spectrum[alias], 1e-15))
        assert reduction > 25, (mode, factor, reduction)
        peak = float(np.max(np.abs(resample_poly(audio, 32, 1, window=('kaiser', 10)))))
        rows.append(dict(mode=mode, oversampling=factor, alias_hz=alias,
                         alias_reduction_db=float(reduction), reconstructed_peak=peak))
report = dict(passed=True, cases=len(rows), minimum_alias_reduction_db=min(r['alias_reduction_db'] for r in rows),
              note='Isolated coloration, clipper/limiter disabled. Specific coherent tones, not a universal aliasing bound.', results=rows)
(directory / 'report.json').write_text(json.dumps(report, indent=2))
print(json.dumps({k:v for k,v in report.items() if k != 'results'}, indent=2))
