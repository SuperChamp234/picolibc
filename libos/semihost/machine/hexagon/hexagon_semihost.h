/*
Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
SPDX-License-Identifier: BSD-3-Clause-Clear

Redistribution and use in source and binary forms, with or without
modification, are permitted (subject to the limitations in the
disclaimer below) provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

  * Neither the name of Qualcomm Technologies, Inc. nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _HEXAGON_SEMIHOST_H_
#define _HEXAGON_SEMIHOST_H_

#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/* System call codes — single source of truth used for both the enum and
 * the sys_semihost() validity check. Add new opcodes here only. */
#define HEXAGON_SYSCALL_CODES(X) \
    X(SYS_OPEN,        0x01) \
    X(SYS_CLOSE,       0x02) \
    X(SYS_WRITEC,      0x03) \
    X(SYS_WRITE0,      0x04) \
    X(SYS_WRITE,       0x05) \
    X(SYS_READ,        0x06) \
    X(SYS_READC,       0x07) \
    X(SYS_ISERROR,     0x08) \
    X(SYS_ISTTY,       0x09) \
    X(SYS_SEEK,        0x0a) \
    X(SYS_FLEN,        0x0c) \
    X(SYS_TMPNAM,      0x0d) \
    X(SYS_REMOVE,      0x0e) \
    X(SYS_RENAME,      0x0f) \
    X(SYS_CLOCK,       0x10) \
    X(SYS_TIME,        0x11) \
    X(SYS_SYSTEM,      0x12) \
    X(SYS_ERRNO,       0x13) \
    X(SYS_GET_CMDLINE, 0x15) \
    X(SYS_HEAPINFO,    0x16) \
    X(SYS_EXIT,        0x18) \
    X(SYS_FTELL,       0x100) \
    X(SYS_FSTAT,       0x101) \
    X(SYS_STAT,        0x103) \
    X(SYS_GETCWD,      0x104) \
    X(SYS_ACCESS,      0x105) \
    X(SYS_OPENDIR,     0x180) \
    X(SYS_CLOSEDIR,    0x181) \
    X(SYS_READDIR,     0x182) \
    X(SYS_EXEC,        0x185) \
    X(SYS_FTRUNC,      0x186)

#define _HEXAGON_ENUM_ENTRY(name, val) name = val,
enum hexagon_system_call_code {
    HEXAGON_SYSCALL_CODES(_HEXAGON_ENUM_ENTRY)
};

/* Software interrupt */
#define SWI "trap0 (#0)"

/* Hexagon semihosting calls */
int  flen(int fd);
int  hexagon_ftell(int fd);
int  get_cmdline(char *buffer, int count);

int  hexagon_semihost(enum hexagon_system_call_code code, int *args);

void hexagon_semihost_errno(int err);
enum {
    HEX_EPERM = 1,
    HEX_ENOENT = 2,
    HEX_EINTR = 4,
    HEX_EIO = 5,
    HEX_ENXIO = 6,
    HEX_EBADF = 9,
    HEX_EAGAIN = 11,
    HEX_ENOMEM = 12,
    HEX_EACCES = 13,
    HEX_EFAULT = 14,
    HEX_EBUSY = 16,
    HEX_EEXIST = 17,
    HEX_EXDEV = 18,
    HEX_ENODEV = 19,
    HEX_ENOTDIR = 20,
    HEX_EISDIR = 21,
    HEX_EINVAL = 22,
    HEX_ENFILE = 23,
    HEX_EMFILE = 24,
    HEX_ENOTTY = 25,
    HEX_ETXTBSY = 26,
    HEX_EFBIG = 27,
    HEX_ENOSPC = 28,
    HEX_ESPIPE = 29,
    HEX_EROFS = 30,
    HEX_EMLINK = 31,
    HEX_EPIPE = 32,
    HEX_ERANGE = 34,
    HEX_ENAMETOOLONG = 36,
    HEX_ENOSYS = 38,
    HEX_ELOOP = 40,
    HEX_EOVERFLOW = 75,
};

struct __SYS_STAT {
    uint64_t dev;
    uint64_t ino;
    uint32_t mode;
    uint32_t nlink;
    uint64_t rdev;
    uint32_t size;
    uint32_t __pad1;
    uint32_t atime;
    uint32_t mtime;
    uint32_t ctime;
    uint32_t __pad2;
};

#define MAP_STAT(p, h)                        \
    do {                                      \
        memset((p), 0, sizeof(*(p)));         \
        (p)->st_dev = (h)->dev;               \
        (p)->st_ino = (h)->ino;               \
        (p)->st_mode = (h)->mode;             \
        (p)->st_nlink = (h)->nlink;           \
        (p)->st_rdev = (h)->rdev;             \
        (p)->st_size = (h)->size;             \
        (p)->st_atime = (uint32_t)(h)->atime; \
        (p)->st_mtime = (uint32_t)(h)->mtime; \
        (p)->st_ctime = (uint32_t)(h)->ctime; \
    } while (0)

/*
 * sys_semihost() is implemented in hexagon_sys_semihost.c using
 * HEXAGON_SYSCALL_CODES to validate opcodes before issuing trap0.
 */
uintptr_t sys_semihost(uintptr_t op, uintptr_t param);

#endif
