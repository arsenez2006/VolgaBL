# Generate a config
config_data = configuration_data()
config_data.set('version', bl_version)
configure_file(input : 'config' / 'version.h.in',
               output : 'version.h',
               configuration : config_data)

# Include directories
config_include = include_directories('.')
common_include = include_directories('include')

# Make a dependency
common_dep = declare_dependency(include_directories: [ config_include, common_include ])
