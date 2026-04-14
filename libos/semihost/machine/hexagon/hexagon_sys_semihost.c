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

/*
 * sys_semihost() — bridge between the common semihost public API and the
 * Hexagon trap interface, used by libos/semihost/common/sys_*.c.
 *
 * Opcodes not in HEXAGON_SYSCALL_CODES are reserved or ARM-specific.
 * Issuing trap0 with an unknown opcode is undefined behavior per the spec
 * and will typically crash the simulator. Return -1/ENOSYS instead.
 *
 * HEXAGON_SYSCALL_CODES is the single source of truth for valid opcodes;
 * adding a new opcode to the X-macro in hexagon_semihost.h automatically
 * allows it here.
 *
 * Special case: SYS_EXIT uses direct-passing on Hexagon (exit code in r2),
 * not the r01-array interface used by the ARM common layer. The common
 * sys_semihost_exit() passes an ARM exception code as the param; we map
 * ADP_Stopped_ApplicationExit to exit(0) and treat everything else as
 * exit(1), then call _exit() directly.
 */

#include "hexagon_semihost.h"
#include <errno.h>
#include <unistd.h>
#include <semihost.h>

uintptr_t
sys_semihost(uintptr_t op, uintptr_t param)
{
    if (op == SYS_EXIT) {
        _exit(param == ADP_Stopped_ApplicationExit ? 0 : 1);
    }

    if (op == SYS_SEEK) {
        /*
         * The Hexagon spec returns the newly set offset on success, but the
         * common sys_semihost_seek() caller and the ARM convention expect 0
         * on success and -1 on error. Normalize here.
         */
        int ret = hexagon_semihost(SYS_SEEK, (int *)param);
        return (ret == -1) ? (uintptr_t)-1 : 0;
    }

    /*
     * SYS_ELAPSED (0x30) and SYS_TICKFREQ (0x31) are RESERVED in the
     * Hexagon semihosting spec — issuing trap0 with these opcodes is
     * undefined behavior and will typically crash the simulator.
     * Return -1/ENOSYS so callers (e.g. common/times.c) degrade gracefully.
     */
    if (op == 0x30 || op == 0x31) {
        errno = ENOSYS;
        return (uintptr_t)-1;
    }

#define _HEXAGON_CHECK_CODE(name, val) \
    if (op == (val))                   \
        return (uintptr_t)hexagon_semihost((enum hexagon_system_call_code)op, \
                                           (int *)param);
    HEXAGON_SYSCALL_CODES(_HEXAGON_CHECK_CODE)
#undef _HEXAGON_CHECK_CODE
    errno = ENOSYS;
    return (uintptr_t)-1;
}
