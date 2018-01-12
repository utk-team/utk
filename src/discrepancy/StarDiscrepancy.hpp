#ifndef _UTK_STAR_DISCREPANCY_
#define _UTK_STAR_DISCREPANCY_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"

namespace utk
{

template<unsigned int D, typename T, typename P>	
class sortX
{
public:
  const Pointset<D, T, P>*  pt_pts;
  bool operator()(int i, int j) {
		return ((*pt_pts)[i].pos()[0] < (*pt_pts)[j].pos()[0]);
  }
};

template<unsigned int D, typename T, typename P>	
class sortY
{
public:
  const Pointset<D, T, P>*  pt_pts;
  bool operator()(int i, int j) {
		return ((*pt_pts)[i].pos()[1] < (*pt_pts)[j].pos()[1]);
  }
};
	
class StarDiscrepancy
{
public:
	
	template<unsigned int D, typename T, typename P>
	bool compute(Pointset<D, T, P>& arg_pts, double& stardiscrepancy)
	{
		if(isIntegerType<T>())
		{
			ERROR("StarDiscrepancy::compute Point coordinates can't be integers, Pointset domain must be the unit hypercube");
			return false;
		}
		if(D != 2)
		{
			ERROR("StarDiscrepancy::compute Star Discrepancy can only be computed in 2D");
			return false;
		}
		for(uint d=0; d<D; d++)
			if(arg_pts.domain.pMin.pos()[d] != 0 || arg_pts.domain.pMax.pos()[d] != 1)
			{
				ERROR("StarDiscrepancy::compute Pointset domain must be the unit hypercube");
				return false;
			}
		if(arg_pts.empty())
		{
			ERROR("StarDiscrepancy::compute Pointset is empty");
			return false;
		}
			
		//The orignal algorithm used std::sort to sort the the
		//indices of the sample points based on which their underlying points
		//std::sort needs a global variable to access pts
		sortX<D, T, P> sx;
		sortY<D, T, P> sy;
		sx.pt_pts = &arg_pts;
		sy.pt_pts = &arg_pts;
	
		int nPts = arg_pts.size();
		
		stardiscrepancy = 0;
		
		//Complexity N log(N)
		std::vector<int> sortedX;
		sortedX.resize(nPts);
		for(int i=0; i<nPts; ++i) sortedX[i] = i;
		std::sort(sortedX.begin(), sortedX.end(), sx);
		
		//Complexity N log(N)
		std::vector<int> sortedY;
		sortedY.resize(nPts);
		for(int i=0; i<nPts; ++i) sortedY[i] = i;
		std::sort(sortedY.begin(), sortedY.end(), sy);

		int* gridCurrent = new int[nPts+1];
		int* gridBlock = new int[nPts+1];
		int* gridX = new int[nPts+1];
		int* gridY = new int[nPts+1];
		/*std::vector<int> gridCurrent(nPts+1, 0);
		std::vector<int> gridBlock(nPts+1, 0);
		std::vector<int> gridX(nPts+1, 0);
		std::vector<int> gridY(nPts+1, 0);*/
		
		//Complexity N
		for(int i=0; i<=nPts; i++)
		{
			gridCurrent[i] = 0;
			gridBlock[i] = 0;
			gridX[i] = 0;
		}
		
		//Complexity N*N
		{
			double boxX0;
			double boxY0;
			double boxX1;
			double boxY1;
			
			for(int j=0; j<=nPts; j++)
			{
				gridY[0] = 0;
				
				int ptsIndice = -1;
				
				//~ #pragma omp parallel for reduction(max : ptsIndice)
				//~ #pragma omp parallel for
				for(int i=0; i<=nPts; i++)
				{
					int v;
					if(i>0 && j>0 && (sortedX[i-1] == sortedY[j-1]))
					{
						v = i;
					}
					else v = -1;
					
					//~ #pragma omp flush(ptsIndice)
					if (v > ptsIndice)
					{
						//~ #pragma omp critical
						{
							if (v > ptsIndice) ptsIndice = v;
						}
					}
				}
				if(ptsIndice < 0) ptsIndice = std::numeric_limits<int>::max();
				
				//~ #pragma omp parallel for 
				for(int i=0; i<=nPts; i++)
				{
					if(i<ptsIndice) gridY[i] = 0;
					else gridY[i] = 1;
				}
				
				//~ #pragma omp parallel for 
				for(int i=0; i<=nPts; i++)
				{
					gridCurrent[i] = gridX[i];
					if(i>0 && j>0)
					{
						gridCurrent[i] += gridY[i-1] + gridBlock[i-1];
					
						if(sortedX[i-1] == sortedY[j-1])
						{
							if(ptsIndice != i) std::cerr << "Ahhhhg! " << i << " != " << ptsIndice << std::endl;
							++gridX[i];
							++gridCurrent[i];
						}
					}
				}
				
				//~ #pragma omp parallel for
				for(int i=0; i<=nPts; i++)
				{
					gridBlock[i] = gridCurrent[i];
				}
				
				if(j>0) boxY0 = arg_pts[sortedY[j-1]].pos()[1];
				else boxY0 = 0.0;
				
				if(j<nPts) boxY1 = arg_pts[sortedY[j]].pos()[1];
				else boxY1 = 1.0;
				
				//~ #pragma omp parallel for reduction(max : discrepancy) 
				//~ #pragma omp parallel for
				for(int i=0; i<=nPts; i++)
				{
					if(i>0) boxX0 = arg_pts[sortedX[i-1]].pos()[0];
					else boxX0 = 0.0;
					
					if(i<nPts) boxX1 = arg_pts[sortedX[i]].pos()[0];
					else boxX1 = 1.0;
				
					double d = stardiscrepancy;
					double error0 = (double)gridCurrent[i]/(double)nPts - boxX0*boxY0;
					if(error0 < 0)
						error0 *= -1; //std::abs(static_cast<double>(gridCurrent[i])/nPts - boxX0*boxY0);
					if(error0 > d)
					{
						d = error0;
					}
					
					//double error1 = std::abs(static_cast<double>(gridCurrent[i])/nPts - boxX1*boxY1);
					double error1 = (double)gridCurrent[i]/(double)nPts - boxX1*boxY1;
					if(error1 < 0)
						error1 *= -1;
					if(error1 > d)
					{
						d = error1;
					}
					
					//double error2 = std::abs(static_cast<double>(gridCurrent[i])/nPts - boxX0*boxY1);
					double error2 = (double)gridCurrent[i]/(double)nPts - boxX0*boxY1;
					if(error2 < 0)
						error2 *= -1;
					if(error2 > d)
					{
						d = error2;
					}
					
					//double error3 = std::abs(static_cast<double>(gridCurrent[i])/nPts - boxX1*boxY0);
					double error3 = (double)gridCurrent[i]/(double)nPts - boxX1*boxY0;
					if(error3 < 0)
						error3 *= -1;
					if(error3 > d)
					{
						d = error3;
					}
					
					//~ #pragma omp flush(discrepancy)
					if (d > stardiscrepancy)
					{
						//~ #pragma omp critical
						{
							if (d > stardiscrepancy) stardiscrepancy = d;
						}
					}
				}
			}
		}
		
		delete[] gridCurrent;
		delete[] gridBlock;
		delete[] gridX;
		
		return true;
	}
	
protected:

};

}//end namespace utk

#endif
