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

//    convertToFloat();
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

}

void type2str(const cv::Mat& mat) {
    int type = mat.type();
    std::string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch ( depth ) {
        case CV_8U:  r = "8U"; break;
        case CV_8S:  r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;
    }

    r += "C";
    r += (chans+'0');

    std::cout << r << std::endl;
}

void Depth::convertToDisplay() {
    type2str(mat);
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