#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define VIEWPORT_WIDTH 1.0
#define VIEWPORT_HEIGHT 1.0
#define PROJECTION_PLANE 1.0
#define NUM_OBJECTS_SCENE 3

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

typedef struct Color
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} color;

typedef struct Point
{
	float x;
	float y;
	float z;
} point;

typedef struct Vector
{
	float x;
	float y;
	float z;
} vector;

typedef struct Sphere
{
	point center;
	float radius;
	color colors;
} sphere;

typedef struct Pixel
{
	int x;
	int y;
	color colors;
} pixel;

const color BACKGROUND_COLOR = {
	255,
	255,
	255
};

bool graphicsInit(void);
void checkIntersection(point camera, vector ray, float solutions[2], sphere currentObject);
void graphicsCleanup(void);
void clearScreen(void);
point newPoint(float x, float y, float z);
vector newVector(point a, point b);
sphere newSphere(point center, float radius, color colors);
color newColor(uint8_t red, uint8_t green, uint8_t blue);
void rayTraceScene(point camera, sphere scene[NUM_OBJECTS_SCENE]);
void drawPixel(pixel currentPixel);
pixel newPixel(int x, int y, color colors);
color traceRay(point camera, vector ray, float paramMin, float paramMax, sphere scene[NUM_OBJECTS_SCENE]);

int main(int argc, char *argv[])
{
	int success = 0;

	// Graphical Variables
	point camera = newPoint(0, 0, 0);

	// Scene objects
	sphere scene[NUM_OBJECTS_SCENE];
	sphere redSphere = newSphere(newPoint(0, -1, 3), 1, newColor(255, 0, 0));
	sphere greenSphere = newSphere(newPoint(-2, 0, 4), 1, newColor(0, 255, 0));
	sphere blueSphere = newSphere(newPoint(2, 0, 4), 1, newColor(0, 0, 255));
	scene[0] = redSphere;
	scene[1] = blueSphere;
	scene[2] = greenSphere;

	if(graphicsInit())
	{
		rayTraceScene(camera, scene);
		SDL_Delay(5000);
	}
	else 
	{
		printf("Failed to initialize graphics, aborting.\n");
		success = -1;
	}

	graphicsCleanup();
	return success;
}

point newPoint(float x, float y, float z)
{
	point newPoint;
	newPoint.x = x;
	newPoint.y = y;
	newPoint.z = z;
	return newPoint;
}

vector newVector(point a, point b)
{
	vector newVector;
	newVector.x = b.x - a.x;
	newVector.y = b.y - a.y;
	newVector.z = b.z - a.z;
	return newVector;
}

sphere newSphere(point center, float radius, color colors)
{
	sphere newSphere;
	newSphere.center = center;
	newSphere.radius = radius;
	newSphere.colors = colors;
	return newSphere;
}

color newColor(uint8_t red, uint8_t green, uint8_t blue)
{
	color newColor;
	newColor.red = red;
	newColor.green = green;
	newColor.blue = blue;
	return newColor;
}

pixel newPixel(int x, int y, color colors)
{
	pixel newPixel;
	newPixel.x = x;
	newPixel.y = y;
	newPixel.colors = colors;
	return newPixel;
}

void drawPixel(pixel currentPixel)
{
	currentPixel.x += SCREEN_WIDTH / 2;
	currentPixel.y = (SCREEN_HEIGHT / 2) - currentPixel.y;
	SDL_SetRenderDrawColor(renderer, currentPixel.colors.red, currentPixel.colors.green, currentPixel.colors.blue, 255);
	SDL_RenderDrawPoint(renderer, currentPixel.x, currentPixel.y);
}

void rayTraceScene(point camera, sphere scene[NUM_OBJECTS_SCENE])
{
	point viewport;

	for(int x = -SCREEN_WIDTH / 2; x < SCREEN_WIDTH / 2; x++)
	{
		for(int y = -SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT / 2; y++)
		{
			printf("x: %d | y: %d\n", x, y);
			viewport = newPoint((float)x * ((float)VIEWPORT_WIDTH / SCREEN_WIDTH), (float)y * ((float)VIEWPORT_HEIGHT / SCREEN_HEIGHT), PROJECTION_PLANE);
			vector ray = newVector(camera, viewport); // get the vector from the camera canvas to the viewport
			color colors = traceRay(camera, ray, 1.0, FLT_MAX, scene);
			pixel currentPixel = newPixel(x, y, colors);
			drawPixel(currentPixel);
		}
	}

	SDL_RenderPresent(renderer);
}

color traceRay(point camera, vector ray, float paramMin, float paramMax, sphere scene[NUM_OBJECTS_SCENE])
{
	float closestPoint = paramMax; // closest point on object struck by ray
	sphere *closestObject = NULL;
	float solutions[2]; // two solutions from quadratic parametric equation

	for(int object = 0; object < NUM_OBJECTS_SCENE; object++)
	{
		checkIntersection(camera, ray, solutions, scene[object]);
		// printf("After: Sol 1: %f | Sol 2: %f\n", solutions[0], solutions[1]);
		if(solutions[0] > paramMin && solutions[0] < paramMax && solutions[0] < closestPoint)
		{
			closestPoint = solutions[0];
			closestObject = &scene[object];
			printf("Solution Detected\n");
		}
		if(solutions[1] > paramMin && solutions[1] < paramMax && solutions[1] < closestPoint)
		{
			closestPoint = solutions[1];
			closestObject = &scene[object];
			printf("Solution Detected\n");
		}
	}

	if(closestObject == NULL)
	{
		return BACKGROUND_COLOR;
	}

	return closestObject->colors;
}

void checkIntersection(point camera, vector ray, float solutions[2], sphere currentObject)
{
	float a;
	float b;
	float c;
	float discriminant;
	vector camToSphere = newVector(currentObject.center, camera);

	a = (ray.x * ray.x) + (ray.y * ray.y) + (ray.z * ray.z);
	b = 2 * ((camToSphere.x * ray.x) + (camToSphere.y * ray.y) + (camToSphere.z * ray.z));
	c = (camToSphere.x * camToSphere.x) + (camToSphere.y * camToSphere.y) + (camToSphere.z * camToSphere.z) - (currentObject.radius * currentObject.radius);

	discriminant = (b * b) - (4 * a * c);
	 printf("Discriminant: %f\n", discriminant);
	 printf("B^2: %f\n", b*b);
	 printf("-4AC: %f\n", -4*a*c);

	if(discriminant < 0.0000)
	{
		solutions[0] = FLT_MAX;
		solutions[1] = FLT_MAX;
	}
	else 
	{
		solutions[0] = (-b + sqrt(discriminant)) / (2 * a);
		solutions[1] = (-b - sqrt(discriminant)) / (2 * a);
	}

	// printf("Sol 1: %f | Sol 2: %f\n", solutions[0], solutions[1]);
}

bool graphicsInit(void)
{
	bool success = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initalized. SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		window = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if(window == NULL)
		{
			printf("Unable to create window. SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if(renderer == NULL)
			{
				printf("Unable to create renderer. SDL Error: %s\n", SDL_GetError());
				success = false;
			}
		}
	}
	
	return success;
}

void clearScreen(void)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // background
	SDL_RenderClear(renderer);
}

void graphicsCleanup(void)
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}
