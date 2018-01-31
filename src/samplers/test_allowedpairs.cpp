#include <fstream>
#include "../pointsets/Pointset.hpp"

/*int main(int argc, char** argv)
{
	std::ifstream file0("sobolsamples_0.dat");
	std::ifstream file1("sobolsamples_1.dat");
	std::ifstream file2("sobolsamples_2.dat");
	std::ifstream file3("sobolsamples_3.dat");
	std::ifstream file4("sobolsamples_4.dat");
	std::ifstream file5("sobolsamples_5.dat");
	std::ifstream file6("sobolsamples_6.dat");
	std::ifstream file7("sobolsamples_7.dat");
	std::ifstream file8("sobolsamples_8.dat");
	std::ifstream file9("sobolsamples_9.dat");
	
	utk::Pointset< 10, double, utk::Point<10, double> > pts;
	std::ofstream ptsfile("pts.dat");
	
	int nbpts = atoi(argv[1]);
	pts.resize(nbpts);
	for(int i=0; i<nbpts; i++)
	{
		double val0, val1, val2, val3, val4, val5, val6, val7, val8, val9;
		int id0, id1, id2, id3, id4, id5, id6, id7, id8, id9;
		file0 >> id0; file0 >> val0;
		file1 >> id1; file1 >> val1;
		file2 >> id2; file2 >> val2;
		file3 >> id3; file3 >> val3;
		file4 >> id4; file4 >> val4;
		file5 >> id5; file5 >> val5;
		file6 >> id6; file6 >> val6;
		file7 >> id7; file7 >> val7;
		file8 >> id8; file8 >> val8;
		file9 >> id9; file9 >> val9;
		
		pts[id0].pos()[0] = val0;
		pts[id1].pos()[1] = val1;
		pts[id2].pos()[2] = val2;
		pts[id3].pos()[3] = val3;
		pts[id4].pos()[4] = val4;
		pts[id5].pos()[5] = val5;
		pts[id6].pos()[6] = val6;
		pts[id7].pos()[7] = val7;
		pts[id8].pos()[8] = val8;
		pts[id9].pos()[9] = val9;
	}
	
	for(int i=0; i<nbpts; i++)
		ptsfile << pts[i] << std::endl;
	
	ptsfile.close();
}*/



#include "SamplerSobolIndices.hpp"
#include "SamplerVanDerCorput.hpp"
#include "SamplerHammersley.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include "../io/imageIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

