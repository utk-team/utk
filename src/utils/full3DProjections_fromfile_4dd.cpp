#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>
#include <fstream>

using namespace utk;

#define D 4

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	
	std::string param_pointset = "";
	std::string param_proj_file="";
	parser.addShortOption('i', &param_pointset, 1, assignString, displayString, (char*)"<string>\tThe input pointset", (char*)"Input");
	parser.addShortOption('o', &param_proj_file, 1, assignString, displayString, (char*)"<string>\tThe output projections files", (char*)"Output");

	parser.parse(argc, argv);
	
	if(param_pointset=="")
	{
		ERROR("Missing option -i");
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	
	PointsetWriter< 3, double, Point<3, double> > writer;

	PointsetReader< D, double, Point<D, double> > stream;
	stream.open(param_pointset);
	
	Pointset< D, double, Point<D, double> > pts;
	while(stream.readPointset(pts))
	{
		double proj[D] = {0};
		int max = 1 << D;
		for(int i=7; i<max; i++)
		{
			int nb = 0;
			int b0 = 0;
			int b1 = 0;
			int b2 = 0;
			for(int p=0; p<D; p++)
			{
				proj[p] = 0;
				int b = (i>>p) & 1;
				if(b)
				{	
					if(nb==0)
						b0 = p;
					if(nb==1)
						b1 = p;
					if(nb==2)
						b2 = p;
					nb++;
					proj[p] = 1;
				}
			}
			if(nb != 3)
				continue;
			
			Pointset< 3, double, Point<3, double> > projected = pts.Projection3D(Vector<D, double>(proj));
			
			char name[100];
			sprintf(name, param_proj_file.c_str(), b0, b1, b2);
			
			
			writer.open(name);
			writer.writePointset(projected);
			writer.close();
		}
		
		pts.clear();
	}
	
	return 0;
}