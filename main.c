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
#define NUM_OBJECTS_SCENE 4
#define NUM_LIGHTS_SCENE 3

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
	float specular;
} sphere;

typedef struct Pixel
{
	int x;
	int y;
	color colors;
} pixel;

typedef enum {AMBIENT, POINT, DIRECTIONAL} light; 

typedef struct LightSource
{
	light type;
	float intensity;
	point position;
	vector direction;
} light_source;

//typedef struct Scene
//{
//	light_source lights;
//	sphere spheres[NUM_OBJECTS_SCENE];
//} scene;

const color BACKGROUND_COLOR = {
	255,
	255,
	255
};

bool graphicsInit(void);
float dotProduct(vector i, vector j);
float calculateLight(point P, vector normal, light_source lights[NUM_LIGHTS_SCENE], vector v, float specular);
float getVectorMagnitude(vector v);
void checkIntersection(point camera, vector ray, float solutions[2], sphere currentObject);
void graphicsCleanup(void);
void clearScreen(void);
point newPoint(float x, float y, float z);
vector newVector(point a, point b);
sphere newSphere(point center, float radius, color colors, float specular);
color newColor(uint8_t red, uint8_t green, uint8_t blue);
void rayTraceScene(point camera, sphere scene[NUM_OBJECTS_SCENE], light_source lights[NUM_LIGHTS_SCENE]);
void drawPixel(pixel currentPixel);
pixel newPixel(int x, int y, color colors);
color traceRay(point camera, vector ray, float paramMin, float paramMax, sphere scene[NUM_OBJECTS_SCENE], light_source lights[NUM_LIGHTS_SCENE]);
light_source newAmbientLight(float intensity);
light_source newPointLight(float intensity, point location);
light_source newDirLight(float intensity, vector direction);

int main(int argc, char *argv[])
{
	int success = 0;

	// Graphical Variables
	point camera = newPoint(0, 0, 0);

	// Scene objects
	sphere redSphere = newSphere(newPoint(0, -1, 3), 1, newColor(255, 0, 0), 500);
	sphere greenSphere = newSphere(newPoint(-2, 0, 4), 1, newColor(0, 255, 0), 500);
	sphere blueSphere = newSphere(newPoint(2, 0, 4), 1, newColor(0, 0, 255), 10);
	sphere bigYellowSphere = newSphere(newPoint(0, -5001, 0), 5000, newColor(255, 255, 0), 1000);
	sphere scene[NUM_OBJECTS_SCENE] = {redSphere, blueSphere, greenSphere, bigYellowSphere};

	// Scene lights
	light_source ambLight1 = newAmbientLight(0.2);
	light_source pointLight1 = newPointLight(0.6, newPoint(2, 1, 0));
	light_source dirLight1 = newDirLight(0.2, newVector(newPoint(0, 0, 0), newPoint(1, 4, 4)));
	light_source lights[NUM_LIGHTS_SCENE] = {ambLight1, pointLight1, dirLight1};

	if(graphicsInit())
	{
		rayTraceScene(camera, scene, lights);
		SDL_Delay(10000);
	}
	else 
	{
		printf("Failed to initialize graphics, aborting.\n");
		success = -1;
	}

	graphicsCleanup();
	return success;

	// ride, captain, ride
	// upon your mystery ship!
}

light_source newAmbientLight(float intensity)
{
	light_source newLight;
	newLight.type = AMBIENT;
	newLight.intensity = intensity;
	return newLight;
}

light_source newPointLight(float intensity, point position)
{
	light_source newLight;
	newLight.type = POINT;
	newLight.intensity = intensity;
	newLight.position = position;
	return newLight;
}

