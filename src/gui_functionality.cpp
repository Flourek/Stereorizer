//
// Created by Flourek on 19/03/2023.
//

#include <opencv2/opencv.hpp>
#include "GL/gl3w.h"
#include "nfd.h"
#include "imgui.h"
#include "header.h"
#include "GLFW/glfw3.h"

cv::Mat adjustDepth(const cv::Mat& input_depth, float contrast, float brightness, float highlights, GuiSettings& flags){

    cv::Mat depth = input_depth.clone();

    contrast += 0.5f;
    brightness -= 0.5f;
    highlights -= 0.5f;

    contrast = pow(contrast, 3);
    brightness *= 256;

    for( int y = 0; y < depth.rows; y++ ) {
        for( int x = 0; x < depth.cols; x++ ) {
            depth.at<uchar>(y,x) =  cv::saturate_cast<uchar>( contrast * depth.at<uchar>(y, x) + brightness );
        }
    }

    if (flags.depth_invert)
        cv::bitwise_not(depth, depth);

    return depth;
}

cv::Mat updateStereo(const cv::Mat &input, cv::Mat depth, GuiSettings &opt, cv::Mat &mask) {
    cv::Mat result;

    // Normalize for every image size and multiply by the ui setting
    float multiplier = 1.0f;
    try{
        multiplier = stof(opt.deviation_multiplier);
    } catch (std::exception& e) {}

    float deviation = opt.deviation;
    deviation *= ((float) input.cols / 1000);
    deviation *= multiplier;

    result = shift_pixels(input, depth, 3, mask, deviation);
    result = inpaint(input, result, deviation);

    if (opt.anaglyph_overlay)
        result = anaglyphize(input, result);

    return result;
}

cv::Mat maskPostProcess(const cv::Mat &mask, GuiSettings &opt) {
    cv::Mat blurred, new_mask = mask.clone();

    // Invert
    bitwise_not(new_mask, new_mask);

    // Remove noise
    int size = (int)opt.mask1;

    cv::Mat kernel = cv::Mat::ones(size, size, CV_8UC1);
    cv::morphologyEx(new_mask, new_mask, cv::MORPH_CLOSE, kernel);
    cv::erode(new_mask, new_mask, kernel);


    cv::blur(new_mask, blurred, cv::Size(opt.mask2, opt.mask2 ) );
    new_mask += blurred;

    return new_mask;
}


void ImageCenteredWithAspect(GLuint texture, int target_width, int width, int height) {

    ImVec2 display_size, center_pos, pivot;
    float aspect = (float) height / width;

    if (aspect > 1){
        display_size =  ImVec2(target_width / aspect, target_width );
        center_pos = ImVec2(ImGui::GetWindowPos().x + target_width / 2, ImGui::GetWindowPos().y );
        pivot = ImVec2(0.5, 0);
    } else {
        display_size = ImVec2(target_width, target_width * aspect );
        center_pos = ImVec2(ImGui::GetWindowPos().x , ImGui::GetWindowPos().y + target_width / 2);
        pivot = ImVec2(0, 0.5);
    }
    ImGui::SetNextWindowPos( center_pos, 0, pivot);

    ImGui::BeginChild("LeftImageCenterer", display_size);
    ImGui::Image( reinterpret_cast<void*>( static_cast<intptr_t>( texture ) ), display_size);
    ImGui::EndChild();

}

void convertMatToTexture(cv::Mat img, GLuint &texture, GLint gl_filter, float scale) {

    cv::cvtColor(img, img, cv::COLOR_BGR2RGBA);

    cv::Mat resized_img;
    cv::resize(img, resized_img, cv::Size(img.cols / scale, img.rows / scale));

// Generate the texture
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

    GLint internal_format = GL_RGBA;
    GLenum format = GL_RGBA;

    glTexImage2D( GL_TEXTURE_2D, 0, internal_format, resized_img.cols, resized_img.rows, 0, format, GL_UNSIGNED_BYTE, resized_img.data );

}


void changeInputImage(cv::Mat& input_image, const std::string& input_path, const cv::Mat& depth, GuiSettings& flags){

    cv::Mat new_image = cv::imread(input_path, cv::IMREAD_COLOR);

    if (!new_image.empty())
        input_image = new_image.clone();
    else
        std::cout << "imag empty: " << input_path << std::endl;

//    flags.size_mismatch = checkSizeMismatch(input_image, depth);
}


bool checkSizeMismatch(const cv::Mat& image, const cv::Mat& depth){
    return (image.size != depth.size);
}




// Get filepath from user dropping a file on the window
// Seems this goofy GLFW Callback function is the only way to do it and you can't pass ur own arguments to it
std::shared_ptr<std::string> chuj;
GuiSettings* flags_global;

bool openFileDialog(std::string& input_path, std::string& filename, const std::string& default_path){
    nfdchar_t *outPath;
    auto def_path = const_cast<nfdchar_t *>(default_path.c_str());
    nfdresult_t result = NFD_OpenDialog(&outPath, def_path, true);

    if ( result == NFD_OKAY ) {
        input_path = outPath;
        filename = input_path.substr(input_path.find_last_of('\\') + 1);
        free(outPath);
        return true;
    }

    return false;
}


void dropCallback(GLFWwindow *window, int count, const char** paths){
    *chuj = paths[0];
    (*flags_global).update_input = true;
}

void dragDropInputFile(GLFWwindow *window, std::shared_ptr<std::string> output_path, GuiSettings &flags) {
    glfwSetDropCallback(window, dropCallback);
    chuj = output_path;
    flags_global = &flags;
    std::cout << &output_path << std::endl;
}






