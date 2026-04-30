#include <kernel/drivers/drv_devicefs.h>
#include <kernel/file/dir.h>
#include <kernel/file/path.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/def.h>
#include <kernel/fault/error.h>

DEFINE_PERCPU_VAR(kmem_fs_allocator, struct kmem_cache*)

static volatile uint64_t v_inode_id;
static volatile spinlock_t v_inode_id_spin;

static struct vfs_inode* _dfs_alloc_inode(struct device* dev) {
    struct vfs_inode* v_inode = kmem_cache_alloc(kmem_fs_allocator);
    if(IS_ERR(v_inode))
        return ERR_PTR(v_inode);
    
    //Race Condition
    spin_lock(&v_inode_id_spin);
    v_inode->f_id = v_inode_id++;
    spin_unlock(&v_inode_id_spin);

    v_inode->f_blk_count = 0;
    v_inode->f_blk_off = 0;
    v_inode->f_create_time = 0;
    v_inode->f_ftype = DIR_TYPE_MNT;
    v_inode->f_last_modified_time = 0;
    v_inode->f_read_time = 0;
    v_inode->f_op = dev->operation;
    v_inode->f_flag = 0;
    v_inode->f_perm = 0;
    v_inode->sblock = NULL;

    return v_inode;
}

struct directory* devicefs_mount(char* path, char* name, struct device* dev) {
    struct directory *dir, *ret;
    struct vfs_inode* v_inode;

    dir = find_path_dir_from_root(path);

    if(IS_ERR(dir)) 
        return ERR_PTR(dir);

    v_inode = _dfs_alloc_inode(dev);
    if(IS_ERR(v_inode))
        return ERR_PTR(v_inode);

    ret = create_dir_node(dir, v_inode, name);
    if(IS_ERR(ret))
        return ERR_PTR(ret);

    return ret;
}

void devicefs_init() {
    kmem_cache_init(&kmem_fs_allocator, sizeof(struct vfs_inode));
    spin_init(&v_inode_id_spin);
    v_inode_id = 0;
}