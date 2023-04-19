//
// Created by Flourek on 21/03/2023.
//

#include "bezier.hpp"
#include "imgui.h"
#include "header.h"
#include "Depth.h"
#include "Opt.h"

void GuiDepthPanel(Depth &depth, float width) {
    using namespace ImGui;

    auto opt = Opt::Get();
    auto flags = Opt::GetFlags();

    BeginGroup();
    Text(" Depth Map");

    BeginChild("DepthImageContainer", ImVec2(width, width), true, ImGuiWindowFlags_NoScrollbar);
    ImageCenteredWithAspect(depth.texture, width, depth.aspect);
    EndChild();

    BeginChild("DepthContainer", ImVec2(width, 0));
    if( CollapsingHeader("Depth map source", ImGuiTreeNodeFlags_DefaultOpen) ) {
        BeginChild("Seethe", ImVec2(0, 180), false);
        Indent( 8.0f );
        NewLine();

        GuiFileDialog("MiDaS", depth.path);
        GuiFileDialog("Model", depth.path);

        NewLine();

        BeginRightAlign("1233123", 1);
        if( Button("Generate", ImVec2(GetContentRegionAvail().x, 20)) ){
            flags.midas_run = true;
        }
        EndRightAlign();

        Unindent( 8.0f );
        EndChild();
    }



    if( CollapsingHeader("Adjustments", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent( 8.0f );
        NewLine();

        static ImVec2 foo[10] = {ImVec2(-1,0)};
        if (ImGui::Curve("##Das editor", ImVec2(442, 200), 10, foo)) {
            depth.adjust(foo);
            flags.update_depth |= true;
        }

        BeginChild("##Editor", ImVec2(442, 0));
            ImGui::Image( (void*)(intptr_t) depth.lut_preview.texture, ImVec2(442, 5));
            NewLine();
            AlignTextToFramePadding();
            NewLine();
            SameLine(0, 200);
            Text("Color map:");
            SameLine();
            SetNextItemWidth(GetContentRegionAvail().x);
            if ( ImGui::Combo("##Colormap", &opt.depth_color_map, "Grayscale\0Inferno\0Jet\0Turbo\0Parula\0\0") ) {
                depth.color_map = opt.depth_color_map;
                flags.update_depth |= true;
            }

        EndChild();


        Unindent( 8.0f );
    }

    EndChild();

    EndGroup();
}