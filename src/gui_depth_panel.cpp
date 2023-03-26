//
// Created by Flourek on 21/03/2023.
//

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "header.h"
#include "../libs/IconsFontAwesome5.h"

std::string model_path = "C:/Users/Flourek/CLionProjects/Stereorizer/weights/dpt_beit_large_512.pt";

using namespace ImGui;
void GuiDepthPanel(GuiSettings &flags, GLuint depth_texture, float width, float input_aspect, std::string filename,
                   std::string input_path, std::string output_path, float &depth_contrast, float &depth_brigthness,
                   float &depth_highlights, cv::Mat &input_depth, cv::Mat &input)
{
    BeginChild("chujnia", ImVec2(width, 0));
    Text("Depth:");
//                    PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 0.2));
    BeginChild("DepthImageContainer", ImVec2(width, width), true, ImGuiWindowFlags_NoScrollbar);
    ImageCenteredWithAspect(depth_texture, width, input_depth.cols, input_depth.rows);
    EndChild();
    PopStyleColor();

//                PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1, 0.1, 0.1, 1));

    if( CollapsingHeader("Depth map source", ImGuiTreeNodeFlags_DefaultOpen) ) {
        BeginChild("Seethe", ImVec2(0, 180), false);
        Indent( 8.0f );
        SeparatorText("MiDaS");
        if( Button("Generate", ImVec2(150, 20)) ){
            generateDepthMap(input_path, model_path, input_depth, flags);
            flags.update_depth |= true;
        }
        NewLine();
        Button("Select model");
        SameLine();
        PushItemWidth(0.60 * width);
        InputText("##Model", &filename, ImGuiInputTextFlags_ReadOnly);

        NewLine();
        SeparatorText("File from disk");
        NewLine();

//                        PopItemWidth();
        if (ImGui::Button("Select file ")) {
            flags.update_input |= openFileDialog(input_path, filename, output_path);
        }
        SameLine();
        PushItemWidth(0.60 * width);
        InputText("##labele", &filename, ImGuiInputTextFlags_ReadOnly);
        NewLine();
        Unindent( 8.0f );
        EndChild();
    }

    if( CollapsingHeader("Adjustments", ImGuiTreeNodeFlags_DefaultOpen) ){
        BeginChild("cipy", ImVec2(0, 100));
        Indent( 8.0f );
        flags.update_depth |= ImGui::SliderFloat("Contrast",    &depth_contrast, 0.0f, 1.0f);
        flags.update_depth |= ImGui::SliderFloat("Brigthness",  &depth_brigthness, 0.0f, 1.0f);
        flags.update_depth |= ImGui::SliderFloat("Highlights",  &depth_highlights, 0.0f, 1.0f);
        flags.update_depth |= ImGui::Checkbox("Invert",         &flags.depth_invert);
        Unindent( 8.0f );
        EndChild();
    }
    PopStyleColor(1);

    ImGui::EndChild();
}