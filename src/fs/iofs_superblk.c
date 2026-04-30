#include <kernel/vfs/vfs.h>

#define IOFS_MAGIC_NUMBER 0x000000011D105D5B

static void iofs_sb_init(struct vfs_superblock* block) {
    
}

static struct vfs_superblock block = {
    .v_inode_cnt = 0,
    .v_magic = IOFS_MAGIC_NUMBER,
    .v_inode_off = 0,
};