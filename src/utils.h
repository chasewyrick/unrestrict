#include <common.h>
#include <sys/param.h>

#ifndef _UTILS_H
#define _UTILS_H

#define PROC_PIDPATHINFO_MAXSIZE    (4*MAXPATHLEN)
extern int proc_pidpath(pid_t pid, void *buffer, uint32_t buffersize);

char *get_path_for_pid(pid_t pid);

#endif /* _UTILS_H */
