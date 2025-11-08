from conan import ConanFile
from conan.tools.cmake import cmake_layout


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("zlib/1.3.1", override=True)
        self.requires("openssl/3.4.1", override=True)
        self.requires("paho-mqtt-cpp/1.5.3")
        self.requires("spdlog/1.15.3")
        self.requires("libcurl/8.12.1")
        self.requires("nlohmann_json/3.12.0")
        self.requires("ixwebsocket/11.4.5")
        self.requires("tz/2023c")
        # self.requires("inja/3.4.0")
        self.requires("boost/1.84.0")
        self.requires("protobuf/5.27.0")
        self.requires("sqlite_orm/1.9.1")

    def configure(self):
        self.options["boost"].without_stacktrace = False
        self.options["boost"].with_stacktrace_backtrace = True
        self.options["sqlite_orm"].SQLITE_ORM_ENABLE_CXX_17 = True

    def layout(self):
        cmake_layout(self)
