# vHelix Dashboard
This program is a tool for quickly creating DNA origami nanostructures from polyhedral 3D meshes based on the vHelix Autodesk Maya plugin by Björn Högberg and Johan Gardell at the lab of [Björn Högberg](http://www.hogberglab.net/) at Karolinska Insitutet.
Developed by Henrik Granö, [Natural computation](https://research.cs.aalto.fi/nc/) at Aalto University.

## Installation

a) Download the prebuilt binaries from https://www.vhelix.net (not yet available)

b) To build the program on your system, you need the following dependencies:

- Qt 5.9.9 available from: https://www.qt.io/download
- Python 3.8 with Python/C API

PhysXSDK 3.3 is free software as of 2015 and its binaries are included, if you use another platform you need to recompile these for your system: https://github.com/yangzhengxing/PhysX-3.3. You also need to compile the libraries on your own if you need a debug build.
For convenience, it is recommended to use Qt Creator for building: https://www.qt.io/download

Included modules and source code:
- [Lemon Graph library](https://lemon.cs.elte.hu/trac/lemon)
- [Boost C++ libraries](https://www.boost.org/)
- [bscor by Abdulmelik Mohammed (Aalto University)](https://github.com/mohamma1/bscor)
- [tacoxDNA](https://github.com/lorenzo-rovigatti/tacoxDNA),  (A. Suma, E. Poppleton, M. Matthies, P. Šulc, F. Romano, A. A. Louis, J. P. K. Doye, C. Micheletti and L. Rovigatti, ["TacoxDNA: A userfriendly web server for simulations of complex DNA structures, from single strands to origami"](https://doi.org/10.1002/jcc.26029), J. Comput. Chem. 40, 2586 (2019))
- [vHelix](https://www.vhelix.net)
- [objToPly by Nabeel Hussain](https://github.com/nabeel3133/file-converter-.obj-to-.ply)
- [oxDNAviewer](https://github.com/sulcgroup/oxdna-viewer)

Configure the .cmake file to find the specified Qt modules as well as Python.h and Python38.lib files for the Python/C API.

## Quick start

## Usage

## Detailed information

## Todo

- Automated strand gap filling
- .oxdna + .top + .vmeta DNA model importer
- 3D performance improvements (instancing/batching of entities)
- Bugfixes