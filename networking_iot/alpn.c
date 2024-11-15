/**
 * ALPN protocol negotiation
 * Author: jk1806
 * Created: 2024-11-12
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init alpn_init(void)
{
    pr_info("alpn: Initializing\n");
    return 0;
}

static void __exit alpn_exit(void)
{
    pr_info("alpn: Exiting\n");
}

module_init(alpn_init);
module_exit(alpn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("ALPN protocol negotiation");
MODULE_VERSION(MODULE_VERSION);
