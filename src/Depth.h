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

    Depth(const std::string &path, struct GuiSettings &opt);

    void convert_to_float();

    void adjust();

};


#endif //STEREORIZER_DEPTH_H
