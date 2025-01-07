#include"cube_obj.hpp"
#include <ranges>
#include <algorithm>
#include <execution>
#include "gcem.hpp"

glm::vec3 CoordSystem::approximate(const glm::vec3& v) {
    const float epsilon = 1e-6f;
    return glm::vec3(
        std::abs(v.x) < epsilon ? 0.0f : v.x,
        std::abs(v.y) < epsilon ? 0.0f : v.y,
        std::abs(v.z) < epsilon ? 0.0f : v.z
    );
}

void CoordSystem::rotate(char plane, float angle){
        glm::mat4 rotationMatrix{1.f};
        if(plane == 'x') rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
        if(plane == 'y') rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
        if(plane == 'z') rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

        i = glm::normalize(approximate(glm::vec3(rotationMatrix * glm::vec4(i, 0.0f))));
        j = glm::normalize(approximate(glm::vec3(rotationMatrix * glm::vec4(j, 0.0f))));
        k = glm::normalize(approximate(glm::vec3(rotationMatrix * glm::vec4(k, 0.0f))));
    }

std::tuple<char, int> CoordSystem::getAxis(char axis) {
    glm::vec3 vec{};
    std::tuple<char, int> res;
    if (axis == 'x') vec = i;
    if (axis == 'y') vec = j;
    if (axis == 'z') vec = k;

    if (vec.x != 0.0f) { std::get<0>(res) = 'x'; std::get<1>(res) = int(vec.x); }
    if (vec.y != 0.0f) { std::get<0>(res) = 'y'; std::get<1>(res) = int(vec.y); }
    if (vec.z != 0.0f) { std::get<0>(res) = 'z'; std::get<1>(res) = int(vec.z); }

    return res;
}

void CubeObj::rotate(char plane, float angle, bool toRound){
    // rotation step
    glm::vec3 axis{};
    if(plane == 'x') axis = glm::vec3(1.0f, 0.0f, 0.0f);
    if(plane == 'y') axis = glm::vec3(0.0f, 1.0f, 0.0f);
    if(plane == 'z') axis = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
    transform.translation = glm::vec3(rotationMatrix * glm::vec4(transform.translation, 1.0f));

    // orientation step
    char correct_plane = NULL;
    int sign = 0;

    std::vector<char> xyz{'x','y','z'};
    for(char _plane_ : xyz){
        char _plane_res_ = std::get<0>(transform.coordSystem.getAxis(_plane_));
        char _sign_res_ = std::get<1>(transform.coordSystem.getAxis(_plane_));
        if(_plane_res_ == plane){
            correct_plane = _plane_;
            sign = _sign_res_;
        }
    }

    glm::quat rotMatrix{};
    float sin = std::sin(sign * angle * 0.5f);
    float cos = std::cos(sign * angle * 0.5f);
    if(correct_plane == 'x') rotMatrix = glm::quat(cos, sin , 0.0f, 0.0f);
    if(correct_plane == 'y') rotMatrix = glm::quat(cos, 0.0f , sin, 0.0f);
    if(correct_plane == 'z') rotMatrix = glm::quat(cos, 0.0f , 0.0f, sin);

    transform.quatRotation = transform.quatRotation * rotMatrix;

    // rounding step
    if(toRound){
        // approximate translation
        transform.translation = glm::round(transform.translation / 0.001f) * 0.001f;
        /*
        float translationComponents[3] = {transform.translation.x, transform.translation.y, transform.translation.z};
        for(int i = 0;i < 3;i++){
            signValue = (translationComponents[i] > 0.0f) ? +1 : -1;
            translationComponents[i] = (glm::epsilonEqual(std::abs(translationComponents[i]), 0.25f, 0.1f)) ? signValue * 0.25f : 0.0f;
        }
        */

        constexpr auto sin45 = gcem::sin(glm::radians(45.0f));

        auto processComponent = [](float& rotationComponent) {
            float signValue = (rotationComponent > 0.0f) ? +1 : -1;
            // value around 0.0f
            if(std::abs(rotationComponent) < 1e-1f) rotationComponent = 0.0f;
            // value near +1 or -1
            if(std::abs(rotationComponent) > 0.9f) rotationComponent = signValue * 1.0f;
            // value near 0.5f or -0.5f
            if(glm::epsilonEqual(std::abs(rotationComponent), 0.5f, 0.1f)) rotationComponent = signValue * 0.5f;
            // value around sin(45.0f)
            if(glm::epsilonEqual(std::abs(rotationComponent), 0.7f, 0.1f)) rotationComponent = signValue * sin45;
        };

        float rotationComponents[4] = {transform.quatRotation.w, transform.quatRotation.x, transform.quatRotation.y, transform.quatRotation.z};
        std::for_each(std::execution::par, std::begin(rotationComponents), std::end(rotationComponents), processComponent);
    }
}