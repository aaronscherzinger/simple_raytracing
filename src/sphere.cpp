#include "sphere.h"

#include "material.h"

Sphere::Sphere(glm::vec3 center, float radius, const Material* material)
: m_center(center)
, m_radius(radius)
, m_material(material)
{
}

bool Sphere::Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const
{
    glm::vec3 oc = r.Origin() - m_center;
    float a = glm::dot(r.Direction(), r.Direction());
    float b = glm::dot(oc, r.Direction());
    float c = dot(oc, oc) - m_radius*m_radius;
    float discriminant = b*b - a*c;

    if (discriminant > 0.f)
    {
        // check first intersection
        float tmp = (-b - glm::sqrt(discriminant)) / a;
        bool inRange = (tmp < tMax && tmp > tMin );

        if (!inRange)
        {
            // check second intersection
            tmp = (-b + glm::sqrt(discriminant)) / a;
            inRange = (tmp < tMax && tmp > tMin );
        }
        
        if (inRange)
        {
            rec.t = tmp;
            rec.p = r.PointAt(rec.t);
            rec.normal = (rec.p - m_center) / m_radius;
            rec.material = m_material;
            return true;
        }
    }

    return false;
}
