/**
 * Thread Protocol
 * Author: jk1806
 * Created: 2024-09-20
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init thread_protocol_init(void)
{
    pr_info("thread_protocol: Initializing\n");
    return 0;
}

static void __exit thread_protocol_exit(void)
{
    pr_info("thread_protocol: Exiting\n");
}

module_init(thread_protocol_init);
module_exit(thread_protocol_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Thread Protocol");
MODULE_VERSION(MODULE_VERSION);
