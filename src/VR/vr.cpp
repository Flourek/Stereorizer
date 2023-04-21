

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "opencv2/opencv.hpp"


#include "shader.h"
#include "camera.h"
#include <iostream>
#include "openvr.h"
#include "../VRController.h"


namespace VR {

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
bool vr_enabled = true;


struct Cameras{
private:
    glm::vec3 worldup = {0.0f, 1.0f, 0.0f};

public:
    Camera left  = Camera(glm::vec3(0.0f, 0.0f, 3.0f), worldup);
    Camera right = Camera(glm::vec3(0.0f, 0.0f, 3.0f),  worldup);

    Camera *array[2] = {&left, &right};

} Cameras;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float imageAspect = 1.0f;

float vertices[] = {
        -1.0f, -1.0f,  1.0f,     0.0f, 1.0f,
         1.0f, -1.0f,  1.0f,     1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,     1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,     1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,     0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,     0.0f, 1.0f,
};

// world space positions of our cubes
glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  1.0f,  -4.0f),
};

GLuint makeQuadTexture(const ThreadSafeMat& mat){
    GLuint texture;
    cv::Mat image = mat.get();
    cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);
    imageAspect = (float) image.rows / image.cols;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}

void updateQuadTexture(GLuint texture, const ThreadSafeMat& mat){

    cv::Mat image = mat.get();
    cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.cols, image.rows, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << glGetError();
    }
    int w, h;
    int miplevel = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);
    std::cout << w <<  " " << image.cols <<  " "<< h <<  " "<<  image.rows << "\n";

}

GLuint makeEyeTexture(){
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RENDER_WIDTH, RENDER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int w, h;
    int miplevel = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);
    std::cout << w << " " << h << "\n";


    return texture;
}

glm::mat4 convertSteamVRmatToGLM( const vr::HmdMatrix34_t &matPose ) {
    glm::mat4 matrixObj(
            matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
            matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
            matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
            matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
    );
    return matrixObj;
}

glm::mat4  getHMDMatrixPoseEye( vr::Hmd_Eye nEye ) {

    vr::HmdMatrix34_t matEye = vr::VRSystem()->GetEyeToHeadTransform(nEye );
    glm::mat4 mat(
            matEye.m[0][0], matEye.m[1][0], matEye.m[2][0], 0.0,
            matEye.m[0][1], matEye.m[1][1], matEye.m[2][1], 0.0,
            matEye.m[0][2], matEye.m[1][2], matEye.m[2][2], 0.0,
            matEye.m[0][3], matEye.m[1][3], matEye.m[2][3], 1.0f
    );

    return mat;
}

glm::mat4 getHMDMatrixProjectionEye( vr::Hmd_Eye nEye ) {

    vr::HmdMatrix44_t mat =  vr::VRSystem()->GetProjectionMatrix( nEye, 0.1, 100.0f);

    return glm::mat4(
            mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
            mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
            mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
            mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
    );
}

std::string g_inputPath = "w.jpg";

