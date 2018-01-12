#ifndef _UTK_SAMPLER_STRATIFIED_NROOKS_
#define _UTK_SAMPLER_STRATIFIED_NROOKS_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

namespace utk
{

	class SamplerStratifiedNRooks
	{
	public:

		SamplerStratifiedNRooks() { m_jittering_range = 1.0f; setRandomSeedTime(); }

		void setRandomSeed(long unsigned int arg_seed_swapping, long unsigned int arg_seed_jittering)
		{
			m_mersenneTwister_swapping.seed(arg_seed_swapping);
			m_mersenneTwister_jittering.seed(arg_seed_jittering);
		}

		void setRandomSeedTime()
		{
			m_mersenneTwister_swapping.seed(std::chrono::system_clock::now().time_since_epoch().count());
			m_mersenneTwister_jittering.seed(std::chrono::system_clock::now().time_since_epoch().count());
		}

		void setJitteringRange(double arg_jittering_range)
		{
			m_jittering_range = fabs(arg_jittering_range);
		}

		template<unsigned int D, typename T, typename P>
		bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
		{
			if(!arg_pts.empty())
			{
				WARNING("SamplerNRooks::generateSamples the pointset to fill is not empty, clearing it ...");
				arg_pts.clear();
			}

			if (m_jittering_range < 1.0f)
			{
				WARNING("SamplerStratified::generateSamples jittering doesn't allow points to be placed inside the each whole strata");
			}
			else if (m_jittering_range > 1.0f)
			{
				WARNING("SamplerStratified::generateSamples jittering allows points to be placed outside of their strata");
			}

			if(!isFloatingType<T>())
			{
				ERROR("SamplerNRooks::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
				return false;
			}

			//load domain & toricity
			for(uint d = 0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			arg_pts.toricity = 1;

			arg_pts.resize(arg_points);

			m_distribution = std::uniform_int_distribution<int>(0, arg_points - 1);
			double unit = (1.0 / arg_points);

			for(uint i = 0; i < arg_points; i++)
			{
				double pos = i * unit;
				for(uint d = 0; d < D; d++)
				{
					double jittered = getRandom01() * m_jittering_range * unit;
					arg_pts[i].pos()[d] = pos + jittered;
				}
			}

			/// N-Rooks Swapping
			for(uint i = 0; i < arg_points-2; i++)
			{
				for(uint d = 0; d < D; d++)
				{
					int j = genRandomAB(i, arg_points-1);
					std::swap(arg_pts[i].pos()[d], arg_pts[j].pos()[d]);
				}
			}



			return true;
		};

	protected:
		double m_jittering_range;
		std::mt19937 m_mersenneTwister_jittering;

		std::mt19937 m_mersenneTwister_swapping;
		std::uniform_int_distribution<int> m_distribution;

		int genRandomAB(int A, int B)
		{
			return m_distribution(m_mersenneTwister_swapping);
		}

		double getRandom01()
		{
			return (double)m_mersenneTwister_jittering() / (double)m_mersenneTwister_jittering.max();
		}
	};

} //end namespace utk


#endif
