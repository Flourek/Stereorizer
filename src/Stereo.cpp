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
    target_size = depth.original.size();
}


void Stereo::run(const GuiSettings& opt) {

    mask = 0;
    right.mat = 0;

    // Resize inputs so that they all match
    resizeAll(opt.viewport_scale);

    deviation = (opt.deviation * opt.deviation_multiplier) * (float) target_size.width / 500;

    ShiftPixels();

    if(opt.inpainting_enable)
        Inpaint(right.mat, mask, deviation);

    if(opt.mask_overlay)
        right.mat += maskPostProcess(opt);


    right.convertToDisplay();
    right.updateTexture();
}


// Set to -1 to match sizes
void Stereo::resizeAll(int scale) {

    target_size = cv::Size( depth.original.cols / scale, depth.original.rows / scale  );
    std::cout << target_size << std::endl;

    if(depth.mat.size() != target_size){
        cv::resize(depth.original, depth.mat, target_size);
        depth.convertToDisplay();
        depth.createTexture();
    }

    if(left.mat.size() != target_size)
        cv::resize(left.original, left.mat, target_size);

    if(right.mat.size() != target_size){
        cv::resize(right.original, right.mat, target_size);
        right.convertToDisplay();
        right.createTexture();
    }

    if(mask.size() != target_size)
        cv::resize(mask, mask, target_size);


}




void Stereo::ShiftPixels() {

    #define M_PI 3.14159265358979323846

    depth.mat.forEach<ushort>(
        [this] ( ushort &pixel, const int * position ){

            int row = position[0];
            int col = position[1];

            float dis = 256 - (pixel / 256) + dampener;

            double view_angle = 2.0 * atan(ipd / (2.0 * dis));
            double disparity = (view_angle * 180.0 / M_PI) * (focal_length / pixel_size);
            int col_r = col - disparity;

            if (col_r >= 0 && col_r < left.mat.cols) {
                right.mat.at<cv::Vec3b>(row, col_r) = left.mat.at<cv::Vec3b>(row, col);
                mask.at<uchar>(row, col_r) = 255;
            }
        }
    );

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
    cv::Mat blurred, res = mask.clone();

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


