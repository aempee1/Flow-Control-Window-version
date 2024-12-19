
# FlowControlDesktop - Window Version

This is the Window version of the FlowControlDesktop application. Follow the steps below to set up the project on your machine.

## Steps to Install the Project (Windows):

### 1. Clone the Project
Clone the repository from GitHub:
```bash
git clone https://github.com/aempee1/FlowControlDesktop-Window-version-.git
```

### 2. Install vcpkg
Download and install [vcpkg](https://github.com/microsoft/vcpkg) by following the instructions on their GitHub page.

### 3. Install Required Packages
Use `vcpkg` to install the necessary dependencies:
```bash
vcpkg install wxwidgets boost libmodbus
```

### 4. Edit Path in `CMakeLists.txt`
Edit the `CMakeLists.txt` file in the project to match the paths on your machine. Specifically, update the paths for `vcpkg`, `wxWidgets`, `NanoSVG`, and any other necessary paths.

For example:
```cmake
set(CMAKE_TOOLCHAIN_FILE "path_to_your_vcpkg/scripts/buildsystems/vcpkg.cmake")
set(wxWidgets_DIR "path_to_your_vcpkg/installed/x64-windows/share/wxwidgets")
set(NanoSVG_DIR "path_to_your_vcpkg/installed/x64-windows/share/nanosvg")
```

### 5. Make Build Directory
Create a directory for building the project:
```bash
mkdir build
```

### 6. Run CMake
Navigate to the `build` directory and run the `cmake` command:
```bash
cd build
cmake ..
```

### 7. Install CMake (if not installed)
If you don't have CMake installed, you can download and install it from the official website: [CMake Download](https://cmake.org/download/).

### 8. Open the Project in Visual Studio
In the `build` directory, you should see a `.sln` file. Open this file in Visual Studio to begin development.

### 9. Finish
You are now ready to start developing and running the project on your machine.

## Troubleshooting
If you encounter any issues, ensure that all the required dependencies are properly installed and that the paths in `CMakeLists.txt` are correct.

For additional help, refer to the documentation of the dependencies or open an issue in the GitHub repository.
