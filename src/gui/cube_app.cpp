#include"cube_app.hpp"

#include"controller.hpp"
#include"render_system.hpp"
#include"camera.hpp"
#include"../constants.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>

#include<cassert>
#include<stdexcept>
#include<array>
#include<chrono>
#include <thread>
#include <ranges>

CubeApp::CubeApp(){
    loadGameObjects();
}

CubeApp::~CubeApp(){}

void CubeApp::run(){
    RenderSystem renderSystem{device, renderer.getSwapChainRenderPass()};
    Camera camera{};
    //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = CubeObj::createGameObject();
    viewerObject.transform.translation.z = -2.5f;
    Controller controller{};

    GLFWwindow* window = cubeGUI.getGLFWwindow();

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto nextFrame = currentTime;

    auto maxFrameRate = renderSystem.getMaxFrameRate();
    long long frameDeltaTime = 1000000.0f/maxFrameRate;

    while(!cubeGUI.shouldClose()){
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        controller.orbitAroundCube(window, frameTime, viewerObject);
        controller.rotateCube(window, frameTime, gameObjects);
        controller.solveCube();

        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = renderer.getAspectRatio();
        //camera.setOrthographicProjection(-aspect, aspect,-1, 1, -1, 1);
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

        if(auto commandBuffer = renderer.beginFrame()){
            renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }
          // Calculate the time to sleep to maintain a consistent frame rate
        nextFrame += std::chrono::microseconds(frameDeltaTime);
        std::this_thread::sleep_until(nextFrame);
    }

    vkDeviceWaitIdle(device.device());
}

void load4x4(int scaler, std::string basePath, std::vector<std::string>& models, std::vector<glm::vec3>& translation){
    for(int i = -1;i <= 2;i++)
        for(int j = -1;j <= 2;j++)
            for(int k = -1;k <= 2;k++)
                //if(!(i == 0 && j == 0 && k == 0))
                if(!((k == 0 || k == 1) && ((i==0 && j == 0) || (i == 0 && j == 1) || (i==1 && j == 0) || (i == 1) && (j == 1))))
                    translation.push_back(glm::vec3(((float) i)/scaler,((float) j)/scaler,((float) k)/scaler));

    models = {
        basePath + "/corner_yellowOrangeBlue.obj",    // 0
        basePath + "/edge_yellowOrange.obj",          // 1
        basePath + "/edge_yellowOrange.obj",          // 1
        basePath + "/corner_yellowGreenOrange.obj",   // 2
        basePath + "/edge_orangeBlue.obj",            // 3
        basePath + "/center_orange.obj",              // 4
        basePath + "/center_orange.obj",              // 4
        basePath + "/edge_greenOrange.obj",           // 5
        basePath + "/edge_orangeBlue.obj",            // 3
        basePath + "/center_orange.obj",              // 4
        basePath + "/center_orange.obj",              // 4
        basePath + "/edge_greenOrange.obj",           // 5
        basePath + "/corner_whiteOrangeBlue.obj",     // 6
        basePath + "/edge_whiteOrange.obj",           // 7
        basePath + "/edge_whiteOrange.obj",           // 7
        basePath + "/corner_whiteGreenOrange.obj",    // 8
        basePath + "/edge_yellowBlue.obj",            // 9
        basePath + "/center_yellow.obj",              // 10
        basePath + "/center_yellow.obj",              // 10
        basePath + "/edge_yellowGreen.obj",           // 11
        basePath + "/center_blue.obj",                // 12
        basePath + "/center_green.obj",               // 13
        basePath + "/center_blue.obj",                // 12
        basePath + "/center_green.obj",               // 13
        basePath + "/edge_whiteBlue.obj",             // 14
        basePath + "/center_white.obj",               // 15
        basePath + "/center_white.obj",               // 15
        basePath + "/edge_whiteGreen.obj",            // 16
        basePath + "/edge_yellowBlue.obj",            // 9
        basePath + "/center_yellow.obj",              // 10
        basePath + "/center_yellow.obj",              // 10
        basePath + "/edge_yellowGreen.obj",           // 11
        basePath + "/center_blue.obj",                // 12
        basePath + "/center_green.obj",               // 13
        basePath + "/center_blue.obj",                // 12
        basePath + "/center_green.obj",               // 13
        basePath + "/edge_whiteBlue.obj",             // 14
        basePath + "/center_white.obj",               // 15
        basePath + "/center_white.obj",               // 15
        basePath + "/edge_whiteGreen.obj",            // 16 //
        basePath + "/corner_yellowBlueRed.obj",       // 17
        basePath + "/edge_yellowRed.obj",             // 18
        basePath + "/edge_yellowRed.obj",             // 18
        basePath + "/corner_yellowRedGreen.obj",      // 19
        basePath + "/edge_blueRed.obj",               // 20
        basePath + "/center_red.obj",                 // 21
        basePath + "/center_red.obj",                 // 21
        basePath + "/edge_redGreen.obj",              // 22
        basePath + "/edge_blueRed.obj",               // 20
        basePath + "/center_red.obj",                 // 21
        basePath + "/center_red.obj",                 // 21
        basePath + "/edge_redGreen.obj",              // 22
        basePath + "/corner_whiteBlueRed.obj",        // 23
        basePath + "/edge_whiteRed.obj",              // 24
        basePath + "/edge_whiteRed.obj",              // 24
        basePath + "/corner_whiteRedGreen.obj"        // 25
    };
}

