#include <iostream>
#include <bitset>
#include "../../pointsets/Pointset.hpp"
#include "../../io/fileIO.hpp"

#include <boost/compute/algorithm/transform.hpp>
#include <boost/compute/algorithm/sort.hpp>
#include <boost/compute/algorithm/sort.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/functional/math.hpp>
#include <boost/compute/algorithm/count_if.hpp>
#include <boost/compute/function.hpp>

namespace compute = boost::compute;

using namespace utk;


// template<uint D>
// void permut(Pointset<D, uint, Point<D, uint> >& pts, Pointset<D, uint, Point<D, uint> >& out_pts, Vector<D, unsigned long long int> permuts)
// {
// 	
// 	uint tilesize = pts.size();
// 	
// 	int ind;
// 	std::bitset<64> xpermuts[D];
// 	for(uint d=0; d<D; d++)
// 		xpermuts[d] = permuts[d];
// 
// 	uint NBITS = log2(tilesize);
// 	uint PERMUTS_NBITS = tilesize-1;
// 	
// 	for(uint ipt=0; ipt<tilesize; ipt++) {
// 		std::bitset<64> xresdigits[D];
// 		std::bitset<64> thisSobolPt[D];
// 		for(uint d=0; d<D; d++)
// 			thisSobolPt[d] = pts[ipt].pos()[d];
// 
// 		for(uint ilevel=0; ilevel<NBITS; ilevel++) {
// 			
// 			for(uint d=0; d<D; d++)
// 			{
// 				ind = (pts[ipt].pos()[d] >> (NBITS-ilevel)) + (pow(2,ilevel)-1);
// 				xresdigits[d].set(NBITS-ilevel-1, (xpermuts[d][PERMUTS_NBITS-ind-1] ^ thisSobolPt[d][NBITS-ilevel-1]) );
// 			}
// 		}
// 		
// 		for(uint d=0; d<D; d++)
// 			out_pts[ipt].pos()[d] = xresdigits[d].to_ulong();
// 	}
// }
// 
template<uint D>
bool permutFromNbitsToMbits(uint N, uint M, const Vector<D, uint>& fixed_point, const Vector<D, unsigned long long int>& permutN, Vector<D,unsigned long long int>& permutM, uint K)
{
	int tilesize = pow(K, D);
	int LOGK= log2(tilesize);
	
	uint fixed[D];
	for(uint d=0; d<D; d++)
		fixed[d] = fixed_point[d];
	
	std::vector< std::array<uint, D> > x;
	x.resize(LOGK-1);
	for(uint d=0; d<D; d++)
	{
		int nb_flags = 2;
		int orig_lvl = tilesize-2 -1;
		for(uint i=1; i<LOGK; i++)
		{
			x[i-1][d] = orig_lvl - (fixed[d]/(tilesize/nb_flags));
			orig_lvl -= nb_flags;
			nb_flags*=2;
		}
	}
	std::bitset<64> xpermutsNbits[D];
	for(uint d=0; d<D; d++)
		xpermutsNbits[d] = permutN[d];
	
	std::bitset<64> xpermutsMbits[D];
	int xcount[D];
	for(uint d=0; d<D; d++)
		xcount[d] = tilesize-LOGK-2; // bitset indexing: 0==least significant bit
	for(uint i=1; i<tilesize-1; i++) { // start form the second i because xpermuts15bits[0].x = xpermuts15bits[0].y = 0;
		int ind = tilesize-2-i;
		
		for(uint d=0; d<D; d++)
		{
			bool isFlag=true;
			for(uint i=0; i<LOGK-1; i++)
				isFlag &= (ind != x[i][d]);
			
			if( isFlag ) {
				xpermutsMbits[d][ind] = xpermutsNbits[d][xcount[d]];
				xcount[d]--;
			}
		}
	}
	
	for(uint d=0; d<D; d++)
		permutM[d] = xpermutsMbits[d].to_ullong();
	
	return true;
}
// 
// template<uint D>
// double maximalMinimumDistance(Pointset<D, uint, Point<D, uint> > pts) {
// 	
// 	double mindist = 10000000;
// 	double tilesize = pts.size();
// 	
// 	for (uint ipts = 0; ipts < pts.size(); ipts++) {
// 		for (uint ipts_to_compare = ipts+1; ipts_to_compare < pts.size(); ipts_to_compare++) {
// 			double dist_sq = 0;
// 			for(uint d=0; d<D; d++)
// 			{
// 				double dx = (double)pts[ipts].pos()[d]/tilesize - (double)pts[ipts_to_compare].pos()[d]/tilesize;
// 				dist_sq += dx*dx;
// 			}
// 			if (dist_sq < mindist) mindist = dist_sq;
// 		}
// 	}
// 	return sqrt(mindist);
// }
// 
// void show_tree(Vector<2, unsigned long long int>& permut, uint K)
// {
// 	uint tilesize = pow(K, 2);
// 	uint depth = log2(tilesize);
// 	
// 
// 	std::bitset<64> tree = permut[0];
// 	std::cout << "Permut " << permut[0] << " = " << tree.to_ullong() << std::endl;
// 
// 	int n=tilesize-2;
// 	for(uint i=0; i<depth; i++)
// 	{
// 		for(int j=0; j<(1<<i); j++)
// 		{
// 			std::cout << tree[n] << " ";
// 			n--;
// 		}
// 		std::cout << std::endl;
// 	}
// }

