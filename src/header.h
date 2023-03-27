//
// Created by Flourek on 15/03/2023.
//

#ifndef STEREOREO_HEADER_H
#define STEREOREO_HEADER_H

#include <opencv2/opencv.hpp>
#include "GLFW/glfw3.h"
#include "imgui.h"


struct GuiSettings {
    bool depth_invert       = false;
    bool anaglyph_overlay   = false;
    bool mask_overlay       = false;
    bool live_refresh       = true;
    bool mask_blur          = false;
    int mask_blur_size      = 50;
    double viewport_scale      = 2;

    bool update_stereo      = false;
    bool update_input       = true;
    bool update_depth       = false;
    bool force_update       = false;
    bool size_mismatch      = false;

    bool save_sbs           = true;
    bool save_depth         = false;
    bool save_mask          = false;
    bool save_stereo        = false;

    bool inpainting_glitch  = false;

    bool zoom_window_stick  = false;
    float zoom_level        = 4.0f;
    ImVec2 zoom_click_pos;

    float deviation             = 100.0f;
    std::string deviation_multiplier  = "1.0";

    bool midas_first_execution = true;

    int x = 1;
    int y = 1;
    int z = 1;

    std::string chuj;
};

class Stereo {
public:
    float deviation = 3;
    cv::Mat left;
    cv::Mat right;
    cv::Mat depth;
    cv::Mat mask;

    Stereo();
    Stereo(const cv::Mat &left, const cv::Mat &depth, cv::Mat &right, float deviation);

    struct ShiftPixels {
        Stereo& stereo;

        explicit ShiftPixels(Stereo& stereo) : stereo(stereo) {}

        static cv::Mat run(Stereo &stereo);
        void operator () (float &pixel, const int * position) const;
    };

    struct Inpaint {
        Stereo& stereo;
        explicit Inpaint(Stereo& stereo) : stereo(stereo) {}

        static cv::Mat run(Stereo &stereo);
        void operator () (uchar &pixel, const int * position) const;
    };

};


cv::Mat inpaint(const cv::Mat& left_img,cv:: Mat right_img, float deviation, cv::Mat &mask);

void shift_pixels(const cv::Mat &left_img, cv::Mat &depth, cv::Mat &result, cv::Mat &mask, float deviation);

cv::Mat anaglyphize(cv::Mat left_img, cv::Mat right_img);

void convertMatToTexture(cv::Mat img, GLuint &texture, GLint gl_filter = GL_LINEAR, float scale = 2);

bool openFileDialog(std::string& input_path, std::string& filename, const std::string& default_path);

void changeInputImage(cv::Mat& input_image, const std::string& input_path, const cv::Mat& depth, GuiSettings& flags);

cv::Mat adjustDepth(const cv::Mat& input_depth, float contrast, float brightness, float highlights, GuiSettings& flags);

cv::Mat updateStereo(Stereo &stereo, GuiSettings &opt);

void ImageCenteredWithAspect(GLuint texture, int target_width, int width, int height);

void dragDropInputFile(GLFWwindow *window, std::shared_ptr<std::string> output_path, GuiSettings &flags);

void GuiDepthPanel(GuiSettings &flags, GLuint depth_texture, float width, float input_aspect, std::string filename,
                   std::string input_path, std::string output_path, float &depth_contrast, float &depth_brigthness,
                   float &depth_highlights, cv::Mat &depth, cv::Mat &input);

void GuiImagePanel(GuiSettings &opt, const cv::Mat &image, float target_width, GLuint texture, std::string &input_path,
                   float &deviation);

void GuiResultPanel(GuiSettings &opt, GLuint &texture, GLuint &zoom_texture, cv::Mat &result, const cv::Mat &mask,
                    const cv::Mat &image, float width, std::string &output_path);

int generateDepthMap(std::string input_path, std::string model_path, cv::Mat &depth_image, GuiSettings &opt);

bool checkSizeMismatch(const cv::Mat& image, const cv::Mat& depth);

cv::Mat maskPostProcess(const cv::Mat &mask, GuiSettings &opt);

void importModules();
#endif //STEREOREO_HEADER_H
