//
// Created by Flourek on 19/04/2023.
//

#ifndef STEREORIZER_OPT_H
#define STEREORIZER_OPT_H
#include "iostream"
#include "imgui.h"

struct flags{
    bool update_stereo      = false;
    bool update_input       = true;
    bool update_depth       = false;
    bool force_update       = false;
    bool size_mismatch      = false;
    bool live_refresh       = true;
    bool midas_run          = false;
    bool midas_first_execution = true;
    bool vr_enabled          = false;
};


class Opt {
private:
    static Opt s_Instance;

public:
    static Opt& Get() { return s_Instance; }

    static flags& GetFlags() { return Get().flag; }


//    Opt(Opt &other) = delete;
    void operator=(const Opt &) = delete;


public:
    // Logic flags
    flags flag;

    // Variables
    bool depth_invert       = false;
    bool anaglyph_overlay   = false;
    bool mask_overlay       = false;
    bool mask_blur          = false;
    int mask_blur_size      = 50;
    double viewport_scale   = 2;

    bool save_sbs           = true;
    bool save_depth         = false;
    bool save_mask          = false;
    bool save_stereo        = false;

    int depth_color_map     = 1;
    bool inpainting_enable  = true;
    bool inpainting_glitch  = false;

    // Paths
    std::string image_path;
    std::string model_path = "C:/Users/Flourek/CLionProjects/Stereorizer/weights/dpt_beit_large_512.pt";
    std::string midas_path = "C:/Users/Flourek/CLionProjects/Stereorizer/MiDaS/";
    std::string output_path = "C:/Users/Flourek/CLionProjects/Stereorizer/output/";

    float v[5] = { 0.950f, 0.050f, 0.795f, 0.035f };
    int chuej = 3;

    bool zoom_window_stick  = false;
    float zoom_level        = 4.0f;
    ImVec2 zoom_click_pos;

    float deviation         = 100.0f;
    float deviation_multiplier = 1.0f;


    int x = 14;
    int y = 14;
    int z = 1;

};


#endif //STEREORIZER_OPT_H

