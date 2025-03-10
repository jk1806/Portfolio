/**
 * NTP Implementation
 * Author: jk1806
 * Created: 2025-03-05
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init ntp_init(void)
{
    pr_info("ntp: Initializing\n");
    return 0;
}

static void __exit ntp_exit(void)
{
    pr_info("ntp: Exiting\n");
}

module_init(ntp_init);
module_exit(ntp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("NTP Implementation");
MODULE_VERSION(MODULE_VERSION);
