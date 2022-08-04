# Earthquake Simulator
SENG 475 Final project by Owen Jaques and Finn Morin.

## Physics System
This project uses 'ragdoll physics' to simulate the shaking, falling, and general movement of whatever the user chooses to create on the screen.

### Ragdoll Physics
Ragdoll physics is fairly straightfoward, at a high level particles, which are subjected to gravity and external forces, are connected by joints
which simulate pseudo-rigid bodies. By moving one particle the others must move as well in order to maintain the length of their connecting joint. By
connecting several particles using joints in different shapes you can simulate realistic physics for a wide variety of things. 

One common way of implementing ragdoll physics uses Verlet integration, which is the method chosen for this project. Thomas Jakobsen describes the
algorithms and methods that were used to implement such a system for the game 'Hitman: Codename 47' in his
[paper](http://graphics.cs.cmu.edu/nsp/course/15-869/2006/papers/jakobsen.htm) which was extremely helpful in developing the physics system used for 
our project.

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

## Credit
Rendering text is painful in OpenGL, and even worse on UVic's OpenGL 2.1 (released 2004). Because of this we could not get Qt 3 (Also released 2004)
to work and had to resort to other UI libraries. We settled on Cairo and Pango for rendering text as the libraries are shockingly up to date on the
UVic systems (both ~2018). The code for this is based on [this tutorial](https://dthompson.us/font-rendering-in-opengl-with-pango-and-cairo.html)
which is an awesome tutorial for getting this up and running. It is not a direct copy however, as it has been adapted for C++ and optimized to avoid
many unnecessary calls to pango and cairo's APIs.

As mentioned above the [paper](http://graphics.cs.cmu.edu/nsp/course/15-869/2006/papers/jakobsen.htm) on ragdoll physics using Verlet integration by
Thomas Jakobsen was extremely helpful in developing the ragdoll physics system used for our project. Part of the code for the general ragdoll physics
system implements algorithms and methods descibed in that paper. While the paper does have a few code samples, they are not exhaustive and were not
used in the code for this project.
