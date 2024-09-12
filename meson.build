project('VolgaBL', 'c', version: run_command('git', 'describe', '--tags', check: true).stdout().strip())

# Compile arguments
c_args = [
  '-Wall',
  '-Wpedantic',
  '-ansi',
  '-fno-builtin',
  '-ffreestanding',
  '-fno-pic',
  '-fno-pie',
  '-fno-common']
c_link_args = [
  c_args,
  '-nostartfiles',
  '-nodefaultlibs',
  '-nolibc',
  '-nostdlib',
  '-s',
  '-static',
  '-Wl,--oformat=binary']
c16_args = [
  '-march=i386',
  '-m16']

# Bootloader version
bl_version = meson.project_version()

# Import common library
subdir('bootloader/common')

# Import MBR
subdir('bootloader/mbr')
