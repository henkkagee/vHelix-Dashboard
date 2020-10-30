# vHelix Dashboard
This program is a tool for quickly creating DNA origami nanostructures from polyhedral 3D meshes based on the vHelix Autodesk Maya plugin by Björn Högberg and Johan Gardell at the lab of [Björn Högberg](http://www.hogberglab.net/) at Karolinska Insitutet.
Developed by Henrik Granö, [Natural computation](https://research.cs.aalto.fi/nc/) at Aalto University.

## Installation

a) Download the prebuilt binaries from http://www.vhelix.net (not yet available), or use the existing Win64 beta provided here.

b) To build the program on your system, you need the following dependencies:

- Qt 5.9.9 available from: https://www.qt.io/download
- Python 3.8 with Python/C API headers and libraries included, which is usually the case in regular installations

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

### Detailed Linux / MacOS installation instructions

This is for a build using shared Qt library objects, which is somewhat straightforward but the resulting executable can not directly be run on other systems due to dependency issues. For static builds, you need to build Qt static libraries with the `-static` flag in Qt's ./configure, and probably Python3.8.a with `-fPIE` compiler flag. Getting everything to link properly with static linking in the end is rather problematic.

- Install CMake, g++ and Python 3.8.

- Build Lemon using their instructions https://lemon.cs.elte.hu/trac/lemon/wiki/InstallLinux . This is rather straightforward and results in a libemon.a library file.

- Build PhysX-3.3. There are configuration files (Makefiles, Visual Studio .sln files...) for each operating system in PhysXSDK/Source/compiler. If you get warnings treated as errors during the build process, remove all -Werror flags from all .mk files in source/compiler/<system, e.g. linux64>. The compiling process generates lots of warnings which probably are not feasible to fix yourself. You should end up with shared objects in PhysXSDK/Bin and static libraries in PhysXSDK/Lib.

- Clone the vHelix-Dashboard repository, and switch to the linux-version branch. Place the libemon.a and the .a and .so files from PhysXSDK/Lib and PhysXSDK/Bin in src/vHelix/lib.

- Configure the CMakeLists.txt file in src/vHelix to find your python header files (e.g. usr/include/python3.8) and your python libraries (libpython 3.8.a and libpython3.8.so usually in usr/lib/python3.8).

- Download an installer for Qt 5.9.9 from http://download.qt.io/official_releases/qt/5.9/5.9.9/ or build it yourself from http://download.qt.io/official_releases/qt/5.9/5.9.9/single/. *Note!* For some Linux distros with Qt5 packages, Qt3D and/or Qt3DExtras with Qt3DExtrasConfig.cmake is not included which causes trouble when building. Consider downloading the above installer or building from source if this seems to be a problem.

Now, either open the CMakeLists.txt file from QtCreator or run it with CMake. If you get errors related to multiple definitions of Dialog.h, you need to do the following in the vHelix_autogen/include folder generated by CMake:

- Change line 24 from "class Ui_Dialog" to "class Ui_RpolyDialog" and line 104 from "class Dialog: public Ui_Dialog" to "class RpolyDialog: public Ui_RpolyDialog {}"
- Change line 25 from "class Ui_Dialog" to "class Ui_SequenceDialog" and line 128 from "class Dialog: public Ui_Dialog" to "class SequenceDialog: public Ui_SequenceDialog {}"

This is because .ui files for user interfaces are automatically incorrectly generated into header files, this inconvenience will be fixed soon.

Once CMake succeeds in generating the Makefile, run it with `make` or build the project in Qt Creator.

## Quick start

Workflow/pipeline for generating a sequence and model from a .ply or .obj model:

- Import a .ply or .obj file from File->Import
- Click on the .ply file you imported in the workspace window (lower left pane) to select it, and route it from Route->Atrail
- If the routing is successful, the last line that appeard in the console pane should verify that you got a valid A-trail in the meshes
- With the same .ply still selected, click Relaxation->PhysX. Here, you can estimate scaffold strand base usage based on the scaling value. Once you have scaled the model appropriately, you can run the relaxation. It may take a while, and in some cases with very short edges (small scaling value) it may run forever. The relaxation is ready when the console pane updates and an .rpoly file appears in the workspace.
- The .rpoly file can be converted to oxDNA file format from Export->Export selection to oxDNA, or double clicked to generate and view the full model in the 3D graphics pane on the right. This can take a while depending on you hardware.
- if you want to view the model in oxDNA format, there is a shortcut to a local oxDNAviewer from oxDNA->Open oxDNA viewer. You can drag and drop the .conf and .top files directly in the oxDNAviewer window to load them.
- Once the 3D model appears on the right (performance is still rather poor), you can add a sequence to the scaffold strand from Edit->Add sequence, and after this you can export the strands as a .csv file from Export->Export strand sequences.
- To export the generated model, click File->Save current model to save it in oxDNA format with a meta file (.conf, .top and .vmeta). Importer not yet implemented...

## Todo

- Automated strand gap filling
- .oxdna + .top + .vmeta DNA model importer
- Base estimation including filled in strand gaps (separately)
- Qt3D performance improvements (instancing/batching of entities)
- Bugfixes: Python API crashes, strand-, helix- and base memory use optimizations
- Linux: Python modules not importing
