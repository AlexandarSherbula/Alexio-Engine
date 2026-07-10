# Alexio Engine

Alexio Engine is a work in progress lightweight 2D Game Engine for experimentation, and small‑scale projects.

This project is actively evolving and serves both as a personal engine and a long‑term exploration of rendering, ECS, input, asset management, and engine architecture.

![Alexio-Engine](/Resources/engine-demo.gif?raw=true "Demonstration")

# How to Build (WINDOWS ONLY)
Currently Engine is supported only on Windows. Linux support will be added eventually.

## Prerequisites:
- GPU that supports OpenGL 4.5 or DirectX11
- Git
- CMake 3.15
- Visual Studio (IDE that was tested on. It should work on other compilers too, but proceed with caution)

## Clone the repository

Start off with cloning the repository:

`git clone --recursive https://github.com/AlexandarSherbula/Alexio-Engine`

Use `git submodule update --init` if submodules were not clone in repository.

## Setup the build

Within the repository there is a `cmake_build.bat` file. It runs the command `cmake -S . -B build ` on your command prompt.

Within the repository, the 'build' folder was generated and inside you will find the build files (In case of Visual Studio, the .sln file along with .vcxproj for each module Engine has)

# Features
At the moment, the Engine is a bare minumum C++ framework for developing games or any other interactive applications. The current features include:

- Window: Support for all Window related events (Resizing, Moving, Minimizing and Closing)
- Input: Support for Keyboard, Mouse and Gamepads
- Rendering: Support for drawing basic shapes (Lines, Quads and Circles), along with Texture support for both OpenGL and DirectX11
- Audio: Support for playing .wav, .ogg, .mp3 and .flac files

# Roadmap
These are current broad plans for future additions to the engine, so this is subject to be changed.

- Create a level editor that will be for designing maps, scenes and game design altogether.
- Add a scripting support to be use for developing games.
