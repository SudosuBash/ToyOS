#include <kernel/vfs/vfs.h>
#include <kernel/mm/mm.h>
#include <kernel/fault/error.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/fs/devicefs.h>

DEFINE_PERCPU_VAR(kmem_fs_percpu_sb_allocator, struct kmem_cache)
DEFINE_PERCPU_VAR(kmem_fs_percpu_allocator, struct kmem_cache)
static struct kmem_cache* kmem_fs_allocator;
static struct kmem_cache* kmem_fs_sb_allocator;

extern void init_dir_module();
static void __init_superblk_operation(struct vfs_superblock* blk) {
    blk->v_op.vfs_sb_create_inode = NULL;
    blk->v_op.vfs_sb_del_inode = NULL;
    blk->v_op.vfs_sb_get_inode = NULL;
    blk->v_op.vfs_sb_destroy = NULL;
    blk->v_op.vfs_sb_init = NULL;
}

struct vfs_superblock* alloc_superblock() {
    struct vfs_superblock* v_sblk = kmem_cache_alloc(kmem_fs_sb_allocator);
    if(IS_ERR(v_sblk))
        return ERR_PTR(v_sblk);
    __init_superblk_operation(v_sblk);
    return v_sblk;
}

static void __init_inode_operation(struct vfs_inode* inode) {
    inode->f_op.vfs_inode_read = NULL;
    inode->f_op.vfs_inode_rename = NULL;
    inode->f_op.vfs_inode_write = NULL;
    inode->f_op.vfs_get_subdir = NULL;
}

struct vfs_inode* alloc_inode() {
    struct vfs_inode* v_inode = kmem_cache_alloc(kmem_fs_allocator);
    if(IS_ERR(v_inode))
        return ERR_PTR(v_inode);
    v_inode->f_create_time = 0;
    v_inode->f_last_modified_time = 0;
    v_inode->f_read_time = 0;
    
    __init_inode_operation(v_inode);

    return v_inode;
}

void init_vfs() {
    kmem_fs_allocator = THIS_CPU_PTR(kmem_fs_percpu_allocator);
    kmem_fs_sb_allocator = THIS_CPU_PTR(kmem_fs_percpu_sb_allocator);
    kmem_cache_init(kmem_fs_allocator, sizeof(struct vfs_inode));
    kmem_cache_init(kmem_fs_sb_allocator, sizeof(struct vfs_superblock));

    init_dir_module();
    init_devicefs();
}
