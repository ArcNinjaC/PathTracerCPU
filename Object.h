#pragma once
#include "Vector.h"
#include "Ray.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>




struct Material
{
    Vec3 diffuse_reflectance = Vec3(1.0,1.0,1.0);
    Vec3 emission_color = Vec3(1.0,1.0,1.0);
    Vec3 specular_reflectance = Vec3(1.0, 1.0, 1.0);
    Vec3 absorbance = Vec3(0.0,0.0,0.0);
    double IOR = 2.0;
    double transparency = 0.0;
    double emission_strength = 0.0;
    double roughness = 1.0;
    double metallic = 0.0;
    double specular_probability = 0.0;
    bool HasMaterial = false;


    
private:
    SDL_Surface* diffuse_reflectance_texture = nullptr;

    
    int maxX;
    int maxY;
public:

    void materialize(const char* image)
    {
        diffuse_reflectance_texture = IMG_Load(image);

        if (!diffuse_reflectance_texture) { "ERROR LOADING TEXTURE!"; }
        else
        {
            if (SDL_MUSTLOCK(diffuse_reflectance_texture)) {
                SDL_LockSurface(diffuse_reflectance_texture);
            }

            
            maxX = diffuse_reflectance_texture->w - 1;
            maxY = diffuse_reflectance_texture->h - 1;

            if (SDL_MUSTLOCK(diffuse_reflectance_texture))
            {
                SDL_UnlockSurface(diffuse_reflectance_texture);
            }
            HasMaterial = true;
        }
    }

    virtual Vec3 color_at(Vec3 uv0)
    {
        if (HasMaterial) { 
            if (SDL_MUSTLOCK(diffuse_reflectance_texture)) {
                SDL_LockSurface(diffuse_reflectance_texture);
            }

            int Px = uv0.x * maxX;
            int Py = uv0.y * maxY;


            if (uv0.x > 1 || uv0.x < 0) { std::cout << "whoops" << std::endl; }

            Uint8* pixels = (Uint8*)diffuse_reflectance_texture->pixels;
            Uint32 bytesPerPixel = diffuse_reflectance_texture->format->BytesPerPixel;
            Uint8* location = pixels + Py * diffuse_reflectance_texture->pitch + Px * bytesPerPixel;

            

            // Get the pixel value
            Uint32 pixelColor;
            switch (bytesPerPixel) {
            case 1:
                pixelColor = *location;
                break;
            case 2:
                pixelColor = *(Uint16*)location;
                break;
            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                    pixelColor = location[0] << 16 | location[1] << 8 | location[2];
                }
                else {
                    pixelColor = location[0] | location[1] << 8 | location[2] << 16;
                }
                break;
            case 4:
                pixelColor = *(Uint32*)location;
                break;
            default:
                pixelColor = 0; // Should not happen
            }

          
            Vec3 out;
            Uint8 R, G, B;

            SDL_GetRGB(pixelColor, diffuse_reflectance_texture->format, &R, &G, &B);

            if (SDL_MUSTLOCK(diffuse_reflectance_texture))
            {
                SDL_UnlockSurface(diffuse_reflectance_texture);
            }

            //std::cout << B << std::endl;

            out.x = static_cast<double>(R);
            out.y = static_cast<double>(G);
            out.z = static_cast<double>(B);

          
           // std::cout << out.y << std::endl;

            return out / 255.0;

            


        }
        else { return diffuse_reflectance; }
    }
};



struct HitInfo
{
    bool hit;
    double dist;
    Vec3 point;
    Vec3 normal;
    Material* material;
    Vec3 uv0;


    void correct_normal(Vec3 dir)
    {
        if (normal.dot(dir) > 0) { normal *= -1.0; }
    }
};


struct Object
{
    Material material;
    virtual HitInfo intersect(const Ray& ray) = 0;
};




struct Plane : Object
{
    Vec3 point = Vec3();
    Vec3 normal = Vec3();

