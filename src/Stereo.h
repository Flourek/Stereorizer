//
// Created by Flourek on 29/03/2023.
//

#ifndef STEREORIZER_STEREO_H
#define STEREORIZER_STEREO_H

#include <opencv2/opencv.hpp>
#include "Image.h"
#include "Depth.h"
#include "header.h"

class Stereo {
public:
    float deviation = 3;
    class Image& left;
    class Depth& depth;
    class Image& right;
    cv::Mat mask;
    cv::Size target_size;

    float ipd = 6.5;
    float focal_length = 18.0;
    float pixel_size = 0.17;
    float dampener = 1.0f;
    int near_distance = 100;
    int far_distance = 1000;

    Stereo(Image &left, Depth &depth, Image &right, float deviation);

    void run(const GuiSettings& opt);

    cv::Mat maskPostProcess(const GuiSettings &opt);

    double calcBinocularDisparity(double distance);

    void ShiftPixels(const GuiSettings &opt);

    static void Inpaint(cv::Mat& right, cv::Mat& mask, float deviation);

    void resizeAll(int scale = 1);



};


#endif //STEREORIZER_STEREO_H
