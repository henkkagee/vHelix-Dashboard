message("-- Linking PxPhysics libraries")
set(LIBRARIES
    debug PhysX3DEBUG_x64 optimized PhysX3_x64
    debug PhysX3CommonDEBUG_x64 optimized PhysX3Common_x64
    debug PhysX3CharacterKinematicDEBUG_x64 optimized PhysX3CharacterKinematic_x64
    debug PhysX3CookingDEBUG_x64 optimized PhysX3Cooking_x64
    )
add_library(PXPHYSICS PhysX3_x64.dll PhysX3Common_x64.dll PhysX3CharacterKinematic_x64.dll PhysX3Cooking_x64.dll nvToolsExt64_1.dll PxFoundation_x64.dll PxPvdSDK_x64.dll glut32.dll)
set_target_properties(PXPHYSICS PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "DEBUG")
