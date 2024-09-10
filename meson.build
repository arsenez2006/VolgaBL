project('VolgaBL', 'c', version: run_command('git', 'describe', '--tags', check: true).stdout().strip())

# Bootloader version
bl_version = meson.project_version()

# Import common library
subdir('bootloader/common')
