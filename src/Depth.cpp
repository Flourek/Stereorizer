//
// Created by Flourek on 30/03/2023.
//

#include "bezier.hpp"
#include "Depth.h"

Depth::Depth(const std::string &path) : Image(path) {
    cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
    mat.convertTo(mat, CV_8UC1);
    convertToFloat();
    original = mat.clone();
    convertToDisplay();
    updateTexture();
}


void Depth::convertToFloat() {
    mat.convertTo(float_mat, CV_32FC1);

    double depth_min, depth_max;
    minMaxLoc(float_mat, &depth_min, &depth_max);

    if (depth_min != depth_max)
        float_mat = (float_mat - depth_min) / (depth_max - depth_min);
    else
        float_mat = float_mat / 255.0;

}


void Depth::adjust(ImVec2 points[]) {

    mat = original.clone();

    uchar lutValues[256];
    for (int i = 0; i < 256; ++i) {
        float x = (float) i / 256;
        float y = ImGui::CurveValueSmooth(x, 10, points);
        lutValues[i] = cv::saturate_cast<uchar>( y * 256);
    }

    cv::Mat colorMap(1, 256, CV_8UC1, lutValues);

    cv::LUT(mat, colorMap, mat);



//
//    float contrast      = opt.depth_contrast;
//    float brightness    = opt.depth_brigthness;
//    float highlights    = opt.depth_highlights;
//    contrast += 0.5f;
//    brightness -= 0.5f;
//    highlights -= 0.5f;
//
//    contrast = pow(contrast, 3);
//    brightness *= 256;
//
//    typedef cv::Vec3b Pixel;
//    mat *= contrast;
//    mat += brightness;
////    for( int y = 0; y < mat.rows; y++ ) {
////        for( int x = 0; x < mat.cols; x++ ) {
////            mat.at<Pixel>(y,x) =  cv::saturate_cast<Pixel>( contrast * mat.at<Pixel>(y, x)  );
////        }
////    }
//
//    if (opt.depth_invert)
//        cv::bitwise_not(mat, mat);
//

}


void Depth::convertToDisplay() {

    cv::Mat new_display = mat.clone();

    if(color_map != 0) {                                      // 0 for grayscale
        int map;

        switch (color_map) {
            case 1:
                map = cv::COLORMAP_INFERNO;
                break;
            case 2:
                map = cv::COLORMAP_JET;
                break;
            case 3:
                map = cv::COLORMAP_TURBO;
                break;
            case 4:
                map = cv::COLORMAP_PARULA;
                break;
        }

        cv::applyColorMap(mat, new_display, map);
    }

    cv::cvtColor(new_display, display_BGRA, cv::COLOR_BGR2BGRA);

}