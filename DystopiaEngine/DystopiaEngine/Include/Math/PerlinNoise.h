/* HEADER *********************************************************************************/
/*!
\file	PerlinNoise.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
C++ implementation of the one and only Ken Perlin (https://mrl.nyu.edu/~perlin/noise/)
All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include "Globals.h"
#include "Vectors.h"
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>

namespace Math
{
	class _DLL_EXPORT PerlinNoise final
	{
		// Permutation Array
		std::vector<int> p;

	public:
		// Initialize with references values for the permutatation vector 
		PerlinNoise();

		// Generate a new permutation vector based on seed
		explicit PerlinNoise(uint32_t seed);

		~PerlinNoise();

		/* Disable copy and move */
		PerlinNoise(const PerlinNoise&) = delete;
		PerlinNoise& operator= (const PerlinNoise&) = delete;
		PerlinNoise(PerlinNoise&&) = delete;
		PerlinNoise& operator= (PerlinNoise&&) = delete;

		// Retrieve Noise value
		float noise(float x, float y, float z) const;
		float noise(float x, float y) const;
		float noise(float x) const;

		/*float noise(Vec2 coord);
		float noise(Vec3 coord);

		inline float operator()(Vec2 coord);
		inline float operator()(Vec3 coord);*/

	private:
		static float fade(float t);
		static float lerp(float t, float a, float b);
		static float grad(int hash, float x, float y, float z);
		static float grad(int hash, float x);
		static float grad(int hash, float x, float y);
	};

	inline PerlinNoise::PerlinNoise()
	{
		p = {
			151,160,137,91,90,15,
			131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
			77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
			5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
			129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
			49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151
			};
	}

	inline PerlinNoise::PerlinNoise(const uint32_t seed)
	{
		p.resize(256);

		// Fill p with values from 0 to 255
		std::iota(p.begin(), p.end(), 0);

		// Initialize a random engine with seed
		std::default_random_engine engine(seed);

		// Shuffle  using the above random engine
		std::shuffle(p.begin(), p.end(), engine);
	}

	inline PerlinNoise::~PerlinNoise()
	{
		p.clear();
	}

	inline float PerlinNoise::noise(float x) const
	{
		int X = static_cast<int>(floor(x)) & 0xff ;
		x -= floor(x);
		float u = fade(x);
		return lerp(u, grad(p[X], x), grad(p[X + 1], x - 1)) * 2;
	}

	/*inline float PerlinNoise::noise(Vec2 coord)
	{
		return noise(static_cast<float>(coord.x), static_cast<float>(coord.y));
	}

	inline float PerlinNoise::noise(Vec3 coord)
	{
		return noise(static_cast<float>(coord.x), static_cast<float>(coord.y), static_cast<float>(coord.z));
	}*/

	/*inline float PerlinNoise::operator()(const Vec2 coord) const
	{
		return noise(coord);
	}

	inline float PerlinNoise::operator()(const Vec3 coord) const
	{
		return noise(coord);
	}*/

	inline float PerlinNoise::noise(float x, float y) const
	{
		int X = static_cast<int>(floor(x)) & 0xff;
		int Y = static_cast<int>(floor(y)) & 0xff;
		x -= floor(x);
		y -= floor(y);
		float u = fade(x);
		float v = fade(y);
		int A	= (p[X] + Y) & 0xff;
		int B	= (p[X + 1] + Y) & 0xff;
		return lerp(v,	lerp(u, grad(p[A	], x, y	   ), grad(p[B], x - 1, y)),
						lerp(u, grad(p[A + 1], x, y - 1), grad(p[B + 1], x - 1, y - 1)));
	}

	inline float PerlinNoise::noise(float x, float y, float z) const
	{
		// Find the unit cube that contains the point
		const int X = static_cast<int>(floor(x)) & 0xff ;
		const int Y = static_cast<int>(floor(y)) & 0xff ;
		const int Z = static_cast<int>(floor(z)) & 0xff ;

		// Find relative x, y,z of point in cube
		x -= floor(x);
		y -= floor(y);
		z -= floor(z);

		// Compute fade curves for each of x, y, z
		const float u = fade(x);
		const float v = fade(y);
		const float w = fade(z);

		// Hash coordinates of the 8 cube corners
		const int A		= (p[X] + Y) & 0xff;
		const int B		= (p[X + 1] + Y) & 0xff;
		const int AA	= (p[A] + Z) & 0xff;
		const int AB	= (p[A + 1] + Z) & 0xff;
		const int BA	= (p[B] + Z) & 0xff;
		const int BB	= (p[B + 1] + Z & 0xff);

		// Add blended results from 8 corners of cube
		const float res =	lerp(w, lerp(v,	lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)), 
											lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))), 
									lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)), 
											lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
		return (res + 1.0F) / 2.0F;
	}

	/* Private functions */
	inline float PerlinNoise::fade(const float t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	inline float PerlinNoise::lerp(const float t, const float a, const float b)
	{
		return a + t * (b - a);
	}

	inline float PerlinNoise::grad(int hash, float x)
	{
		return (hash & 1) == 0 ? x : -x;
	}

	inline float PerlinNoise::grad(int hash, float x, float y)
	{
		return ((hash & 1) == 0 ? x : -x) + ((hash & 2) == 0 ? y : -y);
	}

	inline float PerlinNoise::grad(int hash, float x, float y, float z)
	{
		const int h = hash & 15;
		// Convert lower 4 bits of hash into 12 gradient directions
		const float u = h < 8 ? x : y,
			  v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	
}


#endif

