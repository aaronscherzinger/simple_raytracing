#pragma once

#include "commonheader.h"

#include "material.h"

class Dielectric : public Material
{
public:
    Dielectric(float ri);
    Dielectric() = delete;

    virtual bool Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override;

private:
    bool Refract(const glm::vec3& v, const glm::vec3& n, float niOverNt, glm::vec3& refracted) const;

    float Schlick(float cosine, float refractiveIndex) const;

    float m_refractiveIndex;
};
