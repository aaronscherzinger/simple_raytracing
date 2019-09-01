#include "material.h"

#include "random.h"

glm::vec3 Material::RandomInUnitSphere() const
{
    glm::vec3 p;
    do
    {
        p = 2.f * glm::vec3(GetNextRandom(), GetNextRandom(), GetNextRandom()) - glm::vec3(1.f);
    }
    while(glm::dot(p, p) >= 1.f);

    return p;
}
