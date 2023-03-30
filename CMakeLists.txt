cmake_minimum_required( VERSION 3.6 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )



project( Stereorizer )

set( LIBS_DIR ${CMAKE_CURRENT_LIST_DIR}/libs)
set( IMGUI_DIR ${LIBS_DIR}/imgui )
set( NFD_DIR ${LIBS_DIR}/nativefiledialog )

find_package( OpenCV REQUIRED)
find_package( OpenCV REQUIRED )
find_package( OpenGL REQUIRED )
find_package( PythonLibs REQUIRED)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

set ( project_files
        src/main.cpp
        src/gui_functionality.cpp
        src/gui_image_panel.cpp
        src/gui_depth_panel.cpp
        src/header.h
        src/gui_result_panel.cpp
        src/midas.cpp
        src/Stereo.cpp
        src/Stereo.h
        src/Image.cpp
        src/Image.h
        src/Depth.cpp
        src/Depth.h)

set( imgui_files
        ${IMGUI_DIR}/imconfig.h
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.h
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
        )

set( imgui_impl_files
        ${IMGUI_DIR}/backends/imgui_impl_glfw.h
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        )
include_directories( ${IMGUI_DIR} )
include_directories( ${IMGUI_DIR}/backends )
include_directories( ${IMGUI_DIR}/examples )

set( nfd_files
        ${NFD_DIR}/src/include/nfd.h
        ${NFD_DIR}/src/common.h
        ${NFD_DIR}/src/nfd_common.c
        ${NFD_DIR}/src/nfd_common.h
        ${NFD_DIR}/src/nfd_win.cpp
        )
include_directories( ${NFD_DIR}/)
include_directories( ${NFD_DIR}/src/include)

set( gl3w
        ${LIBS_DIR}/gl3w/GL/gl3w.c
        ${LIBS_DIR}/gl3w/GL/gl3w.h
        ${LIBS_DIR}/gl3w/GL/glcorearb.h
        )
include_directories( ${LIBS_DIR}/gl3w )

set( icons
        ${LIBS_DIR}/IconsFontAwesome5.h
        ${LIBS_DIR}/pyhelper.h
        )

add_executable( Stereorizer ${imgui_files} ${imgui_impl_files} ${gl3w} ${nfd_files} ${icons} ${project_files} )

target_compile_definitions(Stereorizer PUBLIC TRACY_ENABLE=1 )
add_subdirectory(tracy)
include_directories(tracy/public/tracy)
add_library(tracy STATIC tracy/public/TracyClient.cpp)
target_link_libraries (Stereorizer Tracy::TracyClient )

# GLFW
set( GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set( GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${LIBS_DIR}/glfw)
target_link_libraries( Stereorizer glfw)
target_link_libraries( Stereorizer OpenGL::GL)
include_directories( ${LIBS}/glfw/include)

#include( ExternalProject )
#ExternalProject_Add(
#        glfw PREFIX ${LIBS_DIR}/glfw
#        GIT_REPOSITORY https://github.com/glfw/glfw.git
#        GIT_TAG 3.3
#        CMAKE_ARGS
#        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
#        "-DCMAKE_BUILD_TYPE=Debug"
#        "-DGLFW_BUILD_DOCS=OFF"
#        "-DGLFW_BUILD_EXAMPLES=OFF"
#        "-DGLFW_BUILD_TESTS=OFF"
#        "-DBUILD_SHARED_LIBS=OFF"
#)
#ExternalProject_Get_Property( glfw INSTALL_DIR )
#message(E: ${INSTALL_DIR})
#
#set( GLFW_DIR ${INSTALL_DIR} )
#set( GLFW_INCLUDE_DIR ${GLFW_DIR}/include )
#set( GLFW_LIBRARIES   ${GLFW_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX} )
#add_dependencies( Stereorizer glfw )



include_directories( ${OPENGL_INCLUDE_DIR} )

include_directories( ${PYTHON_INCLUDE_DIRS})

target_link_libraries( Stereorizer ${PYTHON_LIBRARIES})
target_link_libraries( Stereorizer ${GLFW_LIBRARIES} )
target_link_libraries( Stereorizer ${OpenCV_LIBS} )
target_link_libraries( Stereorizer ${OPENGL_LIBRARIES} )



file(COPY res DESTINATION .)
file(COPY MiDaS DESTINATION .)

list(APPEND PYTHONPATH "${CMAKE_CURRENT_DIR}/MiDaS")

add_custom_command(
        TARGET Stereorizer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/inference_depth.py
        ${CMAKE_CURRENT_BINARY_DIR}/inference_depth.py)

