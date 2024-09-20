/**
 * Zero-copy network paths
 * Author: jk1806
 * Created: 2024-09-12
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init zero_copy_init(void)
{
    pr_info("zero_copy: Initializing\n");
    return 0;
}

static void __exit zero_copy_exit(void)
{
    pr_info("zero_copy: Exiting\n");
}

module_init(zero_copy_init);
module_exit(zero_copy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Zero-copy network paths");
MODULE_VERSION(MODULE_VERSION);
