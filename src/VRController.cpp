//
// Created by Flourek on 13/04/2023.
//

#include "GL/glew.h"
#include "VRController.h"
#include "GLFW/glfw3.h"

#include "openvr.h"
#include "thread"
#include "vector"
#include "iostream"
#include <opencv2/opencv.hpp>
#include "VR/vr.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"


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

    if ( !MakeWindow() ) return;


    m_thread = std::thread([&]() {
        VR::run(*this);
        glfwDestroyWindow(m_Window);
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

// Returns false if failed to create window
bool VRController::MakeWindow() {

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    m_Window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Viewer", nullptr, m_MainWindow);
    if (m_Window == NULL) {
        std::cout << "Failed to create VR GLFW window" << std::endl;
        return false;
    }

    return true;
}

void VRController::DrawImGui() {
    ImGui::SetNextWindowSize( ImVec2(0, 0) );

    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8,0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(8,0));

    ImGui::Begin("vr", nullptr);
        ImGui::Image( (void*)(intptr_t) m_leftTexture, ImVec2(SCR_WIDTH / 2, SCR_HEIGHT) );
        ImGui::SameLine();
        ImGui::Image( (void*)(intptr_t) m_rightTexture, ImVec2(SCR_WIDTH / 2, SCR_HEIGHT) );

        ImGui::BeginChild("settings", ImVec2(SCR_WIDTH, 50), ImGuiWindowFlags_NoMove |  ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoDecoration );
            static float idk[3];
            ImGui::NewLine();
            ImGui::SliderFloat("the fuck", idk, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("the fucke", idk, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::Text("huhas   aeuifeh      + -?");
        ImGui::EndChild();
    ImGui::End();



    ImGui::PopStyleVar(3);



}