#define SIZEPATTERN 64 //MUST BE 64

struct permutation
{
	int x;
	int y;
};
BOOST_COMPUTE_ADAPT_STRUCT(permutation, permutation, (x, y))

struct pattern64pts
{
	int pts[128];
};
BOOST_COMPUTE_ADAPT_STRUCT(pattern64pts, pattern64pts, (pts))


BOOST_COMPUTE_FUNCTION(double, score_permut_device2, (permutation p),
{
	int patternx[64] = {0,32,16,48,8,40,24,56,4,36,20,52,12,44,28,60,2,34,18,50,10,42,26,58,6,38,22,54,14,46,30,62,1,33,17,49,9,41,25,57,5,37,21,53,13,45,29,61,3,35,19,51,11,43,27,59,7,39,23,55,15,47,31,63};
	int patterny[64] = {0,32,48,16,40,8,24,56,60,28,12,44,20,52,36,4,34,2,18,50,10,42,58,26,30,62,46,14,54,22,6,38,51,19,3,35,27,59,43,11,15,47,63,31,39,7,23,55,17,49,33,1,57,25,9,41,45,13,29,61,5,37,53,21};
	
	int permutted_patternx[64] = {0};
	int permutted_patterny[64] = {0};
	
	uint tilesize = 64;
	
	//Apply owen scrambling
	int ind;
	uint NBITS = 6;
	uint PERMUTS_NBITS = tilesize-1;
	for(uint ipt=0; ipt<tilesize; ipt++) {
		unsigned int xresdigits =0, yresdigits =0;
		unsigned int xpt =patternx[ipt], ypt =patterny[ipt];

		for(uint ilevel=0; ilevel<NBITS; ilevel++) {
			
			ind = (patternx[ipt] >> (NBITS-ilevel)) + ((1<<ilevel)-1);
			
			int bit_permut = (p.x << (PERMUTS_NBITS-ind-1)) & 1;
			int bit_pt = (xpt << (NBITS-ilevel-1)) & 1;
			
			int bu_xresdigits = xresdigits;
			xresdigits = (xresdigits >> (NBITS-ilevel-1)) | (bitpermut ^ bitpt) 
			//xresdigits = (xresdigits << (NBITS-ilevel-1)) | bu_xresdigits;
			
			/*bit_permut = (p.y << (PERMUTS_NBITS-ind-1)) & 1;
			bit_pt = (ypt << (NBITS-ilevel-1)) & 1;
			yresdigits = ((yresdigits >> (NBITS-ilevel-1)) | (bitpermut ^ bitpt)) << (NBITS-ilevel-1);*/
		}
		
		permutted_patternx[ipt] = xresdigits;
		permutted_patterny[ipt] = yresdigits;
	}
	
	//Measure min distance
	double mindist = 10000;
	for (uint ipts = 0; ipts < tilesize; ipts++) {
		for (uint ipts_to_compare = ipts+1; ipts_to_compare < tilesize; ipts_to_compare++) {
			
			double dist_sq = 0;
			double dx = (double)permutted_patternx[ipts]/tilesize - (double)permutted_patternx[ipts_to_compare]/tilesize;
			dist_sq += dx*dx;
			double dy = (double)permutted_patterny[ipts]/tilesize - (double)permutted_patterny[ipts_to_compare]/tilesize;
			dist_sq += dy*dy;
			
			if (dist_sq < mindist) mindist = dist_sq;
		}
	}

	return sqrt(mindist);
});

BOOST_COMPUTE_FUNCTION(double, score_permut_device, (permutation p),
{
    return p.x;
});

BOOST_COMPUTE_FUNCTION(double, is_good_score, (double s),
{
    return s > 0.04 ;
});

