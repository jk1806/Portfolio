/**
 * Ethernet AVB/TSN
 * Author: jk1806
 * Created: 2024-12-10
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init ethernet_avb_tsn_init(void)
{
    pr_info("ethernet_avb_tsn: Initializing\n");
    return 0;
}

static void __exit ethernet_avb_tsn_exit(void)
{
    pr_info("ethernet_avb_tsn: Exiting\n");
}

module_init(ethernet_avb_tsn_init);
module_exit(ethernet_avb_tsn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Ethernet AVB/TSN");
MODULE_VERSION(MODULE_VERSION);
