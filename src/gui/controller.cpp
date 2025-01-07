#include"controller.hpp"

#include<iostream>

#include<glm/gtc/epsilon.hpp>
#include<limits>
#include<thread>
#include <execution>

    // if(animation.U_turn){ rotateAroundAxis('y','U',1,dt,gameObjects); }
    // if(animation.D_turn){ rotateAroundAxis('y','D',-1,dt,gameObjects); }
    // if(animation.F_turn){ rotateAroundAxis('z','F',1,dt,gameObjects); }
    // if(animation.B_turn){ rotateAroundAxis('z','B',-1,dt,gameObjects); }
    // if(animation.R_turn){ rotateAroundAxis('x','R',-1,dt,gameObjects); }
    // if(animation.L_turn){ rotateAroundAxis('x','L',1,dt,gameObjects); }

Controller::Controller() {
    // Map keys to their corresponding animation flags
    keymaps = {
        KeyMapData(keys.u_turn, &animation.U_turn, 'y', 'U', 1),
        KeyMapData(keys.d_turn, &animation.D_turn, 'y', 'D', -1),
        KeyMapData(keys.f_turn, &animation.F_turn, 'z', 'F', 1),
        KeyMapData(keys.b_turn, &animation.B_turn, 'z', 'B', -1),
        KeyMapData(keys.r_turn, &animation.R_turn, 'x', 'R', -1),
        KeyMapData(keys.l_turn, &animation.L_turn, 'x', 'L', 1)
    };
}

void Controller::orbitAroundCube(GLFWwindow* window, float dt, CubeObj& viewerObject){
    glm::vec3 rotate{0};
    if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
    if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
    if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
    if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        viewerObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
    }

    // limit pitch values between about +/- 85ish degrees
    viewerObject.transform.rotation.x = glm::clamp(viewerObject.transform.rotation.x, -1.5f, 1.5f);
    viewerObject.transform.rotation.y = glm::mod(viewerObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = viewerObject.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        viewerObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }
}

void Controller::rotateCube(GLFWwindow* window, float dt, std::vector<CubeObj> &gameObjects){
    if(!solveKeyPressed){
        if(!animation.isRotating()){
            if(glfwGetKey(window, keys.solve) == GLFW_PRESS && !solveKeyPressed && !cube.isSolved()){
                // start calculating solution
                std::cout << "[CONTROLLER] - pressed space, cube turns disabled" << std::endl;
                std::thread solveThread([&](){
                    solver.kociemba(cube.state().c_str());
                });
                solveThread.detach();
                solveKeyPressed = true;
            }

            // change moves from clockwise to anticlockwise or viceversa
            if(glfwGetKey(window, keys.inverse) == GLFW_PRESS)
                inverseKeyPressed = true;

            if(glfwGetKey(window, keys.inverse) == GLFW_RELEASE && inverseKeyPressed) {
                inverse *= -1;
                if(inverse > 0) std::cout << "[CHANGE] - inverse turning disabled" << std::endl;
                else std::cout << "[CHANGE] - inverse turning enabled" << std::endl;
                inverseKeyPressed = false;
            }

            // enable/disable double turning
            if(glfwGetKey(window, keys.double_turn) == GLFW_PRESS)
                doubleTurnKeyPressed = true;

            if(glfwGetKey(window, keys.double_turn) == GLFW_RELEASE && doubleTurnKeyPressed) {
                if(numOfTurns == 1){
                    numOfTurns = 2;
                    std::cout << "[CHANGE] - double turning enabled" << std::endl;
                } else {
                    numOfTurns = 1;
                    std::cout << "[CHANGE] - double turning disabled" << std::endl;
                }
                doubleTurnKeyPressed = false;
            }

            for(auto& keyMap : keymaps){
                if(glfwGetKey(window, keyMap.key) == GLFW_PRESS) {
                    *keyMap.turn = true;
                    targetRotationAngle = glm::radians(90.0f * numOfTurns * inverse);
                    break;
                }
            }
        }
    }

    for(auto &keyMap : keymaps){
        if (*keyMap.turn)
            rotateAroundAxis(keyMap, dt, gameObjects);
    }
}

void Controller::rotateAroundAxis(KeyMapData keyMap, float dt, std::vector<CubeObj> &gameObjects){
    oldRotationAngle = currentRotationAngle;
    currentRotationAngle = glm::mix(currentRotationAngle, targetRotationAngle, rotationSpeed * dt);

    auto faceIds = cube.getFaceId(keyMap.side);
    if (glm::epsilonEqual(currentRotationAngle, targetRotationAngle, 0.01f)) {
            // Use std::for_each with std::execution::par to parallelize the loop
        std::for_each(std::execution::par, faceIds.begin(), faceIds.end(), [&](int objId) {
            auto& obj = gameObjects[objId]; obj.rotate(keyMap.axis, (keyMap.sign * (targetRotationAngle - currentRotationAngle)), true);
            obj.transform.coordSystem.rotate(keyMap.axis, keyMap.sign * targetRotationAngle);
        });

        currentRotationAngle = 0.0f;
        auto sideStr = std::string(1, keyMap.side);
        if(numOfTurns == 2) cube.turn(sideStr + "2");
        else if(inverse == -1) cube.turn(sideStr + "'");
        else cube.turn(sideStr);

        *keyMap.turn = false;
    } else {
        // Use std::for_each with std::execution::par to parallelize the loop
        std::for_each(std::execution::par, faceIds.begin(), faceIds.end(), [&](int objId) {
            gameObjects[objId].rotate(keyMap.axis, (keyMap.sign * (currentRotationAngle - oldRotationAngle)), false);
        });
    }
}

std::optional<KeyMapData> Controller::getMap(char side)
{
    auto it = std::find_if(keymaps.begin(), keymaps.end(), [side](const KeyMapData &keymap)
                           { return keymap.side == side; });
    if (it != keymaps.end())
    {
        return *it;
    }
    else
    {
        return std::nullopt;
    }
}

void Controller::solveCube(){
    if(solver.solutionReady){
        if(!animation.isRotating()){
            if(solver.solution.size() == 0){
                // reset turn variables
                inverse = 1;
                numOfTurns = 1;
                // reset status variables
                solver.solutionReady = false;
                solveKeyPressed = false;
                std::cout << "[CONTROLLER] - cube solved, cube turns enabled" << std::endl;
            } else {
                // getting first move and then removing it from solution list
                std::string turn = solver.solution.front();
                solver.solution.erase(solver.solution.begin());

                inverse = (turn[1] == '\'') ? -1 : 1;
                numOfTurns = (turn[1] == '2') ? 2 : 1;

                char move = turn[0];
                auto map = this->getMap(move);
                *map->turn = true;

                targetRotationAngle = glm::radians(90.0f * numOfTurns * inverse);
                /*
                for(auto& obj : gameObjects){
                    std::cout << obj.getId() << " # "
                        << obj.transform.quatRotation.w << ", "
                        << obj.transform.quatRotation.x << ", "
                        << obj.transform.quatRotation.y << ", "
                        << obj.transform.quatRotation.z << " - "
                        << obj.transform.translation.x << ", "
                        << obj.transform.translation.y << ", "
                        << obj.transform.translation.z << std::endl;
                }
                */
            }
        }
    }
}