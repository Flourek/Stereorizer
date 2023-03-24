//
// Created by Flourek on 15/03/2023.
//

#include <opencv2/opencv.hpp>
#include "header.h"

using namespace std;
using namespace cv;

Mat shift_pixels(const cv::Mat &left_img, Mat depth, const float Distance, cv::Mat &mask, float deviation = 0) {
    int height = left_img.rows;
    int width = left_img.cols;

    if (left_img.size != depth.size)
        return left_img;

    Mat right_img(height, width, CV_8UC3);
    left_img.convertTo(mask, CV_8UC3);

    Mat depth_float;
    depth.convertTo(depth_float, CV_32FC1);


    double depth_min, depth_max;
    minMaxLoc(depth_float, &depth_min, &depth_max);

    if (depth_min != depth_max)
        depth_float = (depth_float - depth_min) / (depth_max - depth_min);
    else
        depth_float = depth_float / 255.0;

    depth_float.convertTo(depth, CV_16UC1);


    right_img = 0;
    mask = 0;


    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {

            float meh = depth_float.at<float>(row, col);
            int dis = meh * deviation;
            int col_r = col - dis;

//            int dis = static_cast<int>((1.0 - pow(depth_float.at<float>(row, col), 2)) * deviation * 5);

//            if(col % 100 == 0 && row % 100 == 0) cout << meh << " " << dis << '\n';

            if (col_r >= 0 && col_r < width) {
                right_img.at<Vec3b>(row, col_r) = left_img.at<Vec3b>(row, col);
                mask.at<Vec3b>(row, col_r) = Vec3b(255, 255, 255);


            }
        }
    }

    return right_img;
}


Mat inpaint(const Mat& left_img, Mat right_img, float deviation) {
    vector<int> zero_rows;
    vector<int> zero_cols;

    for (int row = 0; row < left_img.rows; row++) {
        for (int col = 0; col < left_img.cols; col++) {

            if (right_img.ptr<Vec3b>(row)[col] == Vec3b(0, 0, 0)) {
                zero_rows.push_back(row);
                zero_cols.push_back(col);
            }
        }
    }

    auto r_begin = zero_rows.begin();
    auto r_end = zero_rows.end();
    vector<int>::iterator r;

    auto c_begin = zero_cols.begin();
    auto c_end = zero_cols.end();
    vector<int>::iterator c;

    for (r = r_begin, c = c_begin; r != r_end, c != c_end; r++, c++) {
        for (int offset = 1; offset < (int) deviation; offset++) {
            int r_offset = *c + offset;
            int l_offset = *c - offset;

            if (r_offset <= right_img.cols && right_img.ptr<Vec3b>(*r)[r_offset] != Vec3b(0, 0, 0)) {
                right_img.ptr<Vec3b>(*r)[*c] = right_img.ptr<Vec3b>(*r)[r_offset];
                break;
            }

            if (l_offset >= 0 && right_img.ptr<Vec3b>(*r)[l_offset] != Vec3b(0, 0, 0)) {
                right_img.ptr<Vec3b>(*r)[*c] = right_img.ptr<Vec3b>(*r)[l_offset];

                break;
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

