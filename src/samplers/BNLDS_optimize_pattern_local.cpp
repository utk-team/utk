#include <iostream>
#include <vector>
#include "../../pointsets/Pointset.hpp"
#include "../../io/fileIO.hpp"


bool permutFromNbitsToMbits(uint N, uint M, uint fixed_point[2], unsigned long long int permutN_x, unsigned long long int permutN_y, 
							unsigned long long int& permutM_x, unsigned long long int& permutM_y)
{
	int tilesize = M+1;
	int LOGK=log2(tilesize);
	uint D=2;
	
	//For K=4
	//ind
	//
	//0              14
	//1         13        12
	//2      11   10    9    8
	//3     7 6  5 4   3 2   1 0
	//
	//corresponding to sample pos
	//      1 3  5 7   9 11  13 15
	//      0 2  4 6   8 10  12 14
	
	std::vector< std::array<uint, 2> > x;
	x.resize(LOGK-1);
	for(uint d=0; d<D; d++)
	{
		int nb_flags = 2;
		int orig_lvl = tilesize-2 -1;
		for(int i=1; i<LOGK; i++)
		{
			x[i-1][d] = orig_lvl - (fixed_point[d]/(tilesize/nb_flags));
			orig_lvl -= nb_flags;
			nb_flags*=2;
		}
	}
	
	std::bitset<64> xpermutsNbits[2];
	xpermutsNbits[0] = permutN_x;
	xpermutsNbits[1] = permutN_y;
	
	std::bitset<64> xpermutsMbits[2];
	int xcount[2];
	for(uint d=0; d<D; d++)
		xcount[d] = tilesize-LOGK-2; // bitset indexing: 0==least significant bit
	for(uint i=1; i<tilesize-1; i++) { // start form the second i because xpermuts15bits[0].x = xpermuts15bits[0].y = 0;
		int ind = tilesize-2-i;
		
		for(uint d=0; d<D; d++)
		{
			bool isFlag=true;
			for(int i=0; i<LOGK-1; i++)
				isFlag &= (ind != x[i][d]);
			
			if( isFlag ) {
				xpermutsMbits[d][ind] = xpermutsNbits[d][xcount[d]];
				xcount[d]--;
			}
		}
	}
	
	permutM_x = xpermutsMbits[0].to_ullong();
	permutM_y = xpermutsMbits[1].to_ullong();
	
	return true;
}