light_source newDirLight(float intensity, vector direction)
{
	light_source newLight;
	newLight.type = DIRECTIONAL;
	newLight.intensity = intensity;
	newLight.direction = direction;
	return newLight;
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

sphere newSphere(point center, float radius, color colors, float specular)
{
	sphere newSphere;
	newSphere.center = center;
	newSphere.radius = radius;
	newSphere.colors = colors;
	newSphere.specular = specular;
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

void rayTraceScene(point camera, sphere scene[NUM_OBJECTS_SCENE], light_source lights[NUM_LIGHTS_SCENE])
{
	point viewport;

	for(int x = -SCREEN_WIDTH / 2; x < SCREEN_WIDTH / 2; x++)
	{
		for(int y = -SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT / 2; y++)
		{
			viewport = newPoint((float)x * ((float)VIEWPORT_WIDTH / SCREEN_WIDTH), (float)y * ((float)VIEWPORT_HEIGHT / SCREEN_HEIGHT), PROJECTION_PLANE);
			vector ray = newVector(camera, viewport); // get the vector from the camera canvas to the viewport
			color colors = traceRay(camera, ray, 1.0, FLT_MAX, scene, lights);
			pixel currentPixel = newPixel(x, y, colors);
			drawPixel(currentPixel);
		}
	}

	SDL_RenderPresent(renderer);
}

color traceRay(point camera, vector ray, float paramMin, float paramMax, sphere scene[NUM_OBJECTS_SCENE], light_source lights[NUM_LIGHTS_SCENE])
{
	float closestPoint = paramMax; // closest point on object struck by ray
	sphere *closestObject = NULL;
	float solutions[2]; // two solutions from quadratic parametric equation
	float intensity;
	float normalVecMag;
	point P;
	vector normal;
	vector v;
	int redCorrected;
	int greenCorrected;
	int blueCorrected;

	for(int object = 0; object < NUM_OBJECTS_SCENE; object++)
	{
		checkIntersection(camera, ray, solutions, scene[object]);
		// printf("After: Sol 1: %f | Sol 2: %f\n", solutions[0], solutions[1]);
		if(solutions[0] > paramMin && solutions[0] < paramMax && solutions[0] < closestPoint)
		{
			closestPoint = solutions[0];
			closestObject = &scene[object];
		}
		if(solutions[1] > paramMin && solutions[1] < paramMax && solutions[1] < closestPoint)
		{
			closestPoint = solutions[1];
			closestObject = &scene[object];
		}
	}

	if(closestObject == NULL)
	{
		return BACKGROUND_COLOR;
	}

	P = newPoint(camera.x + (ray.x * closestPoint), camera.y + (ray.y * closestPoint), camera.z + (ray.z * closestPoint));
	normal = newVector(closestObject->center, P);
	normalVecMag = getVectorMagnitude(normal);
	normal.x = normal.x / normalVecMag;
	normal.y = normal.y / normalVecMag;
	normal.z = normal.z / normalVecMag;
	v.x = -ray.x; v.y = -ray.y; v.z = -ray.z;
	intensity = calculateLight(P, normal, lights, v, closestObject->specular);

	// colors corrected for lighting and reflections
	redCorrected = closestObject->colors.red * intensity;
	greenCorrected = closestObject->colors.green * intensity;
	blueCorrected = closestObject->colors.blue * intensity;

	// prevent overflow
	redCorrected = redCorrected > 255 ? 255 : redCorrected;
	greenCorrected = greenCorrected > 255 ? 255 : greenCorrected;
	blueCorrected = blueCorrected > 255 ? 255 : blueCorrected;

	// return the color associated with object
	return newColor(redCorrected, greenCorrected, blueCorrected);
}

void checkIntersection(point camera, vector ray, float solutions[2], sphere currentObject)
{
	float a;
	float b;
	float c;
	float discriminant;
	vector camToSphere = newVector(currentObject.center, camera);

	a = dotProduct(ray, ray);
	b = 2 * dotProduct(camToSphere, ray);
	c = dotProduct(camToSphere, camToSphere) - (currentObject.radius * currentObject.radius);

	discriminant = (b * b) - (4 * a * c);

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

float calculateLight(point P, vector normal, light_source lights[NUM_LIGHTS_SCENE], vector v, float specular)
{
	float normalDotLightDir;
	vector lightDir; // light vector
	vector reflection;
	float reflectDotV;
	float intensity = 0;

	for(int i = 0; i < NUM_LIGHTS_SCENE; i++)
	{
		if(lights[i].type == AMBIENT)
		{
			intensity += lights[i].intensity;
		}
		else
		{
			if(lights[i].type == POINT)
			{
				lightDir = newVector(P, lights[i].position);
			}
			else 
			{
				lightDir = lights[i].direction;
			}

			normalDotLightDir = dotProduct(normal, lightDir);

			if(normalDotLightDir > 0)
			{
				intensity += (lights[i].intensity * normalDotLightDir) / (getVectorMagnitude(normal) * getVectorMagnitude(lightDir));
			}

			if(specular != -1.0)
			{
				reflection.x = 2 * normal.x * normalDotLightDir - lightDir.x;
				reflection.y = 2 * normal.y * normalDotLightDir - lightDir.y;
				reflection.z = 2 * normal.z * normalDotLightDir - lightDir.z;
				reflectDotV = dotProduct(reflection, v);

				if(reflectDotV > 0)
				{
					intensity += lights[i].intensity * pow((double)reflectDotV / (getVectorMagnitude(reflection) * getVectorMagnitude(v)), (double)specular);
				}
			}
		}
	}
	
	return intensity;
}

float getVectorMagnitude(vector v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float dotProduct(vector i, vector j)
{
	return (i.x * j.x) + (i.y * j.y) + (i.z * j.z);
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
		window = SDL_CreateWindow("Magi-Tracer Ray Tracer", SDL_WINDOWPOS_CENTERED,
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
