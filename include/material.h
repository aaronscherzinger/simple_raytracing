#pragma once

#include "commonheader.h"

#include "ray.h"
#include "hitable.h"

class Material
{
public:
    virtual bool Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const = 0;
protected:
    /// simple (and inefficient) uniform sampling of points in the unit sphere
    glm::vec3 RandomInUnitSphere() const;
};
