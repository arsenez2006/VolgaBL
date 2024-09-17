mbr = executable('mbr.bin', 'mbr.S',
                 c_args: [ c_args, c16_args ],
                 link_args : [
                   c_link_args,
                   c16_args,
                   '-Wl,--Map=' + (meson.current_build_dir() / 'mbr.map'),
                   '-Wl,-T' + (meson.current_source_dir() / 'mbr.ld')],
                 dependencies: common_dep)
