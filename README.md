# Magi-tracer, a Simple Ray Tracer in C w/SDL

This repository contains the code for a simple ray tracer I've been writing in C using SDL as a wrapper for OpenGL.
Instead of directly using OpenGL, I use SDL's OpenGL capabilities (only the simple placement of a pixel) to create
3D graphics using a ray tracing algorithm. While it's probably more efficient to use OpenGL shading language, this has
mostly been an exercise in learning. I wanted to learn how ray tracing works by starting with the simplest graphical
capabilities possible, the placement of a pixel. Below is a screenshot of a render from Magi-tracer:

![Magi-tracer Render](https://github.com/Neuro-mancer/Magi-Tracer/blob/main/docs/raytraced.png?raw=true "Magi-Tracer")

## What is Ray Tracing?

The idea of the ray tracing is quite simple in theory: imagine you are trying to paint a scene that is in front of you.
You can divide your canvas into little equally-sized squares. You can then map these squares and have them correspond to your viewpoint.
The color of the square on your canvas is determined by whatever color you see the most from the square in your viewpoint.

Now in real life, the rays of light reflected come from light sources; however,
the origin point for our rays in this program come from the viewpoint of the camera. There is quite a bit of linear algebra
involved in determining the colors reflected back from surfaces and also some knowledge of optics to understand the nature 
of light and model it in a fashion that's realistic. That being said, it's still relatively straightforward if you have a 
solid understanding of the math involved and a decent grasp on programming.

Ray tracing has been around for a long time, although only recently have people started to harness it in real-time 
applications such as games. This is because it's pretty computationally expensive and hardware has only now started
catering to this use.

## Magi-tracer Features
### Current Features

- Written in C and SDL
- Will compile and run on GNU/Linux (Maybe Windows? Have to test.)
- Diffuse lighting
- Specular reflection
- Supports sphere primitives

### Planned Features

- Shadows and reflections
- Ability to reposition the camera
- Multithreading, subsampling, caching, and general optimization of algorithm
- Support for additional primitives
- Transparency and Refraction

## Installation

### Dependencies

The dependencies are as follows:

`SDL2, GCC, Make`
