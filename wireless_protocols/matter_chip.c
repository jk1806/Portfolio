/**
 * Matter/CHIP Protocol
 * Author: jk1806
 * Created: 2024-09-10
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init matter_chip_init(void)
{
    pr_info("matter_chip: Initializing\n");
    return 0;
}

static void __exit matter_chip_exit(void)
{
    pr_info("matter_chip: Exiting\n");
}

module_init(matter_chip_init);
module_exit(matter_chip_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Matter/CHIP Protocol");
MODULE_VERSION(MODULE_VERSION);
