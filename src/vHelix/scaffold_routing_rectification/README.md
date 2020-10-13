scaffold-routing-rectification
==============================

Rectification of a calculated scaffold routing using NVIDIA PhysX.

To run the program, the following files must be placed in the same directory:
*scaffold-routing-rectification.exe found in x64/Release/
*PhysX3_x64.dll, PhysX3Common_x64.dll found in scaffold-routing-rectification/

The program takes the following input arguments:

Usage: scaffold-routing-rectification.exe
        --input=<filename>
        --output=<filename>
        [ --scaling=<decimal> ]
	[ --discretize_lengths=<true|false> ]
        [ --density=<decimal> ]
        [ --spring_stiffness=<decimal> ]
        [ --fixed_spring_stiffness=<decimal> ]
        [ --spring_damping=<decimal> ]
        [ --attach_fixed=<true|false> ]
        [ --static_friction=<decimal> ]
        [ --dynamic_friction=<decimal> ]
        [ --restitution=<decimal> ]
        [ --rigid_body_sleep_threshold=<decimal> ]
        [ --visual_debugger=<true|false> ]

Usually, the rectification is run as:


scaffold-routing-rectification.exe --input=inputfile.rmsh --output=outputfile.rpoly

The visual_debugger is only available when doing a debug build. For this to work, use the scaffold-routing-rectification.exe from x64/Debug/ and PhysX3CHECKED_x64.dll, PhysX3CommonCHECKED_x64.dll from scaffold-routing-rectification/. These require Visual Studio 2013 to be installed for debug builds of the Visual C++ Runtime.

To use the visual_debugger, start NVIDIA PhysX Visual Debugger *before* the scaffold-routing-rectification.exe.

Currently, lengths are discretized taking into account their routing. To disable this, use --discretize_lengths=false

vHelix
==============================

To import the resulting rpoly into vHelix (http://vhelix.net/), in Autodesk Maya click File > Import... then pick any rpoly file.

The importer takes additional arguments, these can be given together with the MEL command for importing files. The two valid arguments are "nicking_min_length" and "nicking_max_length".
nicking_min_length sets the minimum length an edge must be in order for it to be nicked. nicking_max_length decides how many times the accumulated non-nicked strand will be divided.

An example usage of the file command:
file -options "nicking_min_length=28;nicking_max_length=28" -import -type "Text based vhelix" -ra true -mergeNamespacesOnClash false -namespace "bent_rod"  -pr "C:/Users/Johan/Documents/Clones/scaffold-routing-rectification/bent_rod.rpoly";
