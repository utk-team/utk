#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>
#include <tbb/tbb.h>

#include "runDiscrepancy.hpp"

#define D 4

using namespace utk;

std::vector<long double> shared_sum_b;
std::vector<long double> shared_sum_c;

Pointset< D, double, Point<D, double> > pts;
		
class Thread
{
public:
	void operator()(const tbb::blocked_range<size_t>& r) const
	{	
		uint N = pts.size();
		
		//std::cout << "Thread " << r.begin() << "  " << r.end() << " --Start" << std::endl;
		for(unsigned int i=r.begin(); i<r.end(); i++)
		{
			Point<D, double> pti = pts[i];
			long double prodb = 1;
			for(unsigned int j=0; j<D; j++)
			{
				long double uij = pti.pos()[j];
				prodb *= ((3.0 - uij*uij)/2.0);
			}
			shared_sum_b[i] = prodb;

			for(unsigned int iprime=0; iprime<N; iprime++)
			{
				long double prodc = 1;
				Point<D, double> ptiprime = pts[iprime];
				for(unsigned int j=0; j<D; j++)
				{
					long double uij = pti.pos()[j];
					long double uiprimej = ptiprime.pos()[j];
					prodc *= (2 - std::max(uij, uiprimej));
				}
				shared_sum_c[i] += prodc;
			}
		}
		//std::cout << "Thread " << r.begin() << "  " << r.end() << " --Done" << std::endl;
	}

  Thread() {}
};

int main(int argc, char** argv)
{
	uint nbthreads = 8;
	ParamParser_getopt parser;
	
	initParserDiscrepancy(&parser);
	parser.addLongOption((char*)"nbthreads", &nbthreads, 1, assignUInt, displayUInt, (char*)"<uint=4> the number of threads", (char*)"NbThreads");
	parser.parse(argc, argv);
	
	if(!dealParamParserDiscrepancy(&parser))
		return 0;

	PointsetReader< D, double, Point<D, double> > stream;
	stream.open(param_input);
	
	DiscrepancyStatistics stats;

	int size = 0;
	while(stream.readPointset(pts))
	{
		if(param_nbpts != 0)
			pts.resize(param_nbpts);
		
		if(stats.nbpts == 0)
			stats.nbpts = pts.size();
		else if(stats.nbpts != pts.size())
			WARNING("File contains pointsets of different sizes, averaging");
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		long double discrepancy = 0;
		
		for(int d=0; d<D; d++)
		if(pts.domain.pMin.pos()[d] != 0 || pts.domain.pMax.pos()[d] != 1)
		{
			ERROR("GeneralizedL2Discrepancy::compute Pointset domain must be the unit hypercube");
			return 1;
		}
		if(pts.empty())
		{
			ERROR("GeneralizedL2Discrepancy::compute Pointset is empty");
			return 1;
		}
		
		uint N = pts.size();
		
		long double a = pow((4.0/3.0), D);
		long double factor_b = 2.0/(long double)N;
		long double factor_c = 1.0/(long double)(N);
		factor_c *= factor_c;
		
		shared_sum_b.resize(N);
		shared_sum_c.resize(N);
		
		tbb::parallel_for(tbb::blocked_range<size_t>(0, N, N/nbthreads), Thread());
		
		long double sum_b = 0;
		long double sum_c = 0;
		for(uint i=0; i<N; i++)
		{
			sum_b += shared_sum_b[i];
			sum_c += shared_sum_c[i];
		}
		
		discrepancy = sqrt(a - factor_b*sum_b + factor_c*sum_c);
		
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		if(param_verbose)
			std::cout << std::fixed << std::setprecision(5) << "Discrepancy on " << pts.size() << " samples computed in " << time_span.count() << " secs" << std::endl;
		
		if(param_brute)
		{
			if(param_output.empty())
				std::cout << pts.size() << " " << discrepancy << std::endl;
			else
			{
				std::ofstream file(param_output, std::ios::out);
				file << pts.size() << " " << discrepancy << std::endl;
				file.close();
			}
		}
		else
			stats.addValue(discrepancy);
		
		size+=pts.size();
		pts.clear();
	}
	
	stats.nbpts = size/stats.iter;
	
	if(!param_brute)
	{
		stats.computeStatistics();
		
		if(param_output.empty())
			std::cout << stats << std::endl;
		else
		{
			std::ofstream file(param_output, std::ios::out);
			file << stats << std::endl;
			file.close();
		}
	}
	
	return 0;
}
