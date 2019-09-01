#include "hitablelist.h"

void HitableList::AddToList(Hitable* h)
{
    m_list.push_back(h);
}

void HitableList::clear()
{
    m_list.clear();
}

bool HitableList::Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const
{
    HitRecord tmpRecord = { };
    bool hitAnything = false;

    float closestSoFar = tMax;

    for (auto& hitable : m_list)
    {
        if (hitable->Hit(r, tMin, closestSoFar, tmpRecord))
        {
            hitAnything = true;
            closestSoFar = tmpRecord.t;
            rec = tmpRecord;
        }
    }

    return hitAnything;
}
