#pragma once
#include <math.h>
#include <sstream>
#include <string>
//#include <vector>
//#include <limits>
#include <cassert>
//#include <iomanip>


namespace Mo2Lib
{

	//returns a random integer between x and y
	inline int RandInt(int x, int y)
	{
		assert(y >= x && "<RandInt>: y is less than x");
		return rand() / (RAND_MAX / (y - x) + 1) + x;
	}

	//returns a random double between zero and 1
	inline float RandFloat() { return (float)((double)(rand() / (double)(RAND_MAX + 1))); }

	inline float RandInRange(float x, float y)
	{
		return x + RandFloat() * (y - x);
	}

	//returns a random bool
	//inline bool   RandBool()
	//{
	//	if (RandFloat() > 0.5f) return true;
	//
	//	else return false;
	//}

	//returns a random double in the range -1 < n < 1
	inline float RandomClamped() { return RandFloat() - RandFloat(); }


	//returns a random number with a normal distribution. See method at
	//http://www.taygeta.com/random/gaussian.html
	inline float RandGaussian(float mean = 0.0f, float standard_deviation = 1.0f)
	{
		float x1, x2, w, y1;
		static float y2;
		static int use_last = 0;

		if (use_last)		        /* use value from previous call */
		{
			y1 = y2;
			use_last = 0;
		}
		else
		{
			do
			{
				x1 = 2.0f * RandFloat() - 1.0f;
				x2 = 2.0f * RandFloat() - 1.0f;
				w = x1 * x1 + x2 * x2;
			} while (w >= 1.0);

			w = sqrt((-2.0f * log(w)) / w);
			y1 = x1 * w;
			y2 = x2 * w;
			use_last = 1;
		}

		return(mean + y1 * standard_deviation);
	}


	//clamps the first argument between the second two
	template <class T, class U, class V>
	inline void Clamp(T& value, const U& min_val, const V& max_val)
	{
		assert(((V)min_val <= (V)max_val) && "<Clamp>MaxVal < MinVal!");

		if (value < (T)min_val)
		{
			value = (T)min_val;
		}

		if (value > (T)max_val)
		{
			value = (T)max_val;
		}
	}

	//compares two real numbers. Returns true if they are equal
	inline bool isEqual(float a, float b)
	{
		if (fabs(a - b) < 0.0001f)
		{
			return true;
		}

		return false;
	}

}