    Plane(Vec3 point = Vec3(), Vec3 normal = Vec3()) : point(point), normal(normal.normalized()) {}

    HitInfo intersect(const Ray& ray) override
    {
        HitInfo h;
        h.hit = false;

        double dotproduct = ray.direction.dot(normal);

        if (dotproduct == 0 || (ray.origin - point).dot(normal) == 0) { return h; }
        
        double t = -(ray.origin - point).dot(normal) / dotproduct;

        h.hit = (t>=0);
        h.dist = t;
        h.point = ray.at(h.dist);
        h.normal = normal;
        h.material = &material;
        h.correct_normal(ray.direction);

        return h;
    }
};

struct Triangle : Object
{
    Vec3 A;
    Vec3 B;
    Vec3 C;

    Triangle(Vec3 A = Vec3(), Vec3 B = Vec3(), Vec3 C = Vec3()) : A(A), B(B), C(C) {}

    static double measure_Area(Vec3 A, Vec3 B, Vec3 C)
    {
        Vec3 AB = B - A;
        Vec3 AC = C - A;
        //double area = AC.length2() * AB.length2() - (AC.dot(AB)) * (AC.dot(AB));
        double area = (AC.cross(AB)).length() * 0.5;
        return area;//0.5 * std::sqrt(area);
    }

    Plane get_plane()
    {
        Vec3 b = (B - A).normalized();
        Vec3 c = (C - A).normalized();
        Vec3 n = (b.cross(c)).normalized();



        Plane  p = Plane(A, n);
        return p;
    }

    HitInfo intersect(const Ray& ray)
    override {
        Vec3 b = (B - A).normalized();
        Vec3 c = (C - A).normalized();
        Vec3 n = (b.cross(c)).normalized();



        Plane  p = this->get_plane();

        HitInfo out = p.intersect(ray);
        //Vec3 point = out.point - A;

        double current_area = Triangle::measure_Area(A, B, C);

        if (current_area == 0) { throw std::runtime_error("triangle has 0 area"); }
        

        double alpha = Triangle::measure_Area(B, out.point, C);
        double beta = Triangle::measure_Area(A, out.point, C);
        double gamma = Triangle::measure_Area(A, out.point, B);

        alpha /= current_area; beta /= current_area; gamma /= current_area;

        /*
        * Ax = 0, Ay = 0
        * Bx = 1, By = 0
        */


        if (alpha + beta + gamma - 1 > 0.0000000000001 || alpha < 0 || beta < 0 || gamma < 0) { out.hit = false; }

        out.material = &material;
        return out;
    }
};


struct Mesh : Object
{
    std::vector<Triangle> triangles;
    Vec3 center;
    Vec3 orientation;
    Vec3 scale = Vec3(1.0, 1.0, 1.0);

    static Mesh from_STL(const char* path, Vec3 center = Vec3(0,0,5), Vec3 orientation = Vec3(), Vec3 scale = Vec3(1.0,1.0,1.0))
    {
        Mesh out;
        out.center = center;
        out.scale = scale;
        out.orientation = orientation;

        std::ifstream STL_FILE(path);
        

        //STL_FILE.seekg(80);

        if (!STL_FILE.is_open()) {
            std::cout << "error opening file" << std::endl;
            throw std::runtime_error("ERROR OPENING FILE");
        }

        std::string line;

        int current_point_index = 0;
        Triangle curr;

        while (std::getline(STL_FILE, line))
        {
            

            if (line.find("outer loop") != std::string::npos)
            {
                //std::cout << "Reset\n";
                current_point_index = 0;
            }
            else if (line.find("vertex") != std::string::npos)
            {
                std::istringstream vertexstream(line);
                std::string keyword;
                std::cout << line << std::endl;
                Vec3 curr_pt;
                
                vertexstream >> keyword >> curr_pt.x >> curr_pt.y >> curr_pt.z;

                //std::cout << curr_pt.x << std::endl;

                if (current_point_index == 0)
                {
                    curr.A = curr_pt;
                }

                else if (current_point_index == 1)
                {
                    curr.B = curr_pt;
                }

                else if (current_point_index == 2)
                {
                    curr.C = curr_pt;
                }


                current_point_index += 1;

                
            } 
            else if (line.find("endloop") != std::string::npos)
            {
                //std::cout << curr.A.x << "\n" << curr.B.x << "\n";
                std::cout << line << std::endl;
               
                out.triangles.push_back(curr);
                
            }

            
        }

        int count = 0;
        for (Triangle& t : out.triangles)
        {
            std::cout << "\nTRIANGLE:" << count << "\n";
            std::cout << "A: x:" << t.A.x << " y:" << t.A.y << " z:" << t.A.z << "\n";
            std::cout << "B: x:" << t.B.x << " y:" << t.B.y << " z:" << t.B.z << "\n";
            std::cout << "C: x:" << t.C.x << " y:" << t.C.y << " z:" << t.C.z << "\n";
            std::cout << "Area: " << t.measure_Area(t.A, t.B, t.C) << "\n\n";
            
            count++;
        }

        STL_FILE.close();

        return out;
        
    }

