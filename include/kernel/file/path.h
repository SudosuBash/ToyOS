#ifndef _TOYOS_KERN_FILE_PATH
#define _TOYOS_KERN_FILE_PATH

#include <kernel/file/file.h>

char* path_file_name(char* path);
struct directory* find_path_dir(struct directory* dir, char* path);
#endif
