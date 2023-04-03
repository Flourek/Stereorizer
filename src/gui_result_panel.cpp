//
// Created by Flourek on 21/03/2023.
//
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "header.h"
#include "Stereo.h"
#include "../libs/IconsFontAwesome5.h"



void GuiResultPanel(struct Stereo &stereo, class Image &zoom, GuiSettings &opt, float width) {
    using namespace ImGui;
    BeginGroup();

    Text(" Right eye");
    SameLine(0, width - 150);
    Text(" FPS: %.0f", GetIO().Framerate);

    BeginChild("ResultImageContainer", ImVec2(width, width), true, ImGuiWindowFlags_NoScrollbar);

        // Main image
    ImageCenteredWithAspect(stereo.right.texture, width, stereo.right.aspect);







    // Tool tip with zoomed in view
    ImGuiHoveredFlags hover_flags = ImGuiHoveredFlags_AllowWhenBlockedByActiveItem;
    ImGuiHoveredFlags tooltip_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;

    if (ImGui::IsItemHovered(hover_flags) || opt.zoom_window_stick ) {


        ImVec2 cursor, popup_pos;

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 min =  ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        int display_width = max.x - min.x;
        int display_height = max.y - min.y;
        int image_width = zoom.mat.cols;
        int image_height = zoom.mat.rows;

        if (!opt.zoom_window_stick){
            cursor = io.MousePos;
            popup_pos = cursor;
            popup_pos.x = std::clamp(cursor.x, min.x, max.x - 110);
            popup_pos.y += 30;
            SetNextWindowPos(popup_pos);
        }
        SetNextWindowSize( ImVec2(130, 170) );
        Begin("foofee1", nullptr, tooltip_flags);

            if (!opt.zoom_window_stick)
                Text("Click to stick");
            else
                Text("Click to unstick");

            float size = 110;
            if (opt.zoom_window_stick)
                cursor = opt.zoom_click_pos;

            if (opt.zoom_level == 0) opt.zoom_level = 1.0f;

            // To image coordinates
            float x = (cursor.x - min.x) / ( (float) display_width  / zoom.mat.cols );
            float y = (cursor.y - min.y) / ( (float) display_height / zoom.mat.rows );

            float margin = size/2 / opt.zoom_level / opt.viewport_scale;
            // Center and zoom
            x -= margin;
            y -= margin;


            // Normalize to [0,1] also zoom again idk
            ImVec2 uv0 = ImVec2( x / image_width   ,
                                 y / image_height  );

            ImVec2 uv1 = ImVec2( (x + size / opt.zoom_level / opt.viewport_scale) / image_width  ,
                                 (y + size / opt.zoom_level / opt.viewport_scale) / image_height );


            ImGui::Image( (void*)(intptr_t) zoom.texture, ImVec2(110, 110), uv0, uv1);


            SetNextItemWidth(50);
            static int zoom_level = 1;
            opt.zoom_level = pow(2, zoom_level);
            InputFloat("##Zoom", &opt.zoom_level, 0.0f, 0.0f, "x%.1f");
            SameLine();
            if ( Button(ICON_FA_SEARCH_PLUS) ) zoom_level += 1;
            SameLine();
            if ( Button(ICON_FA_SEARCH_MINUS) ) zoom_level -= 1;

            if( !IsWindowHovered() &&  !IsAnyItemHovered() ){
                if( ImGui::IsMouseClicked(ImGuiMouseButton_Left) ){
                    opt.zoom_window_stick ^= 1;
                    opt.zoom_click_pos = io.MousePos;
                }
            }

        End();

    }
    EndChild();


    BeginChild("Viewing", ImVec2(0.3 * width, 0));
    if( CollapsingHeader("Viewing Options", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent( 8.0f );
        NewLine();

        opt.update_stereo |= ImGui::Checkbox("Anaglyph overlay", &opt.anaglyph_overlay);
        opt.update_stereo |= ImGui::Checkbox("Mask overlay", &opt.mask_overlay);
        ImGui::Checkbox("Live Refresh", &opt.live_refresh);

        NewLine();
        Text("Viewport scale:");

        std::string format = opt.viewport_scale == 1 ? "1" : "1/%.0f";

        if ( InputDouble("##e", &opt.viewport_scale, 1.0f, 1.0f, format.c_str()) ){
            if ( opt.viewport_scale <= 0 )
                opt.viewport_scale = 1;
            stereo.resizeAll(opt.viewport_scale);
            opt.update_depth |= true;
        }

        Unindent( 8.0f );
    }
    EndChild();

    SameLine();

    BeginChild("Saving", ImVec2(0.7 * width, 0));
    if( CollapsingHeader("Saving", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent( 8.0f );
        NewLine();

        BeginChild("Include", ImVec2(0.3 * width, 100));
            Checkbox("Side By Side", &opt.save_sbs    );
            Checkbox("Stereo",       &opt.save_stereo );
            Checkbox("Depth",        &opt.save_depth  );
            Checkbox("Mask",         &opt.save_mask  );
        EndChild();
        SameLine();

        BeginChild("Save", ImVec2(0, 100));
            if ( ImGui::Button("Save [S]", ImVec2(0.32 * width, 32)) ){

//                if(opt.save_stereo)
//                    cv::imwrite(output_path + "chuj_RIGHT.jpg" , right.original);
                if(opt.save_mask)
                    cv::imwrite(opt.output_path + "chuj_MASK.jpg" , stereo.maskPostProcess(opt));


                if(opt.save_sbs){
                    cv::Mat sbs;
                    cv::hconcat(stereo.resized_left, stereo.right.mat, sbs);
                    cv::imwrite(opt.output_path + "chuj_SBS_FLAT.jpg" , sbs);
                }


            }

//            if ( ImGui::Button("Show in Explorer", ImVec2(0.32 * width, 32)) )
//                cv::imwrite(output_path + "chuj_SBS_FLAT.jpg" ,result);
        EndChild();

        AlignTextToFramePadding();
        Text("Save to:");
        SameLine();
//        InputText("##output_path", &output_path, ImGuiInputTextFlags_ReadOnly);
        SameLine();
        Button(ICON_FA_FOLDER_OPEN);

        Unindent( 8.0f );
    }
    EndChild();
    EndGroup();

}