"""WAV integration checks; uses only the Python standard library.
Usage: python test_wav_io.py /path/to/BloodyMasterTest
"""
import hashlib
import pathlib
import struct
import subprocess
import sys
import tempfile

exe = str(pathlib.Path(sys.argv[1]).resolve())
checks = 0

def chunk(tag, data):
    return tag + struct.pack('<I', len(data)) + data + b'\0' * (len(data) % 2)

def riff(body):
    return b'RIFF' + struct.pack('<I', len(body) + 4) + b'WAVE' + body

def fmt(codec, bits, channels):
    align = channels * (bits // 8)
    return chunk(b'fmt ', struct.pack('<HHIIHH', codec, channels, 48000, 48000 * align, align, bits))

with tempfile.TemporaryDirectory(prefix='bloodymaster-wav-') as folder:
    root = pathlib.Path(folder)
    def invoke(name, content, success=True):
        global checks
        path = root / (name + '.wav')
        path.write_bytes(content)
        before = hashlib.sha256(content).digest()
        result = subprocess.run([exe, str(path), '--preset', 'bypass'], capture_output=True)
        assert (result.returncode == 0) == success, (name, result.stderr)
        assert hashlib.sha256(path.read_bytes()).digest() == before
        outputs = list(root.glob(name + '_BloodyMaster_*.wav'))
        assert bool(outputs) == success, name
        checks += 1
        return outputs

    for channels in (1, 2):
        for codec, bits in ((1, 16), (1, 24), (1, 32), (3, 32), (3, 64)):
            values = [-.5, 0., .25, .5] * channels
            if codec == 1:
                data = b''.join(int(v * (1 << (bits - 1))).to_bytes(bits // 8, 'little', signed=True) for v in values)
            else:
                data = struct.pack('<' + ('f' if bits == 32 else 'd') * len(values), *values)
            body = fmt(codec, bits, channels) + chunk(b'JUNK', b'abc') + chunk(b'data', data)
            name = f'maître_été_{codec}_{bits}_{channels}'
            output = invoke(name, riff(body))[0]
            raw = output.read_bytes()
            assert struct.unpack_from('<I', raw, 4)[0] + 8 == len(raw)
            assert struct.unpack_from('<' + 'f' * len(values), raw, 56) == tuple(values)
            saved = output.read_bytes()
            invoke(name, riff(body))
            assert output.read_bytes() == saved
            assert len(list(root.glob(name + '_BloodyMaster_*.wav'))) == 2

    valid_fmt = fmt(1, 16, 1)
    valid_data = chunk(b'data', b'\0\0' * 8)
    valid = riff(valid_fmt + valid_data)
    invalid = {
        'short': b'RIFF',
        'riff_size': valid[:4] + struct.pack('<I', 1) + valid[8:],
        'duplicate_fmt': riff(valid_fmt + valid_fmt + valid_data),
        'duplicate_data': riff(valid_fmt + valid_data + valid_data),
        'partial_header': riff(valid_fmt + valid_data + b'x'),
        'missing_padding': riff(valid_fmt + valid_data + b'JUNK\x01\0\0\0x'),
        'truncated_data': riff(valid_fmt + b'data' + struct.pack('<I', 200) + b'\0\0'),
        'unaligned': riff(valid_fmt + chunk(b'data', b'x')),
        'nan': riff(fmt(3, 32, 1) + chunk(b'data', struct.pack('<f', float('nan')))),
        'infinity': riff(fmt(3, 64, 1) + chunk(b'data', struct.pack('<d', float('inf')))),
        'three_channels': riff(fmt(1, 16, 3) + chunk(b'data', b'\0' * 12)),
    }
    for name, content in invalid.items():
        invoke(name, content, False)
print(f'PASS: {checks} WAV executions (formats, Unicode, collisions, invalid data, source integrity)')
