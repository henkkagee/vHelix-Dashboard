cmake_minimum_required(VERSION 3.5)

project(vHelix LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Qt53DExtras_DIR "/home/Qt599/5.9.9/gcc_64/lib/cmake/Qt53DExtras")
set(Qt5Core_DIR "/home/Qt599/5.9.9/gcc_64/lib/cmake/Qt5Core")
set(Qt5_DIR "/home/Qt599/5.9.9/gcc_64/lib/cmake/Qt5")
set(QT_QMAKE_EXECUTABLE "/home/Qt599/5.9.9/gcc_64/bin/qmake")

#set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu")

# ----------------- You need to config this for your own setup so the linker can  --------------------
# ------------------------------ find your Python38 installation ------------------------------------
project(root)
include_directories(${root_SOURCE_DIR}/include)
# Here:
include_directories(/usr/include/python3.8)
link_directories(${root_SOURCE_DIR}/lib)
# And here
link_directories(/usr/lib/python3.8/config-3.8-x86_64-linux-gnu)
link_directories(${root_SOURCE_DIR}/scaffold_routing_rectification)
add_subdirectory(scaffold_routing_rectification/)
#add_executable(lemon ${root_SOURCE_DIR})
#target_link_libraries(lemon)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED 3DCore 3DInput 3DRender 3DExtras)

if(ANDROID)
  add_library(vHelix SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    console.cpp
    console.h
    mainwindow.ui
    vhelix.cpp
    vhelix.h
  )
else()
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
    graphicsview.h
    graphicsview.cpp
    rpolyparser.h
    rpolyparser.cpp
    model.h
    model.cpp
    dna.h
    dna.cpp
    sequencedialog.ui

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

    # Modified vHelix maya plugin source code
    # Not used
    # -- Headers --
    include/mayahelix/Definition.h
    #[[
    include/mayahelix/ApplySequence.h
    include/mayahelix/ApplySequenceGui.h
    include/mayahelix/BackboneArrow.h
    include/mayahelix/Connect.h
    include/mayahelix/CreateCurves.h
    include/mayahelix/CreatorGui.h
    include/mayahelix/Disconnect.h
    include/mayahelix/Definition.h
    include/mayahelix/DNA.h
    include/mayahelix/Duplicate.h
    include/mayahelix/ExportStrands.h
    include/mayahelix/ExtendGui.h
    include/mayahelix/ExtendStrand.h
    include/mayahelix/FillStrandGaps.h
    include/mayahelix/FindFivePrimeEnds.h
    include/mayahelix/Helix.h
    include/mayahelix/HelixBase.h
    include/mayahelix/JSONTranslator.h
    include/mayahelix/Locator.h
    include/mayahelix/opengl.h
    include/mayahelix/OxDnaTranslator.h
    include/mayahelix/PaintStrand.h
    include/mayahelix/RetargetBase.h
    include/mayahelix/RoutedMeshTranslator.h
    include/mayahelix/StrandLengthCount.h
    include/mayahelix/TargetHelixBaseBackward.h
    include/mayahelix/TextBasedTranslator.h
    include/mayahelix/ToggleCylinderBaseView.h
    include/mayahelix/ToggleLocatorRender.h
    include/mayahelix/ToggleShowSuggestedConnections.h
    include/mayahelix/Tracker.h
    include/mayahelix/Utility.h
    include/mayahelix/Creator.h
    include/mayahelix/model/Base.h
    include/mayahelix/model/Color.h
    include/mayahelix/model/Helix.h
    include/mayahelix/model/Material.h
    include/mayahelix/model/Object.h
    include/mayahelix/model/Strand.h
    include/mayahelix/model/Space.h
    include/mayahelix/controller/ApplySequence.h
    include/mayahelix/controller/TextBasedImporter.h
    include/mayahelix/controller/Connect.h
    include/mayahelix/controller/CreateCurves.h
    include/mayahelix/controller/Disconnect.h
    include/mayahelix/controller/Duplicate.h
    include/mayahelix/controller/ExportStrands.h
    include/mayahelix/controller/ExtendStrand.h
    include/mayahelix/controller/FillStrandGaps.h
    include/mayahelix/controller/JSONImporter.h
    include/mayahelix/controller/OxDnaExporter.h
    include/mayahelix/controller/OxDnaImporter.h
    include/mayahelix/controller/PaintStrand.h
    include/mayahelix/controller/Operation.h
    include/mayahelix/controller/RoutedMeshImporter.h
    include/mayahelix/controller/StrandLengthCount.h
    include/mayahelix/data/BackBoneArrow.h
    include/mayahelix/json/json.h
    include/mayahelix/json/json-forwards.h
    include/mayahelix/view/BaseShape.h
    include/mayahelix/view/BaseShapeUI.h
    include/mayahelix/view/ConnectSuggestionsContext.h
    include/mayahelix/view/ConnectSuggestionsContextCommand.h
    include/mayahelix/view/ConnectSuggestionsLocatorNode.h
    include/mayahelix/view/ConnectSuggestionsToolCommand.h
    include/mayahelix/view/HelixShape.h
    include/mayahelix/view/HelixShapeUI.h
    # -- .cpp
    mayahelix/ApplySequence.cpp
    mayahelix/ApplySequenceGui.cpp
    mayahelix/Connect.cpp
    mayahelix/CreateCurves.cpp
    mayahelix/Creator.cpp
    mayahelix/CreatorGui.cpp
    mayahelix/Disconnect.cpp
    mayahelix/DNA.cpp
    mayahelix/Duplicate.cpp
    mayahelix/ExportStrands.cpp
    mayahelix/ExtendGui.cpp
    mayahelix/ExtendStrand.cpp
    mayahelix/FillStrandGaps.cpp
    mayahelix/FindFivePrimeEnds.cpp
    mayahelix/Helix.cpp
    mayahelix/HelixBase.cpp
    mayahelix/jsoncpp.cpp
    mayahelix/JSONTranslator.cpp
    mayahelix/Locator.cpp
    mayahelix/main.cpp
    mayahelix/opengl.cpp
    mayahelix/OxDnaTranslator.cpp
    mayahelix/PaintStrand.cpp
    mayahelix/RetargetBase.cpp
    mayahelix/RoutedMeshTranslator.cpp
    mayahelix/StrandLengthCount.cpp
    mayahelix/StrandLengthCount.cpp
    mayahelix/TargetHelixBaseBackward.cpp
    mayahelix/TextBasedTranslator.cpp
    mayahelix/ToggleCylinderBaseView.cpp
    mayahelix/ToggleLocatorRender.cpp
    mayahelix/ToggleShowSuggestedConnections.cpp
    mayahelix/Tracker.cpp
    mayahelix/Utility.cpp
    mayahelix/controller/TextBasedImporterController.cpp
    mayahelix/controller/ApplySequenceController.cpp
    mayahelix/controller/ConnectController.cpp
    mayahelix/controller/CreateCurvesController.cpp
    mayahelix/controller/DisconnectController.cpp
    mayahelix/controller/DuplicateController.cpp
    mayahelix/controller/ExportStrandsController.cpp
    mayahelix/controller/ExtendStrandController.cpp
    mayahelix/controller/FillStrandGapsController.cpp
    mayahelix/controller/JSONImporterController.cpp
    mayahelix/controller/OxDnaExporterController.cpp
    mayahelix/controller/OxDnaImporterController.cpp
    mayahelix/controller/PaintStrandController.cpp
    mayahelix/controller/RoutedMeshImporterController.cpp
    mayahelix/controller/StrandLengthCountController.cpp
    mayahelix/model/ObjectModel.cpp
    mayahelix/model/BaseModel.cpp
    mayahelix/model/ColorModel.cpp
    mayahelix/model/HelixModel.cpp
    mayahelix/model/MaterialModel.cpp
    mayahelix/model/StrandModel.cpp
    mayahelix/view/BaseShape.cpp
    mayahelix/view/BaseShapeUI.cpp
    mayahelix/view/ConnectSuggestionsContext.cpp
    mayahelix/view/ConnectSuggestionsContextCommand.cpp
    mayahelix/view/ConnectSuggestionsLocatorNode.cpp
    mayahelix/view/ConnectSuggestionsToolCommand.cpp
    mayahelix/view/double_arrow.cpp
    mayahelix/view/HelixShape.cpp
    mayahelix/view/HelixShapeUI.cpp]]
  )
