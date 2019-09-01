cmake_minimum_required(VERSION 3.6)

# Release build is default, see https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()
# Allow only two build types for cmake-gui (and Visual Studio)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# project settings
set (project_name "raytracing")
project(${project_name})

if (UNIX)
    # for unix builds, we create a separate /bin dir, Visual Studio will create its own directories for each build type
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

# gather list of header and source files
file (GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file (GLOB_RECURSE H_FILES ${PROJECT_SOURCE_DIR}/include/*.h)

# create a file structure for Visual Studio
if (WIN32)
    foreach(src IN LISTS CPP_FILES)
        if (IS_ABSOLUTE "${src}")
            file(RELATIVE_PATH source_rel "${PROJECT_SOURCE_DIR}" "${src}")
        else()
            set(source_rel "${src}")
        endif()
        get_filename_component(source_path "${source_rel}" PATH)
        string(REPLACE "/" "\\" source_path_msvc "${source_path}")
        source_group("${source_path_msvc}" FILES "${source_rel}")
    endforeach()

    foreach(incl IN LISTS H_FILES)
        if (IS_ABSOLUTE "${incl}")
            file(RELATIVE_PATH incl_rel "${PROJECT_SOURCE_DIR}" "${incl}")
        else()
            set(incl_rel "${incl}")
        endif()
        get_filename_component(incl_path "${incl_rel}" PATH)
        string(REPLACE "/" "\\" incl_path_msvc "${incl_path}")
        source_group("${incl_path_msvc}" FILES "${incl_rel}")
    endforeach()
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")

#######################
## external libraries #
#######################

# header-only libs
include_directories("${PROJECT_SOURCE_DIR}/ext/glm")
include_directories("${PROJECT_SOURCE_DIR}/ext/stb")

set(additional_libraries "")

# locate headers and libs for SDL2
if (UNIX)
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    set(additional_libraries "${SDL2_LIBRARIES} ${additional_libraries}")
elseif (WIN32)
    # use included SDL2 version
    include_directories("${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/include")
    # Support 32 or 64 bit build
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
        set(additional_libraries "${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x64/SDL2.lib;${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x64/SDL2main.lib;${additional_libraries}")
        set(post_build_copy_list "${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x64/SDL2.dll;${post_build_copy_list}") 
    else ()
        set(additional_libraries "${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x86/SDL2.lib;${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x86/SDL2main.lib;${additional_libraries}")
        set(post_build_copy_list "${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x86/SDL2.dll;${post_build_copy_list}")
    endif ()
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

##########################
# end external libraries #
##########################

# define c++ version
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CCXX_STANDARD_11)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CCXX_STANDARD_REQUIRED ON)

# set other compiler options
if (UNIX)
    set(UNIX_DEBUG_COMPILE_FLAGS "-g -Wall -Werror --pedantic-errors")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${UNIX_DEBUG_COMPILE_FLAGS}")
endif()

# enable OpenMP support
#find_package(OpenMP REQUIRED)
#set(additional_libraries "${OpenMP_CXX_LIBRARIES};${additional_libraries}")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

# build the project from source and header files
add_executable (${project_name} ${CPP_FILES} ${H_FILES})

# define libraries
string(STRIP "${additional_libraries}" additional_libraries)
target_link_libraries (${project_name} ${additional_libraries})

# define binary dir
set_property(TARGET ${project_name} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# define c++ version
set_property(TARGET ${project_name} PROPERTY CCXX_STANDARD_11)
set_property(TARGET ${project_name} PROPERTY CCXX_STANDARD_REQUIRED ON)

# set other compiler options
if (UNIX)
    set(unix_debug_compile_flags "-g -Wall -Werror --pedantic-errors")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${unix_debug_compile_flags}")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    # we need pthreads for C++ 11 support on GCC
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
endif()

# post-build action: copy necessary files
if (WIN32)
    # for running from Visual Studio and from executable itself, 64-bit and 32-bit
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
        add_custom_command(TARGET ${project_name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x64/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2.dll)
        add_custom_command(TARGET ${project_name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x64/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/SDL2.dll)
    else()
        add_custom_command(TARGET ${project_name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x86/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2.dll)
        add_custom_command(TARGET ${project_name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/ext/sdl2/win32-vc/SDL2-2.0.10/lib/x86/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/SDL2.dll)
    endif()
endif()

