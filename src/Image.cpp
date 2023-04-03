//
// Created by Flourek on 29/03/2023.
//

#include "Image.h"
#include "GL/glcorearb.h"

Image::Image(const std::string &path) {
    changeImage(path);
    convertToDisplay();
}

Image::Image(const cv::Mat &input_mat) {
    mat = input_mat.clone();
    convertToDisplay();
    createTexture();
}


void Image::createTexture() {
    convertToDisplay();

    // Generate the texture
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

    GLint internal_format = GL_RGBA;
    GLenum format = GL_BGRA;

    glTexImage2D( GL_TEXTURE_2D, 0, internal_format, display_BGRA.cols, display_BGRA.rows, 0, format, GL_UNSIGNED_BYTE, display_BGRA.data );

}

void Image::convertToDisplay(){
    display_BGRA = cv::Mat(mat.rows, mat.cols, CV_8UC4);

    if(mat.channels() == 3)
        cv::cvtColor(mat, display_BGRA, cv::COLOR_BGR2BGRA);
    else if (mat.channels() == 1)
        cv::cvtColor(mat, display_BGRA, cv::COLOR_GRAY2BGRA);
}


void Image::updateTexture() {

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, display_BGRA.cols, display_BGRA.rows, GL_BGRA,
                    GL_UNSIGNED_BYTE, display_BGRA.data);
}


void Image::resize(float scale) {
    cv::resize(original, mat, cv::Size(original.cols / scale, original.rows / scale));
    createTexture();
}


int Image::getScaleSuggestion(){
    int scale = sqrt( mat.total() / 1000000 );
    if (scale == 0) scale = 1;
    return scale;
}


void Image::changeImage(const std::string &new_path) {
    path = new_path;

    // Update the original image if it exists
    cv::Mat new_image = cv::imread(path, cv::IMREAD_COLOR);

    if (new_image.empty())
        return;

    original = new_image.clone();
    resize(1);

    // Update aspect ratio
    aspect = (float) mat.rows / (float) mat.cols;

    createTexture();
}


// Returns true if sizes are the same
bool Image::checkMismatch(const Image& a, const Image& b){
    return ( a.mat.size != b.mat.size );
}