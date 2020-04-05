cmake_minimum_required(VERSION 3.6)

project(pypvrtex)

set(EXECUTABLE_NAME "pypvrtex")

option(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)
set(ENABLE_STATIC ON "Build static libraries" FORCE)
set(ENABLE_SHARED FALSE "Build shared libraries" FORCE)

find_package (Python 3 COMPONENTS Interpreter Development REQUIRED)
find_package(PythonLibs 3 REQUIRED)

message("Python_FOUND:${Python_FOUND}")
message("Python_VERSION:${Python_VERSION}")
message("Python_Development_FOUND:${Python_Development_FOUND}")
message("Python_LIBRARIES:${Python_LIBRARIES}")
message("PYTHON_LIBRARY_DIR:${PYTHON_LIBRARY_DIR}")

include_directories(${Python_INCLUDE_DIRS} )

##############################################################
# Setup compiler options and configurations
##############################################################

if(MSVC)
    set(COMMON_OPTIONS /W3 /wd4996 /we4239 /we4002 /we4700 /we4305 /EHsc /MP)
    set(DEBUG_OPTIONS /MTd /Od /Zi)
    set(RELEASE_OPTIONS /MT /Ox /fp:fast /GL /GR-)
elseif(APPLE)
    set(COMMON_OPTIONS -DHAVE_PTHREAD -DGUID_CFUUID -lstdc++fs)
    set(DEBUG_OPTIONS -g)
    set(RELEASE_OPTIONS -Ofast )
else()
    set(COMMON_OPTIONS -DHAVE_PTHREAD  -lstdc++fs)
    set(DEBUG_OPTIONS -g)
    set(RELEASE_OPTIONS -O2)
endif()

set(DEBUG_OPTIONS ${DEBUG_OPTIONS} ${COMMON_OPTIONS})
set(RELEASE_OPTIONS ${RELEASE_OPTIONS} ${COMMON_OPTIONS})

add_compile_options(
        "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>"
        "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /INCREMENTAL:NO")

    add_definitions(-D__STDC_LIMIT_MACROS)
    add_definitions(-D__STDC_FORMAT_MACROS)
    add_definitions(-D__STDC_CONSTANT_MACROS)

    add_definitions(-DHAVE_SSE42)

    add_definitions(-D_HAS_EXCEPTIONS=0)
    add_definitions(-D_SCL_SECURE)
    add_definitions(-D_SECURE_SCL)
    add_definitions(-D_ALLOW_KEYWORD_MACROS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(-DWINVER=0x0602)
    add_definitions(-D_WIN32_WINNT=0x0602)
    add_definitions(-DNTDDI_VERSION=0x06020000)
endif()

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")
    endif ()
else ()
    set (CMAKE_CXX_STANDARD 14)
endif ()

set (CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS "-fPIC -g  ${CMAKE_C_FLAGS}")
# set (CMAKE_CXX_FLAGS "-fPIC -g -fno-strict-aliasing -fno-common -dynamic -Os -pipe -fwrapv  -Wall -DENABLE_DTRACE ${CMAKE_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS "-fPIC -g  -msse2 -msse3 -msse4 -mpopcnt -funsafe-math-optimizations ${CMAKE_CXX_FLAGS}")

##############################################################
# Includes
##############################################################
include_directories(3dparty/pybind11/include)
include_directories(3dparty/PVRTexTool/Include)
include_directories(${PYTHON_INCLUDE_DIR})
include_directories(sources)
include_directories("${PROJECT_BINARY_DIR}")
link_directories("${PYTHON_LIBRARY_DIR}")
link_directories("3dparty/PVRTexTool/Linux_x86_64")
link_directories("3dparty/PVRTexTool/Windows_x86_64")
link_directories("3dparty/PVRTexTool/OSX_x86")

# Main sources
##############################################################
file(GLOB_RECURSE SOURCES sources/*.cpp sources/*.h sources/*.c sources/*.cc)

##############################################################
# Targets
##############################################################
add_library(pypvrtex SHARED ${SOURCES})

##############################################################
# Linkage
##############################################################
target_link_libraries(pypvrtex PUBLIC PVRTexLib pthread ${Python_LIBRARIES})

SET_TARGET_PROPERTIES(pypvrtex PROPERTIES PREFIX "_")
