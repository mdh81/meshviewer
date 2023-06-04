# MeshViewer
A 3D viewer application written in C++ using OpenGL

# Capabilities

The viewer is its infancy. It supports:

* Loading STL files
* Rendering triangle meshes as wireframe or shaded
* Orbiting the camera around cardinal axes
* Zoom
* Built-in event handler that can register member/free functions as event handlers for keyboard and mouse events

<img width="1028" alt="Screen Shot 2022-08-28 at 2 46 33 PM" src="https://user-images.githubusercontent.com/73474502/187095797-da56a625-d4c1-45c9-b55b-1e92a4c3a5ab.png">


# Future plans

In order of priority:

* Add support for mouse and keyboard interaction to inspect the model. Arcball implementation is what is planned
* Add support for various render modes: flat, smooth, wireframe
* Add support for an interactive orientation marker that allows view to be oriented along any axis. Preferrably using a virtual sphere like manipulator
* Add support for more releastic lighting models
* A sketch plane...  á la blender
* "Show me how" mode to deconstruct view and projection transformations
* "Show me how" mode to deconstruct arcball interaction
* Annotate mode that allows picking points and transforming the pick results through various systems
* A simple UI that's written in OpenGL that allows exercising these various features
