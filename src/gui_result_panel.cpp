//
// Created by Flourek on 21/03/2023.
//

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "header.h"
#include "../libs/IconsFontAwesome5.h"

using namespace ImGui;

void GuiResultPanel(GuiSettings &opt, GLuint &texture, GLuint &zoom_texture, cv::Mat &result, const cv::Mat &mask,
                    const cv::Mat &image, float width, std::string &output_path) {
    BeginGroup();

    Text("Result - \"Right eye\"");
    BeginChild("ResultImageContainer", ImVec2(width, width), true, ImGuiWindowFlags_NoScrollbar);

        // Main image
        ImageCenteredWithAspect(texture, width, result.cols, result.rows);

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 pos = ImGui::GetCursorScreenPos();

    // Tool tip with zoomed in view
        ImGuiHoveredFlags hover_flags = ImGuiHoveredFlags_AllowWhenBlockedByActiveItem;
        ImGuiHoveredFlags tooltip_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;

        if (ImGui::IsItemHovered(hover_flags) || opt.zoom_window_stick ) {

            ImVec2 cursor, window_pos;
            if (!opt.zoom_window_stick){
                cursor = io.MousePos;
                window_pos = cursor;
                window_pos.x = std::clamp(cursor.x, pos.x, pos.x + width - 110);
                window_pos.y += 30;
                SetNextWindowPos(window_pos);
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

                if (opt.zoom_level == 0) opt.zoom_level = 0.5;

                //Image coordinates
                float x = cursor.x - pos.x - size/2/opt.zoom_level + 8;
                float y = cursor.y - pos.y - size/2/opt.zoom_level + 4 + width;

                float w = width;
                float h = width * ( (float) result.rows / (float) result.cols);
                std::cout << x << " " << y << std::endl;

                // Normalize to [0,1]
                ImVec2 uv0 = ImVec2( x / w , y / h);
                ImVec2 uv1 = ImVec2( (x + size / opt.zoom_level) / w ,
                                     (y + size / opt.zoom_level) / h );

                ImGui::Image(reinterpret_cast<void*>( static_cast<intptr_t>( zoom_texture ) ),
                             ImVec2(110, 110),
                             uv0, uv1);
                SetNextItemWidth(50);
                InputFloat("##Zoom", &opt.zoom_level, 0.0f, 0.0f, "x%.1f");
                SameLine();
                if ( Button(ICON_FA_SEARCH_PLUS) ) opt.zoom_level += 0.5f;
                SameLine();
                if ( Button(ICON_FA_SEARCH_MINUS) ) opt.zoom_level -= 0.5f;

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
        opt.update_stereo |= ImGui::Checkbox("Anaglyph overlay", &opt.anaglyph_overlay);
        opt.update_stereo |= ImGui::Checkbox("Mask overlay", &opt.mask_overlay);
        ImGui::Checkbox("Live Refresh", &opt.live_refresh);
        Unindent( 8.0f );
    }
    EndChild();

    SameLine();

    BeginChild("Saving", ImVec2(0.7 * width, 0));
    if( CollapsingHeader("Saving", ImGuiTreeNodeFlags_DefaultOpen) ){
        Indent( 8.0f );

        BeginChild("Include", ImVec2(0.3 * width, 100));
        Checkbox("Side By Side", &opt.save_sbs    );
        Checkbox("Stereo",       &opt.save_stereo );
        Checkbox("Depth",        &opt.save_depth  );
        Checkbox("Mask",         &opt.save_mask  );
        EndChild();
        SameLine();
        BeginChild("Save", ImVec2(0, 100));
        if ( ImGui::Button("Save [S]", ImVec2(0.32 * width, 32)) ){

            if(opt.save_stereo)
                cv::imwrite(output_path + "chuj_RIGHT.jpg" , result);
            if(opt.save_mask)
                cv::imwrite(output_path + "chuj_MASK.jpg" , maskPostProcess(mask, opt));


            if(opt.save_sbs){
                cv::Mat sbs;
                cv::hconcat(image, result, sbs);
                cv::imwrite(output_path + "chuj_SBS_FLAT.jpg" , sbs);
            }


        }

        if ( ImGui::Button("Show in Explorer", ImVec2(0.32 * width, 32)) )
            cv::imwrite(output_path + "chuj_SBS_FLAT.jpg" ,result);
        EndChild();

        AlignTextToFramePadding();
        Text("Save to:");
        SameLine();
        InputText("##output_path", &output_path, ImGuiInputTextFlags_ReadOnly);
        SameLine();
        Button(ICON_FA_FOLDER_OPEN);

        Unindent( 8.0f );
    }
    EndChild();
    EndGroup();
}