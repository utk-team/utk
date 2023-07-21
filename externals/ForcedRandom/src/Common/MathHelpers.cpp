#include <cfloat>
#include <cmath>
#include <vector>
#include <limits>

#include "MathHelpers.h"

namespace MathHelpers {
    double GetMax(const std::vector<double>& data)
	{
        double max = std::numeric_limits<double>::lowest();

		for (double v : data)
		{
			if (v > max)
			{
				max = v;
			}
		}

		return max;
    }

    double GetMean(const std::vector<double>& data)
	{
        double mean = 0.0;

		for (double v : data)
		{
			mean += v;
		}

        return mean / static_cast<double>(data.size());
    }

    double log2(double a)
	{
        return std::log(a) / std::log(2.0);
    }

	bool isPowerOfTwo(int x)
	{
		while (((x % 2) == 0) && (x > 1))
		{
			x /= 2;
		}

		return (x == 1);
	}
}
