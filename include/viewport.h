#pragma once

class Viewport
{
public:
    Viewport(int width, int height)
    : m_width(width)
    , m_height(height)
    {
        m_viewportSizeRcp = glm::vec2(1.f / static_cast<float>(m_width), 1.f / static_cast<float>(m_height));
        m_horizontalLinearFov = (static_cast<float>(m_width) / static_cast<float>(m_height));
    }

    Viewport() = delete;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    float GetHorizontalLinearFov() const { return m_horizontalLinearFov; }

    glm::vec2 GetViewportSizeRcp() const { return m_viewportSizeRcp; }
    
private:
    // resolution
    int m_width;
    int m_height;

    // reciprocal resolution
    glm::vec2 m_viewportSizeRcp;

    // linear fov
    float m_horizontalLinearFov;
};
