

#include "barPerlin.h"

Perlin::Perlin(int seed_in, int frequency_in) {
	seed = seed_in;
	frequency = frequency_in;
}

void Perlin::setup(int seed_in, int frequency_in){
    seed = seed_in;
    frequency = frequency_in;
}


float Perlin::lerp(float a0, float a1, float w) {
	return (1.0 - w)*a0 + w*a1;
}

float Perlin::dot_grid_gradient(int ix, int iy, float x, float y) {

	// Compute the distance vector
	float dx = x - (double)ix;
	float dy = y - (double)iy;

	// Compute the dot-product
	return (dx*grad(ix, iy, 0) + dy*grad(ix, iy, 1));
}

//Gradient function
float Perlin::grad(float x, float y, float z) {
	int h = seed & 15;
	double u = h<8 ? x : y,
		v = h<4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Compute Perlin noise at coordinates x, 0
float Perlin::perlin(float x) {
	return perlin(x, 0);
}

// Compute Perlin noise at coordinates x, y
float Perlin::perlin(float x, float y) {

	x /= frequency;
	y /= frequency;
	// Determine grid cell coordinates
	int x0 = (x > 0.0 ? (int)x : (int)x - 1);
	int x1 = x0 + 1;
	int y0 = (y > 0.0 ? (int)y : (int)y - 1);
	int y1 = y0 + 1;

	// Determine interpolation weights
	// Could also use higher order polynomial/s-curve here
	float sx = x - (double)x0;
	float sy = y - (double)y0;

	// Interpolate between grid point gradients
	float n0, n1, ix0, ix1, value;
	n0 = dot_grid_gradient(x0, y0, x, y);
	n1 = dot_grid_gradient(x1, y0, x, y);
	ix0 = lerp(n0, n1, sx);
	n0 = dot_grid_gradient(x0, y1, x, y);
	n1 = dot_grid_gradient(x1, y1, x, y);
	ix1 = lerp(n0, n1, sx);
	value = lerp(ix0, ix1, sy);
	

	return value*8.0-1.0;
}
