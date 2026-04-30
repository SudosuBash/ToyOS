#include <kernel/file/path.h>
#include <kernel/file/dir.h>
#include <kernel/stdlib.h>
#include <kernel/data_struct/htable.h>
#include <kernel/def.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/fault/error.h>

uint8_t path_is_root(char* path) {
    return strlen(path) == 1 && path[0] == '/';
}

char* path_file_name(char* path) {
    char* pend = path + strlen(path) - 1;
    while(*pend && *pend != '/') 
        pend--;
    return ++pend;
}

static struct directory* __find_path_dir(struct directory* dir, char* path, uint64_t hash) {
    struct linklist_head* curr;
    char* name = path_file_name(path);
    list_for_entry(&(dir->d_children), curr) {
        struct directory* d_curr = directory_of(curr);
        if(strcmp(name, d_curr->d_name))
            return d_curr;
    }
    return NULL;
}

struct directory* find_path_dir(struct directory* dir, char* path) {
    struct directory* root;
    char* pend = path + strlen(path);
    while(pend-path != 1 && *(pend-1) == '/')
        *(pend--) = 0;
    
    uint64_t hash = 0;
    for(char* p = path; p < pend; p++) 
        hash += (uint64_t) (*p);
    hash = hlist_calc_hash(hash);

    if((root = dir_cache_find(path, hash)) != NULL) 
        return root;
    
    for(char* p = path; p < pend; p++) {
        char fname[FILE_DIR_NAME_MAX] = {0};
        char* fp = fname;

        while(*p!='/') 
            *(fp++) = *(p++);

        if(!(root->d_flag & DIR_FLAG_LOADED)) {
            root->d_inode->f_op.vfs_load_subdir(root->d_inode, root); //加载目录到directory
            root->d_flag |= DIR_FLAG_LOADED;
        }

        if((root = __find_path_dir(root, path, hash)) == NULL) {
            return ERR_PTR(ENOEXT);
        }

        if(!(root->d_inode->f_flag & DIR_TYPE_DIR))
            return NULL;
    }

    dir_cache_add(hash, root);
    return root;
}