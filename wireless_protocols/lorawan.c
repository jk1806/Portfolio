/**
 * LoRaWAN Implementation
 * Author: jk1806
 * Created: 2024-10-01
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init lorawan_init(void)
{
    pr_info("lorawan: Initializing\n");
    return 0;
}

static void __exit lorawan_exit(void)
{
    pr_info("lorawan: Exiting\n");
}

module_init(lorawan_init);
module_exit(lorawan_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("LoRaWAN Implementation");
MODULE_VERSION(MODULE_VERSION);
