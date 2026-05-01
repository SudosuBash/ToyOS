#include <kernel/fs/devicefs.h>
#include <kernel/file/dir.h>
#include <kernel/file/path.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/def.h>
#include <kernel/fault/error.h>

//为了速度, 可以直接把它拆出来
static volatile uint64_t v_inode_id;
static spinlock_t v_inode_id_spin;

static struct vfs_inode* _dfs_alloc_inode(struct device* dev) {
    struct vfs_inode* v_inode = alloc_inode();
    if(IS_ERR(v_inode))
        return ERR_PTR(v_inode);
    
    spin_lock(&v_inode_id_spin);
    v_inode->f_id = v_inode_id++;
    spin_unlock(&v_inode_id_spin);

    v_inode->f_blk_count = 0;
    v_inode->f_blk_off = 0;
    v_inode->f_ftype = DIR_TYPE_FILE;
    v_inode->f_flag = 0;
    v_inode->f_perm = 0666;
    v_inode->sblock = NULL;

    return v_inode;
}

struct directory* devicefs_mount(char* path, char* name, struct device* dev) {
    struct directory *dir, *ret;
    struct vfs_inode* v_inode;

    dir = find_path_dir_from_root(path);
    if(path[0] != '/')
        return ERR_PTR(ENOEXT);
        
    if(IS_ERR(dir)) 
        return ERR_PTR(dir);

    v_inode = _dfs_alloc_inode(dev);
    if(IS_ERR(v_inode))
        return ERR_PTR(v_inode);
    ret = create_dir_node(name, dir, v_inode);
    if(IS_ERR(ret))
        return ERR_PTR(ret);
    ret->d_oper = dev->operation; //绑定驱动程序操作步骤
    return ret;
}

stat_t* devicefs_unmount(char* path, struct device* dev) {
    if(path[0] != '/') 
        return ERR_PTR(ENOEXT);
    
    struct directory* dir = find_path_dir_from_root(path);
    if(IS_ERR(dir))
        return ERR_PTR(ENOEXT);
    
    stat_t* st = del_dir_node(&dir);
    return ERR_PTR(st);    
}

void init_devicefs() {
    spin_init(&v_inode_id_spin);
    v_inode_id = 0;
}