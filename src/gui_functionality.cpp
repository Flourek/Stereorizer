//
// Created by Flourek on 19/03/2023.
//

#include <opencv2/opencv.hpp>
#include "GL/glew.h"
#include "nfd.h"
#include "imgui.h"
#include "header.h"
#include "GLFW/glfw3.h"
#include "Tracy.hpp"
#include "../libs/IconsFontAwesome5.h"
#include "misc/cpp/imgui_stdlib.h"

#include "Stereo.h"
#include "Opt.h"

void setImGuiSettings(float resolution_scale){
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
    Opt::Get().image_path = paths[0];
    Opt::Get().update_input = true;

}






