# MeshViewer
A 3D viewer application written in C++ using OpenGL. All math support is provided by my other project [3dmath](https://github.com/mdh81/3dmath)

# Build

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

```

# Capabilities

The viewer supports the following features:

* Loading STL and PLY files
* Rendering triangle meshes as wireframe or shaded
* Orbiting the camera around cardinal axes
* Zoom
* Gradient backgrounds
* Built-in event handler that supports static and dynamic callbacks 
* Arcball rotation (WIP in branch arcball)
* Emscripten builds

<img width="1021" alt="Screenshot 2023-06-03 at 7 34 25 PM" src="https://github.com/mdh81/meshviewer/assets/73474502/db2c053d-deea-44f6-bd8d-eb2c4fc23540">

# Future plans

In order of priority:

* Add support for various render modes: flat, smooth, wireframe
* Add support for an interactive orientation marker that allows view to be oriented along any axis. Preferably using a virtual sphere like manipulator
* Add support for more realistic lighting models
* A sketch plane...  á la blender
* "Show me how" mode to deconstruct view and projection transformations
* "Show me how" mode to deconstruct arcball interaction
* Annotate mode that allows picking points and transforming the pick results through various systems
* A simple UI that's written in OpenGL that allows exercising these various features
