#pragma once

#include "commonheader.h"

#include "hitable.h"

class Material;

class Sphere : public Hitable
{
public:
    Sphere(glm::vec3 center, float radius, const Material* material);

    Sphere() = delete;

    glm::vec3 GetCenter() const { return m_center; }
    float GetRadius() const { return m_radius; }
    const Material* GetMaterial() const { return m_material; }

    virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const override;

private:
    glm::vec3 m_center;
    float m_radius;

    const Material* m_material;
};
