#ifndef _TOYOS_LOG_VFPRINTF_H
#define _TOYOS_LOG_VFPRINTF_H

#include <kernel/base/rio_queue.h>
#include <kernel/def.h>
#include <kernel/base/rio_broadcast.h>

void register_print_reader(struct rio_reader *reader);
int vfprintf(struct rio_broadcast* buf, const char *restrict fmt, va_list ap);
int fprintf(struct rio_broadcast* buf, const char *fmt, ...);
char get_print_buf(struct rio_reader* reader);
int kprintf(const char* fmt, ...);
void init_log();
#endif