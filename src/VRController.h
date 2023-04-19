//
// Created by Flourek on 13/04/2023.
//

#include "openvr.h"
#include "thread"
#include <opencv2/opencv.hpp>
#include "VR/ThreadSafeMat.h"

#ifndef STEREORIZER_VRCONTROLLER_H
#define STEREORIZER_VRCONTROLLER_H


class VRController {
public:
    ~VRController();

    static VRController& Get();

    ThreadSafeMat m_left;
    ThreadSafeMat m_right;


    bool sizeChanged = true;

    uint32_t m_leftTexture;
    uint32_t m_rightTexture;
    bool texturesUpdated;

    static void Run();
    void SetMats(cv::Mat& left, cv::Mat& right);
    void SetTextures(uint32_t left_texture, uint32_t right_texture);


private:
    vr::EVRInitError m_Error;
    vr::IVRSystem* m_pVRSystem;
    std::thread m_thread;

    void IRun();
    void Init();


    static VRController s_Instance;



};


#endif //STEREORIZER_VRCONTROLLER_H
