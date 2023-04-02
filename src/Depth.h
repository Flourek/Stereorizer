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
    int color_map = 1; // Inferno

    Depth(const std::string &path);

    void convertToFloat();

    void convertToDisplay();

    void adjust(ImVec2 points[]);

};


#endif //STEREORIZER_DEPTH_H
