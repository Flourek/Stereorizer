//
// Created by Flourek on 15/03/2023.
//

#ifndef STEREOREO_HEADER_H
#define STEREOREO_HEADER_H

#include <opencv2/opencv.hpp>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "Depth.h"
#include "Python.h"
#include "Stereo.h"


struct GuiSettings {
    // Logic flags
    bool update_stereo      = false;
    bool update_input       = true;
    bool update_depth       = false;
    bool force_update       = false;
    bool size_mismatch      = false;
    bool live_refresh       = true;
    bool midas_run          = false;
    bool midas_first_execution = true;
    bool vr_enabled          = false;



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
    std::string chuj;
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







//cv::Mat anaglyphize(cv::Mat left_img, cv::Mat right_img);

void convertMatToTexture(cv::Mat img, GLuint &texture, GLint gl_filter = GL_LINEAR, float scale = 2);

bool openFileDialog(std::string& input_path, std::string& filename, const std::string& default_path);

void changeInputImage(cv::Mat& input_image, const std::string& input_path, const cv::Mat& depth, GuiSettings& flags);

cv::Mat adjustDepth(const cv::Mat& input_depth, float contrast, float brightness, float highlights, GuiSettings& flags);

cv::Mat updateStereo(class Stereo &stereo, GuiSettings &opt);



// Gui
void setImGuiSettings(float resolution_scale);

void dropCallback(GLFWwindow *window, int count, const char** paths);

void GuiDepthPanel(Depth &depth, float width);

void GuiImagePanel(Image &left, Stereo &stereo, float target_width);

void GuiResultPanel(struct Stereo &stereo, class Image &zoom, float width);

void ImageCenteredWithAspect(GLuint &texture, int target_width, float aspect);

void dragDropInputFile(GLFWwindow *window, std::shared_ptr<std::string> output_path);

void TextCentered(const std::string& text);

void GuiFileDialog(const std::string& text, std::string& path);

bool RightAlignedSlider(const std::string& label, float *x, float v_min, float v_max);

void RightAlignNextItem(char const *text = "");

void BeginRightAlign(char const *label, int item_count
);

void EndRightAlign();

// Depth

int generateDepthMap(std::string input_path, std::string model_path, Depth &depth, PyInterpreterState *interp);

void importModules();

bool checkSizeMismatch(const cv::Mat& image, const cv::Mat& depth);

cv::Mat maskPostProcess(const cv::Mat &mask, const GuiSettings &opt);





#endif //STEREOREO_HEADER_H
