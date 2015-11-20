#include <random>

class Random
{
private:
	std::mt19937 _engine{ std::random_device{}() };
public:
	Random() = default;
	Random(std::mt19937::result_type seed)
		: _engine(seed)
	{
	}

	void SetSeed(std::mt19937::result_type seed)
	{
		_engine.seed(seed);
	}

	int GetInteger(int min, int max)
	{
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(_engine);
	}

	float GetFloat(float min, float max)
	{
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(_engine);
	}

	bool GetBoolean()
	{
		return GetInteger(0, 1) == 1;
	}
};
