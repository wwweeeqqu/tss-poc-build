/*
 * my_ksuinit.c — minimal standalone replacement for SukiSU-Ultra ksuinit.
 *
 * Compiled aarch64 static (glibc), placed in init_boot ramdisk as /init.
 * Boot flow:
 *   1. kernel exec /init  (us, as PID 1)
 *   2. we mount /proc + /sys
 *   3. setup /dev/kmsg for logging
 *   4. load /kernelsu.ko (SukiSU module) via init_module + kallsyms relocation
 *   5. load /tss_dump.ko (our hook module) via same path
 *   6. unlink /init (ourselves)
 *   7. symlink init.real -> /init
 *   8. execve /init  (which is init.real, the real OPlus init)
 *
 * Step 5 is the only NEW step versus stock ksuinit. Everything else mirrors
 * the Rust ksuinit logic at SukiSU-Ultra/userspace/ksuinit/src/init.rs.
 *
 * Why this approach (vs forking Rust ksuinit): we already have user_loader.c
 * which implements 95% of the work (read ko, ELF kallsyms prerelocate, call
 * init_module(2)). Re-using it as a single-file C drop-in is simpler than
 * setting up a Rust musl cross-build for a 7-line patch.
 *
 * Build (in GitHub Actions, same toolchain that produces user_loader):
 *   aarch64-linux-gnu-gcc -static -O2 -o my_ksuinit my_ksuinit.c
 *
 * IMPORTANT: this binary will be PID 1 if installed as /init. Crashing here
 * panics the kernel.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define SHN_UNDEF 0
#define SHN_ABS   0xfff1

typedef struct {
    uint32_t st_name;
    uint8_t  st_info;
    uint8_t  st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
} __attribute__((packed)) Elf64_Sym;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} __attribute__((packed)) Elf64_Shdr;

/* Log to /dev/kmsg if available, else /dev/null. We're PID 1 so we can't
 * exec a logger — use raw write. */
static int kmsg_fd = -1;
static void klog(const char *fmt, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n <= 0) return;
    if (kmsg_fd >= 0) write(kmsg_fd, buf, n);
}

static int file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static int mount_fs(const char *src, const char *target, const char *type) {
    if (!file_exists(target)) {
        mkdir(target, 0755);
    }
    if (mount(src, target, type, 0, NULL) < 0) {
        klog("<3>my_ksuinit: mount %s on %s failed: %s\n", type, target, strerror(errno));
        return -1;
    }
    return 0;
}

static void setup_kmsg(void) {
    /* Try /dev/kmsg, else create one with mknod */
    int fd = open("/dev/kmsg", O_WRONLY | O_CLOEXEC);
    if (fd < 0) {
        mknod("/kmsg", S_IFCHR | 0666, makedev(1, 11));
        fd = open("/kmsg", O_WRONLY | O_CLOEXEC);
    }
    kmsg_fd = fd;
    /* Disable kmsg rate limiting */
    int rl = open("/proc/sys/kernel/printk_devkmsg", O_WRONLY);
    if (rl >= 0) { write(rl, "on\n", 3); close(rl); }
}

static uint64_t kallsyms_lookup(FILE *fp, const char *target) {
    rewind(fp);
    char line[1024], type, name[256];
    uint64_t addr;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%lx %c %255s", &addr, &type, name) >= 3) {
            if (strcmp(name, target) == 0 && addr != 0) return addr;
        }
    }
    return 0;
}

