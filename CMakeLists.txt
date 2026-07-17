cmake_minimum_required(VERSION 3.15...4.0)
project(Alexio-Engine)

# Set configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)

# Make Release behave like RelWithDebInfo
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS_RELEASE    "${CMAKE_C_FLAGS_RELEASE} /Zi" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /Zi" CACHE STRING "" FORCE)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS_RELEASE    "${CMAKE_C_FLAGS_RELEASE} -g" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -g" CACHE STRING "" FORCE)
endif()

# Dist still stripped and optimized
set(CMAKE_C_FLAGS_DIST       "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DIST     "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG" CACHE STRING "" FORCE)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Editor)

# Compiler/runtime settings
if(MSVC)
    # Use static runtime
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Zi")
    set(CMAKE_CXX_FLAGS_DIST    "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG")
    
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG") # Debug and Release will still have the console
    set(CMAKE_EXE_LINKER_FLAGS_DIST "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") # Dist will have the console stripped
	
    add_compile_options(/utf-8)
else()
    # GCC/Clang
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")
    set(CMAKE_CXX_FLAGS_DIST    "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O3 -s")
endif()

# Add subprojects
add_subdirectory(Engine)
add_subdirectory(Sandbox)
add_subdirectory(Editor)