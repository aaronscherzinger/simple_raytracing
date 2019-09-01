#pragma once

#include "hitable.h"

#include <vector>

class HitableList : public Hitable
{
public:
    HitableList() = default;

    void AddToList(Hitable* h);

    void clear();

    virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const; 

private:
    std::vector<Hitable*> m_list;
};
