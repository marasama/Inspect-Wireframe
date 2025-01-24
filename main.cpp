#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

typedef struct
{
	float x;
	float y;
	float z;
} vec3;

typedef struct
{
	vec3 vertices[3];
} triangle;

typedef struct
{
	std::vector<triangle> tris;
} mesh;

typedef struct
{
	float m[4][4];
} mat4x4;

void process_event(SDL_Event *event, int *running);

void multiplyMaxtrixVector(vec3 &i, vec3 &o, mat4x4 &m);

typedef struct
{
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    double thickness;
} AID_Line;

void AID_PutPixel(SDL_Surface *surface, int x, int y, uint32_t color)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	if (x >= 0 && x < surface->w && y >= 0 && y < surface->h)
	{
		uint32_t *pixel = (uint32_t *)((uint8_t *)surface->pixels + y * surface->pitch + x * 4);
		*pixel = color;
	}

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}
void AID_DrawLine(SDL_Surface *surface, AID_Line *line, uint32_t color)
{
    if (SDL_MUSTLOCK(surface))
        SDL_LockSurface(surface);

    int x1 = line->start_x;
    int y1 = line->start_y;
    int x2 = line->end_x;
    int y2 = line->end_y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sign_x = (x1 < x2) ? 1 : -1;
    int sign_y = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    int half_thickness = (int)(line->thickness / 2.0);

    while (1)
    {
        for (int i = -half_thickness; i <= half_thickness; i++)
        {
            for (int j = -half_thickness; j <= half_thickness; j++)
                AID_PutPixel(surface, x1 + i, y1 + j, color);
        }

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sign_x;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sign_y;
        }
    }

    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
}

void AID_PutRectangle(SDL_Surface *surface, int x, int y, int w, int h, uint32_t color)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	for (int i = x; i < x + w; i++)
	{
		for (int j = y; j < y + h; j++)
		{
			AID_PutPixel(surface, i, j, color);
		}
	}
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}
int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Wireframe Landscape", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
	if (window == NULL)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		return (-1);
	}
	SDL_Surface *screen = SDL_GetWindowSurface(window);

	if (screen == NULL)
	{
		printf("Could not get window surface: %s\n", SDL_GetError());
		return (-1);
	}

	mesh cube;
	
	triangle tmp = {{{0, 0, 0}, {0, 1, 0}, {1, 1, 0}}};
	cube.tris.push_back(tmp);
	tmp = {{{0, 0, 0}, {1, 1, 0}, {1, 0, 0}}};
	cube.tris.push_back(tmp);
	tmp = {{{0, 0, 0}, {0, 0, 1}, {0, 1, 1}}};
	cube.tris.push_back(tmp);
	tmp = {{{0, 0, 0}, {0, 1, 1}, {0, 1, 0}}};
	cube.tris.push_back(tmp);
	tmp = {{{0, 0, 0}, {0, 0, 1}, {1, 0, 1}}};
	cube.tris.push_back(tmp);
	tmp = {{{0, 0, 0}, {1, 0, 1}, {1, 0, 0}}};
	cube.tris.push_back(tmp);
	tmp = {{{1, 1, 1}, {1, 0, 1}, {0, 0, 1}}};
	cube.tris.push_back(tmp);
	tmp = {{{1, 1, 1}, {0, 0, 1}, {0, 1, 1}}};
	cube.tris.push_back(tmp);
	tmp = {{{1, 1, 1}, {0, 1, 1}, {0, 1, 0}}};
	cube.tris.push_back(tmp);
	tmp = {{{1, 1, 1}, {0, 1, 0}, {1, 1, 0}}};
	cube.tris.push_back(tmp);
	tmp = {{{1, 1, 1}, {1, 1, 0}, {1, 0, 0}}};
	cube.tris.push_back(tmp);
	tmp = {{{1, 1, 1}, {1, 0, 0}, {1, 0, 1}}};
	cube.tris.push_back(tmp);

	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = (float)HEIGHT / (float)WIDTH;
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);
	mat4x4 matProj;
	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;

	SDL_Event event;
	int running = 1;
	while (running)
	{
		while (SDL_PollEvent(&event))
			process_event(&event, &running);
		AID_PutRectangle(screen, 0, 0, WIDTH, HEIGHT, 0xFF000000);
		AID_Line line = {0, 0, 100, 100, 1.0};
		AID_DrawLine(screen, &line, 0xFFFFFFFF);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(10);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return (0);
}

void multiplyMaxtrixVector(vec3 &i, vec3 &o, mat4x4 &m)
{
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
	if (w != 0.0f)
	{
		o.x /= w;
		o.y /= w;
		o.z /= w;
	}
}

void process_event(SDL_Event *event, int *running)
{
	switch (event->type)
	{
		case SDL_QUIT:
			*running = 0;
			break;
		case SDL_KEYDOWN:
			switch (event->key.keysym.sym)
			{
				case SDLK_ESCAPE:
					*running = 0;
					break;
			}
			break;
	}
}
