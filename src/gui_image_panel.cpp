//
// Created by Flourek on 21/03/2023.
//

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "GL/glew.h"
#include "header.h"
#include "Image.h"
#include "Stereo.h"

#include "Opt.h"

// Returns true if ImGui was interacted with
void GuiImagePanel(Image &left, Stereo &stereo, float target_width) {

    using namespace ImGui;
    bool update = false;

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

    if (Opt::GetFlags().size_mismatch){
        BeginChild("scrolling", ImVec2(243, 35));
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
            ImGui::SliderFloat("##", (float*) &Opt::Get().mask_blur_size, 1.0f, 255.0f);
        EndRightAlign();
//        opt.update_stereo |= RightAlignedSlider("Strength", (float*) &opt.mask_blur_size, 1.0f, 255.0f);

        NewLine();

        RightAlignNextItem("Inpainting");
        update |= Checkbox("Enable", &Opt::Get().inpainting_enable);
        SameLine(0, 10);
        update |= Checkbox("Glitched", &Opt::Get().inpainting_glitch);
        update |= SliderFloat("damp",  &Opt::Get().deviation,     1.0f, 500.0f);
        update |= SliderInt("diffef",  &Opt::Get().x,             0.0f, 100.0f);
        update |= SliderInt("iterat",  &Opt::Get().y,             1.0f, 255.0f);


//        opt.update_stereo |= RightAlignedSlider("Ciul", (float*) &opt.y, 0.0f, 10.0f);
        Unindent(8.0f);
        NewLine();
    }

    if( CollapsingHeader("Stereo", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent(8.0f);
        NewLine();

        update |= SliderFloat("##Deviatione", &Opt::Get().deviation, 0.0f, 100.0f);
        SameLine();
        AlignTextToFramePadding();
        Text("x");
        SetNextItemWidth(45);
        SameLine();
        static std::string chuj = "1.0";
        update |= InputText("##Multipliere", &chuj, ImGuiInputTextFlags_CharsDecimal);
        try{
            Opt::Get().deviation_multiplier = std::stof(chuj);
        } catch (std::exception& e) {}

        update |= SliderFloat("damp", &stereo.dampener,     1.0f, 1000.0f);
        update |= SliderFloat("ipde",  &stereo.ipd,          1.0f, 10.0f);
        update |= SliderFloat("foca", &stereo.focal_length, 1.0f, 100.0f);
        update |= SliderFloat("size", &stereo.pixel_size,   0.001f, 1.0f);
        update |= InputInt("Near Distance", &stereo.near_distance);
        update |= InputInt("Far Distance", &stereo.far_distance);

        NewLine();
        update |= Button("Stereoify");

        Unindent(8.0f);
        NewLine();
    }
    EndChild();
    EndGroup();

    Opt::GetFlags().update_stereo = update;

}

