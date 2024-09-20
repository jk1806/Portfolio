/**
 * NIC offloading
 * Author: jk1806
 * Created: 2024-09-18
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init nic_offloads_init(void)
{
    pr_info("nic_offloads: Initializing\n");
    return 0;
}

static void __exit nic_offloads_exit(void)
{
    pr_info("nic_offloads: Exiting\n");
}

module_init(nic_offloads_init);
module_exit(nic_offloads_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("NIC offloading");
MODULE_VERSION(MODULE_VERSION);
