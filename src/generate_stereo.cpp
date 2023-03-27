//
// Created by Flourek on 15/03/2023.
//

#include <opencv2/opencv.hpp>
#include "header.h"
#include "Tracy.hpp"

using namespace std;
using namespace cv;


//Stereo::Stereo() {}

Stereo::Stereo(const cv::Mat &left, const cv::Mat &depth, cv::Mat &right, float deviation) {
    this->left = left.clone();
    this->right = right;
    this->depth = depth.clone();
    this->deviation = deviation;
    this->mask = cv::Mat(left.rows, left.cols, CV_8UC1);
}



cv::Mat Stereo::ShiftPixels::run(Stereo &stereo) {
    Stereo::ShiftPixels sp(stereo);
    stereo.mask =0;
    stereo.depth.forEach<float>( sp );
    return stereo.right;
}


// Functor for use with foreach() on a depth map
void Stereo::ShiftPixels::operator () (float &pixel, const int * position) const {
//    std::cout << "chujxD" << std::endl;
    int row = position[0];
    int col = position[1];

    int dis = pixel * stereo.deviation;
    int col_r = col - dis;

    if (col_r >= 0 && col_r < stereo.left.cols) {
        stereo.right.at<cv::Vec3b>(row, col_r) = stereo.left.at<cv::Vec3b>(row, col);
        stereo.mask.at<uchar>(row, col_r) = 255;
    }
}


cv::Mat Stereo::Inpaint::run(Stereo &stereo) {
    Stereo::Inpaint functor(stereo);
    cv::Mat add;
    cvtColor(stereo.mask, add, cv::COLOR_GRAY2BGR);
    bitwise_not(add, add);

//    stereo.right += add;
    stereo.mask.forEach<uchar>( functor );
    return stereo.right;
//    stereo.right = inpaint(stereo.left, stereo.right, stereo.deviation, add);
}

// Functor for use with foreach() on a mask of pixels to inpaint
    void Stereo::Inpaint::operator () (uchar &pixel, const int * position) const {

    //    pixel = 50;
        if ( pixel != 0 ) return;

        int row = position[0];
        int col = position[1];
//        stereo.right.at<cv::Vec3b>(row, col) = Vec3b(255, 0, 0);


        for (int offset = 1; offset < (int) stereo.deviation; offset++) {
            int r_offset = col - offset;
            int l_offset = col - offset;

            if (r_offset <= stereo.right.cols && stereo.right.ptr<Vec3b>(row)[r_offset] != Vec3b(0, 0, 0)) {

                stereo.right.ptr<Vec3b>(row)[col] = stereo.right.ptr<Vec3b>(row)[r_offset];
                break;
            }
            if (l_offset >= 0  && stereo.right.ptr<Vec3b>(row)[l_offset] != Vec3b(0, 0, 0)) {

                stereo.right.ptr<Vec3b>(row)[col] = stereo.right.ptr<Vec3b>(row)[l_offset];
                break;
            }
        }

    }


Mat inpaint(const Mat& left_img, Mat right_img, float deviation, cv::Mat &mask) {

    for (int row = 0; row < mask.rows; row++) {
        for (int col = 0; col < mask.cols; col++) {

//            if (mask.ptr<Vec3b>(row)[col] != Vec3b(0, 0, 0)) continue;

            for (int offset = 1; offset < (int) deviation; offset++) {
                int r_offset = col + offset;
                int l_offset = col - offset;

                if (l_offset >= 0 && right_img.ptr<Vec3b>(row)[l_offset] != Vec3b(0, 0, 0)) {

                    right_img.ptr<Vec3b>(row)[col] = right_img.ptr<Vec3b>(row)[l_offset];
                    break;
                }

                if (r_offset <= right_img.cols && right_img.ptr<Vec3b>(row)[r_offset] != Vec3b(0, 0, 0)) {

                    right_img.ptr<Vec3b>(row)[col] = right_img.ptr<Vec3b>(row)[r_offset];
                    break;
                }

            }
        }
    }
    return right_img;
}


Mat anaglyphize(Mat left_img, Mat right_img){
    int height = left_img.rows;
    int width = left_img.cols;
    Mat anaglyph(height, width, CV_8UC3);
    anaglyph = 0;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {

            anaglyph.ptr<Vec3b>(row)[col][2] = left_img.ptr<Vec3b>(row)[col][2];
        }
    }
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){

            anaglyph.ptr<Vec3b>(row)[col][1] = right_img.ptr<Vec3b>(row)[col][1];
            anaglyph.ptr<Vec3b>(row)[col][0] = right_img.ptr<Vec3b>(row)[col][0];
        }
    }

    return anaglyph;
}

