/*
 *  _____ _____ ________  _
 * /  __//  __//  __/\  \//
 * | |  _|  \  |  \   \  / 
 * | |_//|  /_ |  /_  /  \ 
 * \____\\____\\____\/__/\\
 *
 * Graphics Environment for EXperimentations.
 *  Copyright (C) 2006 INRIA - Project ALICE
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: 
 *
 *     ALICE Project - INRIA
 *     INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */

#include "../Geex/graphics/geexapp.h"
#include "../Geex/basics/file_system.h"
#include "../Geex/basics/stopwatch.h"
//#include <glut_viewer/tweak_bar.h>
#include <iostream>
#include <fstream>
#include "cvt.h"

//void Lloyd() ;

//extern std::string boundary_filename_ ;
//int param_nb_real=1;


namespace Geex {

    class CVTApp : public GeexApp {

    public:
        CVTApp(int argc, char** argv) : GeexApp(argc, argv) { 

		hdr_ = false ;
		boundary_filename_ = "square.line";
		nb_lambda_ = 30.0;
        nb_iter_ = 100;
		non_convex_ = GL_FALSE;

        }

        CVT* cvt() { return static_cast<CVT*>(scene()) ; }

	    virtual void init_scene() {
            scene_ = new CVT ;
            std::cerr << "Non convex = " 
                      << (non_convex_ ? "true" : "false") 
                      << "(use +non_convex to set)" << std::endl ;
            cvt()->set_non_convex_mode(non_convex_) ;

            if(boundary_filename_.length() > 0) {
                cvt()->load_boundary(boundary_filename_) ;
            }
            cvt()->insert_random_vertices(nb_points_) ;
			cvt()->capCVT_lambda() = nb_lambda_;
        }

        void Lloyd() {
            cvt()->lloyd(nb_iter_) ;
        }

        void Lloyd(int nb_iter) {
            cvt()->lloyd(nb_iter) ;
        }

        void NewtonLloyd() {
            cvt()->newton_lloyd(nb_iter_) ;
        }

        void reset() {
            cvt()->clear() ;
            cvt()->insert_random_vertices(nb_points_) ;
        }

		void save(int i=0) {
			std::string filename = out_filename_;
			cvt()->save(filename , i);
		}
		
		std::vector<double> get_samples()
		{
			return cvt()->get_samples();
		}

		void load() {
			std::string filename = Geex::FileSystem::get_project_root() + 
				"/data/" + "input_pts.dat" ;
			cvt()->load(filename) ;
		}

    public:
		std::string boundary_filename_ ;
		std::string out_filename_;
		int nb_points_ ;
		GLfloat nb_lambda_;
		GLfloat nb_iter_ ;
		GLboolean non_convex_ ;
    } ;
}

//Geex::CVTApp* cvt_app() { return static_cast<Geex::CVTApp*>(Geex::GeexApp::instance()) ; }

/*int main(int argc, char** argv) {
    	Geex::initialize() ;

	srand((unsigned int)time(0));

	Geex::CVTApp CVT_App(argc, argv);
	
	//std::cerr << "============= Init Scene ==========" << std::endl;
	CVT_App.init_scene();
	//std::cerr << std::endl;
	
	for(int i=0; i<param_nb_real; i++)
	{
		CVT_App.reset();
		
		//std::cerr << "============= CapCVT iterating  ==========" << std::endl;
		CVT_App.NewtonLloyd();
		//std::cerr << std::endl;

		//std::cerr << "============= Save data  ==========" << std::endl;
		CVT_App.save(i);
		//std::cerr << std::endl;
	
	}

	//int aa;
	//std::cin >> aa;

	return 0;
}*/
