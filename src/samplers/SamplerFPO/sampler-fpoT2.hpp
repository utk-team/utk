#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>

#include "DT.h"
#include "util.h"

MTRand rng(time(NULL));

enum Strategy {
    GLOBAL, LOCAL, HYBRID
};

void generate_random(vector<Point_2> &points, unsigned npoints, MTRand &rng)
{
    points.reserve(npoints);
    for (unsigned i = 0; i < npoints; ++i) {
        Point_2 p(rng.randExc(), rng.randExc());
        points.push_back(p);
    }
}

void generate_darts(vector<Point_2> &points, unsigned npoints, MTRand &rng)
{
    const double rnorm  = 0.725;
    const double mdnorm = sqrt(SQRT3 * 0.5 * npoints);
    const double md     = rnorm / mdnorm;
    const double sqr_md = md * md;
    
    points.reserve(npoints);
    while (points.size() < npoints) {
        while (true) {
            Point_2 cand(rng.randExc(), rng.randExc());
            bool hit = true;
            for (unsigned i = 0; i < points.size(); ++i) {
                if (sqr_dist_unit_torus(cand, points[i]) < sqr_md) {
                    hit = false;
                    break;
                }
            }
            if (hit) {
                points.push_back(cand);
                break;
            }
        }
    }
}

void update_statistics(const DT &dt, unsigned it, double &global_md,
                       double &avg_md, bool output = true)
{
    dt.get_statistics(global_md, avg_md);
    
    const double mdnorm = sqrt(SQRT3 * 0.5 * dt.number_of_vertices());
    global_md *= mdnorm;
    avg_md    *= mdnorm;
}

void fpoSampler(utk::Pointset< 2, double, utk::Point< 2, double > >& pts, int nPts)
{
	unsigned max_iter  = 10000;
    double max_md      = 0.925;
    Strategy strategy  = GLOBAL;
    bool silent        = false;
    unsigned npoints   = nPts;
    
	double global_md = 0;
	double avg_md    = 0;
	double old_avg   = avg_md;
    
	// Initial points
	vector<Point_2> points;
	generate_random(points, npoints, rng);
	
	if (npoints == 1)
	{
		utk::Point<2, double> p;
		p.pos()[0] = points[0][0];
		p.pos()[1] = points[0][1];
		pts.push_back(p);
		return;
	}
	
	// Set up initial triangulation
	DT dt(points, true);
	
	update_statistics(dt, 0, global_md, avg_md, !silent);
	
	unsigned it = 0;
	vector<VH> neighbors;
	neighbors.reserve(10);
	
	// Set up vertex processing order
	// We choose a random order to ensure there is no correlation between the
	// original order of the point set and farthest-point optimization
	unsigned order[npoints];
	for (unsigned i = 0; i < npoints; ++i) order[i] = i;
	shuffle(order, npoints, rng);
	
	while (true)
	{
		// Main loop that moves each point to the farthest point, i.e. the
		// center of the largest circumcircle of a triangle in the DT
		for (unsigned i = 0; i < npoints; ++i)
		{
			// Pick removal candidate
			unsigned r = order[i];
			Point_2 cand = dt.get_vertex(r)->point();
			
			neighbors.clear();                 // Candidate's neighborhood
			dt.incident_vertices(r, neighbors);
			
			double cand_md = DBL_MAX;          // Candidate's local mindist
			for (unsigned i = 0; i < neighbors.size(); ++i) {
				cand_md = std::min(cand_md, 
						 CGAL::squared_distance(cand, neighbors[i]->point()));
			}
			
			Circle_2 c(cand, cand_md);         // Empty circle about candidate

			// Remove candidate
			dt.clear_vertex(r); 
			
			// Search for largest circumcircle in triangulation
			FH face;
			Circle_2 l = Circle_2(Point_2(0, 0), 0);
			switch (strategy) {
				case GLOBAL:
					l = dt.global_largest_circumcircle(face);
					break;
				case LOCAL:
					l = dt.local_largest_circumcircle(neighbors, face);
					break;
				case HYBRID:
					if (old_avg < 0.930)
						l = dt.global_largest_circumcircle(face);
					else
						l = dt.local_largest_circumcircle(neighbors, face);
					break;
			}

			// Set center of largest circumcircle as new vertex
			if (l.squared_radius() > c.squared_radius()) {
				Point_2 l_center = wrap_unit_torus(l.center());
				dt.set_vertex(r, l_center, face);
			} else
				dt.set_vertex(r, c.center(), face);
		}
		++it;
		update_statistics(dt, it, global_md, avg_md, !silent);
		
		if (it >= max_iter || global_md >= max_md)
			break;

		if (avg_md - old_avg == 0.0) break;
		old_avg = avg_md;
	}
	
	dt.save_vertices(pts);
}

/*int main(int argc, char** argv)
{
	std::string fn_output;
	std::string method;
	int nPts;
	int nPatches;
	
	boostPO::variables_map vm;
	boostPO::options_description desc("Allowed options");
	desc.add_options()
		("help,h",
			"produce help message")
		("output,o",
			boostPO::value<std::string>(&fn_output),
			"output filename")
		("nPts,n",
			boostPO::value<int>(&nPts)->default_value(4096),
			"number of point per point set")
		("nPatches,m",
			boostPO::value<int>(&nPatches)->default_value(1),
			"number of point sets")
		("binary,b",
			"write in binary mode")
		;
		
	boostPO::positional_options_description p;
	p.add("input", -1);
	
	try
	{	
		boostPO::store(
			boostPO::command_line_parser(argc, argv).
				options(desc).positional(p).run(), vm);
		boostPO::notify(vm);
	}
	catch(boost::program_options::error& e)
	{
		std::cerr << e.what() << std::endl;
		std::cout << desc << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if(vm.count("help"))
	{
		std::cout << desc << std::endl;
		exit(EXIT_SUCCESS);
	}
	
	if(vm.count("output") == 0)
	{
		std::cerr << "the option '--output' is required but missing" << std::endl;
		std::cout << desc << std::endl;
		exit(EXIT_FAILURE);
	}
	

	try
	{
		srand(time(NULL));
		
		//Open Output
		stk::io::PointSetOutputStream<2, double, double> stream;
		stream.setValueType(stk::io::PointSetStream::VAL_NONE);
		stream.setPositionType(stk::io::PointSetStream::POS_DOUBLE);
		if(vm.count("binary"))
		{
			stream.setBinary(true);
		}
		stream.open(fn_output);
		
		double meanTime = 0;
		double meanPts = 0;
		
		std::cout << "Point set " << 1 << "/" << nPatches << "       \r";
		std::cout.flush();
		
		for(int psCur=0; psCur<nPatches; psCur++)
		{
			utk::Pointset< 2, double, utk::Point< 2, double > > pts;
			
			boost::timer::cpu_timer timer;
	
			//Sampling function
			fpoSampler(pts, nPts);
	
			boost::timer::nanosecond_type timeSystem = timer.elapsed().system;
			boost::timer::nanosecond_type timeUser = timer.elapsed().user;
			boost::timer::nanosecond_type timeWall = timer.elapsed().wall;
			meanTime += (static_cast<double>(timeWall)/1000000000.0 - meanTime)/(psCur+1);
	
			meanPts += (static_cast<double>(pts.size()) - meanPts)/(psCur+1);
			
			std::cout << meanTime << " sec, " << meanPts << " pts" << std::endl;
			
	
			stream.write(pts);
		}
		
		std::cout << nPatches << (nPatches > 1 ? " point sets" : " point set") << " generated                             " << std::endl;
		
		stream.close();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
*/