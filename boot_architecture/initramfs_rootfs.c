/**
 * initramfs/rootfs Implementation
 * Author: jk1806
 * Created: 2024-02-20
 * 
 * Early userspace filesystem implementation
 * Handles CPIO archive parsing and root filesystem mounting
 */

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/fs_types.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/cpio.h>
#include <linux/initramfs.h>

#define INITRAMFS_MAGIC 0x070701  // New ASCII format
#define INITRAMFS_MAGIC_OLD 0x070702  // CRC format
#define MAX_INITRAMFS_SIZE (16 * 1024 * 1024)  // 16MB limit

struct initramfs_entry {
    struct list_head list;
    char *name;
    void *data;
    size_t size;
    umode_t mode;
    uid_t uid;
    gid_t gid;
    unsigned int mtime;
};

static LIST_HEAD(initramfs_entries);
static void *initramfs_data;
static size_t initramfs_size;

/**
 * Parse CPIO header
 */
static int parse_cpio_header(const char *p, struct cpio_header *hdr)
{
    unsigned long ino, mode, nlink, uid, gid, mtime, filesize, namesize;
    
    if (memcmp(p, "070701", 6) != 0 && memcmp(p, "070702", 6) != 0) {
        pr_err("initramfs: Invalid CPIO magic\n");
        return -1;
    }
    
    ino = cpio_parse_hex8(p + 6);
    mode = cpio_parse_hex8(p + 14);
    uid = cpio_parse_hex8(p + 22);
    gid = cpio_parse_hex8(p + 30);
    nlink = cpio_parse_hex8(p + 38);
    mtime = cpio_parse_hex8(p + 46);
    filesize = cpio_parse_hex8(p + 54);
    namesize = cpio_parse_hex8(p + 62);
    
    hdr->ino = ino;
    hdr->mode = mode;
    hdr->uid = uid;
    hdr->gid = gid;
    hdr->nlink = nlink;
    hdr->mtime = mtime;
    hdr->filesize = filesize;
    hdr->namesize = namesize;
    
    return 0;
}

/**
 * Extract file from CPIO archive
 */
static int extract_cpio_file(const char *data, size_t *offset)
{
    struct cpio_header hdr;
    struct initramfs_entry *entry;
    const char *name;
    void *file_data;
    size_t name_len, data_len;
    size_t header_size = 110;  // CPIO header size
    
    if (parse_cpio_header(data + *offset, &hdr) < 0)
        return -1;
    
    name = data + *offset + header_size;
    name_len = (hdr.namesize + 3) & ~3;  // Align to 4 bytes
    
    // Check for end marker
    if (strcmp(name, "TRAILER!!!") == 0) {
        *offset += header_size + name_len;
        return 1;  // End of archive
    }
    
    // Allocate entry
    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry)
        return -1;
    
    entry->name = kstrdup(name, GFP_KERNEL);
    if (!entry->name) {
        kfree(entry);
        return -1;
    }
    
    entry->mode = hdr.mode;
    entry->uid = hdr.uid;
    entry->gid = hdr.gid;
    entry->mtime = hdr.mtime;
    
    data_len = (hdr.filesize + 3) & ~3;  // Align to 4 bytes
    
    if (hdr.filesize > 0) {
        file_data = kmalloc(hdr.filesize, GFP_KERNEL);
        if (!file_data) {
            kfree(entry->name);
            kfree(entry);
            return -1;
        }
        memcpy(file_data, data + *offset + header_size + name_len, hdr.filesize);
        entry->data = file_data;
        entry->size = hdr.filesize;
    } else {
        entry->data = NULL;
        entry->size = 0;
    }
    
    list_add_tail(&entry->list, &initramfs_entries);
    
    *offset += header_size + name_len + data_len;
    
    pr_info("initramfs: Extracted %s (%zu bytes)\n", entry->name, entry->size);
    
    return 0;
}

/**
 * Load and parse initramfs
 */
static int __init initramfs_load(void)
{
    const char *data;
    size_t offset = 0;
    int ret;
    
    pr_info("initramfs: Loading CPIO archive\n");
    
    // Get initramfs data (embedded or from bootloader)
    data = (const char *)__initramfs_start;
    initramfs_size = __initramfs_end - __initramfs_start;
    
    if (initramfs_size == 0) {
        pr_warn("initramfs: No initramfs found\n");
        return 0;
    }
    
    if (initramfs_size > MAX_INITRAMFS_SIZE) {
        pr_err("initramfs: Archive too large (%zu bytes)\n", initramfs_size);
        return -1;
    }
    
    initramfs_data = vmalloc(initramfs_size);
    if (!initramfs_data) {
        pr_err("initramfs: Failed to allocate memory\n");
        return -1;
    }
    
    memcpy(initramfs_data, data, initramfs_size);
    
    // Parse CPIO archive
    while (offset < initramfs_size) {
        ret = extract_cpio_file(initramfs_data, &offset);
        if (ret > 0)  // End marker
            break;
        if (ret < 0) {
            pr_err("initramfs: Failed to extract file at offset %zu\n", offset);
            return -1;
        }
    }
    
    pr_info("initramfs: Loaded %zu entries\n", 
            list_size(&initramfs_entries));
    
    return 0;
}

/**
 * Mount root filesystem
 */
static int __init mount_rootfs(void)
{
    struct initramfs_entry *entry;
    char *root_dev = NULL;
    int ret;
    
    // Find root device in initramfs
    list_for_each_entry(entry, &initramfs_entries, list) {
        if (strcmp(entry->name, "/root") == 0) {
            root_dev = entry->data;
            break;
        }
    }
    
    if (!root_dev) {
        // Try default root device
        root_dev = "root=/dev/sda1";
    }
    
    pr_info("initramfs: Mounting rootfs from %s\n", root_dev);
    
    // Mount root filesystem
    ret = do_mount_root(root_dev, "ext4", MS_RDONLY, "");
    if (ret) {
        pr_err("initramfs: Failed to mount rootfs\n");
        return ret;
    }
    
    pr_info("initramfs: Root filesystem mounted\n");
    
    return 0;
}

early_initcall(initramfs_load);
rootfs_initcall(mount_rootfs);