int main(int argc, char** argv)
{
	if(argc != 3 && argc != 2)
	{
		std::cout << "To optimize a pattern: ./ " << argv[0] << " <patternfile>" << std::endl;
		std::cout << "To display a pattern: ./ " << argv[0] << " <patternfile> <1>" << std::endl;
		return 0;
	}
	
	if(argc == 3)
	{
		utk::Pointset< 2, double, utk::Point<2, double> > pts;
		utk::PointsetReader< 2, double, utk::Point<2, double> > reader;
		reader.open(argv[1]);
		reader.readPointset(pts);
		reader.close();
		assert(pts.size() == SIZEPATTERN);
		for(unsigned int i=0; i<pts.size(); i++)
			std::cout << pts[i].pos()[0]*SIZEPATTERN << ",";
		std::cout << "\n";
		for(unsigned int i=0; i<pts.size(); i++)
			std::cout << pts[i].pos()[1]*SIZEPATTERN << ",";
		std::cout << "\n";
		return 0;
	}
	
	utk::Pointset< 2, double, utk::Point<2, double> > pts;
	utk::PointsetReader< 2, double, utk::Point<2, double> > reader;
	reader.open(argv[1]);
	reader.readPointset(pts);
	reader.close();
	assert(pts.size() == SIZEPATTERN);

	// get default device and setup context
	compute::device device = compute::system::default_device();
	compute::context context(device);
	compute::command_queue queue(context, device);

	int tilesize = SIZEPATTERN;
	std::cout << "Tilesize =" << tilesize << std::endl;
	
	uint N, M;
	M = tilesize-1;
	N = M - log2(tilesize);
	int digit_optim = std::min(N, 4u);
	std::cout << "Optimizing " << digit_optim << " digits" << std::endl;
	
	int nbpermut = (1 << digit_optim);
	
	std::cout << "Nb 2D Permut =" << nbpermut*nbpermut << std::endl;
	
	// fill host vector with permutations
	std::vector<permutation> input_host_vector(nbpermut*nbpermut);
	//std::vector<pattern64pts> input_host_vector2(nbpermut*nbpermut);
	// create host result vector
	std::vector<double> result_host_vector(nbpermut*nbpermut);
	
	// create a vector on the device
	compute::vector<permutation> input_device_vector(input_host_vector.size(), context);
	//compute::vector<pattern64pts> input_device_vector2(input_host_vector.size(), context);
	// create a result vector on the device
	compute::vector<double> result_device_vector(input_host_vector.size(), context);
	

	//fill host vector with M bits permutations
	utk::Vector<2, unsigned int> pts0; 
	pts0[0] = pts[0].pos()[0]*SIZEPATTERN;
	pts0[1] = pts[0].pos()[1]*SIZEPATTERN;
	for(int i=0; i<nbpermut; i++)
	for(int j=0; j<nbpermut; j++)
	{
		utk::Vector<2, unsigned long long int> permutNbits, permutMbits;
		permutNbits[0] = i; permutNbits[1] = j;
		permutFromNbitsToMbits(N, M, pts0, permutNbits, permutMbits, sqrt(SIZEPATTERN));
		input_host_vector[i*nbpermut + j].x = permutMbits[0];
		input_host_vector[i*nbpermut + j].y = permutMbits[1];
	}
	/*for(int i=0; i<nbpermut; i++)
	for(int j=0; j<nbpermut; j++)
	{
		for(int x=0; x<64; x++)
		{
			input_host_vector2[i*nbpermut + j].pts[2*x] = (int)pts[i].pos()[0];
			input_host_vector2[i*nbpermut + j].pts[2*x+1] = (int)pts[i].pos()[1];
		}
	}*/
	
	// transfer data from the host to the device
	compute::copy(input_host_vector.begin(), input_host_vector.end(), input_device_vector.begin(), queue);
	//compute::copy(input_host_vector2.begin(), input_host_vector2.end(), input_device_vector2.begin(), queue);
	
	// calculate the score of each permutation
	compute::transform(
						input_device_vector.begin(),
						input_device_vector.end(),
					    //input_device_vector2.begin(),
						result_device_vector.begin(),
						score_permut_device2,
						queue
						);
	
	
	// transfer results from the device to the host
	compute::copy(result_device_vector.begin(), result_device_vector.end(), result_host_vector.begin(), queue);

	// count number of "good" permuts"
	size_t count = compute::count_if(result_device_vector.begin(), result_device_vector.end(), is_good_score, queue);

	//double check on CPU
	//int count_cpu = std::count_if(result_host_vector.begin(), result_host_vector.end(), [](double i){return i > 0.04;});


	std::cout << "Count= "<<count << std::endl; //<< "  CPU count= "<<count_cpu<<std::endl;
	
	return 0;
}