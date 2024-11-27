#pragma once
#include "Camera.h"
#include "Object.h"
#include "Ray.h"
#include <array>
#include <vector>

#define RENDERER_SIMPLE 0
#define RENDERER_COMPLEX 1

#define MAX_BOUNCE 5
#define NORMAL_OFFSET_HP 0.001
#define HIT_DIST_TOLERANCE 0.000000001
#define NUM_RAYS_PP 50.0


Vec3 gamma_correct(Vec3 col)
{
	return Vec3(std::sqrt(col.x), std::sqrt(col.y), std::sqrt(col.z));
}

struct Scene
{
	std::vector<Sphere> spheres = {};
	std::vector<Plane> planes = {};
	std::vector<Triangle> triangles = {};
	std::vector<Mesh> meshes = {};
	Sphere SkyBox = Sphere(Vec3(0,0,0), 1.0);
	Camera* cam = nullptr;
	Vec3 SIMPLE_LIGHT_DIR = Vec3(-1, -1, -1).normalized();
	double render_distance = 100.0;
	//double HIT_DIST_TOLERANCE = 0.000001;

	Scene() = default;
};




struct RENDERER
{
	Scene* scene;
	int CONFIG; // 1 if complex and 0 if simple


	Vec3 render(double u, double v)
	{
		Ray ray = scene->cam->get_ray(u, v);
		Vec3 out_color;
		//get_color(ray);\

		if (CONFIG)
		{

			for (int i = 0; i < NUM_RAYS_PP; i++)
			{
				out_color += get_color(ray);
			}

			out_color /= NUM_RAYS_PP;
		}
		else
		{
			out_color = get_color(ray);
		}

		if (out_color.x > 1) { out_color.x = 1.0; }
		if (out_color.y > 1) { out_color.y = 1.0; }
		if (out_color.z > 1) { out_color.z = 1.0; }

		out_color = gamma_correct(out_color);

		return out_color * 255.0;

	}

