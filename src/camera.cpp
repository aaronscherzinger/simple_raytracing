#include "camera.h"

constexpr float pi = 3.14159265358979323846f;
constexpr float pi2 = 2.f * pi;

Trackball::Trackball()
    : m_center(glm::vec3(0.f, 0.f, 0.f))
    , m_sphereCoordinates(2.5f, pi / 2.f, pi / 2.5f)
    , m_cameraDirty(true)
    {
    }

glm::vec3 Trackball::SphericalToCartesian(glm::vec3 sphereCoordinates) const
{
    float x = glm::sin(sphereCoordinates.z) * glm::cos(sphereCoordinates.y);
    float z = glm::sin(sphereCoordinates.z) * glm::sin(sphereCoordinates.y);
    float y = glm::cos(sphereCoordinates.z);

    return sphereCoordinates.x * glm::vec3(x,y,z);
}

const Camera& Trackball::GetCamera() const
{
    if (m_cameraDirty)
    {
        UpdateCamera();
    }

    return m_camera;
}

void Trackball::UpdateCamera() const
{
    glm::vec3 position = SphericalToCartesian(m_sphereCoordinates) + m_center;
    m_camera.SetOrigin(position);

    m_camera.SetDirection(glm::normalize(m_center - position));

    // we do not set the full elevation angle, so we can still use the y-axis as the temporary up vector
    glm::vec3 tmpUp(0.f, 1.f, 0.f);
    glm::vec3 right = glm::normalize(glm::cross(m_camera.GetDirection(), tmpUp));
    glm::vec3 up = glm::cross(right, m_camera.GetDirection());

    m_camera.SetUp(up);
    m_camera.SetRight(right);

    m_cameraDirty = false;
}

void Trackball::UpdateElevationAngle(float change)
{
    static constexpr float minElevation = 0.01f;
    static constexpr float maxElevation = pi - 0.01f;

    m_sphereCoordinates.z = glm::clamp(m_sphereCoordinates.z + change, minElevation, maxElevation);

    m_cameraDirty = true;
}

void Trackball::UpdatePolarAngle(float change)
{
    m_sphereCoordinates.y += change;

    while(m_sphereCoordinates.y > pi2)
    {
        m_sphereCoordinates.y -= pi2;
    }

    while (m_sphereCoordinates.y < 0.f)
    {
        m_sphereCoordinates.y += pi2;
    }

    m_cameraDirty = true;
}
