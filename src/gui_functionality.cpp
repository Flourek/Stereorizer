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





cv::Mat maskPostProcess(const cv::Mat &mask, const GuiSettings &opt) {
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
//    ImGui::SetCursorPosX(-text_width);
//    RightAlignNextItem();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", text.c_str());
    ImGui::SameLine();
    ImGui::SetNextItemWidth(357);
    ImGui::InputText("##output_path", &path, ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    ImGui::Button(ICON_FA_FOLDER_OPEN);
}

bool RightAlignedSlider(const std::string& label, float *x, float v_min, float v_max){
    float text_child_width = ImGui::GetWindowSize().x / 2;
    float text_width = ImGui::CalcTextSize(label.c_str()).x;
    float height = ImGui::GetFrameHeight();

    ImGui::BeginChild(label.c_str(), ImVec2(text_child_width, height));
        ImGui::SameLine(text_child_width - text_width);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label.c_str());
    ImGui::EndChild();
    ImGui::SameLine();

    std::string hidden_label = "##" + label;

    return  ImGui::SliderFloat(hidden_label.c_str(), x, v_min, v_max);
}


void RightAlignNextItem(char const *text) {
    float width = ImGui::GetWindowSize().x / 2;
    float offset = 70;

    ImGui::NewLine();
    float text_width = ImGui::CalcTextSize(text).x;
    ImGui::SameLine(0, width - text_width - offset);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", text);
    ImGui::SameLine();
}


void BeginRightAlign(char const *label, int item_count) {
    float child_width = ImGui::GetWindowSize().x / 2;
    float child_height = item_count * ImGui::GetFrameHeightWithSpacing();

    float padding_right = 60.0f;
    float offset = 70;
    ImGui::NewLine();
    ImGui::SameLine(0, child_width - offset);
    ImGui::BeginChild(label, ImVec2(child_width + offset - padding_right, child_height));
}

void EndRightAlign(){
    ImGui::EndChild();
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






