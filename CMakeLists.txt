cmake_minimum_required( VERSION 3.6 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )



project( Stereorizer )

set( LIBS_DIR ${CMAKE_CURRENT_LIST_DIR}/libs)
set( NFD_DIR ${LIBS_DIR}/nativefiledialog )

find_package( OpenCV REQUIRED)
find_package( OpenGL REQUIRED )
find_package( PythonLibs REQUIRED)

set(APP_ICON_RESOURCE_WINDOWS "res/icon.rc")

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
        src/Depth.h
        src/VRController.cpp
        src/VRController.h

        src/VR/camera.h
        src/VR/shader.h
        src/VR/vr.cpp
        src/VR/vr.h src/VR/ThreadSafeMat.h)

# IMGUI
set( IMGUI_DIR ${LIBS_DIR}/imgui-docking )
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
        ${LIBS_DIR}/imgui-bezier/bezier.hpp
        ${LIBS_DIR}/imgui-bezier/bezier.cpp
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
include_directories( ${LIBS_DIR}/imgui-bezier )

# NATIVEFILEDIALOG
set( nfd_files
        ${NFD_DIR}/src/include/nfd.h
        ${NFD_DIR}/src/common.h
        ${NFD_DIR}/src/nfd_common.c
        ${NFD_DIR}/src/nfd_common.h
        ${NFD_DIR}/src/nfd_win.cpp
        )
include_directories( ${NFD_DIR}/)
include_directories( ${NFD_DIR}/src/include)

# GLEW
include_directories(  ${LIBS_DIR}/glew/include )

# GLAD
set( glad
    ${LIBS_DIR}/glad/src/glad.c
)
include_directories(${LIBS_DIR}/glad/include)

#GLM
include_directories(${LIBS_DIR}/glm)

# EXE ICON
set( icons
        ${LIBS_DIR}/IconsFontAwesome5.h
        ${LIBS_DIR}/pyhelper.h
        src/Opt.h src/Opt.cpp)


add_executable( Stereorizer
        ${imgui_files}
        ${imgui_impl_files}
        ${gl3w}
        ${glad}
        ${nfd_files}
        ${icons}
        ${project_files}
        ${openvr}
        ${APP_ICON_RESOURCE_WINDOWS}
        )

# Tracy
target_compile_definitions(Stereorizer PUBLIC TRACY_ENABLE=1 )
add_subdirectory(tracy)
include_directories(tracy/public/tracy)
add_library(tracy STATIC tracy/public/TracyClient.cpp)
target_link_libraries (Stereorizer Tracy::TracyClient )

# OpenVR
set(OPENVR_DIR ${LIBS_DIR}/openvr/)

# Find OpenVR library and include directories
find_library(OPENVR_LIBRARIES openvr_api HINTS ${OPENVR_DIR}/lib/win64)
find_path(OPENVR_INCLUDE_DIRS openvr.h HINTS ${OPENVR_DIR}/headers)
include_directories(${OPENVR_INCLUDE_DIRS})
target_link_libraries(Stereorizer ${OPENVR_LIBRARIES})


#target_link_libraries (Stereorizer ${LIBS_DIR}/openvr/lib/win64/openvr_api.lib )
#include_directories( ${LIBS_DIR}/openvr/headers )



# GLFW
set( GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set( GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${LIBS_DIR}/glfw)
target_link_libraries( Stereorizer glfw)
target_link_libraries( Stereorizer OpenGL::GL)
include_directories( ${LIBS}/glfw/include)

include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${PYTHON_INCLUDE_DIRS})

target_link_libraries( Stereorizer ${PYTHON_LIBRARIES})
target_link_libraries( Stereorizer ${GLFW_LIBRARIES} )
target_link_libraries( Stereorizer ${OpenCV_LIBS} )
target_link_libraries( Stereorizer ${OPENGL_LIBRARIES} )
target_link_libraries( Stereorizer ${LIBS_DIR}/glew/lib/Release/x64/glew32.lib )

message( w: ${OPENGL_INCLUDE_DIR}  )


file(COPY res DESTINATION .)
file(COPY MiDaS DESTINATION .)

list(APPEND PYTHONPATH "${CMAKE_CURRENT_DIR}/MiDaS")

add_custom_command(
        TARGET Stereorizer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/inference_depth.py
        ${CMAKE_CURRENT_BINARY_DIR}/inference_depth.py)

function(copy_file file)
    add_custom_command(
            TARGET GLVR PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/${file}
            ${CMAKE_CURRENT_BINARY_DIR}/${file}
    )
endfunction()