/* Load a kernel module: read, kallsyms-relocate, init_module(2). */
static int load_module(const char *path) {
    klog("<6>my_ksuinit: loading %s\n", path);

    int fd = open(path, O_RDONLY);
    if (fd < 0) { klog("<3>my_ksuinit: open %s: %s\n", path, strerror(errno)); return -1; }
    struct stat st;
    fstat(fd, &st);
    size_t sz = st.st_size;
    uint8_t *buf = malloc(sz);
    if (!buf || read(fd, buf, sz) != (ssize_t)sz) {
        klog("<3>my_ksuinit: read %s failed\n", path);
        close(fd); free(buf); return -1;
    }
    close(fd);

    if (memcmp(buf, "\x7f""ELF", 4) != 0) {
        klog("<3>my_ksuinit: %s not ELF\n", path);
        free(buf); return -1;
    }

    uint64_t e_shoff = *(uint64_t*)(buf + 0x28);
    uint16_t e_shnum = *(uint16_t*)(buf + 0x3c);
    uint16_t e_shstrndx = *(uint16_t*)(buf + 0x3e);

    Elf64_Shdr *shdrs = (Elf64_Shdr*)(buf + e_shoff);
    uint8_t *shstrtab = buf + shdrs[e_shstrndx].sh_offset;
    Elf64_Shdr *symtab = NULL, *strtab = NULL;
    for (int i = 0; i < e_shnum; i++) {
        const char *nm = (const char*)(shstrtab + shdrs[i].sh_name);
        if (strcmp(nm, ".symtab") == 0) symtab = &shdrs[i];
        else if (strcmp(nm, ".strtab") == 0) strtab = &shdrs[i];
    }
    if (!symtab || !strtab) {
        klog("<3>my_ksuinit: %s no symtab/strtab\n", path);
        free(buf); return -1;
    }

    /* Drop kptr_restrict so /proc/kallsyms shows real addresses */
    int kr = open("/proc/sys/kernel/kptr_restrict", O_WRONLY);
    if (kr >= 0) { write(kr, "0\n", 2); close(kr); }

    FILE *ks = fopen("/proc/kallsyms", "r");
    if (!ks) {
        klog("<3>my_ksuinit: /proc/kallsyms not readable\n");
        free(buf); return -1;
    }

    Elf64_Sym *syms = (Elf64_Sym*)(buf + symtab->sh_offset);
    const char *symstrs = (const char*)(buf + strtab->sh_offset);
    int nsyms = symtab->sh_size / sizeof(Elf64_Sym);
    int patched = 0, unresolved = 0;
    for (int i = 0; i < nsyms; i++) {
        if (syms[i].st_shndx != SHN_UNDEF || syms[i].st_name == 0) continue;
        const char *nm = symstrs + syms[i].st_name;
        uint64_t addr = kallsyms_lookup(ks, nm);
        if (addr) {
            syms[i].st_shndx = SHN_ABS;
            syms[i].st_value = addr;
            patched++;
        } else {
            klog("<3>my_ksuinit: %s UNRESOLVED symbol %s\n", path, nm);
            unresolved++;
        }
    }
    fclose(ks);
    klog("<6>my_ksuinit: %s symbols patched=%d unresolved=%d\n", path, patched, unresolved);
    if (unresolved) { free(buf); return -1; }

    long rc = syscall(SYS_init_module, buf, sz, "");
    int e = errno;
    free(buf);
    if (rc != 0) {
        klog("<3>my_ksuinit: init_module(%s) failed rc=%ld errno=%d (%s)\n",
             path, rc, e, strerror(e));
        return -1;
    }
    klog("<6>my_ksuinit: %s loaded OK\n", path);
    return 0;
}

/* Check if KernelSU is already loaded (built into kernel). */
static int has_kernelsu(void) {
    /* SukiSU exposes /proc/sys/kernel/su_compat_node or /dev/ksu */
    if (access("/dev/ksu", F_OK) == 0) return 1;
    return 0;
}

int main(int argc, char **argv, char **envp) {
    setup_kmsg();
    klog("<5>my_ksuinit: Hello — booting via custom ksuinit\n");

    mount_fs("proc", "/proc", "proc");
    mount_fs("sysfs", "/sys", "sysfs");

    setup_kmsg();  /* may have access to /dev/kmsg now via /proc-mediated /dev */

    /* Load kernelsu.ko (required for root persistence) */
    if (has_kernelsu()) {
        klog("<6>my_ksuinit: KernelSU already loaded, skipping\n");
    } else {
        load_module("/kernelsu.ko");
    }

    /* Load our tss_dump.ko (best-effort, non-fatal) */
    if (file_exists("/tss_dump.ko")) {
        load_module("/tss_dump.ko");
    } else {
        klog("<5>my_ksuinit: no /tss_dump.ko, skipping\n");
    }

    /* Replace /init symlink to point at real init */
    unlink("/init");
    const char *real_init = file_exists("/init.real") ? "/init.real" : "/system/bin/init";
    if (symlink(real_init, "/init") < 0) {
        klog("<3>my_ksuinit: symlink %s -> /init failed: %s\n", real_init, strerror(errno));
    }

    klog("<5>my_ksuinit: handing off to %s\n", real_init);
    execve("/init", argv, envp);

    /* execve never returns on success */
    klog("<0>my_ksuinit: execve(/init) failed: %s\n", strerror(errno));
    return 1;
}
