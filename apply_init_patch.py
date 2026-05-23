"""Apply 7-line patch to SukiSU-Ultra userspace/ksuinit/src/init.rs.

Inserts /tss_dump.ko loading right after kernelsu.ko block, before unlink("/init").
Re-uses the existing load_module_from_path() function.
"""
import sys, re
from pathlib import Path

INSERT_BLOCK = '''
    // Load /tss_dump.ko if present in ramdisk (custom kernel hook module)
    if std::path::Path::new("/tss_dump.ko").exists() {
        log::info!("Loading tss_dump.ko..");
        if let Err(e) = load_module_from_path("/tss_dump.ko") {
            log::error!("Cannot load tss_dump.ko: {:?}", e);
        }
    }
'''


def main():
    if len(sys.argv) != 2:
        print("usage: apply_init_patch.py <init.rs path>")
        sys.exit(1)
    p = Path(sys.argv[1])
    src = p.read_text(encoding='utf-8')

    if 'tss_dump.ko' in src:
        print('already patched, skipping')
        return

    # Anchor: the closing "}" of the kernelsu.ko if/else, right before
    # the "// And now we should prepare the real init" comment.
    marker = '// And now we should prepare the real init'
    if marker not in src:
        print(f'ERROR: anchor {marker!r} not found in {p}')
        sys.exit(1)

    new = src.replace(marker, INSERT_BLOCK.lstrip('\n') + '\n    ' + marker)
    p.write_text(new, encoding='utf-8')
    print(f'patched {p}: +{len(new) - len(src)} bytes')


if __name__ == '__main__':
    main()
