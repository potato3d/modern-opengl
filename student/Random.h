#pragma once
#include <functional>
#include <random>

template<typename t_value = double, typename t_engine = std::mt19937>
std::function<t_value()> make_random(t_value min, t_value max, t_value seed = std::random_device()())
{
	typedef typename std::conditional<std::is_integral<t_value>::value,
			std::uniform_int_distribution<t_value>,
			std::uniform_real_distribution<t_value>>::type dist_type;

	if(!std::is_integral<t_value>::value)
	{
		max = std::nextafter(max, std::numeric_limits<t_value>::max());
	}

	t_engine engine;
	engine.seed(seed);
	return std::bind(dist_type(min, max), engine);
}

template<typename t_value = double, typename t_engine = std::mt19937>
std::function<t_value()> make_random(t_value seed = std::random_device()())
{
	t_value min = 0.0;
	t_value max = 1.0;

	if(std::is_integral<t_value>::value)
	{
		max = std::numeric_limits<t_value>::max();
	}

	return make_random(min, max, seed);
}
