//The MIT License (MIT)
//
//Copyright (c) 2015-2020 Stanislav
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include "env_map_math.h"

enum ENVMAP_FACES
{
	ENVMAP_POSITIVEX,
	ENVMAP_NEGATIVEX,
	ENVMAP_POSITIVEY,
	ENVMAP_NEGATIVEY,
	ENVMAP_POSITIVEZ,
	ENVMAP_NEGATIVEZ
};

//Functions that transforms 3d direction to cubemap 2d coordinates and vica versa
inline double2 cube2uv(double3 IN, int* n)
{
	double2 uv;
	double3 inSQ=IN;

	if (inSQ.x<0)
	{
		inSQ.x=-inSQ.x;
	}
	if (inSQ.y<0)
	{
		inSQ.y=-inSQ.y;
	}
	if (inSQ.z<0)
	{
		inSQ.z=-inSQ.z;
	}

	uv.x=0.5;
	uv.y=0.5;

	if (inSQ.x>=inSQ.y&&inSQ.x>=inSQ.z)
	{
		if (IN.x>0)
		{
			*n = 0;
			uv.x+=IN.z/IN.x*0.5;
			uv.y-=IN.y/IN.x*0.5;
		}
		else
		{
			*n = 1;
			uv.x+=IN.z/IN.x*0.5;
			uv.y+=IN.y/IN.x*0.5;
		}
	}

	if (inSQ.y>inSQ.x&&inSQ.y>=inSQ.z)
	{
		if(IN.y>0)
		{
			*n = 2;
			uv.x-=IN.x/IN.y*0.5;
			uv.y+=IN.z/IN.y*0.5;
		}
		else
		{
			*n = 3;
			uv.x+=IN.x/IN.y*0.5;
			uv.y+=IN.z/IN.y*0.5;
		}
	}
	if(inSQ.z>inSQ.x&&inSQ.z>inSQ.y)
	{
		if(IN.z>0)
		{
			*n = 4;
			uv.x-=IN.x/IN.z*0.5;
			uv.y-=IN.y/IN.z*0.5;
		}
		else
		{
			*n = 5;
			uv.x-=IN.x/IN.z*0.5;
			uv.y+=IN.y/IN.z*0.5;
		}
	}
	return uv;
}

inline double3 uv2cube(double2 uv, int n)
{
	double3 v(0.0, 0.0, 0.0);

	switch(n)
	{
	case ENVMAP_POSITIVEX:
		{
			v.x = 1;
			v.z = v.x*(2*uv.x - 1.0);
			v.y = v.x*(1.0 - 2*uv.y);
		}
	break;
	case ENVMAP_NEGATIVEX:
		{
			v.x = -1;
			v.z = v.x*(2*uv.x - 1.0);
			v.y = v.x*(2*uv.y - 1.0);
		}
	break;
	case ENVMAP_POSITIVEY:
		{
			v.y = 1;
			v.x = v.y*(1.0 - 2*uv.x);
			v.z = v.y*(2*uv.y - 1.0);
		}
	break;
	case ENVMAP_NEGATIVEY:
		{
			v.y = -1;
			v.x = v.y*(2*uv.x - 1.0);
			v.z = v.y*(2*uv.y - 1.0);
		}
	break;
	case ENVMAP_POSITIVEZ:
		{
			v.z = 1;
			v.x = v.z*(1.0 - 2*uv.x);
			v.y = v.z*(1.0 - 2*uv.y);
		}
	break;
	case ENVMAP_NEGATIVEZ:
		{
			v.z = -1;
			v.x = v.z*(1.0 - 2*uv.x);
			v.y = v.z*(2*uv.y - 1.0);
		}
	}
	v.Normalize();
	return v;
}
