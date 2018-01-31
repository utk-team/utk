#include <iostream>
#include <boost/compute/core.hpp>
#include <vector>
#include <boost/concept_check.hpp>
#include "../pointsets/Pointset.hpp"
#include "../io/fileIO.hpp"
#include "../io/histogramIO.hpp"

#include <boost/compute/algorithm/transform.hpp>
#include <boost/compute/algorithm/sort.hpp>
#include <boost/compute/algorithm/max_element.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/functional/math.hpp>
#include <boost/compute/algorithm/count_if.hpp>
#include <boost/compute/function.hpp>

namespace compute = boost::compute;


const char source2[] = BOOST_COMPUTE_STRINGIZE_SOURCE(__kernel void computeScoreMinDist(__global const unsigned int* permut_lsb,
																				__global const unsigned int* permut_msb,
																				__global const unsigned int* pattern,
																				__global const float* pcf,
																				__global float* min_dist,
                                                                                __global float* score
																				//__global unsigned int* debug
                                                                              )
													{
														uint gid = get_global_id(0);
														
														unsigned int permut_x_lsb = permut_lsb[2*gid];
														unsigned int permut_y_lsb = permut_lsb[2*gid+1];
														unsigned int permut_x_msb = permut_msb[2*gid];
														unsigned int permut_y_msb = permut_msb[2*gid+1];
														
														
														unsigned int permutN_lsb[2];
														unsigned int permutN_msb[2];
														permutN_lsb[0] = permut_x_lsb;
														permutN_lsb[1] = permut_y_lsb;
														permutN_msb[0] = permut_x_msb;
														permutN_msb[1] = permut_y_msb;
														
														unsigned int permutM_lsb[2];
														unsigned int permutM_msb[2];
														permutM_lsb[0] = 0;
														permutM_lsb[1] = 0;
														permutM_msb[0] = 0;
														permutM_msb[1] = 0;
														
														uint tilesize = 64;
														uint LOGK=6;
														uint D=2;
														
														uint x[5][2];
														for(uint d=0; d<D; d++)
														{
															int nb_flags = 2;
															int orig_lvl = tilesize-2 -1;
															for(uint i=1; i<LOGK; i++)
															{
																x[i-1][d] = orig_lvl - (pattern[d]/(tilesize/nb_flags));
																orig_lvl -= nb_flags;
																nb_flags*=2;
															}
														}
														
														uint xcount[2];
														for(uint d=0; d<D; d++)
															xcount[d] = tilesize-LOGK-2; // bitset indexing: 0==least significant bit
															
														for(uint i=1; i<tilesize-1; i++) { // start form the second i because xpermuts15bits[0].x = xpermuts15bits[0].y = 0;
															uint ind = tilesize-2-i;
															
															for(uint d=0; d<D; d++)
															{
																bool isFlag=true;
																for(uint i=0; i<(uint)(LOGK-1); i++)
																	isFlag &= (ind != x[i][d]);
																
																if( isFlag ) 
																{
																	if(xcount[d] < 32)
																	{
																		int digit_value = ((permutN_lsb[d] >> xcount[d]) & 1);
																		if(ind < 32)
																		{
																			unsigned int mask = (unsigned int)((unsigned int)1 << ind);
																			if(digit_value)
																				permutM_lsb[d] |= mask;
																			else
																				permutM_lsb[d] &= (unsigned int)~mask;
																		}
																		else
																		{
																			unsigned int mask = (unsigned int)((unsigned int)1 << (ind-32));
																			if(digit_value)
																				permutM_msb[d] |= mask;
																			else
																				permutM_msb[d] &= (unsigned int)~mask;
																		}
																	}
																	else
																	{
																		int digit_value = ((permutN_msb[d] >> (xcount[d]-32)) & 1);
																		if(ind < 32)
																		{
																			unsigned int mask = (unsigned int)((unsigned int)1 << ind);
																			if(digit_value)
																				permutM_lsb[d] |= mask;
																			else
																				permutM_lsb[d] &= (unsigned int)~mask;
																		}
																		else
																		{
																			unsigned int mask = (unsigned int)((unsigned int)1 << (ind-32));
																			if(digit_value)
																				permutM_msb[d] |= mask;
																			else
																				permutM_msb[d] &= (unsigned int)~mask;
																		}
																	}
																	xcount[d]--;
																}
															}
														}
														
														permut_x_lsb = permutM_lsb[0];
														permut_y_lsb = permutM_lsb[1];
														permut_x_msb = permutM_msb[0];
														permut_y_msb = permutM_msb[1];
														
														
														unsigned int permutted_pattern[128];
														
														int ind;
														unsigned int xpermuts_lsb[2];
														xpermuts_lsb[0] = permut_x_lsb;
														xpermuts_lsb[1] = permut_y_lsb;
														
														unsigned int xpermuts_msb[2];
														xpermuts_msb[0] = permut_x_msb;
														xpermuts_msb[1] = permut_y_msb;

														uint NBITS = 6;
														uint PERMUTS_NBITS = tilesize-1;
														
														for(uint ipt=0; ipt<tilesize; ipt++) 
														{
															unsigned int xresdigits[2];
															unsigned int thisSobolPt[2];
															for(uint d=0; d<2; d++)
																thisSobolPt[d] = pattern[2*ipt+d];
															xresdigits[0] = 0;
															xresdigits[1] = 0;

															for(uint ilevel=0; ilevel<NBITS; ilevel++) {
																
																for(uint d=0; d<2; d++)
																{
																	int power=(1 << ilevel);
																	ind = (pattern[2*ipt+d] >> (NBITS-ilevel)) + (power-1);
																	int digit_value_1 = 0;
																	if(PERMUTS_NBITS-ind-1 < 32)
																		digit_value_1 =  (xpermuts_lsb[d] >> (PERMUTS_NBITS-ind-1)) & 1;
																	else
																		digit_value_1 =  (xpermuts_msb[d] >> (PERMUTS_NBITS-ind-1-32)) & 1;
																	int digit_value_2 =  (thisSobolPt[d] >> (NBITS-ilevel-1)) & 1;
																	int digit_value = (digit_value_1 ^ digit_value_2);
																	int digit_pos = NBITS-ilevel-1;
																	unsigned int mask = (unsigned int)((unsigned int)1 << digit_pos);
																	if(digit_value)
																		xresdigits[d] |= mask;
																	else
																		xresdigits[d] &= (unsigned int)~mask;
																}
															}
															
															for(uint d=0; d<2; d++)
																permutted_pattern[2*ipt+d] = xresdigits[d];
														}

														float mindist = 10000.0;
														for (uint ipts = 0; ipts < tilesize; ipts++) {
															for (uint ipts_to_compare = ipts+1; ipts_to_compare < tilesize; ipts_to_compare++) {
																
																float dist_sq = 0.0;
																float dx = (float)permutted_pattern[2*ipts]/(float)tilesize - (float)permutted_pattern[2*ipts_to_compare]/(float)tilesize;
																dist_sq += dx*dx;
																float dy = (float)permutted_pattern[2*ipts+1]/(float)tilesize - (float)permutted_pattern[2*ipts_to_compare+1]/(float)tilesize;
																dist_sq += dy*dy;
																
																if (dist_sq < mindist) mindist = dist_sq;
															}
														}
														min_dist[gid] = sqrt(mindist);
														score[gid]= min_dist[gid];
													}
                                                    );

