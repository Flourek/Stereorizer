//
// Created by Flourek on 29/03/2023.
//

#include "Image.h"
#include "GL/glcorearb.h"

Image::Image(const std::string &path) {
    display_BGRA = cv::Mat(mat.rows, mat.cols, CV_8UC4);
    changeImage(path);
    createTexture();

}


void Image::createTexture() {
    convertToDisplay();

    // Generate the texture
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

    GLint internal_format = GL_RGBA;
    GLenum format = GL_BGRA;

    glTexImage2D( GL_TEXTURE_2D, 0, internal_format, display_BGRA.cols, display_BGRA.rows, 0, format, GL_UNSIGNED_BYTE, display_BGRA.data );

}

void Image::convertToDisplay(){
    cv::cvtColor(mat, display_BGRA, cv::COLOR_BGR2BGRA);
};

void Image::updateTexture() {

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, display_BGRA.cols, display_BGRA.rows, GL_BGRA,
                    GL_UNSIGNED_BYTE, display_BGRA.data);
}


void Image::resize(float scale) {
    cv::resize(mat, display_BGRA, cv::Size(mat.cols / scale, mat.rows / scale));
    createTexture();
}


void Image::changeImage(const std::string &new_path) {
    path = new_path;

    // Update the original image if it exists
    cv::Mat new_image = cv::imread(path, cv::IMREAD_COLOR);

    if (!new_image.empty())
        mat = new_image.clone();
    else
        std::cout << "imag empty: " << path << std::endl;

    // Update aspect ratio
    aspect = (float) mat.rows / (float) mat.cols;


    // Resize the input for performance reasons (1000x1000 image = 1)
//    int scale = sqrt( new_image.total() / 1000000 );
//    if (scale == 0) scale = 1;
//    opt.viewport_scale = scale;


//    resize(scale);
    createTexture();

}

// Returns true if sizes are the same
bool Image::checkMismatch(const Image& a, const Image& b){
    return ( a.mat.size != b.mat.size );
}