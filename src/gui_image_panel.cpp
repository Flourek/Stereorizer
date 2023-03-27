//
// Created by Flourek on 21/03/2023.
//

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "GL/gl3w.h"
#include "header.h"
#include "../libs/IconsFontAwesome5.h"

using namespace ImGui;

void GuiImagePanel(GuiSettings &opt, const cv::Mat &image, float target_width, GLuint texture, std::string &input_path,
                   float &deviation) {
    BeginGroup();
    Text(" [Left eye], %f", GetIO().Framerate);

    BeginChild("LeftImageContainer", ImVec2(target_width, target_width), true, ImGuiWindowFlags_NoScrollbar);
        ImageCenteredWithAspect(texture, target_width, image.cols, image.rows);
    EndChild();

    BeginChild("Input File", ImVec2(target_width, 0));
    if( CollapsingHeader("Imput fil", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent(8.0f);
        AlignTextToFramePadding();
        Text("File:");
        SameLine();
        InputText("##output_path", &input_path, ImGuiInputTextFlags_ReadOnly);
        SameLine();
        Button(ICON_FA_FOLDER_OPEN);


        opt.update_stereo |= SliderInt("Mask", &opt.mask_blur_size, 1.0f, 255.0f);
        opt.update_stereo |= Checkbox("MaskOpacity", &opt.mask_blur);

        Unindent(8.0f);
        NewLine();
    }
//    EndChild();

//    if (opt.size_mismatch){
//        BeginChild("scrolling", ImVec2(243, 35));
////                        PushStyleColor(ImGuiCol_ChildBg, IM_COL32(200,20,20,255));
//        PushItemWidth(235);
//        SameLine(6);
//
//        AlignTextToFramePadding();
//        TextWrapped("Source image and the depth image must be the same size");
//        EndChild();
//
//        SameLine(0, 0);
//
//        BeginChild("scrollinge", ImVec2(60, 35));
//        PushStyleColor(ImGuiCol_Button, IM_COL32(150,20,20,255));
//        Button("Resize", ImVec2(60, 35));
//        PopStyleColor(2);
//        EndChild();
//    }

//    BeginChild("chujj", ImVec2(target_width, 0));
    if( CollapsingHeader("Inpainting", ImGuiTreeNodeFlags_DefaultOpen) ){
        opt.update_stereo |= Checkbox("Glitched", &opt.inpainting_glitch);
        opt.update_stereo |= SliderInt("Masky", &opt.y, 0.0f, 10.0f);
        NewLine();
    }
//    EndChild();

//    PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
//    BeginChild("Black spacer", ImVec2(target_width, 100));
//    EndChild();
//    PopStyleColor();

//    BeginChild("StereoOptions", ImVec2(target_width, 0));
    if( CollapsingHeader("Stereo", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent(8.0f);
        SeparatorText("Deviation");

        opt.update_stereo |= SliderFloat("##Deviatione", &opt.deviation, 0.0f, 100.0f);
        SameLine();
        AlignTextToFramePadding();
        Text("x");
        SetNextItemWidth(45);
        SameLine();
        opt.update_stereo |= InputText("##Multipliere", &opt.deviation_multiplier, ImGuiInputTextFlags_CharsDecimal);
        NewLine();
        opt.force_update |= Button("Stereoify");

        Unindent(8.0f);
        NewLine();
    }
    EndChild();




    EndGroup();
}

