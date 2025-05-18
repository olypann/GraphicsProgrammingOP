

#include <common/camera.hpp>

Camera::Camera(const glm::vec3 Eye, const glm::vec3 Target)
{
    eye = Eye;
    target = Target;
}

void Camera::calculateMatrices()
{
    view = glm::lookAt(eye, eye + front, worldUp);

    projection = glm::perspective(fov, aspect, near, far);
}

void Camera::calculateCameraVectors()
{
    front = glm::vec3(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch));
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::cross(right, front);
}


void Camera::quaternionCamera()
{
    
    // camera orientation usihg Euler angles
    Quaternion newOrientation(-pitch, yaw);

    // SLERP
    orientation = Maths::SLERP(orientation, newOrientation, 0.2f);
    // view matrix
    view = orientation.matrix() * Maths::translate(-eye);
    // projection matrix
    projection = glm::perspective(fov, aspect, near, far);

    // vectors for the camera
    right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    up = glm::vec3(view[0][1], view[1][1], view[2][1]);
    front = -glm::vec3(view[0][2], view[1][2], view[2][2]);
}