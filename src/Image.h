//
// Created by Flourek on 29/03/2023.
//

#ifndef STEREORIZER_IMAGE_H
#define STEREORIZER_IMAGE_H

#include <opencv2/opencv.hpp>
#include "GLFW/glfw3.h"
#include "header.h"

class Image {
public:
    cv::Mat mat;
    cv::Mat display_BGRA;
    GLuint texture;
    GLint gl_filter = GL_LINEAR;
    float aspect;
    std::string path;
    std::string filename;
    struct GuiSettings &opt;

    Image(const std::string& path, GuiSettings& opt);

    void changeImage(const std::string &new_path);

    void resize(float scale = -1);

    void createTexture();

    void updateTexture();

    static bool compareSize(const Image& a, const Image& b);


};


#endif //STEREORIZER_IMAGE_H
