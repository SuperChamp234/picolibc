#
# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright © 2019 Keith Packard
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#

has_arm_semihost = false
has_semihost_test_flag = false
has_semihost_args = false
has_semihost = true

src_semihost = []

subdir('fake')

machine_dir = 'machine' / host_cpu_family
if fs.is_dir(machine_dir)
  subdir(machine_dir)
endif

if has_arm_semihost
  subdir('common')
elif has_semihost
  # Non-ARM semihosting (e.g. Hexagon): add common/ to the include path so
  # that <semihost.h> is findable, and compile the sys_semihost_* wrappers
  # from common/ that are shared across all semihosting architectures.
  # ARM-only sources (sys_elapsed, sys_exit_extended, sys_feature, sys_tickfreq)
  # are excluded. common/times.c is included — SYS_ELAPSED (0x30) is handled
  # in the machine bridge (hexagon_sys_semihost.c) to return ENOSYS safely.
  inc = [inc, include_directories('common')]
  inc_args += '-I' + meson.current_source_dir() / 'common'
  if really_install
    install_headers('common/semihost.h', install_dir : include_dir)
  endif
  src_semihost += files([
    'common/sys_clock.c',
    'common/sys_close.c',
    'common/sys_errno.c',
    'common/sys_exit.c',
    'common/sys_flen.c',
    'common/sys_get_cmdline.c',
    'common/sys_getc.c',
    'common/sys_heapinfo.c',
    'common/sys_iserror.c',
    'common/sys_istty.c',
    'common/sys_open.c',
    'common/sys_putc.c',
    'common/sys_read.c',
    'common/sys_remove.c',
    'common/sys_rename.c',
    'common/sys_seek.c',
    'common/sys_system.c',
    'common/sys_time.c',
    'common/sys_tmpnam.c',
    'common/sys_write.c',
    'common/sys_write0.c',
    'common/times.c',
  ])
endif

foreach params : targets
  target = params['name']
  target_dir = params['dir']
  target_c_args = params['c_args']

  _src = get_variable('src_semihost_' + target, src_semihost)

  if _src == []
    _src = src_semihost_fake
  endif

  instdir = join_paths(lib_dir, target_dir)

  libsemihost_name = 'semihost'

  if meson.version().version_compare('>=1.10')
    _lib = static_library(libsemihost_name,
			  _src,
                          build_subdir : target_dir,
			  install : really_install,
			  install_dir : instdir,
			  include_directories : inc,
			  c_args : target_c_args + c_args)
  else
    target_lib_prefix = params['lib_prefix']
    _lib = static_library(join_paths(target_dir, target_lib_prefix + libsemihost_name),
			  _src,
			  install : really_install,
			  install_dir : instdir,
			  include_directories : inc,
			  c_args : target_c_args + c_args)
  endif

  set_variable('lib_semihost' + target, _lib)
  
endforeach

