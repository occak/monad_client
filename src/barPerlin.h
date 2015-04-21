

#pragma once
#ifndef _PERLIN_H
#define _PERLIN_H

class Perlin {
public:
	Perlin(int seed, int frequency);
    void setup(int seed, int frequency);
	float perlin(float x);
	float perlin(float x, float y);
private:
	int seed;
	int frequency;

	float lerp(float a0, float a1, float w);
	float dot_grid_gradient(int ix, int iy, float x, float y);
	float grad(float x, float y, float z);
};

#endif