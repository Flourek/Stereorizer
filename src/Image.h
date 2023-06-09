//
// Created by Flourek on 29/03/2023.
//

#ifndef STEREORIZER_IMAGE_H
#define STEREORIZER_IMAGE_H

#include <opencv2/opencv.hpp>
#include "GLFW/glfw3.h"

class Image {
public:
    cv::Mat original;
    cv::Mat mat;
    cv::Mat display_BGRA;
    GLuint texture;
    GLint gl_filter = GL_LINEAR;
    float aspect;
    std::string path;
    std::string filename;

    Image(const std::string& path);

    Image(const cv::Mat& input_mat );

    void changeImage(const std::string &new_path);

    void resize(float scale = -1);

    void createTexture();

    void updateTexture();

    void convertToDisplay();

    int getScaleSuggestion();

    static bool checkMismatch(const Image& a, const Image& b);


};


#endif //STEREORIZER_IMAGE_H
