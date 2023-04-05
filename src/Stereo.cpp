//
// Created by Flourek on 29/03/2023.
//

#include "Stereo.h"
#include <opencv2/opencv.hpp>
#include "header.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;



Stereo::Stereo(Image &left, Depth &depth, Image &right, float deviation)
    : left(left), depth(depth), right(right), deviation(deviation)
{
    mask = cv::Mat(left.mat.rows, left.mat.cols, CV_8UC1);
    resizeAll(1);
}


void Stereo::run(const GuiSettings& opt) {
    deviation = (opt.deviation * opt.deviation_multiplier) * (float) resized_left.cols / 500;

    mask = 0;
    right.mat = 0;

    cv::Size target_size = left.mat.size();
    if(right.mat.size() != target_size)
        cv::resize(right.mat, right.mat, target_size);

    if(depth.mat.size() != target_size)
        cv::resize(depth.mat, depth.mat, target_size);

    if(mask.size() != target_size)
        cv::resize(mask, mask, target_size);


    cv::resize(resized_depth, resized_depth, resized_left.size());
    right.mat = Stereo::ShiftPixels();

    if(opt.inpainting_enable)
        Inpaint(right.mat, mask, deviation);

    if(opt.mask_overlay)
        right.mat += maskPostProcess(opt);


    right.convertToDisplay();
    right.updateTexture();
}


void Stereo::resizeAll(int scale) {
    auto new_size = cv::Size( left.mat.cols /scale,  left.mat.rows / scale);
    cv::resize(left.mat,        resized_left,   new_size );
    cv::resize(depth.mat,       resized_depth,  new_size );
    cv::resize(mask,            resized_mask,   new_size );
    cv::resize(right.mat,       right.mat,      new_size );
    right.createTexture();
}




cv::Mat Stereo::ShiftPixels() {

    #define M_PI 3.14159265358979323846
    cv::Mat result(left.mat.rows, left.mat.cols, CV_8UC3);
    result = 0;

    depth.mat.forEach<ushort>(
        [this, &result] ( ushort &pixel, const int * position ){

            int row = position[0];
            int col = position[1];

            float dis = 256 - (pixel / 256) + dampener;

            double view_angle = 2.0 * atan(ipd / (2.0 * dis));
            double disparity = (view_angle * 180.0 / M_PI) * (focal_length / pixel_size);
            int col_r = col - disparity;

            if (col_r >= 0 && col_r < left.mat.cols) {
                result.at<cv::Vec3b>(row, col_r) = left.mat.at<cv::Vec3b>(row, col);
                mask.at<uchar>(row, col_r) = 255;
            }
        }
    );

    return result.clone();
}


void Stereo::Inpaint(cv::Mat& right, cv::Mat& mask, float deviation){

    mask.forEach<uchar>(
        [&right, &deviation] ( uchar &pixel, const int * position ){

            if ( pixel != 0 ) return;

            int row = position[0];
            int col = position[1];

            for (int offset = 1; offset < (int) deviation; offset++) {
                int r_offset = col - offset;
                int l_offset = col - offset;

                if (r_offset <= right.cols && right.ptr<Vec3b>(row)[r_offset] != Vec3b(0, 0, 0)) {

                    right.ptr<Vec3b>(row)[col] = right.ptr<Vec3b>(row)[r_offset];
                    break;
                }
                if (l_offset >= 0  && right.ptr<Vec3b>(row)[l_offset] != Vec3b(0, 0, 0)) {

                    right.ptr<Vec3b>(row)[col] = right.ptr<Vec3b>(row)[l_offset];
                    break;
                }
            }
        }
    );


}



cv::Mat Stereo::maskPostProcess(const GuiSettings &opt) {
    cv::Mat blurred, res = resized_mask.clone();

    if(res.channels() == 3)
        cvtColor(res, res, cv::COLOR_BGR2GRAY);

//    std::cout << res.channels() << " " << CV_8UC1 << std::endl;

    // Invert
    bitwise_not(res, res);

    // Kernel sizes
    int morph = 2;
    int erode = 2;
    int blur  = 11;

    cv::Mat kernelEx = cv::Mat::ones(morph, morph, CV_8UC1);
    cv::Mat kernel_erode = cv::Mat::ones(erode, erode, CV_8UC1);

    // Remove noise, patch holes, smooth edges
    cv::morphologyEx(res, res, cv::MORPH_CLOSE, kernelEx);
    cv::erode(res, res, kernel_erode);
    cv::blur(res, res, cv::Size(blur, blur) );
    cv::threshold(res, res, 127,255, cv::THRESH_BINARY);
    cv::erode(res, res, erode);

    if(opt.mask_blur){

        // Get the right edge of each island
        cv::Mat edge(res.rows, res.cols, CV_8UC1);
        typedef uchar Pixel;

        float opacity_step = (float) 255 / opt.mask_blur_size;

        // this is litteraly 10 times faster than normal nested for loops
        edge.forEach<Pixel> (
                [&res, &edge, &opacity_step, opt] (Pixel &pixel, const int * position) -> void {

                    if(position[1] == 0 || position[1] == res.cols) return;

                    Pixel previous = res.at<Pixel>(position[0], position[1] - 1);
                    Pixel current  = res.at<Pixel>(position[0], position[1]);
                    if( previous == 255 && current == 0){

                        for (int i = 0; i < opt.mask_blur_size; ++i) {
                            if ( position[1] + i > res.cols ) return;
                            edge.at<Pixel>(position[0], position[1] + i) = cv::saturate_cast<uchar>(255 - i * opacity_step * 4);
                        }
                    }
                }
        );

        res += edge;
    }

    if(res.channels() == 1)
        cvtColor(res, res, cv::COLOR_GRAY2BGR);

    return res;
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