const char source[] = BOOST_COMPUTE_STRINGIZE_SOURCE(__kernel void computeScorePCF(__global const unsigned int* permut_lsb,
																				__global const unsigned int* permut_msb,
																				__global const unsigned int* pattern,
																				__global const float* pcf,
																				__global float* min_dist,
                                                                                __global float* score
																				//__global unsigned int* debug
                                                                              )
													{
														uint gid = get_global_id(0);
														
														unsigned int permut_x_lsb = permut_lsb[2*gid];
														unsigned int permut_y_lsb = permut_lsb[2*gid+1];
														unsigned int permut_x_msb = permut_msb[2*gid];
														unsigned int permut_y_msb = permut_msb[2*gid+1];
														
														
														unsigned int permutN_lsb[2];
														unsigned int permutN_msb[2];
														permutN_lsb[0] = permut_x_lsb;
														permutN_lsb[1] = permut_y_lsb;
														permutN_msb[0] = permut_x_msb;
														permutN_msb[1] = permut_y_msb;
														
														unsigned int permutM_lsb[2];
														unsigned int permutM_msb[2];
														permutM_lsb[0] = 0;
														permutM_lsb[1] = 0;
														permutM_msb[0] = 0;
														permutM_msb[1] = 0;
														
														uint tilesize = 64;
														
														
														uint LOGK=6;
														uint D=2;
														
														uint x[5][2];
														for(uint d=0; d<D; d++)
														{
															int nb_flags = 2;
															int orig_lvl = tilesize-2 -1;
															for(uint i=1; i<LOGK; i++)
															{
																x[i-1][d] = orig_lvl - (pattern[d]/(tilesize/nb_flags));
																orig_lvl -= nb_flags;
																nb_flags*=2;
															}
														}
														
														uint xcount[2];
														for(uint d=0; d<D; d++)
															xcount[d] = tilesize-LOGK-2; // bitset indexing: 0==least significant bit
															
														for(uint i=1; i<tilesize-1; i++) { // start form the second i because xpermuts15bits[0].x = xpermuts15bits[0].y = 0;
															uint ind = tilesize-2-i;
															
															for(uint d=0; d<D; d++)
															{
																bool isFlag=true;
																for(uint i=0; i<(uint)(LOGK-1); i++)
																	isFlag &= (ind != x[i][d]);
																
																if( isFlag ) 
																{
																	if(xcount[d] < 32)
																	{
																		int digit_value = ((permutN_lsb[d] >> xcount[d]) & 1);
																		if(ind < 32)
																		{
																			unsigned int mask = (unsigned int)((unsigned int)1 << ind);
																			if(digit_value)
																				permutM_lsb[d] |= mask;
																			else
																				permutM_lsb[d] &= (unsigned int)~mask;
																		}
																		else
																		{
																			unsigned int mask = (unsigned int)((unsigned int)1 << (ind-32));
																			if(digit_value)
																				permutM_msb[d] |= mask;
																			else
																				permutM_msb[d] &= (unsigned int)~mask;
																		}
																	}
																	else
																	{
																		int digit_value = ((permutN_msb[d] >> (xcount[d]-32)) & 1);
																		if(ind < 32)
																		{
																			unsigned int mask = (unsigned int)((unsigned int)1 << ind);
																			if(digit_value)
																				permutM_lsb[d] |= mask;
																			else
																				permutM_lsb[d] &= (unsigned int)~mask;
																		}
																		else
																		{
																			unsigned int mask = (unsigned int)((unsigned int)1 << (ind-32));
																			if(digit_value)
																				permutM_msb[d] |= mask;
																			else
																				permutM_msb[d] &= (unsigned int)~mask;
																		}
																	}
																	xcount[d]--;
																}
															}
														}
														
														permut_x_lsb = permutM_lsb[0];
														permut_y_lsb = permutM_lsb[1];
														permut_x_msb = permutM_msb[0];
														permut_y_msb = permutM_msb[1];
														
														
														unsigned int permutted_pattern[128];
														
														int ind;
														unsigned int xpermuts_lsb[2];
														xpermuts_lsb[0] = permut_x_lsb;
														xpermuts_lsb[1] = permut_y_lsb;
														
														unsigned int xpermuts_msb[2];
														xpermuts_msb[0] = permut_x_msb;
														xpermuts_msb[1] = permut_y_msb;

														uint NBITS = 6;
														uint PERMUTS_NBITS = tilesize-1;
														
														for(uint ipt=0; ipt<tilesize; ipt++) 
														{
															unsigned int xresdigits[2];
															unsigned int thisSobolPt[2];
															for(uint d=0; d<2; d++)
																thisSobolPt[d] = pattern[2*ipt+d];
															xresdigits[0] = 0;
															xresdigits[1] = 0;

															for(uint ilevel=0; ilevel<NBITS; ilevel++) {
																
																for(uint d=0; d<2; d++)
																{
																	int power=(1 << ilevel);
																	ind = (pattern[2*ipt+d] >> (NBITS-ilevel)) + (power-1);
																	int digit_value_1 = 0;
																	if(PERMUTS_NBITS-ind-1 < 32)
																		digit_value_1 =  (xpermuts_lsb[d] >> (PERMUTS_NBITS-ind-1)) & 1;
																	else
																		digit_value_1 =  (xpermuts_msb[d] >> (PERMUTS_NBITS-ind-1-32)) & 1;
																	int digit_value_2 =  (thisSobolPt[d] >> (NBITS-ilevel-1)) & 1;
																	int digit_value = (digit_value_1 ^ digit_value_2);
																	int digit_pos = NBITS-ilevel-1;
																	unsigned int mask = (unsigned int)((unsigned int)1 << digit_pos);
																	if(digit_value)
																		xresdigits[d] |= mask;
																	else
																		xresdigits[d] &= (unsigned int)~mask;
																}
															}
															
															for(uint d=0; d<2; d++)
																permutted_pattern[2*ipt+d] = xresdigits[d];
														}
														
														float mindist = 10000.0;
														for (uint ipts = 0; ipts < tilesize; ipts++) {
															for (uint ipts_to_compare = ipts+1; ipts_to_compare < tilesize; ipts_to_compare++) {
																
																float dist_sq = 0.0;
																float dx = (float)permutted_pattern[2*ipts]/(float)tilesize - (float)permutted_pattern[2*ipts_to_compare]/(float)tilesize;
																dist_sq += dx*dx;
																float dy = (float)permutted_pattern[2*ipts+1]/(float)tilesize - (float)permutted_pattern[2*ipts_to_compare+1]/(float)tilesize;
																dist_sq += dy*dy;
																
																if (dist_sq < mindist) mindist = dist_sq;
															}
														}
														min_dist[gid] = sqrt(mindist);
														
														if(min_dist[gid] < 0.07)
														{
															score[gid] = 0.01;
														}
														else
														{
															float norm_dist = 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*tilesize) );
															float smoothing = 0.1 * norm_dist;
															float ra = 1.5*smoothing;
															float rb = 2.5*norm_dist;
															
															float ani_smoothing = 0.01;
															float aa = 0;
															float ab = 2*M_PI;
															
															uint nbbins = 20;
															float pcf_pattern[20];
															float anisotropy_pattern[20];
															for(uint pcfid = 0; pcfid < nbbins; ++pcfid)
															{
																pcf_pattern[pcfid] = 0;
																anisotropy_pattern[pcfid] = 0;
															}

															float bin_size = (rb-ra)/(float)nbbins;
															uint covering = ((2.0*smoothing)/bin_size)+1;
															if(covering > nbbins)
																covering = nbbins;
															
															float ani_bin_size = (ab-aa)/(float)nbbins;
															uint ani_covering = ((2.0*ani_smoothing)/ani_bin_size)+1;
															if(ani_covering > nbbins)
																ani_covering = nbbins;

															for(uint i=0; i<tilesize; i++)
															{
																for(uint j=(i+1); j<tilesize; j++)
																{
																	float dist_sq = 0;
																	float dx = (float)permutted_pattern[2*i]/(float)tilesize - (float)permutted_pattern[2*j]/(float)tilesize;
																	dx*=dx;
																	dist_sq += dx;
																	float dy = (float)permutted_pattern[2*i+1]/(float)tilesize - (float)permutted_pattern[2*j+1]/(float)tilesize;
																	dy*=dy;
																	dist_sq += dy;
																	float dxy = dist_sq;
																	dx=sqrt(dx);
																	dy=sqrt(dy);
																	
																	float vector_angle = atan(dy/dx);
																	int ani_init_bin = vector_angle/bin_size;
																	anisotropy_pattern[ani_init_bin] += 1;

																	int init_bin = sqrt(dxy)/bin_size;
																	int ida = init_bin - covering;
																	int idb = init_bin + covering;
																	if(ida < 0) ida = 0;
																	if(idb > nbbins) idb = nbbins;
																	for(uint pcfid = ida; pcfid < idb; ++pcfid)
																	{
																		float r = ra + pcfid*(rb - ra) / (float) nbbins;
																		float dist = r - sqrt(dxy);
																		float val = ( 1.0/(sqrt(2*M_PI)*smoothing) ) * exp( -0.5*(dist*dist)/(smoothing*smoothing) );
																		pcf_pattern[pcfid] += 2*val;
																	}
																}
															}

															for(uint pcfid = 0; pcfid < nbbins; ++pcfid)
															{
																float r = ra + pcfid*(rb - ra) / (float) nbbins;

																float cov = 1.0 - ((2.0*r)/M_PI)*2.0 + ((r*r)/M_PI);
																float factor = 2.0*M_PI*r*cov;
																factor = 1.0/factor;
																
																pcf_pattern[pcfid] *= factor;
																pcf_pattern[pcfid] /= (tilesize*(tilesize-1));
																anisotropy_pattern[pcfid] /= 20.0;
															}

															float linf = 0;
															float ani_linf = 0;
															for(uint i=0; i<nbbins; i++)
															{
																if(pcf_pattern[i] != 0 && fabs(pcf[i] - pcf_pattern[i]) > linf )
																	linf = fabs(pcf[i] - pcf_pattern[i]);
																
																if(anisotropy_pattern[i] != 0 && fabs(1 - anisotropy_pattern[i]) > ani_linf )
																	ani_linf = fabs(1 - anisotropy_pattern[i]);
															}
															
															score[gid] = 10 -linf -ani_linf;
														}
													}
                                                    );


