# vHelix Dashboard
This program is a tool for quickly creating DNA origami nanostructures from polyhedral 3D meshes based on the vHelix Autodesk Maya plugin by Björn Högberg and Johan Gardell at the lab of [Björn Högberg](http://www.hogberglab.net/) at Karolinska Insitutet.
Developed by Henrik Granö, [Natural computation](https://research.cs.aalto.fi/nc/) at Aalto University.

## Installation

For Linux/MacOS instructions and code, use the `linux-version` branch.

a) Download the prebuilt binaries from http://www.vhelix.net (not yet available), or use the existing Win64 beta provided here.

b) To build the program on your system, you need the following dependencies:

- Qt 5.9.9 available from: https://www.qt.io/download
- Python 3.8 with Python/C API libraries and headers which are probably automatically installed

Included modules and source code:
- [PhysXSDK 3.3]((https://github.com/yangzhengxing/PhysX-3.3). PhysXSDK 3.3 is used for physical relaxation DNA structures. It free software as of 2015 and its win64 binaries are included.
- [Lemon Graph library](https://lemon.cs.elte.hu/trac/lemon)
- [Boost C++ libraries](https://www.boost.org/)
- [bscor by Abdulmelik Mohammed (Aalto University)](https://github.com/mohamma1/bscor)
- [tacoxDNA](https://github.com/lorenzo-rovigatti/tacoxDNA),  (A. Suma, E. Poppleton, M. Matthies, P. Šulc, F. Romano, A. A. Louis, J. P. K. Doye, C. Micheletti and L. Rovigatti, ["TacoxDNA: A userfriendly web server for simulations of complex DNA structures, from single strands to origami"](https://doi.org/10.1002/jcc.26029), J. Comput. Chem. 40, 2586 (2019))
- [vHelix](http://www.vhelix.net)
- [objToPly by Nabeel Hussain](https://github.com/nabeel3133/file-converter-.obj-to-.ply)
- [oxDNAviewer](https://github.com/sulcgroup/oxdna-viewer)
- [json by Niels Lohmann](https://github.com/nlohmann/json)

If you use another platform than Win64 you need to recompile PhysXSDK 3.3 and Lemon for your system. You also need to compile the libraries on your own if you need a debug build.

For convenience, it is recommended to use Qt Creator for building: https://www.qt.io/download. If you use other tools, note that the directory structure may be slightly different: Qt Creator places the binary in a separate build folder beside src/.

The software is built using CMake to make cross-platform compilation possible. Configure the CMakeLists.txt file to find the specified Qt modules as well as Python.h and Python38.lib files for the Python/C API. To build, run the CMakeLists.txt file with CMake or open it in Qt Creator. In runtime, all the Python files (as seen in /src or the win64_beta_release for reference) and PhysX .dlls should be beside the vHelix Dashboard executable.

## Quick start

Workflow/pipeline for generating a sequence and model from a .ply or .obj model:

- Import a .ply or .obj file from File->Import
- Click on the .ply file you imported in the workspace window (lower left pane) to select it, and route it from Route->Atrail
- If the routing is successful, the last line that appeard in the console pane should verify that you got a valid A-trail in the meshes
- With the same .ply still selected, click Relaxation->PhysX. Here, you can estimate scaffold strand base usage based on the scaling value. Once you have scaled the model appropriately, you can run the relaxation. It may take a while, and in some cases with very short edges (small scaling value) it may run forever. The relaxation is ready when the console pane updates and an .rpoly file appears in the workspace.
- The .rpoly file can be converted to oxDNA file format from Export->Export selection to oxDNA, or double clicked to generate and view the full model in the 3D graphics pane on the right. This can take a while depending on you hardware.
- if you want to view the model in oxDNA format, there is a shortcut to a local oxDNAviewer from oxDNA->Open oxDNA viewer. You can drag and drop .conf and .top files or an .oxview file directly in the oxDNAviewer window to load them.
- Once the 3D model appears on the right (3D performance not yet optimized), you can add a sequence to the scaffold strand from Edit->Add sequence, and after this you can export the strands as a .csv file from Export->Export strand sequences.
- To export the generated model, click File->Save current model to save it in .oxview format usable in oxView

## Todo

- Automated strand gap filling
- base rotation correction in .oxview export
- Base estimation including filled in strand gaps (separately)
- Qt3D performance improvements
- Bugfixes: Python API crashes, strand-, helix- and base memory use optimizations
- Code cleanup
