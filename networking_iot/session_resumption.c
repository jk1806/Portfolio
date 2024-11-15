/**
 * TLS session resumption
 * Author: jk1806
 * Created: 2024-11-05
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init session_resumption_init(void)
{
    pr_info("session_resumption: Initializing\n");
    return 0;
}

static void __exit session_resumption_exit(void)
{
    pr_info("session_resumption: Exiting\n");
}

module_init(session_resumption_init);
module_exit(session_resumption_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("TLS session resumption");
MODULE_VERSION(MODULE_VERSION);
