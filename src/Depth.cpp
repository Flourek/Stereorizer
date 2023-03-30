//
// Created by Flourek on 30/03/2023.
//

#include "Depth.h"

Depth::Depth(const std::string &path, GuiSettings &opt) : Image(path, opt) {
    convert_to_float();
    original = mat.clone();
}

void Depth::convert_to_float() {
    cv::cvtColor(mat, float_mat, cv::COLOR_BGR2GRAY);
    float_mat.convertTo(float_mat, CV_32FC1);

    double depth_min, depth_max;
    minMaxLoc(float_mat, &depth_min, &depth_max);

    if (depth_min != depth_max)
        float_mat = (float_mat - depth_min) / (depth_max - depth_min);
    else
        float_mat = float_mat / 255.0;

//    right.mat = left.mat.clone();
//    right.createTexture();
//            convertMatToTexture(result, result_texture, GL_LINEAR, 2);
//    stereo.depth.mat = depth_float;

}

void Depth::adjust() {

    mat = original.clone();

    float contrast      = opt.depth_contrast;
    float brightness    = opt.depth_brigthness;
    float highlights    = opt.depth_highlights;
    contrast += 0.5f;
    brightness -= 0.5f;
    highlights -= 0.5f;

    contrast = pow(contrast, 3);
    brightness *= 256;

    for( int y = 0; y < mat.rows; y++ ) {
        for( int x = 0; x < mat.cols; x++ ) {
            mat.at<uchar>(y,x) =  cv::saturate_cast<uchar>( contrast * mat.at<uchar>(y, x) + brightness );
        }
    }

    if (opt.depth_invert)
        cv::bitwise_not(mat, mat);

    convert_to_float();
}