// Path tracer for the personnal project of Aryan Shadzi

#include <iostream>
#include <SDL.h>
#include "Vector.h"
#include "Camera.h"
#include "Object.h"
#include "Ray.h"
#include "Renderer.h"

//hi
constexpr int screen_width = 300;
constexpr int screen_height = 300;

using namespace std;



int main(int argc, char** argv)
{
	SDL_Window* window = SDL_CreateWindow(
		"PP Ray Tracer Aryan Shadzi",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		screen_width,
		screen_height, 
		SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS
	);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// setup
	Scene scene;
	scene.SkyBox.material.materialize("C:\\PP\\PP_RayTracer1\\lt.png");
	RENDERER RENDERER;
	RENDERER.CONFIG = RENDERER_COMPLEX;
	RENDERER.scene = &scene;

	Camera cam = Camera((double)screen_width / (double)screen_height, Vec3(), 1.7);
	cam.position = Vec3(0,0, -0.5);
	
	scene.cam = &cam;




	//DAVIDDD

	scene.planes.push_back(Plane(Vec3(0, 0.5, 0), Vec3(0, -1, 0)));

	scene.planes.push_back(Plane(Vec3(0, -0.5, 0), Vec3(0, 1, 0)));

	scene.planes.push_back(Plane(Vec3(0, 0, 0.5), Vec3(0, 0, -1)));
	scene.planes[2].material.diffuse_reflectance = Vec3(0.3, 0.4, 1.0);

	scene.planes.push_back(Plane(Vec3(0, 0, -0.5), Vec3(0, 0, 1)));

	scene.planes.push_back(Plane(Vec3(0.5, 0, 0), Vec3(-1, 0, 0)));
	scene.planes[4].material.diffuse_reflectance = Vec3(1, 0.4, 0.35);

	scene.planes.push_back(Plane(Vec3(-0.5, 0, 0), Vec3(1, 0, 0)));
	scene.planes[5].material.diffuse_reflectance = Vec3(0.35, 1, 0.4);

	scene.spheres.push_back(Sphere(Vec3(-0.0, 0.0, 0.5), 0.3));
	//scene.spheres[0].material.emission_strength = 1.0;
	scene.spheres[0].material.materialize("C:\\PP\\PP_RayTracer1\\earth.png");
	scene.spheres[0].material.roughness = 0.0;
	scene.spheres[0].material.specular_probability = 0.1;


	//scene.spheres.push_back(Vec3())

	
	scene.meshes.push_back(Mesh::from_STL("C:\\PP\\PP_RayTracer1\\square.stl", Vec3(0.499, 0, 0), Vec3(0.0, 0.0, 0.5 * M_PI), Vec3(0.2)));
	scene.meshes[0].material.emission_strength = 3.0;


	//scene.meshes.push_back(Mesh::from_STL("C:\\PP\\PP_RayTracer1\\bison.stl", Vec3(0, 0, 4), Vec3(0, 4.0, 0.1), Vec3(5, 5, 5)));
	bool RUNNING = true;
	double t = 0.0;
	int frame = 1;
	while (RUNNING)
	{
		//cam.change_angles(0.2, 0.0);
		frame += 1;
		t += 0.1;
		//scene.spheres[0].material.IOR += 0.1;
		//scene.spheres[0].center.y = std::sin(t);
		//scene.spheres[0].center -= Vec3(0, 0.1, 0);
		//cam.change_angles(0.0, 0.1);

		//cam.update_FOV(t);


		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				RUNNING = 0;
			}
		}
		double du = 0.5 / (double)screen_width;  double dv = 0.5 / (double)screen_height;
		double u, v;
		for (int j = 0; j < screen_height; j++)
		{
			v = j / (double)screen_height;
			for (int i = 0; i < screen_width; i++)
			{
				u = i / (double)screen_width;
				
			
				Vec3 color = 
					RENDERER.render(u , v)
					+ RENDERER.render(u+du,v) 
					+ RENDERER.render(u+du,v+dv) 
					+ RENDERER.render(u,v+dv);

				color *= 0.25;


				Uint8 R = static_cast<Uint8>(color.x)  ;
				Uint8 G = static_cast<Uint8>(color.y);
				Uint8 B = static_cast<Uint8>(color.z);

				SDL_SetRenderDrawColor(renderer, R, G, B, 50);
				SDL_RenderDrawPoint(renderer, i, j);

				if ((j * screen_width + i) % 5000 == 0) {
					SDL_RenderPresent(renderer);

					SDL_Event e;
					while (SDL_PollEvent(&e))
					{
						switch (e.type)
						{
						case SDL_QUIT:
							RUNNING = 0;
						}
					}
				}

				
				
			}

			
			
		}
		SDL_RenderPresent(renderer);
		

		


		std::cout << 1 << std::endl;
	}

	return 0;
}
