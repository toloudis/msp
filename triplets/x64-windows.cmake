# The repository's x64-windows triplet, used in place of vcpkg's built-in one
# of the same name (vcpkg-configuration.json lists this folder in
# "overlay-triplets"). It is vcpkg's, plus one exception:
#
# spirv-tools is built static. With dynamic linkage the port installs both
# the static SPIRV-Tools.lib (which SPIRV-Tools-opt.lib, used by
# GraphicsVulkan, is built on) and SPIRV-Tools-shared.lib, a DLL's import
# library with the same functions. vcpkg's MSBuild integration links every
# library in the install into every executable, and linking both of those
# fails with LNK2005 (spvContextCreate already defined).
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

if(PORT STREQUAL "spirv-tools")
    set(VCPKG_LIBRARY_LINKAGE static)
endif()
