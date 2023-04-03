//
// Created by Flourek on 30/03/2023.
//

#include "bezier.hpp"
#include "Depth.h"
#include "Tracy.hpp"

Depth::Depth(const std::string &path) : Image(path) {
    mat = cv::imread(path, CV_16UC1);
    resize(1);
    original = mat.clone();

    // Initialize lut_preview
    ImVec2 points[2] = {ImVec2(0,0), ImVec2(1,1)};
    adjust(points);

    convertToFloat();
    convertToDisplay();
    updateTexture();
}


void Depth::convertToFloat() {
    mat.convertTo(float_mat, CV_32FC1);

    double depth_min, depth_max;
    minMaxLoc(float_mat, &depth_min, &depth_max);

//    std::cout <<  << " " << depth_min << std::endl;
    if (depth_min != depth_max)
        float_mat = (float_mat - depth_min) / (depth_max - depth_min);
    else
        float_mat = float_mat / UINT16_MAX;

}


void Depth::resize(int scale) {
    cv::resize(mat, mat, cv::Size( mat.cols / scale,  mat.rows / scale));
    createTexture();
}


void Depth::adjust(ImVec2 points[]) {

    mat = original.clone();
    ushort lutValues[UINT16_MAX + 1];
    for (int i = 0; i < UINT16_MAX; ++i) {
        float x = (float) i / UINT16_MAX;
        float y = ImGui::CurveValueSmooth(x, 10, points);
        lutValues[i] = cv::saturate_cast<ushort>( y * UINT16_MAX);
    }

    for (int i = 0; i < UINT16_MAX; i += 256) {
        for (int j = 0; j < 10; ++j) {
            lut_preview.mat.at<uchar>(j, i / 256) = lutValues[i] / 256;
        }        
    }
    lut_preview.convertToDisplay();
    lut_preview.updateTexture();

    mat.forEach<ushort>(
        [&lutValues] ( ushort &pixel, const int * position ){
            pixel = lutValues[pixel];
        }
    );


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
    new_display /= 255;
    new_display.convertTo(new_display, CV_8UC3);

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

        cv::applyColorMap(new_display, new_display, map);
    }


    cv::cvtColor(new_display, display_BGRA, cv::COLOR_BGR2BGRA);

}