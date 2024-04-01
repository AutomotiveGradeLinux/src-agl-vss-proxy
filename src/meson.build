cpp = meson.get_compiler('cpp')
grpcpp_reflection_dep = cpp.find_library('grpc++_reflection')

proxy_dep = [
    dependency('glib-2.0'),
    dependency('openssl'),
    dependency('threads'),
    dependency('yaml-cpp'),
    dependency('libmosquitto'),
    dependency('protobuf'),
    dependency('grpc'),
    dependency('grpc++'),
    grpcpp_reflection_dep
]

protoc = find_program('protoc')
grpc_cpp = find_program('grpc_cpp_plugin')

kuksa_protos_base_dir = get_option('protos')
kuksa_protos_dir = kuksa_protos_base_dir / 'kuksa/val/v1'
kuksa_protoc_gen = generator(protoc, \
                       output : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
                       arguments : ['-I=' + kuksa_protos_base_dir,
                         '--cpp_out=@BUILD_DIR@',
                         '@INPUT@'])

protoc_gen = generator(protoc, \
                       output : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
                       arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/../protos',
                         '--cpp_out=@BUILD_DIR@',
                         '@INPUT@'])

generated_protoc_sources = [ \
    kuksa_protoc_gen.process(kuksa_protos_dir / 'types.proto', preserve_path_from : kuksa_protos_base_dir),
    kuksa_protoc_gen.process(kuksa_protos_dir / 'val.proto', preserve_path_from : kuksa_protos_base_dir),
    protoc_gen.process('../protos/vss-notification.proto'),
]

kuksa_grpc_gen = generator(protoc, \
                     output : ['@BASENAME@.grpc.pb.cc', '@BASENAME@.grpc.pb.h'],
                     arguments : ['-I=' + kuksa_protos_base_dir,
                       '--grpc_out=@BUILD_DIR@',
                       '--plugin=protoc-gen-grpc=' + grpc_cpp.path(),
                       '@INPUT@'])
generated_grpc_sources = [ \
    kuksa_grpc_gen.process(kuksa_protos_dir / 'val.proto', preserve_path_from : kuksa_protos_base_dir),
]

src =  [
    'KuksaConfig.cpp',
    'KuksaClient.cpp',
    'MqttConfig.cpp',
    'MqttClient.cpp',
    'SignalUpdateHandler.cpp',
    'SignalUpdateHandlers.cpp',
    'ParseConfig.cpp',
    'ProxyService.cpp',
    'main.cpp',
    generated_protoc_sources,
    generated_grpc_sources,
]

executable('agl-vss-proxy',
           src,
           dependencies: proxy_dep,
           install: true,
           install_dir : get_option('sbindir'))
