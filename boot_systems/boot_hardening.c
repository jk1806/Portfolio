/**
 * Boot Hardening Implementation
 * Author: jk1806
 * Created: 2024-07-10
 * 
 * Security hardening for boot process
 * Disables dangerous commands, locks boot order, secures environment
 */

#include <common.h>
#include <command.h>
#include <env.h>

#define BOOT_HARDENING_VERSION "1.0.0"

/**
 * List of dangerous commands to disable
 */
static const char *dangerous_commands[] = {
    "fdisk",
    "format",
    "erase",
    "reset",
    "go",
    NULL
};

/**
 * Disable dangerous commands
 */
int boot_hardening_disable_commands(void)
{
    int i;
    struct cmd_tbl *cmd;
    
    pr_info("Boot Hardening: Disabling dangerous commands\n");
    
    for (i = 0; dangerous_commands[i] != NULL; i++) {
        cmd = find_cmd(dangerous_commands[i]);
        if (cmd) {
            cmd->maxargs = 0;  // Disable command
            pr_info("Boot Hardening: Disabled '%s'\n", dangerous_commands[i]);
        }
    }
    
    return 0;
}

/**
 * Lock boot order
 */
int boot_hardening_lock_boot_order(void)
{
    char *boot_order;
    
    boot_order = env_get("boot_order");
    if (!boot_order) {
        return -1;
    }
    
    // Set boot order to read-only
    env_set_ro("boot_order");
    
    pr_info("Boot Hardening: Boot order locked to: %s\n", boot_order);
    
    return 0;
}

/**
 * Secure environment variables
 */
int boot_hardening_secure_env(void)
{
    const char *secure_vars[] = {
        "bootcmd",
        "bootargs",
        "bootdelay",
        NULL
    };
    
    int i;
    
    pr_info("Boot Hardening: Securing environment variables\n");
    
    for (i = 0; secure_vars[i] != NULL; i++) {
        env_set_ro(secure_vars[i]);
        pr_info("Boot Hardening: Protected '%s'\n", secure_vars[i]);
    }
    
    return 0;
}

/**
 * Secure JTAG interface
 */
int boot_hardening_secure_jtag(void)
{
    u32 jtag_ctrl;
    
    // Disable JTAG via hardware register
    jtag_ctrl = readl(JTAG_CTRL_REG);
    jtag_ctrl |= JTAG_DISABLE_BIT;
    writel(jtag_ctrl, JTAG_CTRL_REG);
    
    pr_info("Boot Hardening: JTAG interface disabled\n");
    
    return 0;
}

/**
 * Initialize boot hardening
 */
int boot_hardening_init(void)
{
    int ret;
    
    pr_info("Boot Hardening v%s: Initializing\n", BOOT_HARDENING_VERSION);
    
    ret = boot_hardening_disable_commands();
    if (ret) {
        return ret;
    }
    
    ret = boot_hardening_lock_boot_order();
    if (ret) {
        return ret;
    }
    
    ret = boot_hardening_secure_env();
    if (ret) {
        return ret;
    }
    
    ret = boot_hardening_secure_jtag();
    if (ret) {
        return ret;
    }
    
    pr_info("Boot Hardening: All security measures applied\n");
    
    return 0;
}