    void ApplyTransformsPoint(Vec3& p)
    {
        p *= scale;
        p.ApplyRotateTransformes(orientation);
        p += center;
   

    }

    void ApplyTransformsTriangle(Triangle& t)
    {
        this->ApplyTransformsPoint(t.A);
        this->ApplyTransformsPoint(t.B);
        this->ApplyTransformsPoint(t.C);
    }

    HitInfo intersect(const Ray& ray) override
    {
        HitInfo out;
        out.dist = 100;
        out.hit = false;

        for (Triangle t : triangles)
        {
            this->ApplyTransformsTriangle(t);
            HitInfo current = t.intersect(ray);

            if (current.hit && current.dist <= out.dist)
            {
                //std::cout << "woah";
                out = current;
            }
        }
        out.material = &material;
        return out;
    }
};

struct Sphere : Object
{
 
    Vec3 center = Vec3();
    double radius = 1.0;

    Sphere(Vec3 center = Vec3(), double radius = 1.0) : center(center), radius(radius)
    {
    }

    Vec3 uv0(Vec3 point)
    {
        Vec3 relatived = (point - center).normalized();
        Vec3 out;
        out.x = 0.5 + 0.5 * std::atan2(relatived.z, relatived.x) / M_PI;
        out.y = 0.5 + std::asin(relatived.y) / M_PI;
      
        return out;
        
    }

    HitInfo intersect(const Ray& ray) override
    {
        Vec3 k = ray.origin - center;
        double b = 2.0 * ray.direction.dot(k);
        double c = k.length2() - radius * radius;

        double discriminant = b * b - 4 * c;
        double d = std::sqrt(b * b - 4 * c);

        HitInfo h;
        h.hit = false;
        h.material = &material;
        

        if (discriminant < 0.0)
        {

            return h;
        }



        
        if (d == 0.0 && b < 0.0)
        {
            h.dist = -b / 2.0;
            h.hit = true;
            h.point = ray.at(h.dist);
            h.normal = (h.point - center).normalized();
            h.correct_normal(ray.direction);
            h.uv0 = uv0(h.point);
            return h;
        }

        double t1 = (-b - d) / 2.0;

        if (t1 >= 0.0)
        {
            h.dist = t1;
            h.hit = true;
            h.point = ray.at(h.dist);
            h.normal = (h.point - center).normalized();
            h.correct_normal(ray.direction);
            h.uv0 = uv0(h.point);
            return h;
        }

        double t2 = (-b + d) / 2.0;

        if (t2 >= 0.0)
        {
            h.dist = t2;
            h.hit = true;
            h.point = ray.at(h.dist);
            h.normal = (h.point - center).normalized();
            h.correct_normal(ray.direction);
            h.uv0 = uv0(h.point);
        }

        return h;

    }
    
};
