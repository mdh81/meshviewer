# MeshViewer
A 3D viewer application written in C++ using OpenGL. All math support is provided by my other project [3dmath](https://github.com/mdh81/3dmath)

[![Build and Quality Check](https://github.com/mdh81/meshviewer/actions/workflows/cicd.yml/badge.svg)](https://github.com/mdh81/meshviewer/actions/workflows/cicd.yml)

<img width="1021" alt="Screenshot 2023-06-03 at 7 34 25 PM" src="https://github.com/mdh81/meshviewer/assets/73474502/db2c053d-deea-44f6-bd8d-eb2c4fc23540">

# Build & Run

```shell
# Clone repo along with 3dmath submodule
$ git clone --recurse-submodules ssh://git@github.com:443/mdh81/meshviewer.git meshviewer
```

## Desktop

### Dependencies

Install these dependencies before attempting to build the app 

* GLFW
* GLEW
* OpenGL
* GLM (Soon to be replaced)

```shell
# Configure desktop build
$ cd meshviewer
$ mkdir <build_directory>
$ cd <build_directory>
$ cmake -S ../ -B . -DCMAKE_BUILD_TYPE=<Release|Debug> # Build type defaults to Debug

# Build
$ make -j<n> #n is number of parallel jobs

# Run
$ ./meshViewer testfiles/suzanne_subdivided.stl
```

## Web

```shell
# Configure emscripten build
$ cd meshviewer
$ mkdir <build_directory>
$ cd <build_directory>
$ emcmake cmake -S ../ -B . -DCMAKE_BUILD_TYPE=<Release|Debug> -DEMSCRIPTEN=On 

# Build
$ make -j<n> #n is number of parallel jobs

# Run
$ emrun --port <PORT> --browser <chrome|firefox|safari> meshViewer.html testfiles/suzanne_subdivided.stl

Alternatively, run ./embuild.sh <-t Release|Debug> <-b chrome|firefox|safari>
```
