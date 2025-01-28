/**
 * clang-tidy static analysis
 * Author: jk1806
 * Created: 2025-01-10
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init clang_tidy_init(void)
{
    pr_info("clang_tidy: Initializing\n");
    return 0;
}

static void __exit clang_tidy_exit(void)
{
    pr_info("clang_tidy: Exiting\n");
}

module_init(clang_tidy_init);
module_exit(clang_tidy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("clang-tidy static analysis");
MODULE_VERSION(MODULE_VERSION);
