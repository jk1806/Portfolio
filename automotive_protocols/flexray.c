/**
 * FlexRay Protocol
 * Author: jk1806
 * Created: 2024-11-15
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init flexray_init(void)
{
    pr_info("flexray: Initializing\n");
    return 0;
}

static void __exit flexray_exit(void)
{
    pr_info("flexray: Exiting\n");
}

module_init(flexray_init);
module_exit(flexray_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("FlexRay Protocol");
MODULE_VERSION(MODULE_VERSION);
