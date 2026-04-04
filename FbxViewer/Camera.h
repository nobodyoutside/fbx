#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

/// @brief 구형 좌표계 오비트 카메라. 타겟 주위를 회전/줌하며 뷰/프로젝션 행렬을 생성한다.
struct Camera {
    float yaw = 0.0f;
    float pitch = 20.0f;
    float distance = 5.0f;
    glm::vec3 target = {0.0f, 0.0f, 0.0f};

    glm::mat4 viewMatrix() const {
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);
        glm::vec3 eye;
        eye.x = target.x + distance * cosf(pitchRad) * sinf(yawRad);
        eye.y = target.y + distance * sinf(pitchRad);
        eye.z = target.z + distance * cosf(pitchRad) * cosf(yawRad);
        return glm::lookAt(eye, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 projMatrix(float aspect) const {
        return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
    }

    void orbit(float dYaw, float dPitch) {
        yaw += dYaw;
        pitch = std::clamp(pitch + dPitch, -89.0f, 89.0f);
    }

    void zoom(float delta) {
        distance = std::max(0.5f, distance - delta);
    }

    void pan(float dx, float dy) {
        float yawRad = glm::radians(yaw);
        glm::vec3 right = glm::vec3(cosf(yawRad), 0.0f, -sinf(yawRad));
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        target += right * dx * distance * 0.002f;
        target += up * dy * distance * 0.002f;
    }
};

#endif // CAMERA_H
