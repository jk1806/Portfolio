/**
 * EMI/EMC compliance
 * Author: jk1806
 * Created: 2024-11-30
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init emi_emc_init(void)
{
    pr_info("emi_emc: Initializing\n");
    return 0;
}

static void __exit emi_emc_exit(void)
{
    pr_info("emi_emc: Exiting\n");
}

module_init(emi_emc_init);
module_exit(emi_emc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("EMI/EMC compliance");
MODULE_VERSION(MODULE_VERSION);
