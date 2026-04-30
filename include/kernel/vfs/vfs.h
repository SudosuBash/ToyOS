#ifndef _TOYOS_VFS_H
#define _TOYOS_VFS_H

#include <kernel/stdint.h>
#include <kernel/cpu/smp.h>
#include <kernel/file/file.h>
#include <kernel/kernel.h>

#define superblock_of(oper) container_of((oper), struct vfs_sb_operation, v_oper)

struct vfs_inode;

struct vfs_sb_operation { //对于 superblock 的 vfs 操作
    void (*vfs_sb_init)(struct vfs_superblock* vfs_par);
    void (*vfs_sb_destroy)(struct vfs_superblock* vfs_par);
    void (*vfs_sb_del_inode)(struct vfs_superblock* vfs_par);
    void (*vfs_sb_create_inode)(struct vfs_superblock* vfs_par, struct vfs_inode* inode);
    struct vfs_inode* (*vfs_sb_get_inode)(struct vfs_superblock* vfs_par);
};

struct vfs_superblock {
    struct vfs_sb_operation v_op;
    struct linklist_head v_sibling;
    uint64_t v_inode_cnt;
    uint64_t v_inode_off;
    uint64_t v_magic;
};

struct vfs_inode_operation { //对于 inode 的 vfs 操作
    void (*vfs_inode_write)(struct vfs_inode* inode, void* data, size_t len, size_t *seek);
    void (*vfs_inode_read)(struct vfs_inode* inode, size_t seek, size_t len, void* buf);
    void (*vfs_load_subdir)(struct vfs_inode* inode, struct directory* dir);
};

struct vfs_inode { // inode 对象
    struct vfs_inode_operation f_op;
    struct vfs_superblock *sblock;

    uint64_t f_blk_count;
    uint64_t f_blk_off;
    uint64_t f_last_modified_time;
    uint64_t f_create_time;
    uint64_t f_read_time;
    uint64_t f_id; // 编号
    uint16_t f_perm;
    uint16_t f_flag;
    uint16_t f_ftype; 
};

#endif