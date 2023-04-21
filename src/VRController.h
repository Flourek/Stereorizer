//
// Created by Flourek on 13/04/2023.
//

#include "openvr.h"
#include "thread"
#include <opencv2/opencv.hpp>
#include "VR/ThreadSafeMat.h"
#include "GLFW/glfw3.h"
#include "imgui.h"


#ifndef STEREORIZER_VRCONTROLLER_H
#define STEREORIZER_VRCONTROLLER_H

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 400;
const unsigned int RENDER_WIDTH =  4000;
const unsigned int RENDER_HEIGHT = 4000;


class VRController {
public:
    ~VRController();

    static VRController& Get();

    ThreadSafeMat m_left;
    ThreadSafeMat m_right;

    GLFWwindow* m_Window;
    GLFWwindow* m_MainWindow;
    ImGuiContext* m_ImGuiContext;
    ImGuiContext* m_MainImGuiContext;


    bool sizeChanged = true;

    uint32_t m_leftTexture;
    uint32_t m_rightTexture;
    bool texturesUpdated;

    static void Run();
    void SetMats(cv::Mat& left, cv::Mat& right);
    void SetTextures(uint32_t left_texture, uint32_t right_texture);
    void DrawImGui();


private:
    vr::EVRInitError m_Error;
    vr::IVRSystem* m_pVRSystem;
    std::thread m_thread;

    void IRun();
    void Init();
    bool MakeWindow();



    static VRController s_Instance;



};


#endif //STEREORIZER_VRCONTROLLER_H
