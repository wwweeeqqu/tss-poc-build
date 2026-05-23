#!/usr/bin/env python3
"""Patch the vermagic= field in a .ko file's .modinfo section."""
import sys

def patch(in_path, out_path, target_vm):
    with open(in_path, 'rb') as f:
        data = bytearray(f.read())
    target_bytes = target_vm.encode('latin1') + b'\x00'
    print(f'Target vermagic: {target_vm}')
    marker = b'vermagic='
    idx = data.find(marker)
    if idx < 0:
        print('ERROR: no vermagic= marker found')
        return 1
    print(f'Found vermagic= at offset 0x{idx:x}')
    end = data.index(b'\x00', idx)
    old_full = bytes(data[idx:end+1]).decode('latin1', errors='replace')
    print(f'Old: {old_full!r}')
    new_full = marker + target_bytes
    print(f'New: {new_full!r}')
    old_len = end - idx + 1
    new_len = len(new_full)
    if new_len > old_len:
        print(f'ERROR: new ({new_len}) longer than slot ({old_len}).')
        return 2
    data[idx:idx+new_len] = new_full
    for j in range(idx + new_len, idx + old_len):
        data[j] = 0
    with open(out_path, 'wb') as f:
        f.write(data)
    print(f'Patched -> {out_path}  ({len(data)} bytes)')
    return 0

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(__doc__)
        sys.exit(1)
    sys.exit(patch(sys.argv[1], sys.argv[2], sys.argv[3]))
