from conans import ConanFile, CMake

class GrpccbConan(ConanFile):
    name = "grpc_cb"
    version = "0.1"
    license = "Apache-2.0"
    url = "https://github.com/jinq0123/grpc_cb"
    description = "C++ gRPC library with callback interface."
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    
    # conan remote add jinq0123 https://api.bintray.com/conan/jinq0123/test
    # before install, or create from:
    # https://github.com/jinq0123/conan-grpc
    requires = "gRPC/1.4.2@jinq0123/stable", "Protobuf/3.4.1@jinq0123/testing"
    
    generators = "cmake", "Premake"  # A custom generator: PremakeGen/0.1@memsharded/testing
    build_requires = "PremakeGen/0.1@memsharded/testing"
    exports_sources = "src*", "include*", "CMakeLists.txt"

    def build(self):
        cmake = CMake(self)
        self.run('cmake %s %s' % (self.source_folder, cmake.command_line))
        self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        self.copy("*.h")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["grpc_cb"]
