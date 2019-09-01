#pragma once

#include "commonheader.h"

#include "material.h"

/// Diffuse material
class Lambertian : public Material
{
public:
    Lambertian(const glm::vec3& a);
    Lambertian() = delete;

    virtual bool Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override;

private:
    glm::vec3 m_albedo;
};
