#pragma once

#include "commonheader.h"

#include "ray.h"

class Material;

struct HitRecord
{
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    const Material* material;
};

class Hitable
{
public:
    virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const = 0;
};
