#include <cstdlib>
#include <iostream>
#include <fstream>
#include <exception>
#include <ctime>
#include <boost/program_options.hpp>
#include <boost/random.hpp>

#include "scene.h"

#include "../../src/parameters/ParamParser_getopt.hpp"
#include "../../src/pointsets/Pointset.hpp"
#include "../../src/io/fileIO.hpp"

double random01()
{
	static boost::mt19937 rng(time(NULL));
	static boost::uniform_01<boost::mt19937&> zeroone(rng);
	return zeroone();
}

int main(int argc, char** argv)
{
	/* ARG PARSER *****************************************************/

	bool help=false;
	int nReal=1;
	int nPts=1024;
	std::string fn_output;

	utk::ParamParser_getopt parser;

	parser.addShortOption('h', &help, 0, utk::assignBoolTrue, utk::displayBool, "\tDisplays this help message", "");
	parser.addShortOption('n', &nPts, 1, utk::assignInt, utk::displayInt, "[int=1024] \tThe number of samples", "");
	parser.addShortOption('o', &fn_output, 1, utk::assignString, utk::displayString, "[string]\tThe output file", "");
	parser.addShortOption('m', &nReal, 1, utk::assignInt, utk::displayInt, "[int=1]\tThe number of realisations", "Realisations:");

	parser.parse(argc, argv);

	if(fn_output.empty())
	{
		ERROR("Parameter -o mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	if(help)
	{
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}

	int current_real=0;
	while(current_real<nReal)
	{

		/* PROG ***********************************************************/
		try
		{
			Scene bnot_scene;
			std::vector<Point> in_points;

			/* GENERATE ***************************************************/
			std::cout << "Intializing w  " << nPts << " random points" << std::endl;
			for(int i=0; i<nPts; i++)
				in_points.push_back(Point(random01(), random01()));
			std::cout << "Done" << std::endl;

			/* OPTIMIZE ***************************************************/

			std::vector<FT> noise(in_points.size(), 0.0);
			std::vector<FT> weights(in_points.size(), 0.0);
			std::cout << "Construct Tglation" << std::endl;
			bnot_scene.construct_triangulation(in_points, weights, noise);
			std::cout << "Done" << std::endl;

			std::cout << "Optimizing" << std::endl;
			//max newton iter, epsilon, maxiter, ...

			//high number of points
			if(nPts > 150)
				bnot_scene.optimize_all(0.0, 0.0, 500, 0.2, 500, std::cout, true);
			else
			//low number of points
				bnot_scene.optimize_all(0.0, 0.0, 20, 0.2, 20, std::cout, true);

			std::cout << "Done" << std::endl;

			std::vector<Point> out_points;
			bnot_scene.collect_visible_points(out_points);

			/* WRITE ******************************************************/
			{
				utk::Pointset<2, double, utk::Point<2, double> > pts;
				pts.resize(out_points.size());

				for(unsigned int i=0; i<out_points.size(); i++)
				{
					pts[i].pos()[0] = out_points.at(i).x();
					pts[i].pos()[1] = out_points.at(i).y();
				}

				utk::PointsetWriter<2, double, utk::Point<2, double> > writer;
				writer.open(fn_output);
				writer.writePointset(pts);
				writer.close();
			}

		}
		catch(const std::exception& e)
		{
			std::cerr << "Error : " << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}

		current_real++;
	}

	exit(EXIT_SUCCESS);
}
