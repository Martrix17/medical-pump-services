from conan import ConanFile


class MedicalPumpServicesConan(ConanFile):
    name = "medical-pump-services"
    version = "0.1.0"

    settings = "os", "compiler", "build_type", "arch"

    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("gtest/1.14.0")
        self.requires("nlohmann_json/3.11.3")
        self.requires("asio/1.36.0")
        
    def layout(self):
        self.folders.generators = "generators"
        