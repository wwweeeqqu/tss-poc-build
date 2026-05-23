/*
 * user_loader.c — replicate SukiSU-Ultra ksuinit's load_module path in
 * userspace, to test whether OPlus CONFIG_MODULE_SIG_PROTECT=y blocks
 * module loading based on (a) PID/timing or (b) ELF content/signature.
 *
 * Procedure (mirrors ksuinit::load_module):
 *  1. Read .ko ELF into a buffer.
 *  2. Walk .symtab; for every SHN_UNDEF entry with a name, look it up in
 *     /proc/kallsyms and rewrite sym.st_shndx = SHN_ABS + sym.st_value = addr.
 *  3. Call init_module(2) (the legacy syscall taking a buffer, not the fd-based
 *     finit_module).
 *
 * Build (cross-compile aarch64 static):
 *   aarch64-linux-gnu-gcc -static -O2 -o user_loader user_loader.c
 *
 * Usage (root on device):
 *   ./user_loader /data/local/tmp/tss_poc.ko
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/stat.h>

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

static int drop_kptr_restrict(void) {
    FILE *f = fopen("/proc/sys/kernel/kptr_restrict", "w");
    if (!f) { perror("kptr_restrict open"); return -1; }
    fputs("0\n", f);
    fclose(f);
    return 0;
}

/* Look up symbol name in /proc/kallsyms. Returns address or 0. */
static uint64_t kallsyms_lookup(const char *target) {
    static FILE *fp = NULL;
    if (!fp) { fp = fopen("/proc/kallsyms", "r"); }
    if (!fp) return 0;
    rewind(fp);
    char line[1024], type, name[256];
    uint64_t addr;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%lx %c %255s", &addr, &type, name) >= 3) {
            if (strcmp(name, target) == 0) return addr;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <module.ko> [params]\n", argv[0]);
        return 1;
    }
    const char *path = argv[1];
    const char *params = (argc >= 3) ? argv[2] : "";

    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }
    struct stat st;
    fstat(fd, &st);
    size_t sz = st.st_size;
    uint8_t *buf = malloc(sz);
    if (read(fd, buf, sz) != (ssize_t)sz) { perror("read"); return 1; }
    close(fd);
    printf("[+] loaded %zu bytes from %s\n", sz, path);

    if (memcmp(buf, "\x7f""ELF", 4) != 0) { fprintf(stderr, "not ELF\n"); return 1; }

    /* ELF header */
    uint64_t e_shoff   = *(uint64_t*)(buf + 0x28);
    uint16_t e_shentsz = *(uint16_t*)(buf + 0x3a);
    uint16_t e_shnum   = *(uint16_t*)(buf + 0x3c);
    uint16_t e_shstrndx= *(uint16_t*)(buf + 0x3e);

    Elf64_Shdr *shdrs = (Elf64_Shdr*)(buf + e_shoff);
    uint8_t *shstrtab = buf + shdrs[e_shstrndx].sh_offset;

    /* find .symtab + .strtab */
    Elf64_Shdr *symtab = NULL, *strtab = NULL;
    for (int i = 0; i < e_shnum; i++) {
        const char *name = (const char*)(shstrtab + shdrs[i].sh_name);
        if (strcmp(name, ".symtab") == 0) symtab = &shdrs[i];
        else if (strcmp(name, ".strtab") == 0) strtab = &shdrs[i];
    }
    if (!symtab || !strtab) { fprintf(stderr, "no symtab/strtab\n"); return 1; }

    Elf64_Sym *syms = (Elf64_Sym*)(buf + symtab->sh_offset);
    const char *symstrs = (const char*)(buf + strtab->sh_offset);
    int nsyms = symtab->sh_size / sizeof(Elf64_Sym);
    printf("[+] symtab has %d entries\n", nsyms);

    drop_kptr_restrict();

    int patched = 0, unresolved = 0;
    for (int i = 0; i < nsyms; i++) {
        if (syms[i].st_shndx != SHN_UNDEF || syms[i].st_name == 0) continue;
        const char *name = symstrs + syms[i].st_name;
        uint64_t addr = kallsyms_lookup(name);
        if (addr) {
            syms[i].st_shndx = SHN_ABS;
            syms[i].st_value = addr;
            printf("    [%3d] %-40s -> 0x%016lx\n", i, name, addr);
            patched++;
        } else {
            printf("    [%3d] %-40s -> UNRESOLVED\n", i, name);
            unresolved++;
        }
    }
    printf("[+] patched %d / unresolved %d\n", patched, unresolved);

    if (unresolved) {
        fprintf(stderr, "ABORTING: %d symbols unresolved\n", unresolved);
        return 2;
    }

    printf("[+] calling init_module(buf, %zu, '%s')\n", sz, params);
    long rc = syscall(SYS_init_module, buf, sz, params);
    int e = errno;
    if (rc == 0) {
        printf("[+] SUCCESS rc=0\n");
        return 0;
    }
    printf("[!] FAIL rc=%ld errno=%d (%s)\n", rc, e, strerror(e));
    return 3;
}
