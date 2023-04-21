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
    Opt::Get().image_path = "../img/w.jpg";
    zoom.gl_filter = GL_NEAREST;
    zoom.createTexture();

    Stereo stereo = Stereo(left, depth, right, Opt::Get().deviation);


    std::string output_path = "C:/Users/Flourek/CLionProjects/Stereorizer/img/";
    std::string filename = "chuj.jpg";

    GLuint image_texture = 0, depth_texture = 0, result_texture= 0, zoom_texture = 0;

    cv::Mat depth_float;

    Opt& opt = Opt::Get();


    while(!glfwWindowShouldClose(window)){

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        opt.force_update |= ImGui::IsKeyPressed(ImGuiKey_Space);

        // Program flow control
        if (opt.update_input) {
            ZoneScopedN("Change Input");
            std::cout << "me when setting to: "<<Opt::Get().image_path;
            left.changeImage(opt.image_path);
            right.changeImage(opt.image_path);

            opt.viewport_scale = left.getScaleSuggestion();

            zoom.mat = right.mat;
            zoom.createTexture();

            if(opt.vr_enabled)
                VRController::Get().SetMats(stereo.left.mat, stereo.right.mat);

            opt.update_input = false;
            opt.update_depth = true;
            opt.update_stereo = opt.live_refresh;
        }

        if(opt.midas_run){

            std::thread t;

            t = std::thread([&]() {
                generateDepthMap(opt.image_path, Opt::Get().model_path, depth, nullptr);
                std::cout << "DONE XD"<< std::endl;
                stereo.resizeAll( opt.viewport_scale);

                opt.update_depth = true;

            });

            t.detach();
            opt.midas_run = false;
        }

        if (opt.update_depth) {
            ZoneScopedN("Depth");
            depth.convertToDisplay();
            depth.createTexture();

            opt.update_stereo = opt.live_refresh;
            opt.update_depth = false;
        }

        opt.size_mismatch = (left.original.size != depth.original.size );

        if (!opt.size_mismatch){
            ZoneScopedN("Stereo");

            if ((opt.update_stereo && opt.live_refresh) || opt.force_update){
                stereo.run();

//                zoom.mat = right.mat;
//                zoom.createTexture();

                opt.force_update = false;
                opt.update_stereo = false;

                if(opt.vr_enabled){
                    VRController::Get().SetMats(stereo.left.mat, stereo.right.mat);
                    VRController::Get().texturesUpdated = true;
                }


            }
        }

        // User interface
        static int counter = 0;
        ZoneScopedN("ImGUI");

        int vw, vh;
        glfwGetFramebufferSize(window, &vw, &vh);

        ImGui::SetNextWindowPos( ImVec2(0,0) );
        ImGui::SetNextWindowSize( ImVec2(vw, vh) );
        ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                               ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::Indent(16.0f);
        GuiImagePanel(left, stereo, SmallImageSize);

        ImGui::SameLine(0, 0.04*vw);
        GuiDepthPanel(depth, SmallImageSize);

        ImGui::SameLine(0, 0.04*vw);
        GuiResultPanel(stereo, zoom, ResultImageSize);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
        glfwPollEvents();
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