	Vec3 get_color(Ray ray)
	{
		Vec3 ray_color = Vec3(1.0, 1.0, 1.0);
		Vec3 out_color = Vec3(0.0, 0.0, 0.0);

		for (int i = 0; i < MAX_BOUNCE; i++)
		{
			//std::cout << i << std::endl;

			HitInfo hit = ClosestHit(ray);

			

			if (hit.hit)
			{
				Vec3 specular = ray.direction - hit.normal * (ray.direction.dot(hit.normal)) * 2.0;

				if (CONFIG){
					
					if (hit.material->transparency == 0)
					{
						ray.origin = hit.point + hit.normal * NORMAL_OFFSET_HP;
						Vec3 diffuse = (hit.normal + Vec3::random1()).normalized();
						if (diffuse.dot(hit.normal) < 0) { diffuse *= -1.0; }

					


						float isSpecular = (((double)rand() / (RAND_MAX)) < hit.material->specular_probability);
						out_color += ray_color * hit.material->emission_color * hit.material->emission_strength;
						double s = isSpecular * (1 - hit.material->roughness);
						ray.direction = diffuse * (1 - s) * (1 - hit.material->metallic) + specular * s;
						ray.direction.normalize();

					

						ray_color *= hit.material->color_at(hit.uv0) * (1 - isSpecular) + hit.material->specular_reflectance * isSpecular;
					
						if (ray_color.length2() < 0.1) { break; }
					}

					else
					{
						//ray.origin = hit.point - hit.normal * NORMAL_OFFSET_HP;

						Vec3 n = hit.normal * -1.0;
						Vec3 k = n.cross(ray.direction).cross(n);
						k.normalize();

						Vec3 v = ray.direction;

						double n1 = ray.IOR;
						double n2 = hit.material->IOR;
						bool should_beer = false;

						if (n2 == n1) { n2 = 1.0; should_beer = true; }
						ray.IOR = n2;

						double mu = n1 / n2;

						double kComponent = mu * v.dot(k);
						double discriminant_for_refraction = kComponent * kComponent;
						//double discriminant_for_refraction = 1 - mu * mu * (1 - v.dot(n) * v.dot(n));

						//ray.origin = hit.point - hit.normal * NORMAL_OFFSET_HP



						if (discriminant_for_refraction < 1)
						{
							ray.origin = hit.point - hit.normal * NORMAL_OFFSET_HP;

							//double kComponent = mu * v.dot(k);
							double nComponent = std::sqrt(1 - discriminant_for_refraction);

							Vec3 refracted = k * kComponent + n * nComponent;

							double N = v.dot(n);

							double Rsn = n1 * N - n2 * nComponent;
							double Rsd = n1 * N + n2 * nComponent;

							double Rs = std::abs(Rsn / Rsd); Rs *= Rs;

							double Rpn = n1 * nComponent - n2 * N;
							double Rpd = n1 * nComponent + n2 * N;

							double Rp = std::abs(Rpn / Rpd); Rp *= Rp;

							double R = Rp + Rs; R *= 0.5;

							//std::cout << R << std::endl;

							if (((double)rand() / (RAND_MAX)) > R)
							{
								//std::cout << "yay\n";
								ray.direction = refracted;
							}
							else {
								ray.origin = hit.point + hit.normal * NORMAL_OFFSET_HP;
								ray.direction = specular;
							}


						}
						else
						{
							//should_beer = true;
							ray.origin = hit.point + hit.normal * NORMAL_OFFSET_HP;
							ray.direction = specular;
							ray.IOR = n1;
							//return Vec3(1, 0, 0);
						}
					//return ray.direction;
						if (should_beer)
						{
							ray_color.x *= std::exp(-hit.dist * hit.material->absorbance.x);
							ray_color.y *= std::exp(-hit.dist * hit.material->absorbance.y);
							ray_color.z *= std::exp(-hit.dist * hit.material->absorbance.z);
						}
					}//i -= 1;
				}
				else {
					return hit.material->color_at(hit.uv0) * 0.5 * (1 + specular.dot(scene->SIMPLE_LIGHT_DIR));
				}
			}
			else
			{
				Vec3 uv = scene->SkyBox.uv0(ray.direction.normalized());
				//std::cout << uv.x << std::endl;
				out_color += scene->SkyBox.material.color_at(uv) * ray_color * 1;
				break;
			}
		}



		//out_color.x = 0.0;
		//out_color.z = 0.0;
		//out_color.y = 0.0;


		return out_color;
	}

	HitInfo ClosestHit(Ray& ray)
	{
		double tolerance = scene->cam->getPlaneDist() + HIT_DIST_TOLERANCE;


		HitInfo closest_hit;
		closest_hit.hit = false;
		closest_hit.dist = scene->render_distance;

		for (Sphere& s : scene->spheres)
		{
			HitInfo current = s.intersect(ray);

			if (current.hit && current.dist <= closest_hit.dist && current.dist > HIT_DIST_TOLERANCE)
			{
				closest_hit = current;
			}
		}

		for (Plane& p : scene->planes)
		{
			HitInfo current = p.intersect(ray);

			if (current.hit && current.dist <= closest_hit.dist && current.dist > HIT_DIST_TOLERANCE)
			{
				closest_hit = current;
			}
		}

		for (Triangle& t : scene->triangles)
		{
			HitInfo current = t.intersect(ray);

			if (current.hit &&  current.dist <= closest_hit.dist && current.dist > HIT_DIST_TOLERANCE)
			{
				closest_hit = current;
			}
		}

		for (Mesh& m : scene->meshes)
		{
			HitInfo current = m.intersect(ray);

			if (current.hit && current.dist <= closest_hit.dist && current.dist > HIT_DIST_TOLERANCE)
			{
				closest_hit = current;
				//std::cout << "Damn" << std::endl;
			}
			
		}
		

		return closest_hit;
	}
};