bool permutFromNbitsToMbits(uint N, uint M, uint fixed_point[2], unsigned long long int permutN_x, unsigned long long int permutN_y, 
							unsigned long long int& permutM_x, unsigned long long int& permutM_y)
{
	int tilesize = M+1;
	int LOGK=log2(tilesize);
	uint D=2;
	
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
	for(uint i=1; i<(uint)(tilesize-1); i++) { // start form the second i because xpermuts15bits[0].x = xpermuts15bits[0].y = 0;
		uint ind = tilesize-2-i;
		
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


/*void computeScoreCPU(const unsigned int* permut_lsb,
						const unsigned int* permut_msb,
						const unsigned int* pattern,
						const float* pcf,
						float* min_dist,
                        float* score, uint gid)
{
	//Copy OpenCL Kernel Here
}*/

bool optimizeGPU(utk::Pointset<2, unsigned int, utk::Point<2, unsigned int> > pts, utk::Histogram1dd pcf_data, utk::Vector<2, unsigned long long int>& resulting_permut)
{
	assert(pts.size() == 64);
	
	srand48(time(NULL));
	float max_score= 0;
	float max_dist = 0;
	int max_id=0;
	
	int t = time(NULL);
	
	//get default device and setup context
	compute::device device = compute::system::default_device();
	compute::context context(device);
	compute::command_queue queue(context, device);

	int nbbins = 20;
	assert(pcf_data.size() == (uint)nbbins);	
	std::vector<float> local_pcf;
	local_pcf.resize(nbbins);
	for(int i=0; i<nbbins; i++)
		local_pcf[i] = pcf_data[i].second;
	
	std::cout << "Local PCF Vector OK" << std::endl;
	
	//Send PCF to the device
	compute::vector<float> device_pcf(local_pcf.size(), context);
	compute::copy(local_pcf.begin(), local_pcf.end(), device_pcf.begin(), queue);
	std::cout << "Device PCF Vector OK" << std::endl;

	std::vector<unsigned int> local_pattern(pts.size()*2, 0);
	for(uint i=0; i<pts.size(); i++)
	{
		local_pattern[2*i] = pts[i].pos()[0];
		local_pattern[2*i+1] = pts[i].pos()[1];
	}
	std::cout << "Local Pattern Vector OK" << std::endl;
	
	//Send pattern to the device
	compute::vector<unsigned int> device_pattern(pts.size()*2, context);
	compute::copy(local_pattern.begin(), local_pattern.end(), device_pattern.begin(), queue);
	std::cout << "Device Pattern Vector OK" << std::endl;
	
	
	uint tilesize=64;
	uint N, M;
	M = tilesize-1;
	N = M - log2(tilesize);
	uint zero_pt[2]; zero_pt[0] = local_pattern[0]; zero_pt[1] = local_pattern[1];
	int nbfreedigits = 11;
	int npermut = (1 << nbfreedigits);

	int nbbatches=2048;
	int batch_size = (npermut*npermut)/nbbatches;
	
	//Create result vector
	compute::vector<float> device_mindist(batch_size, context);
	std::fill(device_mindist.begin(), device_mindist.end(), 0);
	std::cout << "Device MinDist Vector OK" << std::endl;
	
	compute::vector<float> device_score(batch_size, context);
	std::fill(device_score.begin(), device_score.end(), 0);
	std::cout << "Device Result Vector OK" << std::endl;
	
	compute::vector<unsigned int> device_permutations_lsb(batch_size*2, context);
	compute::vector<unsigned int> device_permutations_msb(batch_size*2, context);
	std::vector<float> local_mindist(batch_size);
	std::vector<float> local_score(batch_size);	
	
	std::cout << "Building Kernel ..." << std::endl;
	
	//Create and build the kernels
	
	//Create and build the kernels
	compute::program m_program;
	m_program = compute::program::create_with_source(source, context);
	try
	{
		m_program.build();
	}
	catch(boost::compute::opencl_error &e)
	{
		std::cout << m_program.build_log() << std::endl;
	}
	compute::kernel score_kernel;
	score_kernel = m_program.create_kernel("computeScorePCF");
	score_kernel.set_arg(0, device_permutations_lsb.get_buffer());
	score_kernel.set_arg(1, device_permutations_msb.get_buffer());
	score_kernel.set_arg(2, device_pattern.get_buffer());
	score_kernel.set_arg(3, device_pcf.get_buffer());
	score_kernel.set_arg(4, device_mindist.get_buffer());
	score_kernel.set_arg(5, device_score.get_buffer());
	std::cout << "OK" << std::endl;

	
	std::cout << "Optimizing first " << nbfreedigits << " digits ..." << std::endl;
	
	std::vector<unsigned long long int> local_permutations_full(npermut*npermut*2, 0);
	for(int i=0; i<npermut; i++)
	{
		for(int j=0; j<npermut; j++)
		{
			local_permutations_full[2*(i*npermut+j)] = (unsigned long long int)((unsigned long long int)i << (unsigned long long int)(N-nbfreedigits));
			local_permutations_full[2*(i*npermut+j)+1] = (unsigned long long int)((unsigned long long int)j << (unsigned long long int)(N-nbfreedigits));
		}
	}
	std::vector<unsigned int> local_permutations_lsb(npermut*npermut*2, 0);
	std::vector<unsigned int> local_permutations_msb(npermut*npermut*2, 0);
	for(int i=0; i<npermut; i++)
	for(int j=0; j<npermut; j++)
	{
		local_permutations_lsb[2*(i*npermut+j)] = (unsigned long long int)((local_permutations_full[2*(i*npermut+j)]) & (unsigned long long int)0xffffff);
		local_permutations_lsb[2*(i*npermut+j)+1] = (unsigned long long int)((local_permutations_full[2*(i*npermut+j)+1]) & (unsigned long long int)0xffffff);
		
		local_permutations_msb[2*(i*npermut+j)] = ((local_permutations_full[2*(i*npermut+j)]) >> 32);
		local_permutations_msb[2*(i*npermut+j)+1] = ((local_permutations_full[2*(i*npermut+j)+1]) >> 32);
	}
	std::cout << "Local Permut Vector OK" << std::endl;

	for(int batch=0; batch<nbbatches; batch++)
	{
		compute::copy(local_permutations_lsb.begin()+(2*batch*batch_size), local_permutations_lsb.begin()+(2*(batch+1)*batch_size), device_permutations_lsb.begin(), queue);
		compute::copy(local_permutations_msb.begin()+(2*batch*batch_size), local_permutations_msb.begin()+(2*(batch+1)*batch_size), device_permutations_msb.begin(), queue);

		//Go Go go !
		//std::cout << batch << "/" << nbbatches << "\r" << std::flush;
		queue.enqueue_1d_range_kernel(score_kernel, 0, batch_size, 0);
		
		compute::copy(device_mindist.begin(), device_mindist.end(), local_mindist.begin(), queue);
		compute::copy(device_score.begin(), device_score.end(), local_score.begin(), queue);
		
		for(int i=0; i<batch_size; i++)
		{
			if( max_score < local_score[i])
			{
				max_dist = local_mindist[i];
				max_id = batch*batch_size+i;
				max_score = local_score[i];
			}
		}
	}
	
	/*unsigned long long int final_perm_x, final_perm_y;
	permutFromNbitsToMbits(N, M, zero_pt, local_permutations_full[2*max_id], local_permutations_full[2*max_id+1], final_perm_x, final_perm_y);
	std::cout << "Done [" << time(NULL) - t << "s]" << std::endl;
	std::cout<< "[DEVICE] Current Best Permut " << final_perm_x << "," << final_perm_y << " (score " << max_score << ")" << "(dist " << max_dist << ")" << std::endl;*/
	
	std::cout << "Optimizing last " << (N-nbfreedigits) << " digits ..." << std::endl;
	for(int i=0; i<npermut; i++)
	{
		for(int j=0; j<npermut; j++)
		{
			unsigned long long int randval = (unsigned long long int)(drand48()*((unsigned long long int)1 << (unsigned long long int)(N-nbfreedigits)));
			local_permutations_full[2*(i*npermut+j)] = local_permutations_full[2*max_id] + randval;
			randval = drand48()*(1 << (N-nbfreedigits));
			local_permutations_full[2*(i*npermut+j)+1] = local_permutations_full[2*max_id+1] + randval;
		}
	}
	for(int i=0; i<npermut; i++)
	for(int j=0; j<npermut; j++)
	{
		local_permutations_lsb[2*(i*npermut+j)] = (unsigned long long int)((local_permutations_full[2*(i*npermut+j)]) & (unsigned long long int)0xffffff);
		local_permutations_lsb[2*(i*npermut+j)+1] = (unsigned long long int)((local_permutations_full[2*(i*npermut+j)+1]) & (unsigned long long int)0xffffff);
		
		local_permutations_msb[2*(i*npermut+j)] = ((local_permutations_full[2*(i*npermut+j)]) >> 32);
		local_permutations_msb[2*(i*npermut+j)+1] = ((local_permutations_full[2*(i*npermut+j)+1]) >> 32);
	}
	std::cout << "Local Permut Vector OK" << std::endl;
	
	for(int batch=0; batch<nbbatches; batch++)
	{
		compute::copy(local_permutations_lsb.begin()+(2*batch*batch_size), local_permutations_lsb.begin()+(2*(batch+1)*batch_size), device_permutations_lsb.begin(), queue);
		compute::copy(local_permutations_msb.begin()+(2*batch*batch_size), local_permutations_msb.begin()+(2*(batch+1)*batch_size), device_permutations_msb.begin(), queue);

		//Go Go go !
		//std::cout << batch << "/" << nbbatches << "\r" << std::flush;
		queue.enqueue_1d_range_kernel(score_kernel, 0, batch_size, 0);
		compute::copy(device_mindist.begin(), device_mindist.end(), local_mindist.begin(), queue);
		compute::copy(device_score.begin(), device_score.end(), local_score.begin(), queue);
		
		for(int i=0; i<batch_size; i++)
		{
			//computeScoreCPU(&local_permutations_lsb[2*batch*batch_size], &local_permutations_msb[2*batch*batch_size], &local_pattern[0], &local_pcf[0], &local_mindist[0], &local_score[0], i);
			//exit(1);
			if( max_score < local_score[i])
			{
				max_dist = local_mindist[i];
				max_id = batch*batch_size+i;
				max_score = local_score[i];
			}
		}
	}
	
	resulting_permut[0] = 0; resulting_permut[1] = 0;
	permutFromNbitsToMbits(N, M, zero_pt, local_permutations_full[2*max_id], local_permutations_full[2*max_id+1], resulting_permut[0], resulting_permut[1]);
	std::cout << "Done [" << time(NULL) - t << "s]" << std::endl;
	std::cout<< "[DEVICE] Best Permut " << resulting_permut[0] << "," << resulting_permut[1] << " (score " << max_score << ")" << "(dist " << max_dist << ")" << std::endl;

	return 0;
}
