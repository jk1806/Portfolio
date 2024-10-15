/**
 * NB-IoT/LTE-M Implementation
 * Author: jk1806
 * Created: 2024-10-10
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init nbiot_ltem_init(void)
{
    pr_info("nbiot_ltem: Initializing\n");
    return 0;
}

static void __exit nbiot_ltem_exit(void)
{
    pr_info("nbiot_ltem: Exiting\n");
}

module_init(nbiot_ltem_init);
module_exit(nbiot_ltem_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("NB-IoT/LTE-M Implementation");
MODULE_VERSION(MODULE_VERSION);
