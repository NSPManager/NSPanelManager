from conan import ConanFile


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("zlib/1.3", override=True)
        self.requires("paho-mqtt-cpp/1.2.0")
        self.requires("spdlog/1.12.0")
        self.requires("libcurl/8.2.1")
        self.requires("nlohmann_json/3.11.2")
        self.requires("ixwebsocket/11.4.3")
        self.requires("tz/2023c")
        self.requires("inja/3.4.0")
        self.requires("boost/1.83.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.22.6")

    def configure(self):
        self.options["boost"].without_stacktrace = False
        self.options["boost"].with_stacktrace_backtrace = True