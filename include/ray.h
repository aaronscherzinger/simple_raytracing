#pragma once

#include "commonheader.h"

class Ray
{
public:
    Ray(const glm::vec3& origin, const glm::vec3& direction)
    : m_origin(origin)
    , m_direction(direction)
    { }

    Ray() = delete;

    Ray(const Ray& other) = default;
    Ray& operator=(const Ray& other) = default;

    ~Ray() = default;

    glm::vec3& Origin() { return m_origin; }
    const glm::vec3& Origin() const { return m_origin; }

    glm::vec3& Direction() { return m_direction; }
    const glm::vec3& Direction() const { return m_direction; }

    glm::vec3 PointAt(float t) const { return m_origin + t * m_direction; }

private:
    glm::vec3 m_origin;
    glm::vec3 m_direction;
};
