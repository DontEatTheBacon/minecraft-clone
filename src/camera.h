#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

enum class CameraDirection
{
    FORWARD,
    BACK,
    LEFT,
    RIGHT
};

namespace CameraSettings
{
    constexpr float YAW { -90.0f };
    constexpr float PITCH { 0.0f };
    constexpr float SPEED { 5.0f };
    constexpr float SENSITIVITY { 0.1f };
    constexpr float ZOOM { 45.0f };
}

class Camera
{
private:
    float m_width {};
    float m_height {};
    
    float m_yaw { CameraSettings::YAW };
    float m_pitch { CameraSettings::PITCH };

    float m_sensitivity { CameraSettings::SENSITIVITY };
    float m_speed { CameraSettings::SPEED };

    float m_zoom { CameraSettings::ZOOM };

    glm::vec3 m_pos { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_front { 0.0f, 0.0f, -1.0f };
    glm::vec3 m_up { 0.0f, 1.0f, 0.0f };

public:
    Camera() = default;

    Camera(float screen_width, float screen_height)
        : m_width { screen_width }, m_height { screen_height }
    {
    }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(m_pos, m_pos + m_front, m_up);
    }

    void updateVectors()
    {
        glm::vec3 direction;
        direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        direction.y = sin(glm::radians(m_pitch));
        direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(direction);
    }

    void processKeyInput(CameraDirection direction, float deltaTime)
    {
        const float vel { m_speed * deltaTime };
        if (direction == CameraDirection::FORWARD)
        {
            m_pos += m_front * vel;
        }

        if (direction == CameraDirection::BACK)
        {
            m_pos -= m_front * vel;
        }

        if (direction == CameraDirection::LEFT)
        {
            m_pos -= glm::normalize(glm::cross(m_front, m_up)) * vel;
        }

        if (direction == CameraDirection::RIGHT)
        {
            m_pos += glm::normalize(glm::cross(m_front, m_up)) * vel;
        }
    }

    void processMouseInput(float xoffset, float yoffset)
    {
        xoffset *= m_sensitivity;
        yoffset *= m_sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        if (m_pitch > 89.0f)
        {
            m_pitch = 89.0f;
        }
        if (m_pitch < -89.0f)
        {
            m_pitch = -89.0f;
        }

        updateVectors();
    }
};

#endif