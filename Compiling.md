# Linux

1. Install Dependencies
    * Archlinux: `pacman -s git make cmake gcc qt6-base`
    * Debian/Ubuntu/Mint: `apt install git make cmake g++  qt6-base-dev`
    * Fedora: `yum install git make cmake gcc qt6-qtbase-devel`
1. Initilize the repository
    1. `git clone https://github.com/Puxtril/TexViewer && cd TexViewer`
    1. Initilize the submodules
        1. `cd lib`
        1. `git submodule update --init --depth 1`
1. Compile
    1. Create a folder to compile (can be anywhere), then `cd` into it.
    1. `cmake <path_to_repo> -DCMAKE_BUILD_TYPE=Release`
    1. `cmake --build . -j8`
        - The `-j8` flag adds multi-threading

# Windows

1. Install vcpkg and Packages
    1. Open terminal in a directory to clone/install vcpkg. Binaries will be installed here.
    1. `git clone https://github.com/microsoft/vcpkg.git` and `cd vcpkg`
    1. Initilize vcpkg with `.\bootstrap-vcpkg.bat`
    1. Install Qt with `.\vcpkg.exe install qtbase`
1. Initilize the repository
    1. `git clone https://github.com/Puxtril/TexViewer && cd TexViewer`
    1. Initilize the submodules
        1. `cd lib`
        1. `git submodule update --init --depth 1`
1. Compile
    1. Create a folder to compile (can be anywhere), then `cd` into it.
    1. `cmake <path_to_repo> -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=<vcpkg_install_dir>\scripts\buildsystems\vcpkg.cmake`
    1. `cmake --build . -j8`
        - The `-j8` flag adds multi-threading
