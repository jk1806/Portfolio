/**
 * Certificate pinning
 * Author: jk1806
 * Created: 2024-11-18
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init cert_pinning_init(void)
{
    pr_info("cert_pinning: Initializing\n");
    return 0;
}

static void __exit cert_pinning_exit(void)
{
    pr_info("cert_pinning: Exiting\n");
}

module_init(cert_pinning_init);
module_exit(cert_pinning_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Certificate pinning");
MODULE_VERSION(MODULE_VERSION);