int main(int argc, char** argv)
{
	SamplerSobolIndices sampler;
	int indices[2];
	int idmax=13;
	int fast=0;
	
/*{	
	std::cout << "TILESIZE 64" << std::endl;
	int tilesize = 64;
	int lvlmax = 4-fast;
	
	std::ofstream texfile("tilesize64_allowedpairs.tex");
	texfile << "\\documentclass[10pt,a4paper]{article}\n \
			\\usepackage[paperheight=10in,paperwidth=10in, margin=1in]{geometry} \n \
			\\usepackage[utf8]{inputenc}\n \
			\\usepackage{amsmath}\n \
			\\usepackage{amsfonts}\n \
			\\usepackage{tikz}\n \
			\\usepackage{amssymb}\n" << std::endl;
	texfile << "\\begin{document}" << std::endl;
	texfile << "\\thispagestyle{empty} \n Tilesize " << tilesize << " \\newline Nbpts: ";
	for(int l=1; l<=lvlmax; l++)
 		texfile << pow(tilesize, l) << " ";
	texfile << "\n \\begin{figure} \\begin{tikzpicture}[scale=1.25]" << std::endl;
	
	for(int id0=1; id0<=idmax; id0++)
	{
		texfile << "\\draw (" << id0+0.5 << "," << 0 << ") node {" << id0 << "};" << std::endl;
		texfile << "\\draw (" << 0 << "," << id0+0.5 << ") node {" << id0 << "};" << std::endl;
	}
	
	for(int id0=1; id0<=idmax; id0++)
	{
		for(int id1=id0+1; id1<=idmax; id1++)
		{
			std::cout << "Sobol " << id0 << "," << id1 << std::endl;
			
			indices[0] = id0;
			indices[1] = id1;
			sampler.setIndices(indices, 2);
			
			bool failed = 0;
			for(int lvl=1; lvl<=lvlmax && !failed;  lvl++)
			{
				int N = pow(tilesize, lvl);
				Pointset< 2, unsigned int, Point<2, unsigned int> > pts;
				sampler.generateSamples<2, unsigned int, Point<2, unsigned int> >(pts, N);

				int nbtiles = N;//pow(tilesize, lvl-1);
				int nbtiles_side = sqrt(nbtiles);	
				//std::cout << N << " samples " << nbtiles << " tiles " << nbtiles_side << " tilesside" << std::endl;

				std::vector<unsigned int> tiles;
				tiles.resize(nbtiles);
				
				
				
				texfile << "\\filldraw[draw=black, fill=black] (" << id0 << "," << id0 << ") rectangle (" << id0+1 << "," << id0+1 << ");" << std::endl;
						
				
				std::cout << N << " samples:";
				int n=0;
				for(n=0; n<N; n++)
				{
					int tilex = (double)((double)pts[n].pos()[0]/(double)N) * nbtiles_side;
					int tiley = (double)((double)pts[n].pos()[1]/(double)N) * nbtiles_side;
					tiles[tilex*nbtiles_side + tiley]++;
					if (tiles[tilex*nbtiles_side + tiley] > 1)
					{
						std::cout << "[FAILED]" << std::endl;
						texfile << "\\filldraw[draw=black, fill=red] (" << id0+((double)(lvl-1)/(double)lvlmax) << "," << id1 << ") rectangle (" << id0+((double)(lvl)/(double)lvlmax) << "," << id1+1 << ");" << std::endl;
						failed=1;
						break;
					}
				}
				if(n==N)
				{
					texfile << "\\filldraw[draw=black, fill=green] (" << id0+((double)(lvl-1)/(double)lvlmax) << "," << id1 << ") rectangle (" << id0+((double)(lvl)/(double)lvlmax) << "," << id1+1 << ");" << std::endl;
					std::cout << "[OK]" << std::endl;
				}
				
				texfile << "\\draw[draw=black, line width=1mm] (" << id0 << "," << id1 << ") rectangle (" << id0+1 << "," << id1+1 << ");" << std::endl;
				
				{
				std::stringstream filename;
				filename << "ptsSobol" <<id0 <<"_"<<id1<<"_"<<N<<"pts.dat";
				std::ofstream file(filename.str());
				for(int n=0; n<N; n++)
					file << pts[n].pos() << std::endl;
				file.close();
				}
				
			}
		}
	}
	
	texfile << "\\end{tikzpicture}" << std::endl;
	texfile << "\\end{figure} \\end{document}" << std::endl;
	texfile.close();
}
	
{
	std::cout << "TILESIZE 16" << std::endl;
	int tilesize = 16;
	int lvlmax = 6-fast;
	
	std::ofstream texfile("tilesize16_allowedpairs.tex");
	texfile << "\\documentclass[10pt,a4paper]{article}\n \
			\\usepackage[paperheight=10in,paperwidth=10in, margin=1in]{geometry} \n \
			\\usepackage[utf8]{inputenc}\n \
			\\usepackage{amsmath}\n \
			\\usepackage{amsfonts}\n \
			\\usepackage{tikz}\n \
			\\usepackage{amssymb}\n" << std::endl;
	texfile << "\\begin{document}" << std::endl;
	texfile << "\\thispagestyle{empty} \n Tilesize " << tilesize << " \\newline Nbpts: ";
	for(int l=1; l<=lvlmax; l++)
 		texfile << pow(tilesize, l) << " ";
	texfile << "\n \\begin{figure} \\begin{tikzpicture}[scale=1.25]" << std::endl;
	
	for(int id0=1; id0<=idmax; id0++)
	{
		texfile << "\\draw (" << id0+0.5 << "," << 0 << ") node {" << id0 << "};" << std::endl;
		texfile << "\\draw (" << 0 << "," << id0+0.5 << ") node {" << id0 << "};" << std::endl;
	}
	
	for(int id0=1; id0<=idmax; id0++)
	{
		for(int id1=id0+1; id1<=idmax; id1++)
		{
			std::cout << "Sobol " << id0 << "," << id1 << std::endl;
			
			indices[0] = id0;
			indices[1] = id1;
			sampler.setIndices(indices, 2);
			
			bool failed=0;
			for(int lvl=1; lvl<=lvlmax && !failed; lvl++)
			{
				int N = pow(tilesize, lvl);
				Pointset< 2, unsigned int, Point<2, unsigned int> > pts;
				sampler.generateSamples<2, unsigned int, Point<2, unsigned int> >(pts, N);

				int nbtiles = N;
				int nbtiles_side = sqrt(nbtiles);	
				//std::cout << N << " samples " << nbtiles << " tiles " << nbtiles_side << " tilesside" << std::endl;

				std::vector<unsigned int> tiles;
				tiles.resize(nbtiles);
				
				texfile << "\\filldraw[draw=black, fill=black] (" << id0 << "," << id0 << ") rectangle (" << id0+1 << "," << id0+1 << ");" << std::endl;
						
				
				std::cout << N << " samples:";
				int n=0;
				for(n=0; n<N; n++)
				{
					int tilex = ((double)pts[n].pos()[0]/(double)N) * nbtiles_side;
					int tiley = ((double)pts[n].pos()[1]/(double)N) * nbtiles_side;
					tiles[tilex*nbtiles_side + tiley]++;
					if (tiles[tilex*nbtiles_side + tiley] > 1)
					{
						std::cout << "[FAILED]" << std::endl;
						texfile << "\\filldraw[draw=black, fill=red] (" << id0+((double)(lvl-1)/(double)lvlmax) << "," << id1 << ") rectangle (" << id0+((double)(lvl)/(double)lvlmax) << "," << id1+1 << ");" << std::endl;
						failed=1;
						break;
					}
				}
				if(n==N)
				{
					texfile << "\\filldraw[draw=black, fill=green] (" << id0+((double)(lvl-1)/(double)lvlmax) << "," << id1 << ") rectangle (" << id0+((double)(lvl)/(double)lvlmax) << "," << id1+1 << ");" << std::endl;
					std::cout << "[OK]" << std::endl;
				}
				
				texfile << "\\draw[draw=black, line width=1mm] (" << id0 << "," << id1 << ") rectangle (" << id0+1 << "," << id1+1 << ");" << std::endl;
				
				
				{
				std::stringstream filename;
				filename << "ptsSobol" <<id0 <<"_"<<id1<<"_"<<N<<"pts.dat";
				std::ofstream file(filename.str());
				for(int n=0; n<N; n++)
					file << pts[n].pos() << std::endl;
				file.close();
				}
				
			}
		}
	}
	
	texfile << "\\end{tikzpicture}" << std::endl;
	texfile << "\\end{figure} \\end{document}" << std::endl;
	texfile.close();
}*/

{
	std::cout << "TILESIZE 256" << std::endl;
	int tilesize = 256;
	int lvlmax = 3-fast;
	
	std::ofstream texfile("tilesize256_allowedpairs.tex");
	texfile << "\\documentclass[10pt,a4paper]{article}\n \
			\\usepackage[paperheight=10in,paperwidth=10in, margin=1in]{geometry} \n \
			\\usepackage[utf8]{inputenc}\n \
			\\usepackage{amsmath}\n \
			\\usepackage{amsfonts}\n \
			\\usepackage{tikz}\n \
			\\usepackage{amssymb}\n" << std::endl;
	texfile << "\\begin{document}" << std::endl;
	texfile << "\\thispagestyle{empty} \n Tilesize " << tilesize << " \\newline Nbpts: ";
	for(int l=1; l<=lvlmax; l++)
 		texfile << pow(tilesize, l) << " ";
	texfile << "\n \\begin{figure} \\begin{tikzpicture}[scale=1.25]" << std::endl;
	
	for(int id0=1; id0<=idmax; id0++)
	{
		texfile << "\\draw (" << id0+0.5 << "," << 0 << ") node {" << id0 << "};" << std::endl;
		texfile << "\\draw (" << 0 << "," << id0+0.5 << ") node {" << id0 << "};" << std::endl;
	}
	
	for(int id0=1; id0<=idmax; id0++)
	{
		for(int id1=id0+1; id1<=idmax; id1++)
		{
			std::cout << "Sobol " << id0 << "," << id1 << std::endl;
			
			indices[0] = id0;
			indices[1] = id1;
			sampler.setIndices(indices, 2);
			
			bool failed=0;
			for(int lvl=1; lvl<=lvlmax && !failed; lvl++)
			{
				int N = pow(tilesize, lvl);
				Pointset< 2, unsigned int, Point<2, unsigned int> > pts;
				sampler.generateSamples<2, unsigned int, Point<2, unsigned int> >(pts, N);

				int nbtiles = N;
				int nbtiles_side = sqrt(nbtiles);	
				//std::cout << N << " samples " << nbtiles << " tiles " << nbtiles_side << " tilesside" << std::endl;

				std::vector<unsigned int> tiles;
				tiles.resize(nbtiles);
				
				texfile << "\\filldraw[draw=black, fill=black] (" << id0 << "," << id0 << ") rectangle (" << id0+1 << "," << id0+1 << ");" << std::endl;
						
				
				std::cout << N << " samples:";
				int n=0;
				for(n=0; n<N; n++)
				{
					int tilex = ((double)pts[n].pos()[0]/(double)N) * nbtiles_side;
					int tiley = ((double)pts[n].pos()[1]/(double)N) * nbtiles_side;
					tiles[tilex*nbtiles_side + tiley]++;
					if (tiles[tilex*nbtiles_side + tiley] > 1)
					{
						std::cout << "[FAILED]" << std::endl;
						texfile << "\\filldraw[draw=black, fill=red] (" << id0+((double)(lvl-1)/(double)lvlmax) << "," << id1 << ") rectangle (" << id0+((double)(lvl)/(double)lvlmax) << "," << id1+1 << ");" << std::endl;
						failed=1;
						break;
					}
				}
				if(n==N)
				{
					texfile << "\\filldraw[draw=black, fill=green] (" << id0+((double)(lvl-1)/(double)lvlmax) << "," << id1 << ") rectangle (" << id0+((double)(lvl)/(double)lvlmax) << "," << id1+1 << ");" << std::endl;
					std::cout << "[OK]" << std::endl;
				}
				
				texfile << "\\draw[draw=black, line width=1mm] (" << id0 << "," << id1 << ") rectangle (" << id0+1 << "," << id1+1 << ");" << std::endl;
				
				
				/*{
				std::stringstream filename;
				filename << "ptsSobol" <<id0 <<"_"<<id1<<"_"<<N<<"pts.dat";
				std::ofstream file(filename.str());
				for(int n=0; n<N; n++)
					file << pts[n].pos() << std::endl;
				file.close();
				}*/
				
			}
		}
	}
	
	texfile << "\\end{tikzpicture}" << std::endl;
	texfile << "\\end{figure} \\end{document}" << std::endl;
	texfile.close();
}

	return 0;
}
