//
// Created by Flourek on 13/04/2023.
//

#include "VRController.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "openvr.h"
#include "thread"
#include "vector"
#include "iostream"
#include <opencv2/opencv.hpp>
#include "VR/vr.h"


//VRController::VRController() {
//    m_thread =
//}

void VRController::Run() {
    Get().IRun();

}


void VRController::SetMats(cv::Mat &left, cv::Mat &right) {
    m_left.set(left);
    m_right.set(right);
//    cv::Mat *mats[2] = {&m_left, &m_right};
}

void VRController::SetTextures(uint32_t left_texture, uint32_t right_texture) {
    m_leftTexture = left_texture;
    m_rightTexture = right_texture;
    texturesUpdated = true;
}

void VRController::IRun() {

    m_thread = std::thread([&]() {
        VR::run(*this);
    });

    m_thread.detach();
}


void VRController::Init() {


}

VRController::~VRController(){
    vr::VR_Shutdown();
}

VRController VRController::s_Instance;

VRController& VRController::Get() {
    return s_Instance;
}


