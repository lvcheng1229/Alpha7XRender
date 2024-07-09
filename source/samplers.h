#pragma once

class CSampler
{
public:
	//static CSampler* create();
	inline int getSamplersPerPixel()const { return samplers_per_pixel; }
private:
	int samplers_per_pixel;
};