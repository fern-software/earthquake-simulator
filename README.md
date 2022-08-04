# Earthquake Simulator
SENG 475 Final project by Owen Jaques and Finn Morin.

## Physics Used
The physics enginge developed for theused for this project

## Credit
Rendering text is painful in OpenGL, and even worse on UVic's OpenGL 2.1 (released 2004). Because of this we could not get Qt 3 (Also released 2004) to work and had to resort to other UI libraries.
We settled on Cairo and Pango for rendering text as the libraries are shockingly up to date on the UVic systems (both ~2018)
The code for this is based on https://dthompson.us/font-rendering-in-opengl-with-pango-and-cairo.html which is an awesome tutorial for getting this up and running.
It is not a direct copy however, as it has been adapted for C++ and optimized to avoid many unnecessary calls to pango and cairo's APIs.
