
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using std::string;

int main(int argc, char* argv[]) {
    // check check check
    if (argc < 2) {
        std::cerr << "No project name provided.\n";
        return 1;
    }

    string arg1 = argv[1];

    if (arg1 == "--help" || arg1 == "-h") {
        std::cout << "koiinit - tiny C++ project initializer\n\n";
        std::cout << "Usage:\n";
        std::cout << "  koiinit <project-name>\n";
        std::cout << "  koiinit <project-name> --cmake\n";
        std::cout << "  koiinit <project-name> --meson\n";
        std::cout << "  koiinit <project-name> --git\n";
        std::cout << "  koiinit <project-name> --meson --git\n";
        std::cout << "  koiinit <project-name> --cmake --git\n";
        std::cout << "  koiinit --help\n\n";
        std::cout << "Options:\n";
        std::cout << "  --cmake   Generate a CMake project instead of Makefile\n";
        std::cout << "  --meson   Generate a Meson project instead of Makefile\n";
        std::cout << "  --help    Show this help message\n";
        std::cout << "  --git     Initialize a Git repository\n";
        return 0;
    }

    bool use_cmake = false;
    bool use_git = false;
    bool use_meson = false;

    for (int i = 2; i < argc; i++) {
        string arg = argv[i];

        if (arg == "--cmake") {
            use_cmake = true;
        } else if (arg == "--git") {
            use_git = true;
        } else if (arg == "--meson") {
            use_meson = true;
        } else {
            std::cerr << "Unknown option: " << arg << '\n';
            return 1;
        }
    }

    if (use_meson && use_cmake) {
        std::cerr << "You cannot select both CMake and Meson.\n";
        return 1;
    }

    string projectname = argv[1];

    // Meson has a function named test so we cannot name the project name
    // test otherwise the compiler fails
    if (projectname == "test" && use_meson) {
        std::cerr << "'test' is reserved by Meson.\n";
        return 1;
    }

    // make the main file and variables.
    if (!std::filesystem::create_directory(projectname)) {
        std::cerr << "Error: Cannot create project directory '" << projectname
                  << "' (already exists).\n";
        return 1;
    }

    // Create project directory
    string srcdir = projectname + "/src/";
    std::filesystem::create_directory(srcdir);

    std::ofstream mainfile(projectname + "/src/main.cpp", std::ios::out | std::ios::noreplace);

    if (!mainfile) {
        std::cerr << "Cannot create main.cpp: file already exists or access denied.\n";
        return 1;
    }

    if (mainfile.is_open()) {
        mainfile << "#include <iostream>\n\n";
        mainfile << "int main() {\n";
        mainfile << "    std::cout << \"Hello, World!\\n\";\n";
        mainfile << "    return 0;\n";
        mainfile << "}\n";
    }

    // We create and open a cmake file to insert our text into it.
    if (use_cmake) {
        std::ofstream cmakefile(projectname + "/CMakeLists.txt",
                                std::ios::out | std::ios::noreplace);

        if (!cmakefile) {
            std::cerr << "Cannot create CMakeLists.txt: file already exists or access denied.\n";
            return 1;
        }

        if (cmakefile.is_open()) {
            cmakefile << "cmake_minimum_required(VERSION 3.16)\n";
            cmakefile << "project(" << projectname << " LANGUAGES CXX)\n\n";
            cmakefile << "set(CMAKE_CXX_STANDARD 23)\n";
            cmakefile << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
            cmakefile << "add_executable(" << projectname << " src/main.cpp)\n";
        }

    } else if (use_meson) {
        // I prob should make a helper to make these
        std::ofstream mesonfile(projectname + "/meson.build", std::ios::out | std::ios::noreplace);

        if (!mesonfile) {
            std::cerr << "Cannot create meson.build: file already exists or access denied.\n";
            return 1;
        }

        if (mesonfile.is_open()) {
            mesonfile << "project('" << projectname
                      << "', 'cpp', default_options: ['cpp_std=c++23'])\n\n";

            mesonfile << "executable('" << projectname << "', 'src/main.cpp', install: true)\n";
        }

    } else {
        std::ofstream makefile(projectname + "/Makefile", std::ios::out | std::ios::noreplace);

        if (!makefile) {
            std::cerr << "Cannot create Makefile: file already exists or access denied.\n";
            return 1;
        }

        if (makefile.is_open()) {
            makefile << R"(CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -O2
CPPFLAGS := -Isrc
LDFLAGS :=
LDLIBS :=

)";

            makefile << "TARGET := " << projectname << "\n";

            makefile << R"(SRC_DIR := src
BUILD_DIR := build

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS := $(OBJECTS:.o=.d)

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

.PHONY: all clean run install uninstall

-include $(DEPS)
)";
        }
    }

    std::ofstream readmefile(projectname + "/README.md", std::ios::out | std::ios::noreplace);

    if (!readmefile) {
        std::cerr << "Cannot create README.md: file already exists or access denied.\n";
        return 1;
    }

    if (readmefile.is_open()) {
        readmefile << "# " << projectname << "\n\n";
        readmefile << "<!--\n";
        readmefile << "This readme was autogenerated by koi.\n";
        readmefile << "Please edit it freely before uploading.\n";
        readmefile << "-->\n";
        // I decided to add the autogenerated message cuz why not?
    }

    // create the gitignore
    std::ofstream gitignore(projectname + "/.gitignore", std::ios::out | std::ios::noreplace);

    if (!gitignore) {
        std::cerr << "Cannot create gitignore: file already exists or access denied.\n";
        return 1;
    }

    if (gitignore.is_open()) {
        gitignore << "# Executable files\n";
        gitignore << "build/\n";
        gitignore << "/" << projectname << "\n";
        gitignore << ".cache/\n\n";
        gitignore << "# Editor files\n";
        gitignore << "*.o\n";
        gitignore << "*.a\n";
        gitignore << "*.so\n";
        gitignore << "compile_commands.json\n";
    }

    std::ofstream formatfile(projectname + "/.clang-format", std::ios::out | std::ios::noreplace);

    if (!formatfile) {
        std::cerr << "Cannot create .clang-format.\n";
        return 1;
    }

    formatfile << R"(BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 100
UseTab: Never
BreakBeforeBraces: Attach
PointerAlignment: Left
ReferenceAlignment: Left
SortIncludes: true
AllowShortFunctionsOnASingleLine: Empty
)";

    // initialize a empty git repo if --git is specified.
    if (use_git) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            char* args[] = {const_cast<char*>("git"),
                            const_cast<char*>("init"),
                            const_cast<char*>("-b"),
                            const_cast<char*>("main"),
                            projectname.data(),
                            nullptr};

            execvp("git", args);

            perror("execvp");
            _exit(127);
        }

        int status = 0;

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            std::cerr << "Git initialization failed.\n";
            return 1;
        }
    }

    // We check to see if its using cmake or meson to change the final print.
    if (use_cmake) {
        std::cout << "Created project '" << projectname << "' using CMake.\n";
    } else if (use_meson) {
        std::cout << "Created project '" << projectname << "' using Meson.\n";
    } else {
        std::cout << "Created project '" << projectname << "' using Make.\n";
    }

    return 0;
}
