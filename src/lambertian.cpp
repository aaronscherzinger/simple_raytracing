#include "lambertian.h"

Lambertian::Lambertian(const glm::vec3& a)
: m_albedo(a)
{
}

bool Lambertian::Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const
{
    // compute random direction due to diffuse reflection
    glm::vec3 target = rec.p + rec.normal + RandomInUnitSphere();
    scattered = Ray(rec.p, glm::normalize(target - rec.p));
    attenuation = m_albedo;

    return true;
}