void dropCallback(GLFWwindow *window, int count, const char** paths){
    if (count > 0)
        g_inputPath = paths[0];
}

    void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                        GLenum severity, GLsizei length,
                                        const GLchar *msg, const void *data)
    {
        char* _source;
        char* _type;
        char* _severity;

        switch (source) {
            case GL_DEBUG_SOURCE_API:
                _source = "API";
                break;

            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                _source = "WINDOW SYSTEM";
                break;

            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                _source = "SHADER COMPILER";
                break;

            case GL_DEBUG_SOURCE_THIRD_PARTY:
                _source = "THIRD PARTY";
                break;

            case GL_DEBUG_SOURCE_APPLICATION:
                _source = "APPLICATION";
                break;

            case GL_DEBUG_SOURCE_OTHER:
                _source = "UNKNOWN";
                break;

            default:
                _source = "UNKNOWN";
                break;
        }

        switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                _type = "ERROR";
                break;

            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                _type = "DEPRECATED BEHAVIOR";
                break;

            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                _type = "UDEFINED BEHAVIOR";
                break;

            case GL_DEBUG_TYPE_PORTABILITY:
                _type = "PORTABILITY";
                break;

            case GL_DEBUG_TYPE_PERFORMANCE:
                _type = "PERFORMANCE";
                break;

            case GL_DEBUG_TYPE_OTHER:
                _type = "OTHER";
                break;

            case GL_DEBUG_TYPE_MARKER:
                _type = "MARKER";
                break;

            default:
                _type = "UNKNOWN";
                break;
        }

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                _severity = "HIGH";
                break;

            case GL_DEBUG_SEVERITY_MEDIUM:
                _severity = "MEDIUM";
                break;

            case GL_DEBUG_SEVERITY_LOW:
                _severity = "LOW";
                break;

            case GL_DEBUG_SEVERITY_NOTIFICATION:
                _severity = "NOTIFICATION";
                break;

            default:
                _severity = "UNKNOWN";
                break;
        }

        printf("%d: %s of %s severity, raised from %s: %s\n",
               id, _type, _severity, _source, msg);
    }

