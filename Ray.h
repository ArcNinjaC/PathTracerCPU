#pragma once
#include "Vector.h"
//#include "Object.h"



struct Ray
{
    Vec3 direction;
    Vec3 origin;
    double IOR = 1.0;

    Ray() = default;
    Ray(Vec3 origin, Vec3 direction) : origin(origin), direction(direction) {}

    Vec3 at(double t)
    const {
        return origin + direction * t;
    }
};