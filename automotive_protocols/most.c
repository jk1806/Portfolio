/**
 * MOST Protocol
 * Author: jk1806
 * Created: 2024-12-01
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init most_init(void)
{
    pr_info("most: Initializing\n");
    return 0;
}

static void __exit most_exit(void)
{
    pr_info("most: Exiting\n");
}

module_init(most_init);
module_exit(most_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("MOST Protocol");
MODULE_VERSION(MODULE_VERSION);
