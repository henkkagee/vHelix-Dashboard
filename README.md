# vHelix Dashboard
This program is a tool for quickly creating DNA origami nanostructures from polyhedral 3D meshes based on the vHelix Autodesk Maya plugin by Björn Högberg and Johan Gardell at the lab of [Björn Högberg](http://www.hogberglab.net/) at Karolinska Insitutet.
Developed by Henrik Granö, [Natural computation](https://research.cs.aalto.fi/nc/) at Aalto University.

## Installation

a) Download the prebuilt binaries from http://www.vhelix.net (not yet available), or use the existing Win64 beta provided here.

b) To build the program on your system, you need the following dependencies:

- Qt 5.9.9 available from: https://www.qt.io/download
- Python 3.8 with Python/C API

Included modules and source code:
- [PhysXSDK 3.3]((https://github.com/yangzhengxing/PhysX-3.3). PhysXSDK 3.3 is used for physical relaxation DNA structures. It free software as of 2015 and its win64 binaries are included.
- [Lemon Graph library](https://lemon.cs.elte.hu/trac/lemon)
- [Boost C++ libraries](https://www.boost.org/)
- [bscor by Abdulmelik Mohammed (Aalto University)](https://github.com/mohamma1/bscor)
- [tacoxDNA](https://github.com/lorenzo-rovigatti/tacoxDNA),  (A. Suma, E. Poppleton, M. Matthies, P. Šulc, F. Romano, A. A. Louis, J. P. K. Doye, C. Micheletti and L. Rovigatti, ["TacoxDNA: A userfriendly web server for simulations of complex DNA structures, from single strands to origami"](https://doi.org/10.1002/jcc.26029), J. Comput. Chem. 40, 2586 (2019))
- [vHelix](http://www.vhelix.net)
- [objToPly by Nabeel Hussain](https://github.com/nabeel3133/file-converter-.obj-to-.ply)
- [oxDNAviewer](https://github.com/sulcgroup/oxdna-viewer)

If you use another platform than Win64 you need to recompile PhysXSDK 3.3 and Lemon for your system. You also need to compile the libraries on your own if you need a debug build.

For convenience, it is recommended to use Qt Creator for building: https://www.qt.io/download. If you use other tools, note that the directory structure may be slightly different: Qt Creator places the binary in a separate build folder beside src/.

The software is built using CMake to make cross-platform compilation possible. Configure the CMakeLists.txt file to find the specified Qt modules as well as Python.h and Python38.lib files for the Python/C API.

## Quick start

## Usage

## Detailed information

## Todo

- Automated strand gap filling
- .oxdna + .top + .vmeta DNA model importer
- Base estimation including filled in strand gaps (separately)
- Qt3D performance improvements (instancing/batching of entities)
- Bugfixes: Python API crashes, strand-, helix- and base memory use optimizations
