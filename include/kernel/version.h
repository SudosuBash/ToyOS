#ifndef _TOYOS_KERNEL_VERSION
#define _TOYOS_KERNEL_VERSION

struct kern_version {
    const char* version;
    const char* version_number;
    const char* build_time;
    const char* platform;
    const char* maintainer;
};
void log_version_info();
#endif