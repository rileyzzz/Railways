// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinNoise.h"
#include <random>
#include <numeric>

float PerlinNoise::lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float PerlinNoise::fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::grad(int32 hash, float x, float y, float z)
{
	int h = hash & 15;
	// Convert lower 4 bits of hash into 12 gradient directions
	double u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

PerlinNoise::PerlinNoise(uint32 seed)
{
	//p.AddUninitialized(256);
	p.resize(256);
	std::iota(p.begin(), p.end(), 0);

	std::default_random_engine engine(seed);
	// Suffle  using the above random engine
	std::shuffle(p.begin(), p.end(), engine);

	// Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}

PerlinNoise::~PerlinNoise()
{
}

float PerlinNoise::noise(float x, float y, float z)
{
	int X = FMath::FloorToInt(x) & 255,
		Y = FMath::FloorToInt(y) & 255,
		Z = FMath::FloorToInt(z) & 255;
	
	x -= FMath::FloorToFloat(x);                                // FIND RELATIVE X,Y,Z
	y -= FMath::FloorToFloat(y);                                // OF POINT IN CUBE.
	z -= FMath::FloorToFloat(z);
	double u = fade(x),                                // COMPUTE FADE CURVES
		v = fade(y),                                // FOR EACH OF X,Y,Z.
		w = fade(z);

	int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,      // HASH COORDINATES OF
	B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;      // THE 8 CUBE CORNERS,

	return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),  // AND ADD
		grad(p[BA], x - 1, y, z)), // BLENDED
		lerp(u, grad(p[AB], x, y - 1, z),  // RESULTS
			grad(p[BB], x - 1, y - 1, z))),// FROM  8
		lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),  // CORNERS
			grad(p[BA + 1], x - 1, y, z - 1)), // OF CUBE
			lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
				grad(p[BB + 1], x - 1, y - 1, z - 1))));
}
