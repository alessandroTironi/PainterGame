#pragma once
#include <stdlib.h>
#include <cmath>

class GaussKernel
{
private:

	float mean;

	float stdev;

	void ComputeKernel(float[] kernel, int size)
	{
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
			{
				int pointX = j - size / 2;
				int pointY = i - size / 2;
				kernel[i * size + j] = (1 / (2 * 3.14f * stdev * stdev))
					* exp(-(pointX * pointX + pointY * pointY) / (2 * stdev * stdev));
			}
	}
public:
	GaussKernel(float mean, float stdev, int size, float[] kernel)
	{
		this->mean = mean;
		this->stdev = stdev;
		ComputeKernel(kernel, size);
	}
};