cmake_minimum_required(VERSION 3.5)

project(vHelix LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( LIBRARIES
       python38
       debug PhysX3DEBUG_x64 optimized PhysX3_x64
       debug PhysX3CommonDEBUG_x64 optimized PhysX3Common_x64
       debug PhysX3ExtensionsDEBUG optimized PhysX3Extensions
       debug PhysXProfileSDKDEBUG optimized PhysXProfileSDK
       debug PhysX3CharacterKinematicDEBUG_x64 optimized PhysX3CharacterKinematic_x64
       debug PhysX3CookingDEBUG_x64 optimized PhysX3Cooking_x64
       debug PhysXVisualDebuggerSDKDEBUG
       #debug lemond
       )
# ----------------- You need to config this for your own setup so the linker can  --------------------
# ------------------------------ find your Python38 installation ------------------------------------
project(root)
include_directories(${root_SOURCE_DIR}/include)
# Here:
include_directories(C:/Python38/include)
link_directories(${root_SOURCE_DIR}/lib)
# And here
link_directories(C:/Python38/libs)
link_directories(${root_SOURCE_DIR}/scaffold_routing_rectification)
add_subdirectory(scaffold_routing_rectification/)
add_subdirectory(include/json)
#add_executable(lemon ${root_SOURCE_DIR})
#target_link_libraries(lemon)

# turn off json library tets
set(JSON_BuildTests OFF CACHE INTERNAL "")

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED 3DCore 3DExtras 3Drender 3DInput)

add_executable(vHelix
    # main files
    main.cpp
    mainwindow.cpp
    mainwindow.h
    console.cpp
    console.h
    dirview.h
    dirview.cpp
    mainwindow.ui
    dialog.ui
    rpolydialog.ui
    vhelix.cpp
    vhelix.h
    processio.h
    processio.cpp
    graphicsview.h
    graphicsview.cpp
    modelhandler.h
    modelhandler.cpp
    model.h
    model.cpp
    dna.h
    dna.cpp
    sequencedialog.ui
    utils.cpp
    utils.h
    instructions.ui

    # BSCOR
    ply_to_dimacs/ply_to_dimacs.hpp
    ply_to_dimacs/ply_to_dimacs.cpp
    ply_to_dimacs/boost_graph_helper.hpp
    ply_to_dimacs/boost_graph_helper.cpp
    ply_to_embedding/ply_to_embedding.hpp
    ply_to_embedding/ply_to_embedding.cpp
    postman_tour/postman_tour.hpp
    postman_tour/postman_tour.cpp
    make_embedding_eulerian/make_embedding_eulerian.hpp
    make_embedding_eulerian/make_embedding_eulerian.cpp
    atrail_search/Atrail_search.hpp
    atrail_search/Atrail_search.cpp
    atrail_search/Atrail.hpp
    atrail_search/Atrail.cpp
    atrail_verify/Atrail_verify.hpp
    atrail_verify/Atrail_verify.cpp

    # Relaxation code
    scaffold_routing_rectification/DNA.h
    scaffold_routing_rectification/ParseSettings.h
    scaffold_routing_rectification/SimulatedAnnealing.h
    scaffold_routing_rectification/Relaxmain.h
    scaffold_routing_rectification/Relaxmain.cpp
    scaffold_routing_rectification/Definition.h
    scaffold_routing_rectification/Definition.cpp
    scaffold_routing_rectification/Helix.h
    scaffold_routing_rectification/Helix.cpp
    scaffold_routing_rectification/Physics.h
    scaffold_routing_rectification/Physics.cpp
    scaffold_routing_rectification/Scene.h
    scaffold_routing_rectification/Scene.cpp
    scaffold_routing_rectification/Utility.h
    scaffold_routing_rectification/Utility.cpp

    # Stylesheet by Alexander Huszagh
    stylesheet/breeze.qrc

    # Modified vHelix maya plugin source code
    # Not used
    include/mayahelix/Definition.h
)


# Force compiler flags from PhysX3 libraries to multithreading MT (MT_StaticRelease)
set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

set_property(TARGET vHelix PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

target_link_libraries(vHelix PRIVATE Qt5::Widgets Qt5::3DCore Qt5::3DExtras
    Qt5::3DRender Qt5::3DInput
    ${LIBRARIES}
    nlohmann_json::nlohmann_json
    #lemon
)
# Automatically copy resources to build directory.
#[[add_custom_command(TARGET vHelix POST_BUILD
                   # Directories

                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${root_SOURCE_DIR}/d3/ $<TARGET_FILE_DIR:vHelix>/d3
                   #COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/gamepads/ $<TARGET_FILE_DIR:vHelix>/gamepads
                   #COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/geometryloaders/ $<TARGET_FILE_DIR:vHelix>/geometryloaders/
                   #COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/iconengines/ $<TARGET_FILE_DIR:vHelix>/iconengines/
                   #COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/imageformats/ $<TARGET_FILE_DIR:vHelix>/imageformats/
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${root_SOURCE_DIR}/libs/ $<TARGET_FILE_DIR:vHelix>/libs/
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${root_SOURCE_DIR}/objToPly/ $<TARGET_FILE_DIR:vHelix>/objToPly/
                   # COMMAND ${CMAKE_COMMAND} -E copy_directory ${root_SOURCE_DIR}/sceneparsers/ $<TARGET_FILE_DIR:vHelix>/sceneparsers/
                   # COMMAND ${CMAKE_COMMAND} -E copy_directory ${root_SOURCE_DIR}/translations/ $<TARGET_FILE_DIR:vHelix>/translations/
                   # Python files
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/base.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/cadnano_utils.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/constants.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/convert.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/pdb.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/quaternion.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/reader_lammps_init.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/readers.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/rpoly_oxDNA.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/topology.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/utils.py $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/base.py $<TARGET_FILE_DIR:vHelix>
                   # Library files
                   # COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/D3DCompiler.dll $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/lib/glut32.dll $<TARGET_FILE_DIR:vHelix>
                   # COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/libEGLd.dll $<TARGET_FILE_DIR:vHelix>
                   # COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/libGLESV2d.dll $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/lib/PhysX3_x64.dll $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/lib/PhysX3CharacterKinematic_x64.dll $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/lib/PhysX3Common_x64.dll $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/lib/PhysX3CommonDEBUG_x64.dll $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/lib/PhysX3Cooking_x64.dll $<TARGET_FILE_DIR:vHelix>
                   # Text files
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/LICENSE.txt $<TARGET_FILE_DIR:vHelix>
                   COMMAND ${CMAKE_COMMAND} -E copy ${root_SOURCE_DIR}/instructions.txt $<TARGET_FILE_DIR:vHelix>

)
]]
