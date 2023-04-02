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

    Stereo(Image &left, Depth &depth, Image &right, float deviation);

    void run(const GuiSettings& opt);

    cv::Mat maskPostProcess(const GuiSettings &opt);

    struct ShiftPixels {
        Stereo& stereo;

        explicit ShiftPixels(Stereo& stereo) : stereo(stereo) {}

        static cv::Mat run(Stereo &stereo);
        void operator () (float &pixel, const int * position) const;
    };

    struct Inpaint {
        Stereo& stereo;
        explicit Inpaint(Stereo& stereo) : stereo(stereo) {}

        static cv::Mat run(Stereo &stereo);
        void operator () (uchar &pixel, const int * position) const;
    };
};


#endif //STEREORIZER_STEREO_H
