#include "dielectric.h"

#include "random.h"

Dielectric::Dielectric(float ri)
: m_refractiveIndex(ri)
{
}

bool Dielectric::Refract(const glm::vec3& v, const glm::vec3& n, float niOverNt, glm::vec3& refracted) const
{
    refracted = glm::refract(v, n, niOverNt);
    if (refracted == glm::vec3(0.f))
    {
        return false;
    }
    else
    {
        return true;
    }
}

float Dielectric::Schlick(float cosine, float refractiveIndex) const
{
    float r0 = (1.f - refractiveIndex) / (1 + refractiveIndex);
    r0 = r0 * r0;
    return r0 + (1.f - r0) * glm::pow((1.f - cosine), 5.f);
}

bool Dielectric::Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const
{
    glm::vec3 outwardNormal(0.f);
    glm::vec3 reflected = glm::reflect(inRay.Direction(), rec.normal);
    float niOverNt = m_refractiveIndex;
    attenuation = glm::vec3(1.f);
    glm::vec3 refracted;
    float reflectProb;
    float cosine;
    
    if (glm::dot(inRay.Direction(), rec.normal) > 0.f)
    {
        outwardNormal = -rec.normal;
        niOverNt = m_refractiveIndex;
        cosine = m_refractiveIndex * glm::dot(inRay.Direction(), rec.normal) / glm::length(inRay.Direction());
    }
    else
    {
        outwardNormal = rec.normal;
        niOverNt = 1.f / m_refractiveIndex;
        cosine = -glm::dot(inRay.Direction(), rec.normal) / glm::length(inRay.Direction()); 
    }

    if (Refract(glm::normalize(inRay.Direction()), glm::normalize(outwardNormal), niOverNt, refracted))
    {
        reflectProb = Schlick(cosine, m_refractiveIndex);
    }
    else
    {
        reflectProb = 1.f;
    }

    if (GetNextRandom() <= reflectProb)
    {
        scattered = Ray(rec.p, glm::normalize(reflected));
    }
    else
    {
        scattered = Ray(rec.p, glm::normalize(refracted));
    }

    return true;
}
