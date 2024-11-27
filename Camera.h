#pragma once
#include "Vector.h"
#include "Ray.h"

double pi = 3.141592653589793238467f;

class Camera
{
private:
    Vec3 front;
    Vec3 up;
    Vec3 right;
    double ClipPlaneDist;
    double aspect_ratio;
    double fov = 90.0f;
    double theta = 0.0f; // xz angle
    double phi = 0.0f; // yz angle

public:
    Vec3 position;

    void update_FOV(double FOV)
    {
        fov = FOV;
        ClipPlaneDist = 1.0/ (2.0 * aspect_ratio * std::tan(0.5 * fov));
        //ClipPlaneDist = 1.0f;
        //std::cout << ClipPlaneDist;
    }

    void update_directions()
    {
        front = Vec3::from_spherical(theta, phi);
        up = Vec3::from_spherical(theta, phi + 0.5f*pi);
        right = Vec3::from_spherical(theta + 0.5f * pi, 0.0);

    }

    double getPlaneDist() { return ClipPlaneDist; }


    Camera(double aspect_ratio, Vec3 position = Vec3(), double fov = 0.0f, double theta = 0.0f, double phi = 0.0f) :
        aspect_ratio(aspect_ratio),
        theta(theta),
        phi(phi),
        fov(fov)
    {
        update_FOV(fov);
        update_directions();
    }

    void change_angles(double dtheta, double dphi)
    {
        theta += dtheta;
        phi += dphi;
        update_directions();
    }

    Vec3 find_pixel(double u, double v)
    {
        double scalar_x = (u - 0.5f) * aspect_ratio;
        double scalar_y = (v - 0.5f) ;
        Vec3 pixel = front * ClipPlaneDist + right  * scalar_x + up * scalar_y;

        return pixel + position;
    }

    Ray get_ray(double u, double v)
    {
        Ray ray;
        ray.direction = find_pixel(u, v) - position;
        ray.direction.normalize();
        ray.origin = position;

        return ray;
    }
};