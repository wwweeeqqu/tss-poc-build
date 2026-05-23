#!/usr/bin/env python3
"""Patch vermagic= in .ko .modinfo section, RESIZING section if needed.

This version handles new vermagic longer than the original slot by:
  - extending the .modinfo section
  - shifting all sections after it by delta bytes
  - updating section header sh_offset fields
  - updating ELF header e_shoff
"""
import sys, struct


def patch(in_path, out_path, target_vm):
    with open(in_path, 'rb') as f:
        data = bytearray(f.read())

    if data[:4] != b'\x7fELF':
        print('ERROR: not ELF')
        return 1
    e_shoff = struct.unpack_from('<Q', data, 0x28)[0]
    e_shnum = struct.unpack_from('<H', data, 0x3c)[0]
    e_shentsize = struct.unpack_from('<H', data, 0x3a)[0]
    e_shstrndx = struct.unpack_from('<H', data, 0x3e)[0]
    SHDR_SIZE = e_shentsize  # 64

    def sh_get(i, field_off, fmt='<Q'):
        return struct.unpack_from(fmt, data, e_shoff + i * SHDR_SIZE + field_off)[0]

    # Find .modinfo section by name via .shstrtab
    shstrtab_off = sh_get(e_shstrndx, 0x18)
    shstrtab_size = sh_get(e_shstrndx, 0x20)
    shstrtab = bytes(data[shstrtab_off:shstrtab_off + shstrtab_size])

    def sec_name(i):
        n = struct.unpack_from('<I', data, e_shoff + i * SHDR_SIZE)[0]
        end = shstrtab.index(b'\x00', n)
        return shstrtab[n:end].decode('latin1', errors='replace')

    modinfo_idx = -1
    for i in range(e_shnum):
        if sec_name(i) == '.modinfo':
            modinfo_idx = i
            break
    if modinfo_idx < 0:
        print('ERROR: no .modinfo section')
        return 1

    modinfo_off = sh_get(modinfo_idx, 0x18)
    modinfo_size = sh_get(modinfo_idx, 0x20)
    print(f'.modinfo: off=0x{modinfo_off:x} size={modinfo_size}')

    # Parse fields
    old_data = bytes(data[modinfo_off:modinfo_off + modinfo_size])
    fields = [f for f in old_data.split(b'\x00') if f]
    new_fields = []
    print('Existing fields:')
    for f in fields:
        s = f.decode('latin1', errors='replace')
        if s.startswith('vermagic='):
            new = 'vermagic=' + target_vm
            print(f'  {s!r}  ->  {new!r}')
            new_fields.append(new)
        else:
            print(f'  {s!r}  (kept)')
            new_fields.append(s)

    new_content = b''.join(f.encode('latin1') + b'\x00' for f in new_fields)
    delta = len(new_content) - modinfo_size
    print(f'Resize delta: {delta:+d}')

    if delta == 0:
        data[modinfo_off:modinfo_off + modinfo_size] = new_content
        with open(out_path, 'wb') as f:
            f.write(data)
        print(f'Patched in place -> {out_path}')
        return 0

    # Resize section
    head = bytes(data[:modinfo_off])
    tail = bytes(data[modinfo_off + modinfo_size:])
    new_data = bytearray(head + new_content + tail)

    # Shift e_shoff if section header table is after modinfo
    after_off = modinfo_off + modinfo_size
    new_e_shoff = e_shoff + delta if e_shoff >= after_off else e_shoff
    struct.pack_into('<Q', new_data, 0x28, new_e_shoff)

    # Update section header offsets + size
    for i in range(e_shnum):
        nh_off = new_e_shoff + i * SHDR_SIZE
        s_off = struct.unpack_from('<Q', new_data, nh_off + 0x18)[0]
        if i == modinfo_idx:
            struct.pack_into('<Q', new_data, nh_off + 0x20, len(new_content))
            continue
        if s_off >= after_off:
            struct.pack_into('<Q', new_data, nh_off + 0x18, s_off + delta)

    with open(out_path, 'wb') as f:
        f.write(new_data)
    print(f'Patched + resized -> {out_path}  ({len(new_data)} bytes)')
    return 0


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print('usage: patch_vermagic.py in.ko out.ko "target vermagic string"')
        sys.exit(1)
    sys.exit(patch(sys.argv[1], sys.argv[2], sys.argv[3]))