int run(VRController &controller) {

    GLFWwindow * window = controller.m_Window;
    glfwMakeContextCurrent( window);
    glewInit();


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetDropCallback(window, dropCallback);


//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }

//     Initialize OpenVR
//    if (vr::VR_IsHmdPresent() && vr_enabled) {
//        auto VRError = vr::VRInitError_None;
//        auto VRSystem = vr::VR_Init(&VRError, vr::VRApplication_Scene);
//
//        if (VRError != vr::VRInitError_None){
//            std::cout << "OpenVR initialization failed: " << vr::VR_GetVRInitErrorAsEnglishDescription(VRError) << std::endl;
//            return 1;
//        }
//
//    }else{
//        std::cout << "HMD not found" << std::endl;
//    }




// During init, enable debug output
    std::cout << glGetString(GL_VERSION) << "\n";

    glDebugMessageCallback( MessageCallback, nullptr );
    glEnable(GL_DEPTH_TEST);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, RENDER_WIDTH, RENDER_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);




    enum eyeEnum{
        LEFT = 0,
        RIGHT = 1
    };

    struct Eye{
        GLuint texture;
        GLuint color;
        vr::EVREye steam = vr::Eye_Left;

        Eye(ThreadSafeMat& mat, vr::EVREye steamEye){
            color  = makeQuadTexture(mat);
            texture = makeEyeTexture();
            steam = steamEye;
        }

        void setColor(ThreadSafeMat& mat){
            color = makeQuadTexture(mat);
        }


    };

    Eye eyes[2] = { {controller.m_left, vr::Eye_Left},
                    {controller.m_right, vr::Eye_Right} };

    bool first = true;

    Shader ourShader("../src/VR/camera.vs", "../src/VR/camera.fs");
    ourShader.use();
    int i = 0;
    controller.m_rightTexture = eyes[RIGHT].texture;
    controller.m_leftTexture =  eyes[LEFT].texture;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        i += 1000000;

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < 2; ++i) {

            if(i==0){
                glClearColor((float) ((float) i / (float) INT_MAX), 0.3f, 0.3f, 1.0f);
            }else{
                glClearColor(0.6f, 0.3f, (float) ((float) i / (float) INT_MAX), 1.0f);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, eyes[i].texture, 0);
            glBindTexture(GL_TEXTURE_2D, eyes[i].color);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            if(i == LEFT)
                glBlitFramebuffer(0, 0, RENDER_WIDTH, RENDER_HEIGHT, 0, 0, SCR_WIDTH / 2, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            if(i == RIGHT)
                glBlitFramebuffer(0, 0, RENDER_WIDTH, RENDER_HEIGHT, SCR_WIDTH / 2, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        }


        glfwPollEvents();
        glfwSwapBuffers(window);
    }
//
//
//    vr::TrackedDevicePose_t vrTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
//    vr::VRCompositor()->SetTrackingSpace(vr::TrackingUniverseStanding);
//    vr::VRChaperone()->ResetZeroPose(vr::TrackingUniverseStanding);
//
//    while (!glfwWindowShouldClose(window)) {
//
//        vr::VRCompositor()->WaitGetPoses(vrTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
//
//        float currentFrame = static_cast<float>(glfwGetTime());
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        glm::mat4 eyeDisparity;
//        glm::mat4 projection;
//
//        if(controller.sizeChanged){
//            eyes[LEFT].color = makeQuadTexture(controller.m_left);
//            eyes[RIGHT].color = makeQuadTexture(controller.m_right);
//
//            controller.sizeChanged = false;
//        }
//
//        if(controller.texturesUpdated){
//            updateQuadTexture(eyes[LEFT].color,  controller.m_left);
//            updateQuadTexture(eyes[RIGHT].color, controller.m_right);
////            std::cout << "update chyba \n";
//            controller.texturesUpdated = false;
//        }
//
//
//
//        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
//
//        for (int i = 0; i < 2; ++i) {
//
//
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, eyes[i].texture, 0);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            glBindTexture(GL_TEXTURE_2D, eyes[i].color);
//            projection = getHMDMatrixProjectionEye(eyes[i].steam);
//            eyeDisparity = getHMDMatrixPoseEye(eyes[i].steam);
//
//            glm::mat4 hmdPose = convertSteamVRmatToGLM( vrTrackedDevicePose[0].mDeviceToAbsoluteTracking );
//            hmdPose = glm::inverse(hmdPose);
//
//            // Moving the quad and applying aspect ratio
//            glm::mat4 model = glm::mat4(1.0f);
//            model = glm::translate(model, cubePositions[0]);
//            model = glm::scale(model, glm::vec3(1.0f, imageAspect, 1.0f) );
//
//            glm::mat4 mvp = projection * hmdPose * eyeDisparity  * model;
//
//            ourShader.use();
//            ourShader.setMat4("mvp", mvp);
//
//            // Render quad
//            glBindVertexArray(VAO);
//            glDrawArrays(GL_TRIANGLES, 0, 6);
//
//            // Draw textures in the companion window
//            glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
//            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//            if(i == LEFT)
//                glBlitFramebuffer(0, 0, RENDER_WIDTH, RENDER_HEIGHT, 0, 0, SCR_WIDTH / 2, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//            if(i == RIGHT)
//                glBlitFramebuffer(0, 0, RENDER_WIDTH, RENDER_HEIGHT, SCR_WIDTH / 2, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//
//
//        }
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//        // Pass textures to OpenVR
//        vr::Texture_t leftEye  = {(void *) (uintptr_t) eyes[LEFT].texture,  vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
//        vr::Texture_t rightEye = {(void *) (uintptr_t) eyes[RIGHT].texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
//
//        int error = 0;
//        error |= vr::VRCompositor()->Submit(vr::Eye_Left, &leftEye);
//        error |= vr::VRCompositor()->Submit(vr::Eye_Right, &rightEye);
//
//        if(error != vr::VRCompositorError_None)
//            std::cout << "Submit error: " << error;
//
//
//        processInput(window);
//        glfwPollEvents();
//        glfwSwapBuffers(window);
//    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(controller.m_Window);
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    for (Camera *cam : Cameras.array) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cam->ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cam->ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cam->ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cam->ProcessKeyboard(RIGHT, deltaTime);

    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        Cameras.right.Position[0] += -0.5 * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        vr::VRChaperone()->ResetZeroPose(vr::TrackingUniverseStanding);

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        Cameras.right.Position[0] += 0.5 * deltaTime;

//    std::cout << Cameras.right.Position[0] << std::endl;


    float zoomSpeed = 0;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
        zoomSpeed = 3;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
        zoomSpeed = -3;
    }
    if(zoomSpeed){
        cubePositions[0][2] += zoomSpeed * deltaTime;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    for (Camera *cam : Cameras.array) {
        cam->ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    for (Camera *cam : Cameras.array) {
        cam->ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

}