endif()

#find_library(LEMON_LIB lemon.lib HINTS ${root_SOURCE_DIR})
#if( ${LEMON_LIB} STREQUAL "LEMON_LIB-NOTFOUND")
#    message("Please set LEMON_LIB variable to location of lemon.lib")
#endif()

#FIND_PATH(LEMON_SOURCE_ROOT_DIR CMakeLists.txt
#  PATHS ${CMAKE_SOURCE_DIR}/lemon ${CMAKE_SOURCE_DIR}/deps/lemon
#  NO_DEFAULT_PATH
#  DOC "Location of LEMON source as a CMAKE subproject")
#
#IF(EXISTS ${LEMON_SOURCE_ROOT_DIR})
#  ADD_SUBDIRECTORY(${LEMON_SOURCE_ROOT_DIR} deps/lemon)
#  SET(LEMON_INCLUDE_DIRS
#    ${LEMON_SOURCE_ROOT_DIR}
#    ${CMAKE_BINARY_DIR}/deps/lemon
#  )
#  SET(LEMON_LIBRARIES lemon)
#  UNSET(LEMON_ROOT_DIR CACHE)
#  UNSET(LEMON_DIR CACHE)
#  UNSET(LEMON_INCLUDE_DIR CACHE)
#  UNSET(LEMON_LIBRARY CACHE)
#ELSE()
#  FIND_PACKAGE(LEMON QUIET NO_MODULE)
#  FIND_PACKAGE(LEMON REQUIRED)
#ENDIF()


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

target_link_libraries(vHelix PRIVATE Qt5::Widgets
    Qt5::3DInput Qt5::3DRender Qt5::3DExtras python3.8
    PhysX3_x64 PhysX3Common_x64 PhysX3Extensions  PhysXProfileSDK
    PhysX3CharacterKinematic_x64 PhysX3Cooking_x64
    )
# lemon   python38
