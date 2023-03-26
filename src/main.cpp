#include <iostream>
#include <opencv2/opencv.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "header.h"
#include "../libs/IconsFontAwesome5.h"
#include <Python.h>



// vw, vh = viewport width, height
#define SmallImageSize 0.24*vw
#define ResultImageSize 0.4*vw


int main( int argc, char* argv[] ) {

    if( !glfwInit() ){
        return -1;
    }

    std::cout << argv[0] << std::endl;

    // Get monitor resolution
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    float resolution_scale = mode->width / 1920;

    GLFWwindow* window = glfwCreateWindow( 1280 * resolution_scale, 720 * resolution_scale, "Stereorizer", nullptr, nullptr );
    glfwMakeContextCurrent( window );
    glfwSwapInterval( 1 );
    gl3wInit();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL( window, true );
    ImGui_ImplOpenGL3_Init( "#version 330" );

    Py_Initialize();




    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

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

    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1, 0.1, 0.1, 1);
    ImGui::GetIO().FontGlobalScale  = 1.0 * resolution_scale;


    GuiSettings opt;
    static float depth_contrast = 0.5f;
    static float depth_brigthness = 0.5f;
    static float depth_highlights = 0.5f;
    static float deviation = 30.0f;


    cv::Mat input_image = cv::imread("../img/f.jpg", cv::IMREAD_COLOR );
    cv::Mat input_depth = cv::imread( "../img/f_DEPTH.png", CV_8UC1);


    float input_aspect = (float)input_image.rows / input_image.cols;

    auto input_path = std::make_shared<std::string>();
    dragDropInputFile(window, input_path, opt);

    std::string output_path = "C:\\Users\\Flourek\\CLionProjects\\Stereorizer\\img\\";
    std::string filename = "ziobioearioghaersiughipuerahgipuerahgpiuerahgpiuearhipureagpiu.jpg";
    *input_path = "C:\\Users\\Flourek\\CLionProjects\\Stereorizer\\img\\f.jpg";

    GLuint image_texture = 0, depth_texture = 0, result_texture= 0, zoom_texture = 0;
    cv::Mat image, depth, result, mask;

    result = input_image.clone();
    depth = input_depth.clone();


    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int vw, vh;
        glfwGetFramebufferSize(window, &vw, &vh);

        opt.force_update |= ImGui::IsKeyPressed(ImGuiKey_Space);

        if (opt.update_input){
            changeInputImage(input_image, *input_path, depth, opt);
            convertMatToTexture(input_image, image_texture);
            opt.update_input = false;
            opt.update_depth = true;
            opt.update_stereo = opt.live_refresh;
            opt.size_mismatch = checkSizeMismatch(input_image, input_depth);
        }

        if (opt.update_depth){
            depth = adjustDepth(input_depth, depth_contrast, depth_brigthness, depth_highlights, opt);
            convertMatToTexture(depth, depth_texture);
            opt.update_depth= false;
            opt.update_stereo = opt.live_refresh;
            opt.size_mismatch = checkSizeMismatch(input_image, input_depth);
        }


        if (!opt.size_mismatch){

            if ( (opt.update_stereo && opt.live_refresh) || opt.force_update){
                result = updateStereo(input_image, depth, opt, mask);
                opt.force_update = false;

                if (opt.mask_overlay){
                    cv::Mat display_mask = maskPostProcess(mask, opt);
                    result += display_mask;
                }

                convertMatToTexture(result, zoom_texture, GL_NEAREST, 1);
                convertMatToTexture(result, result_texture);
                opt.update_stereo = false;
            }
        }



        using namespace ImGui; {
            static int counter = 0;

            SetNextWindowPos( ImVec2(0,0) );
            SetNextWindowSize( ImVec2(vw, vh) );
            Begin("main", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

            Indent(16.0f);
            GuiImagePanel(opt, input_image, SmallImageSize, image_texture, *input_path, deviation);

            SameLine(0, 0.04*vw);
            GuiDepthPanel(opt, depth_texture, SmallImageSize, input_aspect, filename, *input_path, output_path,
                          depth_contrast, depth_brigthness, depth_highlights, input_depth, image);

            SameLine(0, 0.04*vw);
            GuiResultPanel(opt, result_texture, zoom_texture, result, mask, input_image, ResultImageSize, output_path);

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
        glfwSwapBuffers( window );
    }

    Py_Finalize();
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}