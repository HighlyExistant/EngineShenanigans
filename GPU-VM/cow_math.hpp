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
	T average(T* data, uint32_t median) 
	{
		T r_average;
		for (uint32_t i = 0; i < median; i++)
		{
			r_average += data[i];
		}
		return r_average / median;
	}


}
