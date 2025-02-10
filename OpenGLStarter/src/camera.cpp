#include "Camera.h"

// Constructor implementation
Camera::Camera(const glm::vec3& cameraPos, const glm::vec3& cameraFront, const glm::vec3& cameraUp)
    : cameraPos(cameraPos), cameraFront(cameraFront), cameraUp(cameraUp)
{
}

// Setter implementations
void Camera::setCameraPos(const glm::vec3& pos)
{
    cameraPos = pos;
}

void Camera::setCameraFront(const glm::vec3& front)
{
    cameraFront = front;
}

void Camera::setCameraUp(const glm::vec3& up)
{
    cameraUp = up;
}

// Getter implementations
glm::vec3 Camera::getCameraPos() const
{
    return cameraPos;
}

glm::vec3 Camera::getCameraFront() const
{
    return cameraFront;
}

glm::vec3 Camera::getCameraUp() const
{
    return cameraUp;
}