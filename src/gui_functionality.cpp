//
// Created by Flourek on 19/03/2023.
//

#include <opencv2/opencv.hpp>
#include "GL/gl3w.h"
#include "nfd.h"
#include "imgui.h"
#include "header.h"
#include "GLFW/glfw3.h"
#include "Chrono"
#include "Tracy.hpp"
#include "../libs/IconsFontAwesome5.h"
#include "misc/cpp/imgui_stdlib.h"

#include "Stereo.h"



cv::Mat updateStereo(Stereo &stereo, GuiSettings &opt) {
    ZoneScoped;

    cv::Mat result, input_resized, depth_resized;
//    result = input.clone();
//
//    cv::Size new_size( input.cols / opt.viewport_scale, input.rows / opt.viewport_scale);
//    cv::resize(input, input_resized, new_size);
//    cv::resize(depth, depth_resized, new_size);

    // Normalize for every image size and multiply by the ui setting
    float multiplier = 1.0f;
    try{
        multiplier = stof(opt.deviation_multiplier);
    } catch (std::exception& e) {}

    float deviation = opt.deviation;
    deviation *= ((float) stereo.left.mat.cols / 1000);
    deviation *= multiplier;
    stereo.deviation = deviation;


    if(!opt.inpainting_glitch)
        stereo.right.mat = 0;

    result = Stereo::ShiftPixels::run(stereo);

    if(!opt.inpainting_glitch)
        result = Stereo::Inpaint::run(stereo);


    if (opt.anaglyph_overlay)
//        result = anaglyphize(stereo.left, result);

    return result;
}



cv::Mat maskPostProcess(const cv::Mat &mask, GuiSettings &opt) {
    cv::Mat blurred, res = mask.clone();

    if(res.channels() == 3)
        cvtColor(res, res, cv::COLOR_BGR2GRAY);

//    std::cout << res.channels() << " " << CV_8UC1 << std::endl;

    // Invert
    bitwise_not(res, res);

    // Kernel sizes
    int morph = 2;
    int erode = 2;
    int blur  = 11;

    cv::Mat kernelEx = cv::Mat::ones(morph, morph, CV_8UC1);
    cv::Mat kernel_erode = cv::Mat::ones(erode, erode, CV_8UC1);

    // Remove noise, patch holes, smooth edges
    cv::morphologyEx(res, res, cv::MORPH_CLOSE, kernelEx);
    cv::erode(res, res, kernel_erode);
    cv::blur(res, res, cv::Size(blur, blur) );
    cv::threshold(res, res, 127,255, cv::THRESH_BINARY);
    cv::erode(res, res, erode);

    if(opt.mask_blur){

        // Get the right edge of each island
        cv::Mat edge(res.rows, res.cols, CV_8UC1);
        typedef uchar Pixel;


        float opacity_step = (float) 255 / opt.mask_blur_size;

        // this is litteraly 10 times faster than normal nested for loops
        edge.forEach<Pixel> (
            [&res, &edge, &opacity_step, opt] (Pixel &pixel, const int * position) -> void {

                if(position[1] == 0 || position[1] == res.cols) return;

                Pixel previous = res.at<Pixel>(position[0], position[1] - 1);
                Pixel current  = res.at<Pixel>(position[0], position[1]);
                if( previous == 255 && current == 0){

                    for (int i = 0; i < opt.mask_blur_size; ++i) {
                        if ( position[1] + i > res.cols ) return;
                        edge.at<Pixel>(position[0], position[1] + i) = cv::saturate_cast<uchar>(255 - i * opacity_step * 4);
                    }
                }
            }
        );




        res += edge;
    }



    if(res.channels() == 1)
        cvtColor(res, res, cv::COLOR_GRAY2BGR);

    return res;
}


void ImageCenteredWithAspect(GLuint &texture, int target_width, float aspect) {

    ImVec2 display_size, center_pos, pivot;

    if (aspect > 1){
        display_size =  ImVec2(target_width / aspect, target_width );
        center_pos = ImVec2(ImGui::GetWindowPos().x + target_width / 2, ImGui::GetWindowPos().y );
        pivot = ImVec2(0.5, 0);
    } else {
        display_size = ImVec2(target_width, target_width * aspect );
        center_pos = ImVec2(ImGui::GetWindowPos().x , ImGui::GetWindowPos().y + target_width / 2);
        pivot = ImVec2(0, 0.5);
    }
    ImGui::SetNextWindowPos( center_pos, 0, pivot);

    ImGui::BeginChild("LeftImageCenterer", display_size);
    ImGui::Image( reinterpret_cast<void*>( static_cast<intptr_t>( texture ) ), display_size);
    ImGui::EndChild();

}

void TextCentered(const std::string& text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text.c_str());
}

void GuiFileDialog(const std::string& text, std::string& path){
    float text_width = ImGui::CalcTextSize(text.c_str()).x;
    ImGui::SetCursorPosX(-text_width);
    RightAlignNextItem();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", text.c_str());
    ImGui::SameLine();
    ImGui::InputText("##output_path", &path, ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    ImGui::Button(ICON_FA_FOLDER_OPEN);
}

bool RightAlignedSlider(const std::string& label, float *x, float v_min, float v_max){
    float text_child_width = ImGui::GetWindowSize().x / 5;
    float text_width = ImGui::CalcTextSize(label.c_str()).x;
    float height = ImGui::GetFrameHeight();

    ImGui::BeginChild(label.c_str(), ImVec2(text_child_width, height));
        ImGui::SameLine(text_child_width - text_width);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
    ImGui::EndChild();
    ImGui::SameLine();
    return  ImGui::SliderFloat("##Masky", x, v_min, v_max);
}


void RightAlignNextItem(){
    float width = ImGui::GetWindowSize().x / 5;
    ImGui::NewLine();
    ImGui::SameLine(0, width + 8);
}





// Get filepath from user dropping a file on the window
// Seems this goofy GLFW Callback function is the only way to do it and you can't pass ur own arguments to it
std::shared_ptr<std::string> chuj;
GuiSettings* flags_global;

bool openFileDialog(std::string& input_path, std::string& filename, const std::string& default_path){
    nfdchar_t *outPath;
    auto def_path = const_cast<nfdchar_t *>(default_path.c_str());
//    nfdresult_t result = NFD_OpenDialog(&outPath, def_path, true);
//
//    if ( result == NFD_OKAY ) {
//        input_path = outPath;
//        filename = input_path.substr(input_path.find_last_of('\\') + 1);
//        free(outPath);
//        return true;
//    }

    return false;
}


void dropCallback(GLFWwindow *window, int count, const char** paths){
    *chuj = paths[0];
    (*flags_global).update_input = true;
}

void dragDropInputFile(GLFWwindow *window, std::shared_ptr<std::string> output_path, GuiSettings &flags) {
    glfwSetDropCallback(window, dropCallback);
    chuj = output_path;
    flags_global = &flags;
    std::cout << &output_path << std::endl;
}