int main(int argc, char** argv)
{
	srand48(time(NULL));
	double max_dist = 0;
	int max_id=0;
	
	if(argc != 2)
	{
		std::cout << "Usage " << argv[0] << " <pattern_file>" << std::endl;
		return 0;
	}

	utk::Pointset<2, unsigned int, utk::Point<2, unsigned int> >pts;
	utk::PointsetReader<2, unsigned int, utk::Point<2, unsigned int>> pts_reader;
	pts_reader.open(argv[1]);
	pts_reader.readPointset(pts);
	pts_reader.close();
	
	//Send pattern to the device
	std::vector<unsigned int> local_pattern(pts.size()*2, 0);
	for(uint i=0; i<pts.size(); i++)
	{
		local_pattern[2*i] = pts[i].pos()[0];
		local_pattern[2*i+1] = pts[i].pos()[1];
	}
	std::cout << "Pattern Vector OK" << std::endl;
	
	uint tilesize=64;
	uint N, M;
	M = tilesize-1;
	N = M - log2(tilesize);
	uint zero_pt[2]; zero_pt[0] = local_pattern[0]; zero_pt[1] = local_pattern[1];
	int nbfreedigits = 11;
	int npermut = (1 << nbfreedigits);

	//std::cout << "M " << M << " - N " << N << " freedom " << N-nbfreedigits << std::endl;
	std::vector<unsigned long long int> local_permutations(npermut*npermut*2, 0);
	for(int i=0; i<npermut; i++)
	{
		std::cout << i << "/" << npermut << "\r" << std::flush;
		for(int j=0; j<npermut; j++)
		{
			unsigned long long int permi = (unsigned long long int)((unsigned long long int)i << (unsigned long long int)(N-nbfreedigits));
			unsigned long long int permj = (unsigned long long int)((unsigned long long int)j << (unsigned long long int)(N-nbfreedigits));
			permutFromNbitsToMbits(N, M, zero_pt, permi, permj, local_permutations[2*((i*npermut)+j)], local_permutations[2*((i*npermut)+j)+1]);
		}
	}
	std::cout << "Permut Vector OK" << std::endl;

	//Retreive result vector from the device
	std::vector<double> local_score(npermut*npermut, 0);
	
	int t= time(NULL);
	std::cout << "Computing ..." << std::endl;
	for(int i=0; i<npermut*npermut; i++)
	{
		//if(i%10000==0)
		//	std::cout << i << "/" << npermut*npermut << "\r" << std::flush;
		
		int gid = i;
		
		unsigned long long int permut_x = local_permutations[2*gid];
		unsigned long long int permut_y = local_permutations[2*gid+1];
		
		unsigned int permutted_pattern[128];
		
		int ind;
		unsigned long long int xpermuts[2];
		xpermuts[0] = permut_x;
		xpermuts[1] = permut_y;
	
		//uint tilesize = 64;
		uint NBITS = 6;
		uint PERMUTS_NBITS = tilesize-1;
		
		for(uint ipt=0; ipt<tilesize; ipt++) {
			unsigned long long int xresdigits[2];
			unsigned long long int thisSobolPt[2];
			for(uint d=0; d<2; d++)
				thisSobolPt[d] = local_pattern[2*ipt+d];
			xresdigits[0] = 0;
			xresdigits[1] = 0;
	
			for(uint ilevel=0; ilevel<NBITS; ilevel++) {
				
				for(uint d=0; d<2; d++)
				{
					int power=(1 << ilevel);
					ind = (local_pattern[2*ipt+d] >> (NBITS-ilevel)) + (power-1);
					int digit_value_1 =  (xpermuts[d] >> (PERMUTS_NBITS-ind-1)) & 1;
					int digit_value_2 =  (thisSobolPt[d] >> (NBITS-ilevel-1)) & 1;
					int digit_value = (digit_value_1 ^ digit_value_2);
					int digit_pos = NBITS-ilevel-1;
					unsigned long long int mask = (unsigned long long int)((unsigned long long int)1 << digit_pos);
					if(digit_value)
						xresdigits[d] |= mask;
					else
						xresdigits[d] &= (unsigned long long int)~mask;
				}
			}
			
			for(uint d=0; d<2; d++)
				permutted_pattern[2*ipt+d] = xresdigits[d];
		}
						
						
		double mindist = 10000;
		for (uint ipts = 0; ipts < tilesize; ipts++) {
			for (uint ipts_to_compare = ipts+1; ipts_to_compare < tilesize; ipts_to_compare++) {
				
				double dist_sq = 0;
				double dx = (double)permutted_pattern[2*ipts]/tilesize - (double)permutted_pattern[2*ipts_to_compare]/tilesize;
				dist_sq += dx*dx;
				double dy = (double)permutted_pattern[2*ipts+1]/tilesize - (double)permutted_pattern[2*ipts_to_compare+1]/tilesize;
				dist_sq += dy*dy;
				
				if (dist_sq < mindist) mindist = dist_sq;
			}
		}
		
		local_score[gid]= sqrt(mindist);
	}
	
	max_dist = 0;
	max_id = 0;
	for(auto i=0; i < npermut*npermut; ++i)
		if( max_dist < local_score[i])
		{
			max_dist = local_score[i];
			max_id = i;
		}
	
	std::cout << "Done [" << time(NULL) - t << "s]" << std::endl;
	std::cout<< "[CPU] Best Permut " << local_permutations[2*max_id] << "," << local_permutations[2*max_id+1] << " (" << max_dist << ")" << std::endl;
	
	
	/*std::ofstream file("localperm2.dat");
	for(int i=0; i<npermut; i++)
	for(int j=0; j<npermut; j++)
	{
		file << local_permutations[2*(i*npermut+j)] << " " << local_permutations[2*(i*npermut+j)+1] << " " << local_score[(i*npermut+j)] << std::endl;
	}
	file.close();*/

	return 0;
}