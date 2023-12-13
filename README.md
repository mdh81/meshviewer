# MeshViewer
A 3D viewer application written in C++ using OpenGL

# Capabilities

The viewer supports the following features:

* Loading STL and PLY files
* Rendering triangle meshes as wireframe or shaded
* Orbiting the camera around cardinal axes
* Zoom
* Gradient backgrounds
* Built-in event handler that can register member/free functions as event handlers for keyboard and mouse events
* Arcball rotation (WIP in branch arcball)
* Emscripten builds (WIP in branch emscripten)    

<img width="1021" alt="Screenshot 2023-06-03 at 7 34 25 PM" src="https://github.com/mdh81/meshviewer/assets/73474502/db2c053d-deea-44f6-bd8d-eb2c4fc23540">

# Future plans

In order of priority:

* Add support for various render modes: flat, smooth, wireframe
* Add support for an interactive orientation marker that allows view to be oriented along any axis. Preferrably using a virtual sphere like manipulator
* Add support for more releastic lighting models
* A sketch plane...  á la blender
* "Show me how" mode to deconstruct view and projection transformations
* "Show me how" mode to deconstruct arcball interaction
* Annotate mode that allows picking points and transforming the pick results through various systems
* A simple UI that's written in OpenGL that allows exercising these various features
