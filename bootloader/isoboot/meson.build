isoboot = executable(
  'isoboot.bin',
  'pre_init.S',
  c_args: [ c_args, c16_args ],
  link_args: [
    c_link_args,
    c16_args,
    '-Wl,--Map=' + (meson.build_root() / 'isoboot.map'),
    '-Wl,-T' + (meson.current_source_dir() / 'isoboot.ld')
  ],
  dependencies: [ common_dep ]
)
