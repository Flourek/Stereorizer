//
// Created by Flourek on 21/03/2023.
//

#include "bezier.hpp"
#include "imgui.h"
#include "header.h"
#include "Depth.h"

void GuiDepthPanel(Depth &depth, GuiSettings &opt, float width) {
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
            opt.midas_run = true;
        }
        Unindent( 8.0f );
        EndChild();
    }



    if( CollapsingHeader("Adjustments", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent( 8.0f );


        RightAlignNextItem();
        static ImVec2 foo[10] = {ImVec2(-1,0)};
        if (ImGui::Curve("##Das editor", ImVec2(300, 200), 10, foo))
        {
            depth.adjust(foo);
            opt.update_depth |= true;
        }

//        RightAlignNextItem();
//        BeginChild("##awe", ImVec2(222, 250));
//        if ( Bezier("##Linear", opt.v) ){
//            depth.adjust(opt.v);
//            opt.update_depth |= true;
//        }
//        EndChild();
//        SameLine();
//        AlignTextToFramePadding();
//        BeginChild("##awew", ImVec2(0, 250));
//            PushID("chujnia");
//            for (int i = 0; i < 4; ++i) {
//                PushID(i);
//                PushItemWidth(50);
//                SliderFloat("##", &opt.v[i-1], 0.0f, 1.0f );
//                PopID();
//            }
//            PopID();
//        EndChild();



//        opt.update_depth |= RightAlignedSlider("Contrast",   &opt.depth_contrast, 0.0f, 1.0f);
//        opt.update_depth |= RightAlignedSlider("Brigthness", &opt.depth_brigthness, 0.0f, 1.0f);
//        opt.update_depth |= RightAlignedSlider("Highlights", &opt.depth_highlights, 0.0f, 1.0f);
//        RightAlignNextItem();
//        opt.update_depth |= ImGui::Checkbox("Invert", &opt.depth_invert);

//        NewLine();
        RightAlignNextItem();
        if ( ImGui::Combo("##Colormap", &opt.depth_color_map, "Grayscale\0Inferno\0Jet\0Turbo\0Parula\0\0") ) {
            depth.color_map = opt.depth_color_map;
            opt.update_depth |= true;
        }

        Unindent( 8.0f );
    }

    EndChild();

    EndGroup();
}