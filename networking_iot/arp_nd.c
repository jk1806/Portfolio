/**
 * ARP/ND protocol
 * Author: jk1806
 * Created: 2024-10-25
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init arp_nd_init(void)
{
    pr_info("arp_nd: Initializing\n");
    return 0;
}

static void __exit arp_nd_exit(void)
{
    pr_info("arp_nd: Exiting\n");
}

module_init(arp_nd_init);
module_exit(arp_nd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("ARP/ND protocol");
MODULE_VERSION(MODULE_VERSION);
