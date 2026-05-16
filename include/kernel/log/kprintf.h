#ifndef _TOYOS_LOG_VFPRINTF_H
#define _TOYOS_LOG_VFPRINTF_H

#include <kernel/base/rio_queue.h>
#include <kernel/def.h>

int vfprintf(struct rio_buf_queue* buf, const char * fmt, va_list ap);
int fprintf(struct rio_buf_queue* buf, const char *fmt, ...);
char get_print_buf();
int kprintf(const char* fmt, ...);
#endif