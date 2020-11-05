// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class RAILWAYS_API PerlinNoise
{
private:
	std::vector<int32> p;

	float lerp(float t, float a, float b);
	float fade(float t);
	float grad(int32 hash, float x, float y, float z);

public:
	PerlinNoise(uint32 seed);
	~PerlinNoise();

	float noise(float x, float y, float z);
};
