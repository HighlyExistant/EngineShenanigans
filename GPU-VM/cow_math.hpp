#pragma once

namespace cow
{
	// Data Analysis
	template<typename T>
	constexpr T normalize(T min, T max, float t)
	{
		return (t - min) / (max - min);
	}

	template<typename T>
	constexpr T denormalize(T min, T max, float t)
	{
		return t * (max - min) + min;
	}

	template<typename T>
	T average(T* data, unsigned int median) 
	{
		T r_average;
		for (uint32_t i = 0; i < median; i++)
		{
			r_average += data[i];
		}
		return r_average / median;
	}
	template<typename T>
	void clamp(T min, T max, T *pVal) 
	{
		if (*pVal < min)
		{
			*pVal = min;
			return;
		}
		if (*pVal > max) 
		{
			*pVal = max;
			return;
		}
	}

	template<typename T>
	T clamped(T min, T max, T pVal)
	{
		if (*pVal < min)
		{
			return min;
		}
		if (*pVal > max)
		{
			return max;
		}
		return pVal;
	}
}
