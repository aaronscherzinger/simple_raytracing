#include "metal.h"

Metal::Metal(const glm::vec3& a, float fuzziness)
: m_albedo(a)
, m_fuzziness(fuzziness)
{
    if (fuzziness > 1.f)
    {
        fuzziness = 1.f;
    }
    else if (fuzziness < 0.f)
    {
        fuzziness = 0.f;
    }
}

bool Metal::Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const
{
    glm::vec3 reflected = glm::reflect(inRay.Direction(), rec.normal);
    scattered = Ray(rec.p, glm::normalize(reflected + m_fuzziness * (RandomInUnitSphere())));
    attenuation = m_albedo;

    return (dot(scattered.Direction(), rec.normal) > 0);
}
