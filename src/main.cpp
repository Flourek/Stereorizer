#include <iostream>

#include <opencv2/opencv.hpp>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../libs/IconsFontAwesome5.h"
#include <Python.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include "Tracy.hpp"
#define TRACY_ENABLE

// viewport width, height
#define SmallImageSize (0.24*vw)
#define ResultImageSize (0.4*vw)

#include "thread"
#include "VRController.h"
#include "Depth.h"
#include "header.h"
#include "Image.h"
#include "Stereo.h"
#include "Opt.h"


int main( int argc, char* argv[] ) {

    if( !glfwInit() ){
        return -1;
    }

    // Get monitor resolution
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    float resolution_scale = mode->width / 1920;
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow( 1920 * resolution_scale, 1080 * resolution_scale, "Stereorizer", nullptr, nullptr );
    glfwMakeContextCurrent( window );
    glfwSetDropCallback(window, dropCallback);
    glfwSwapInterval( 1 );
    glewInit();

    std::cout << glGetString(GL_VERSION) << "\n";

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL( window, true );
    ImGui_ImplOpenGL3_Init( "#version 330" );
    setImGuiSettings(resolution_scale);

    Py_Initialize();
    PyInterpreterState* pythonInterpeter = PyInterpreterState_Head();

    Image left("../img/w.jpg");
    Depth depth("C:/Users/Flourek/CLionProjects/Stereorizer/cmake-build-relwithdebinfo/output/w-dpt_beit_large_512.png");
    Image right("../img/w.jpg");
    Image zoom("../img/w.jpg");
    zoom.gl_filter = GL_NEAREST;
    zoom.createTexture();

    Stereo stereo = Stereo(left, depth, right, Opt::Get().deviation);

    auto input_path = std::make_shared<std::string>();
    dragDropInputFile(window, input_path);

    std::string output_path = "C:/Users/Flourek/CLionProjects/Stereorizer/img/";
    std::string filename = "chuj.jpg";
    *input_path = "C:/Users/Flourek/CLionProjects/Stereorizer/img/w.jpg";

    GLuint image_texture = 0, depth_texture = 0, result_texture= 0, zoom_texture = 0;

    cv::Mat depth_float;

    auto flags = Opt::GetFlags();
    auto opt = Opt::Get();

    while(!glfwWindowShouldClose(window)){

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int vw, vh;
        glfwGetFramebufferSize(window, &vw, &vh);

        flags.force_update |= ImGui::IsKeyPressed(ImGuiKey_Space);

        if (flags.update_input) {
            ZoneScopedN("Change Input");

            left.changeImage(*input_path);
            right.changeImage(*input_path);

            opt.viewport_scale = left.getScaleSuggestion();

            zoom.mat = right.mat;
            zoom.createTexture();

            if(flags.vr_enabled)
                VRController::Get().SetMats(stereo.left.mat, stereo.right.mat);

            flags.update_input = false;
            flags.update_depth = true;
            flags.update_stereo = flags.live_refresh;
        }

        if(flags.midas_run){

            std::thread t;

            t = std::thread([&]() {
                generateDepthMap(*input_path, Opt::Get().model_path, depth, nullptr);
                std::cout << "DONE XD"<< std::endl;
                stereo.resizeAll( opt.viewport_scale);

                flags.update_depth = true;

            });

            t.detach();
            flags.midas_run = false;
        }

        if (flags.update_depth) {
            ZoneScopedN("Depth");
            depth.convertToDisplay();
            depth.createTexture();

            flags.update_stereo = flags.live_refresh;
            flags.update_depth = false;
        }

        flags.size_mismatch = (left.original.size != depth.original.size );

        if (!flags.size_mismatch){
            ZoneScopedN("Stereo");

            if ((flags.update_stereo && flags.live_refresh) || flags.force_update){
                stereo.run();

//                zoom.mat = right.mat;
//                zoom.createTexture();

                flags.force_update = false;
                flags.update_stereo = false;



                if(flags.vr_enabled){
                    VRController::Get().SetMats(stereo.left.mat, stereo.right.mat);
                    VRController::Get().texturesUpdated = true;
                }


//                if(opt.vr_enabled){
//                }

            }
        }


        using namespace ImGui; {
            static int counter = 0;
            ZoneScopedN("ImGUI");

            SetNextWindowPos( ImVec2(0,0) );
            SetNextWindowSize( ImVec2(vw, vh) );
            Begin("main", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

            Indent(16.0f);
            GuiImagePanel(left, stereo, SmallImageSize);

            SameLine(0, 0.04*vw);
            GuiDepthPanel(depth, SmallImageSize);

            SameLine(0, 0.04*vw);
            GuiResultPanel(stereo, zoom, ResultImageSize);

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
        glfwSwapBuffers( window );

        FrameMark;
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    Py_Finalize();
//    t.join();

    return 0;
}