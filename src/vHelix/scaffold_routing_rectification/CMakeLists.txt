message("-- Linking PxPhysics libraries")
add_library(PXPHYSICS PhysX3_x64.dll PhysX3Common_x64.dll nvToolsExt64_1.dll PxFoundation_x64.dll PxPvdSDK_x64.dll glut32.dll PhysX3CharacterKinematic_x64.dll PhysX3Cooking_x64.dll)
set_target_properties(PXPHYSICS PROPERTIES LINKER_LANGUAGE CXX)