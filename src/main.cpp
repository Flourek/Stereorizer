#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include <opencv2/opencv.hpp>
#include <Python.h>

//#define TRACY_ENABLE
#include "Tracy.hpp"

#include <iostream>
#include "thread"
#include "VRController.h"
#include "Depth.h"
#include "header.h"
#include "Image.h"
#include "Stereo.h"
#include "Opt.h"


// viewport width, height
#define SmallImageSize (0.24*vw)
#define ResultImageSize (0.4*vw)

int main( int argc, char* argv[] ) {

    if( !glfwInit() ) return -1;

    // Get monitor resolution
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    float resolution_scale = mode->width / 1920;

    // Init glfw
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
    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    setImGuiSettings(resolution_scale);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoAutoMerge = true;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Init python
    Py_Initialize();
    PyInterpreterState* pythonInterpeter = PyInterpreterState_Head();

    // Default images
    Image left("../img/w.jpg");
    Depth depth("C:/Users/Flourek/CLionProjects/Stereorizer/cmake-build-relwithdebinfo/output/w-dpt_beit_large_512.png");
    Image right("../img/w.jpg");
    Image zoom("../img/w.jpg");
    Opt::Get().image_path = "../img/w.jpg";
    zoom.gl_filter = GL_NEAREST;
    zoom.createTexture();

    Stereo stereo = Stereo(left, depth, right, Opt::Get().deviation);
    cv::Mat depth_float;

    Opt& opt = Opt::Get();

    while(!glfwWindowShouldClose(window)){
        glfwMakeContextCurrent(window);

        glfwPollEvents();

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
                generateDepthMap(opt.image_path, Opt::Get().model_path, depth, pythonInterpeter);
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

        if (!opt.vr_enabled){
            VRController::Get().SetMats(stereo.left.mat, stereo.right.mat);
            VRController::Get().m_MainWindow = window;
            VRController::Run();
            opt.vr_enabled = true;
        }

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

//        ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
//                                    | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus
//                                    | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_DockNodeHost);

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowPos( ImVec2(viewport->Pos.x, viewport->Pos.y) );
        ImGui::SetNextWindowSize( ImVec2(viewport->Size.x, viewport->Size.y ) );
        ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize);

            ImGui::Indent(16.0f);
            GuiImagePanel(left, stereo, SmallImageSize);

            ImGui::SameLine(0, 0.04*vw);
            GuiDepthPanel(depth, SmallImageSize);

            ImGui::SameLine(0, 0.04*vw);
            GuiResultPanel(stereo, zoom, ResultImageSize);

        ImGui::End();

        if(opt.vr_enabled)
            VRController::Get().DrawImGui();


        ImGui::Render();
        glViewport(0, 0, vw, vh);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );


        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(window);
        }

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