void load3x3(int scaler, std::string basePath, std::vector<std::string>& models, std::vector<glm::vec3>& translation){
    for(int i = -1;i <= 1;i++)
        for(int j = -1;j <= 1;j++)
            for(int k = -1;k <= 1;k++)
                if(!(i == 0 && j == 0 && k == 0))
                    translation.push_back(glm::vec3(((float) i)/scaler,((float) j)/scaler,((float) k)/scaler));

    models = {
        basePath + "/corner_yellowOrangeBlue.obj",    // 0
        basePath + "/edge_yellowOrange.obj",          // 1
        basePath + "/corner_yellowGreenOrange.obj",   // 2
        basePath + "/edge_orangeBlue.obj",            // 3
        basePath + "/center_orange.obj",              // 4
        basePath + "/edge_greenOrange.obj",           // 5
        basePath + "/corner_whiteOrangeBlue.obj",     // 6
        basePath + "/edge_whiteOrange.obj",           // 7
        basePath + "/corner_whiteGreenOrange.obj",    // 8
        basePath + "/edge_yellowBlue.obj",            // 9
        basePath + "/center_yellow.obj",              // 10
        basePath + "/edge_yellowGreen.obj",           // 11
        basePath + "/center_blue.obj",                // 12
        basePath + "/center_green.obj",               // 13
        basePath + "/edge_whiteBlue.obj",             // 14
        basePath + "/center_white.obj",               // 15
        basePath + "/edge_whiteGreen.obj",            // 16
        basePath + "/corner_yellowBlueRed.obj",       // 17
        basePath + "/edge_yellowRed.obj",             // 18
        basePath + "/corner_yellowRedGreen.obj",      // 19
        basePath + "/edge_blueRed.obj",               // 20
        basePath + "/center_red.obj",                 // 21
        basePath + "/edge_redGreen.obj",              // 22
        basePath + "/corner_whiteBlueRed.obj",        // 23
        basePath + "/edge_whiteRed.obj",              // 24
        basePath + "/corner_whiteRedGreen.obj"        // 25
    };
}

void CubeApp::loadGameObjects(){
    int scaler = 4;
    std::vector<glm::vec3> translation;
    std::vector<std::string> models;

    load3x3(scaler, "models/blender" ,models, translation);
    for(const auto& [index, model] : models | std::views::enumerate){
        std::shared_ptr<Model> the_model = Model::createModelFromFile(device, model);
        auto cube = CubeObj::createGameObject();
        cube.model = the_model;
        cube.transform.translation = translation[index];
        cube.transform.scale = { (float) 1/scaler, (float) 1/scaler, (float) 1/scaler };

        gameObjects.push_back(std::move(cube));
    }
}
