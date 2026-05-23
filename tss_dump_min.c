/*
 * tss_dump_min.c — bare-minimum kernel module loaded by my_ksuinit at PID 1.
 *
 * Purpose: validate the boot chain (kernel → my_ksuinit → kernelsu.ko +
 * tss_dump_min.ko → init.real) doesn't brick the device.
 *
 * Does NOT:
 *   - Hook anything
 *   - Touch user-space
 *   - Register kprobes / hardware breakpoints
 *   - Open any sysfs / procfs / chardev
 *
 * Does ONLY:
 *   - printk a marker on init
 *   - printk a marker on exit
 *
 * If this loads + system boots, we know:
 *   1. my_ksuinit replaces SukiSU ksuinit cleanly
 *   2. ksuinit-style ELF kallsyms relocation + init_module works for our ko
 *   3. sig_protect bypass at PID 1 is confirmed for our build chain
 *   4. We can iterate: replace this file with real tss_dump.ko that registers
 *      a hardware breakpoint on libtersafe.so + 0x1ba0d4 when sgame mmaps it
 */
#define KBUILD_MODNAME "tss_dump_min"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init tss_dump_min_init(void)
{
    pr_info("tss_dump_min: ===== HELLO from PID %d (init phase) =====\n",
            current->pid);
    return 0;
}

static void __exit tss_dump_min_exit(void)
{
    pr_info("tss_dump_min: ===== goodbye =====\n");
}

module_init(tss_dump_min_init);
module_exit(tss_dump_min_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tss_poc");
MODULE_DESCRIPTION("Bare-minimum LKM for boot-chain validation");
MODULE_VERSION("0.1");
