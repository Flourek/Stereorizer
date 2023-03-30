//
// Created by Flourek on 15/03/2023.
//

#ifndef STEREOREO_HEADER_H
#define STEREOREO_HEADER_H

#include <opencv2/opencv.hpp>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "Image.h"
//#include "Stereo.h"

struct GuiSettings {
    bool depth_invert       = false;
    bool depth_grayscale    = false;
    bool anaglyph_overlay   = false;
    bool mask_overlay       = false;
    bool live_refresh       = true;
    bool mask_blur          = false;
    int mask_blur_size      = 50;
    double viewport_scale   = 2;

    bool update_stereo      = false;
    bool update_input       = true;
    bool update_depth       = false;
    bool force_update       = false;
    bool size_mismatch      = false;

    bool save_sbs           = true;
    bool save_depth         = false;
    bool save_mask          = false;
    bool save_stereo        = false;

    float depth_contrast = 0.5f;
    float depth_brigthness = 0.5f;
    float depth_highlights = 0.5f;

    bool inpainting_glitch  = false;

    bool zoom_window_stick  = false;
    float zoom_level        = 4.0f;
    ImVec2 zoom_click_pos;

    float deviation         = 100.0f;
    std::string deviation_multiplier  = "1.0";

    bool midas_first_execution = true;

    int x = 1;
    int y = 1;
    int z = 1;

    std::string chuj;
};






//cv::Mat anaglyphize(cv::Mat left_img, cv::Mat right_img);

void convertMatToTexture(cv::Mat img, GLuint &texture, GLint gl_filter = GL_LINEAR, float scale = 2);

bool openFileDialog(std::string& input_path, std::string& filename, const std::string& default_path);

void changeInputImage(cv::Mat& input_image, const std::string& input_path, const cv::Mat& depth, GuiSettings& flags);

cv::Mat adjustDepth(const cv::Mat& input_depth, float contrast, float brightness, float highlights, GuiSettings& flags);

cv::Mat updateStereo(class Stereo &stereo, GuiSettings &opt);



// Gui


void GuiDepthPanel(class Image& depth, GuiSettings &opt, float width);

void GuiImagePanel(class Image& left, GuiSettings &opt, float target_width);

void GuiResultPanel(class Stereo &stereo, GuiSettings &opt, float width);

void ImageCenteredWithAspect(GLuint &texture, int target_width, float aspect);

void dragDropInputFile(GLFWwindow *window, std::shared_ptr<std::string> output_path, GuiSettings &flags);

void TextCentered(const std::string& text);

void GuiFileDialog(const std::string& text, std::string& path);

bool RightAlignedSlider(const std::string& label, float *x, float v_min, float v_max);

void RightAlignNextItem();

// Depth

int generateDepthMap(std::string input_path, std::string model_path, cv::Mat &depth_image, GuiSettings &opt);

void importModules();

bool checkSizeMismatch(const cv::Mat& image, const cv::Mat& depth);

cv::Mat maskPostProcess(const cv::Mat &mask, GuiSettings &opt);


#endif //STEREOREO_HEADER_H
