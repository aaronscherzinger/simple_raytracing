#pragma once

#include "commonheader.h"

class Camera
{
public:
    Camera()
    : m_origin(glm::vec3(0.f))
    , m_direction(glm::vec3(0.f, 0.f, -1.f))
    , m_up(glm::vec3(0.f, 1.f, 0.f))
    , m_right(glm::vec3(1.f, 0.f, 0.f))
    {
    }

    glm::vec3 GetOrigin() const { return m_origin; }
    void SetOrigin(glm::vec3 origin) { m_origin = origin; }

    glm::vec3 GetDirection() const { return m_direction; }
    void SetDirection(glm::vec3 direction) { m_direction = direction; }

    glm::vec3 GetUp() const { return m_up; }
    void SetUp(glm::vec3 up) { m_up = up; }

    glm::vec3 GetRight() const { return m_right; }
    void SetRight(glm::vec3 right) { m_right = right; }

private:
    glm::vec3 m_origin;
    glm::vec3 m_direction;
    glm::vec3 m_up;
    glm::vec3 m_right;
};

class Trackball
{
public:
    Trackball(); 

    glm::vec3 GetCenter() const { return m_center; }
    void SetCenter(glm::vec3 center) { m_center = center; m_cameraDirty = true; }

    float GetRadius() const { return m_sphereCoordinates.x; }
    void SetRadius(float radius) { m_sphereCoordinates.x = radius; m_cameraDirty = true; }

    float GetPolarAngle() const { return m_sphereCoordinates.y; }
    void UpdatePolarAngle(float change);

    float GetElevationAngle() const { return m_sphereCoordinates.z; }
    void UpdateElevationAngle(float change);

    const Camera& GetCamera() const;

private:

    glm::vec3 SphericalToCartesian(glm::vec3 sphereCoordinate) const;
    void UpdateCamera() const;

    mutable Camera m_camera;
    glm::vec3 m_center;
    glm::vec3 m_sphereCoordinates; ///< (radius, polar angle, elevation angle) with polar angle in [0, 2*PI] and elevation angle in [0, PI]
    
    mutable bool m_cameraDirty;
};
