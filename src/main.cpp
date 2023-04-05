#include <iostream>

#include <opencv2/opencv.hpp>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "../libs/IconsFontAwesome5.h"
#include <Python.h>
#include "Depth.h"

#include "header.h"
#include "Image.h"
#include "Stereo.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include "Tracy.hpp"
#define TRACY_ENABLE

// vw, vh = viewport width, height
#define SmallImageSize 0.24*vw
#define ResultImageSize 0.4*vw

#include "future"
#include "thread"


int main( int argc, char* argv[] ) {

    if( !glfwInit() ){
        return -1;
    }

    // Get monitor resolution
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    float resolution_scale = mode->width / 1920;

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow( 1920 * resolution_scale, 1080 * resolution_scale, "Stereorizer", nullptr, nullptr );
    glfwMakeContextCurrent( window );
    glfwSwapInterval( 1 );
    gl3wInit();

    // Setting window icon

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL( window, true );
    ImGui_ImplOpenGL3_Init( "#version 330" );

    Py_Initialize();
    PyInterpreterState* interp = PyInterpreterState_Head();



    // ImGui
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_Button] = ImColor(135, 41, 80);
    style.Colors[ImGuiCol_ChildBg] = ImColor(28, 28, 28);
    style.Colors[ImGuiCol_Header] = ImColor(41, 41, 41);
    style.Colors[ImGuiCol_FrameBg] = ImColor(71, 71, 71);
    style.Colors[ImGuiCol_SliderGrab] = ImColor(153, 52, 94);


    // ImGui Style Settings
    io.Fonts->AddFontDefault();
    float baseFontSize = 20.0f; // 13.0f is the size of the default font. Change to the font size you use.
    float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;
    io.Fonts->AddFontFromFileTTF( "./res/fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges );

    ImGui::GetIO().FontGlobalScale  = 1.0 * resolution_scale;


    GuiSettings opt;

    Image left("../img/hg.jpg");
    Depth depth("../img/hg.png");
    Image right("../img/hg.jpg");
    Image zoom("../img/hg.jpg");
    zoom.gl_filter = GL_NEAREST;
    zoom.createTexture();

    Stereo stereo = Stereo(left, depth, right, opt.deviation);

//    cv::imshow("w", left.display_BGRA);
//    int e = cv::waitKey(0);

    auto input_path = std::make_shared<std::string>();
    dragDropInputFile(window, input_path, opt);

    std::string output_path = "C:\\Users\\Flourek\\CLionProjects\\Stereorizer\\img\\";
    std::string filename = "chuj.jpg";
    *input_path = "C:\\Users\\Flourek\\CLionProjects\\Stereorizer\\img\\hg.jpg";

    GLuint image_texture = 0, depth_texture = 0, result_texture= 0, zoom_texture = 0;

    cv::Mat depth_float;



    while(!glfwWindowShouldClose(window)){

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int vw, vh;
        glfwGetFramebufferSize(window, &vw, &vh);

        opt.force_update |= ImGui::IsKeyPressed(ImGuiKey_Space);

        if (opt.update_input) {
            ZoneScopedN("Change INput");

            left.changeImage(*input_path);
//            right.changeImage(*input_path);
            depth.convertToDisplay();
            depth.createTexture();

            opt.viewport_scale = left.getScaleSuggestion();
            zoom.mat = right.mat;
            zoom.createTexture();

            opt.update_input = false;
            opt.update_depth = true;
            opt.update_stereo = opt.live_refresh;
        }

        if(opt.midas_run){

            std::thread t;

            t = std::thread([&]() {
                generateDepthMap(*input_path, opt.model_path, depth, opt, interp);
                opt.update_depth = true;
                std::cout << "DONE XD"<< std::endl;
                depth.convertToDisplay();
                depth.createTexture();
            });

            t.detach();

            opt.midas_run = false;
        }

        if (opt.update_depth) {
            ZoneScopedN("Depth");
            depth.convertToDisplay();
            depth.updateTexture();

//            depth.convertToFloat();
//            depth.convertToDisplay();
            opt.update_stereo = opt.live_refresh;
            opt.update_depth = false;
        }

        opt.size_mismatch = ( left.original.size != depth.original.size );

        if (!opt.size_mismatch){
            ZoneScopedN("Stereo");

            if ( (opt.update_stereo && opt.live_refresh) || opt.force_update){
                stereo.run(opt);

//                zoom.mat = right.mat;
//                zoom.createTexture();

                opt.force_update = false;
                opt.update_stereo = false;
            }
        }

        using namespace ImGui; {
            static int counter = 0;
            ZoneScopedN("ImGUI");

            SetNextWindowPos( ImVec2(0,0) );
            SetNextWindowSize( ImVec2(vw, vh) );
            Begin("main", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

            Indent(16.0f);
            GuiImagePanel(left, stereo, opt, SmallImageSize);

            SameLine(0, 0.04*vw);
            GuiDepthPanel(depth, opt, SmallImageSize);

            SameLine(0, 0.04*vw);
            GuiResultPanel(stereo, zoom, opt, ResultImageSize);

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