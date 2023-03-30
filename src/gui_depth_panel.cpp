//
// Created by Flourek on 21/03/2023.
//

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "header.h"

std::string model_path = "C:/Users/Flourek/CLionProjects/Stereorizer/weights/dpt_beit_large_512.pt";

void GuiDepthPanel(Image &depth, GuiSettings &opt, float width) {
    using namespace ImGui;
    BeginGroup();

    Text(" Depth Map");

    BeginChild("DepthImageContainer", ImVec2(width, width), true, ImGuiWindowFlags_NoScrollbar);
    ImageCenteredWithAspect(depth.texture, width, depth.aspect);
    EndChild();

    BeginChild("DepthContainer", ImVec2(width, 0));
    if( CollapsingHeader("Depth map source", ImGuiTreeNodeFlags_DefaultOpen) ) {
        BeginChild("Seethe", ImVec2(0, 180), false);
        Indent( 8.0f );

        GuiFileDialog("MiDaS", depth.path);
        GuiFileDialog("Model", depth.path);

        NewLine();
        RightAlignNextItem();
        if( Button("Generate", ImVec2(300, 20)) ){
//            generateDepthMap(input_path, model_path, input_depth, opt);
            opt.update_depth |= true;
        }
        Unindent( 8.0f );
        EndChild();
    }

    if( CollapsingHeader("Adjustments", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent( 8.0f );
        opt.update_depth |= RightAlignedSlider("Contrast", &opt.depth_contrast, 0.0f, 1.0f);
        opt.update_depth |= RightAlignedSlider("Brigthness", &opt.depth_brigthness, 0.0f, 1.0f);
        opt.update_depth |= RightAlignedSlider("Highlights", &opt.depth_highlights, 0.0f, 1.0f);
        RightAlignNextItem();
        opt.update_depth |= ImGui::Checkbox("Invert", &opt.depth_invert);
        RightAlignNextItem();
        opt.update_depth |= ImGui::Checkbox("Grayscale", &opt.depth_grayscale);
        Unindent( 8.0f );
    }

    EndChild();

    EndGroup();
}