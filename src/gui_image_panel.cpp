//
// Created by Flourek on 21/03/2023.
//

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "GL/gl3w.h"
#include "header.h"
#include "Image.h"
#include "../libs/IconsFontAwesome5.h"
#include "Stereo.h"


void GuiImagePanel(Image &left, Stereo &stereo, GuiSettings &opt, float target_width) {

    using namespace ImGui;

    BeginGroup();
    Text(" Left eye");

    BeginChild("LeftImageContainer", ImVec2(target_width, target_width), true, ImGuiWindowFlags_NoScrollbar);
    ImageCenteredWithAspect(left.texture, target_width, left.aspect);
    EndChild();

    BeginChild("Input File", ImVec2(target_width, 0));
    if( CollapsingHeader("Imput fil", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent(8.0f);
        NewLine();

        GuiFileDialog("File:", left.path);

        Unindent(8.0f);
        NewLine();
    }
//    EndChild();

    if (opt.size_mismatch){
        BeginChild("scrolling", ImVec2(243, 35));
//                        PushStyleColor(ImGuiCol_ChildBg, IM_COL32(200,20,20,255));
        PushItemWidth(235);
        SameLine(6);

        AlignTextToFramePadding();
        TextWrapped("Source image and the depth image must be the same size");
        EndChild();

        SameLine(0, 0);

        BeginChild("scrollinge", ImVec2(60, 35));
        PushStyleColor(ImGuiCol_Button, IM_COL32(150,20,20,255));
        Button("Resize", ImVec2(60, 35));
        PopStyleColor(2);
        EndChild();
    }

    if( CollapsingHeader("Inpainting & Mask", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed) ){
        Indent(8.0f);
        NewLine();

//        RightAlignNextItem("Mask blur");
        BeginRightAlign("erf", 1);
        SetNextItemWidth(GetContentRegionAvail().x);
            ImGui::SliderFloat("##", (float*) &opt.mask_blur_size, 1.0f, 255.0f);
        EndRightAlign();
//        opt.update_stereo |= RightAlignedSlider("Strength", (float*) &opt.mask_blur_size, 1.0f, 255.0f);

        NewLine();

        RightAlignNextItem("Inpainting");
        opt.update_stereo |= Checkbox("Enable", &opt.inpainting_enable);
        SameLine(0, 10);
        opt.update_stereo |= Checkbox("Glitched", &opt.inpainting_glitch);
        opt.update_stereo |= SliderFloat("damp", &opt.deviation,     1.0f, 500.0f);
        opt.update_stereo |= SliderInt("diffef", &opt.x,     0.0f, 100.0f);
        opt.update_stereo |= SliderInt("iterat", &opt.y,     1.0f, 255.0f);


//        opt.update_stereo |= RightAlignedSlider("Ciul", (float*) &opt.y, 0.0f, 10.0f);
        Unindent(8.0f);
        NewLine();
    }

    if( CollapsingHeader("Stereo", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent(8.0f);
        NewLine();

        opt.update_stereo |= SliderFloat("##Deviatione", &opt.deviation, 0.0f, 100.0f);
        SameLine();
        AlignTextToFramePadding();
        Text("x");
        SetNextItemWidth(45);
        SameLine();
        static std::string chuj = "1.0";
        opt.update_stereo |= InputText("##Multipliere", &chuj, ImGuiInputTextFlags_CharsDecimal);
        try{
            opt.deviation_multiplier = std::stof(chuj);
        } catch (std::exception& e) {}

        opt.update_stereo |= SliderFloat("damp", &stereo.dampener,     1.0f, 1000.0f);
        opt.update_stereo |= SliderFloat("ipde",  &stereo.ipd,          1.0f, 10.0f);
        opt.update_stereo |= SliderFloat("foca", &stereo.focal_length, 1.0f, 100.0f);
        opt.update_stereo |= SliderFloat("size", &stereo.pixel_size,   0.001f, 1.0f);
        opt.update_stereo |= InputInt("Near Distance", &stereo.near_distance);
        opt.update_stereo |= InputInt("Far Distance", &stereo.far_distance);

                NewLine();
        opt.force_update |= Button("Stereoify");

        Unindent(8.0f);
        NewLine();
    }
    EndChild();




    EndGroup();
}

