/*
 * tss_poc.c — Minimal LKM PoC inspired by paradise.ko (game cheat carrier).
 *
 * Purpose: Verify that an out-of-tree LKM can load into stock OPlus ColorOS
 *          kernel 6.1.75-android14-11 using the vermagic-string trick +
 *          runtime kallsyms_lookup_name resolution (no Module.symvers needed).
 */
#define KBUILD_MODNAME "tss_poc"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/version.h>

static unsigned long (*tss_kallsyms_lookup_name)(const char *name) = NULL;

static int __init resolve_kallsyms(void)
{
    struct kprobe kp = { .symbol_name = "kallsyms_lookup_name" };
    int ret;

    ret = register_kprobe(&kp);
    if (ret < 0) {
        pr_err("tss_poc: register_kprobe(kallsyms_lookup_name) failed: %d\n", ret);
        return ret;
    }
    tss_kallsyms_lookup_name = (typeof(tss_kallsyms_lookup_name))kp.addr;
    unregister_kprobe(&kp);

    if (!tss_kallsyms_lookup_name) {
        pr_err("tss_poc: kallsyms_lookup_name addr is NULL after kprobe\n");
        return -ENOENT;
    }
    pr_info("tss_poc: kallsyms_lookup_name resolved @ 0x%lx\n",
            (unsigned long)tss_kallsyms_lookup_name);
    return 0;
}

static int __init test_dynamic_lookup(void)
{
    const char *targets[] = {
        "find_task_by_vpid",
        "get_pid_task",
        "pid_task",
        "register_user_hw_breakpoint",
        "unregister_hw_breakpoint",
        "__arch_copy_from_user",
        "init_task",
        "kprobe_blacklist",
        NULL
    };
    int i;
    int found = 0;
    for (i = 0; targets[i]; i++) {
        unsigned long addr = tss_kallsyms_lookup_name(targets[i]);
        pr_info("tss_poc: lookup %-35s -> 0x%lx %s\n",
                targets[i], addr, addr ? "OK" : "MISS");
        if (addr) found++;
    }
    pr_info("tss_poc: dynamic lookup test: %d/%d symbols found\n", found, i);
    return found > 0 ? 0 : -ENOENT;
}

static int __init tss_poc_init(void)
{
    int ret;
    pr_info("tss_poc: ===== loading on kernel %s =====\n", UTS_RELEASE);
    ret = resolve_kallsyms();
    if (ret) {
        pr_err("tss_poc: cannot resolve kallsyms - aborting init\n");
        return ret;
    }
    ret = test_dynamic_lookup();
    if (ret) {
        pr_err("tss_poc: dynamic lookup failed - aborting init\n");
        return ret;
    }
    pr_info("tss_poc: ===== init complete, module loaded =====\n");
    return 0;
}

static void __exit tss_poc_exit(void)
{
    pr_info("tss_poc: ===== unloading =====\n");
}

module_init(tss_poc_init);
module_exit(tss_poc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tss_poc");
MODULE_DESCRIPTION("Minimal LKM PoC for tss_sdk_decryptpacket hook research");
MODULE_VERSION("0.1");
