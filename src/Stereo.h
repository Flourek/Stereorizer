//
// Created by Flourek on 29/03/2023.
//

#ifndef STEREORIZER_STEREO_H
#define STEREORIZER_STEREO_H

#include <opencv2/opencv.hpp>
#include "Image.h"
#include "Depth.h"

class Stereo {
public:
    float deviation = 3;
    class Image& left;
    class Depth& depth;
    class Image& right;
    cv::Mat mask;

    cv::Mat resized_left;
    cv::Mat resized_depth;
    cv::Mat resized_mask;


    Stereo(Image &left, Depth &depth, Image &right, float deviation);

    void run(const GuiSettings& opt);

    cv::Mat maskPostProcess(const GuiSettings &opt);

    static cv::Mat ShiftPixels(cv::Mat& left, cv::Mat& depth, cv::Mat& mask, float deviation);

    static void Inpaint(cv::Mat& right, cv::Mat& mask, float deviation);

    void resizeAll(int scale);


};


#endif //STEREORIZER_STEREO_H
