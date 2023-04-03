//
// Created by Flourek on 30/03/2023.
//

#ifndef STEREORIZER_DEPTH_H
#define STEREORIZER_DEPTH_H
#include "Image.h"

class Depth : public Image {
public:
    cv::Mat original;
    cv::Mat float_mat;
    Image lut_preview = Image(cv::Mat(10, 256, CV_8UC1));
    int color_map = 1; // Inferno

    Depth(const std::string &path);

    void convertToFloat();

    void convertToDisplay();

    void resize(int scale);

    void adjust(ImVec2 points[]);

};


#endif //STEREORIZER_DEPTH_H
