//
// Created by Flourek on 21/03/2023.
//

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "header.h"
#include "../libs/IconsFontAwesome5.h"
#include "../libs/IconsFontAwesome5.h"

std::string model_path = "C:/Users/Flourek/CLionProjects/Stereorizer/weights/dpt_beit_large_512.pt";

using namespace ImGui;
void GuiDepthPanel(GuiSettings &flags, GLuint depth_texture, float width, float input_aspect, std::string filename,
                   std::string input_path, std::string output_path, float &depth_contrast, float &depth_brigthness,
                   float &depth_highlights, cv::Mat &input_depth, cv::Mat &input)
{
    BeginGroup();

    Text(" Depth Map");

    BeginChild("DepthImageContainer", ImVec2(width, width), true, ImGuiWindowFlags_NoScrollbar);
        ImageCenteredWithAspect(depth_texture, width, input_depth.cols, input_depth.rows);
    EndChild();

    BeginChild("DepthContainer", ImVec2(width, 0));
    if( CollapsingHeader("Depth map source", ImGuiTreeNodeFlags_DefaultOpen) ) {
        BeginChild("Seethe", ImVec2(0, 180), false);
        Indent( 8.0f );

        GuiFileDialog("MiDaS", filename);
        GuiFileDialog("Model", filename);

        NewLine();
        RightAlignNextItem();
        if( Button("Generate", ImVec2(300, 20)) ){
            generateDepthMap(input_path, model_path, input_depth, flags);
            flags.update_depth |= true;
        }
        Unindent( 8.0f );
        EndChild();
    }

    if( CollapsingHeader("Adjustments", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent( 8.0f );
        flags.update_depth |= RightAlignedSlider("Contrast",    &depth_contrast,   0.0f, 1.0f);
        flags.update_depth |= RightAlignedSlider("Brigthness",  &depth_brigthness, 0.0f, 1.0f);
        flags.update_depth |= RightAlignedSlider("Highlights",  &depth_highlights, 0.0f, 1.0f);
        RightAlignNextItem();
        flags.update_depth |= ImGui::Checkbox("Invert",         &flags.depth_invert);
        RightAlignNextItem();
        flags.update_depth |= ImGui::Checkbox("Grayscale",      &flags.depth_grayscale);
        Unindent( 8.0f );
    }

    EndChild();

    EndGroup();
}