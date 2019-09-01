#pragma once

#include "commonheader.h"

#include "material.h"

class Metal : public Material
{
public:
    Metal(const glm::vec3& a, float fuzziness = 0.f);
    Metal() = delete;

    virtual bool Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override;

private:
    glm::vec3 m_albedo;
    float m_fuzziness;
};
