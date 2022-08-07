# Earthquake Simulator
SENG 475 Final project by Owen Jaques and Finn Morin - An Earthquake Simulation using Ragdoll Physics.

## Usage
Users first start in the build stage where they can place particles then connect them using joints. When they press the play button the simulation
starts. While the simulation is running or beforehand they are free to change the vertical and horizontal magnitude of the earthquake using the
buttons provided. They can pause the simulation at any time using the stop button to construct new stuctures or add to their existing ones.

![30 seconds of usage gif](/img/30s_usage.gif)

## Building, Installing, and Running
All the software for this project is designed to be built and run on UVic's UGLS machines using the SENG475
[SDE](https://ece.engr.uvic.ca/~frodo/courses/cpp/#sde). During the the development for this project, it was found that a one of those machines
(UGLS26) lacked the dependencies neccessary to build the software. Machines that are confirmed to work properly are UGLS7, and UGLS16. We also found
that UGLS9 and UGLS17 worked properly but were much slower running our program.

### Dependencies
This project uses these dependencies which are all found on the UGLS machines.

- OpenGL (v2.1)
- GLEW (v1.10.0)
- glfw3 (v3.2)
- Cairo (v1.15.12)
- GLIB (v2.0)
- GTK2 (v2.0)
- Pango (v1.42.3)
- CGAL (v5.4)

### Build and Install Commands
To build and install the software, simply execute the commands below. Let `$TOP_DIR` denote the top-level directory of the project software and let
`$INSTALL_DIR` denote the directory into which the software is to be installed.

```
cd $TOP_DIR
cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR -DCMAKE_BUILD_TYPE=Release
cmake --build build --clean-first --target install
```

To run a demonstration of the installed program execute the following command:

```
$INSTALL_DIR/bin/demo
```

### Development Build Commands
While developing it is often useful to build the software with address sanitizers and undefined behaviour sanitizers. It is also often useful to
build with debugging flags so the program can be used in a debugger. To build the program with those enabled execute the commands below.

```
cd $TOP_DIR
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=true -DENABLE_UBSAN=true
cmake --build build
```

This will create the program `earth` in the `$TOP_DIR/build` directory.

## Physics System
This project uses 'ragdoll physics' to simulate the shaking, falling, and general movement of whatever the user chooses to create on the screen.

### Ragdoll Physics
Ragdoll physics is fairly straightfoward, at a high level particles, which are subjected to gravity and external forces, are connected by joints
which simulate pseudo-rigid bodies. By moving one particle the others must move as well in order to maintain the length of their connecting joint. By
connecting several particles using joints in different shapes you can simulate realistic physics for a wide variety of things. 

One common way of implementing ragdoll physics uses Verlet integration, which is the method chosen for this project. Thomas Jakobsen describes the
algorithms and methods that were used to implement such a system for the game "Hitman: Codename 47" in his
[paper, "Advanced Character Physics"](http://graphics.cs.cmu.edu/nsp/course/15-869/2006/papers/jakobsen.htm) which was extremely helpful during the
development of the ragdoll physics system used for our project. While the paper does have a few code samples, they are not exhaustive and were not
used in the code for this project.

### Physics System Design
In an effort to promote reusability for other projects, the physics code used for the system was split into two sets of components. The hopefully
reusable set of components is the general ragdoll physics system. It was written in a generalized way to promote reusability for other simulations.
It is composed of three files, which contain three classes:
- [particle.hpp](/include/particle.hpp)
- [joint.hpp](/include/joint.hpp)
- [particle_system.hpp](/include/particle_system.hpp)

The other set of components contain all earthquake specific physics and is composed of just one file
[earthquake_system.hpp](/include/earthquake_system.hpp) containing one class. For example, it contains the method to shake the system up and down.
The class contains an instance of the `ParticleSystem` class which it configures with values specific to our earthquake simulation and which it uses
for all the underlying ragdoll physics.

## User Interface
The user interface is built with OpenGL (for rendering), GLFW (for window management and user input), and Pango+Cairo (for text rendering).

### Textures
To avoid dependence on 3rd party libraries textures are handled in a slightly questionable way defined below. Were this to not be for school with a
specific spec we would just use the stb library.
1.  [stb](https://github.com/nothings/stb/blob/master/stb_image.h) is used to load a PNG texture into memory
2.  Write this to disk as a `.texture` file and quit
3.  Read this file in the main program
4.  Continue as normal

This approach lets us use the nice parsing benefits of stb without any runtime or compiletime dependence on it.

### Text Rendering
Since OpenGL has no support for text rendering we needed a library to handle this for us. Initially we planned to use Qt for user input and text but
the version installed on the UGLS lab machines (v3 - 2004) was too old and lacked enough documentation for us to get it working. Instead a
combination of Pango and Cairo were used to render fonts. These libraries are shockingly up to date on the lab machines (2018) and had good
documentation. The code for this is based on [this tutorial](https://dthompson.us/font-rendering-in-opengl-with-pango-and-cairo.html) which was
extremely helpful for learning these libraries. While we do use some of their code for interfacing with Pango and Cairo it has been adapted for C++
and further optimized/mutilated/wrapped to meet our needs.

### User Input
User input is handled by GLFW's nice and simple mouse and keyboard callbacks. Buttons are rendered to the screen and their bounding boxes are checked
when a user clicks. For placing particles the mouse snaps to a 20x20 grid to (hopefully) make the building process less error prone.
