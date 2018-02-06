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


#include <Geex/numerics/lbfgs_interface.h>

#include <LBFGSB/lbfgs.h>
#include <LBFGSB/lbfgsb.h>
#include <HLBFGS/HLBFGS.h>

#include <iostream>




//************* for LBFGS ***********

void funcgrad(ap::real_1d_array& x, double& f, ap::real_1d_array& g);

void lbfgsnewiteration(
					   const ap::real_1d_array& x,
					   double f,
					   const ap::real_1d_array& g
					   );

//************* for LBFGSB ***********
// uses also funcgrad()

void lbfgsbnewiteration(
						const ap::real_1d_array& x,
						double f,
						const ap::real_1d_array& g
						);

//************* for HLBFGS *************

void evalfunc(int N, double* x, double *prev_x, double* f, double* g) ;

void evalfunction_h(int N, double* x, double* prev_x, double* f, double* g, HESSIAN_MATRIX& m_hessian);

void hlbfgsnewiteration(int iter, int call_iter, double *x, double* f, double *g,  double* gnorm);

//************ for LMBM *************
extern "C"
{
    int lmbmu_(int *n, int *na, int *mcu, int *mc, int *nw, 
        double *x, double *f, double *rpar, int *ipar, int *iout, 
        float *time, float *rtim, double *w);
    int funder_(int *n, double *x, double *f, double *g, int *iterm);
    int lmbmnewiter_(int *iter, int *call_iter, double *x, double* f, double *g,  double* gnorm);
};


namespace Geex {

	class GlobalOptimizerConfig {
	public:

		static void set_funcgrad_callback(funcgrad_fp fp){
			funcgrad_callback = fp;
		}

		static void set_newiteration_callback(newiteration_fp fp){
			newiteration_callback = fp;
		}

		static void set_evalhessian_callback(evalhessian_fp fp){
			evalhessian_callback = fp;
		}

		static void newiteration( const double* x, double f, const double* g, double gnorm ){
			(*newiteration_callback)(N, x, f, g, gnorm);
		}

		static void funcgrad(double* x, double& f, double* g ){
			(*funcgrad_callback)(N, x, f, g);
		}

		static void evalhessian(int N, double *x, double &f, double *g, HESSIAN_MATRIX& hessian){
			(*evalhessian_callback)(N, x, f, g, hessian);
		}

		static void set_N(int n){
			N = n;
		}

	private:
		static funcgrad_fp funcgrad_callback;
		static newiteration_fp newiteration_callback;
		static evalhessian_fp evalhessian_callback;
		static int N;
	};

	newiteration_fp		GlobalOptimizerConfig::newiteration_callback	= nil;
	funcgrad_fp			GlobalOptimizerConfig::funcgrad_callback		= nil;
	evalhessian_fp		GlobalOptimizerConfig::evalhessian_callback = nil;
	int GlobalOptimizerConfig::N = 0;

	void show_bfgs_message(int info) {
		switch(info)  {
		case -1: std::cout <<"wrong parameters were specified " << std::endl;
			break;
		case 0: std::cout <<"interrupted by user " << std::endl;
			break;
		case 1:  std::cout <<"relative function decreasing is less or equal to EpsF " << std::endl;
			break;
		case 2:  std::cout <<" step is less or equal to EpsX" << std::endl;
			break;
		case 4:  std::cout <<"gradient norm is less or equal to EpsG, " << std::endl;
			break;
		case 5:  std::cout <<"number of iterations exceeds MaxIts. " << std::endl;
			break;
		default:
			break;
			}
	}



	void LBFGSOptimizer::optimize( double* x ) {
		gx_assert( newiteration_callback != nil ) ;
		gx_assert( funcgrad_callback != nil ) ;
		gx_assert( n>0 ) ;
		gx_assert( x!=nil ) ;


		GlobalOptimizerConfig::set_newiteration_callback(newiteration_callback);
		GlobalOptimizerConfig::set_funcgrad_callback(funcgrad_callback);
		GlobalOptimizerConfig::set_N(n);

		int info;
		ap::real_1d_array x_ap;
		x_ap.setcontent(1, n, x); //duplicates x

		lbfgsminimize(n,m,x_ap,epsg,epsf,epsx,max_iter,info);

		// copy back the solution
		memcpy(x, x_ap.getcontent(), n*sizeof(double));

		if(verbose){
			show_bfgs_message(info);
		}

	}


	void LBFGSBOptimizer::set_nbd(int nb, int* rhs){
		gx_assert (nb>0 && rhs != nil);
		if(nbd!=nil) delete [] nbd;
		nbd = new int[nb] ;
		memcpy(nbd,rhs,nb*sizeof(int));
	}
	void LBFGSBOptimizer::set_l(int nb, double* rhs){
		gx_assert (nb>0 && rhs != nil);
		if(l!=nil) delete [] l;
		l = new double[nb] ;
		memcpy(l,rhs,nb*sizeof(double));
	}
	
	void LBFGSBOptimizer::set_u(int nb, double* rhs){
		gx_assert (nb>0 && rhs != nil);
		if(u!=nil) delete [] u;
		u = new double[nb] ;
		memcpy(u,rhs,nb*sizeof(double));
	}
		

	void LBFGSBOptimizer::optimize( double* x ) {
		gx_assert( newiteration_callback != nil ) ;
		gx_assert( funcgrad_callback != nil ) ;
		gx_assert( n>0 ) ;
		gx_assert( nbd != nil ) ;
		gx_assert( l != nil ) ;
		gx_assert( u != nil ) ;
		gx_assert( x != nil ) ;

		GlobalOptimizerConfig::set_newiteration_callback(newiteration_callback);
		GlobalOptimizerConfig::set_funcgrad_callback(funcgrad_callback);
		GlobalOptimizerConfig::set_N(n);

		int info;
		ap::real_1d_array x_ap;
		ap::integer_1d_array nbd_ap;
		ap::real_1d_array l_ap, u_ap;


		x_ap.setcontent(1, n, x);
		nbd_ap.setcontent(1, n, nbd);
		l_ap.setcontent(1, n, l);
		u_ap.setcontent(1, n, u);

		lbfgsbminimize(n, m, x_ap, epsg, epsf, epsx, max_iter, nbd_ap, l_ap, u_ap, info);

		memcpy(x, x_ap.getcontent(), n*sizeof(double));

		if(verbose){
			show_bfgs_message(info);
		}

	}

	HLBFGSOptimizer::HLBFGSOptimizer() {
		b_m1qn3 = false; b_cg = false;
	}

	void HLBFGSOptimizer::optimize( double* x ) {
		gx_assert( newiteration_callback != nil ) ;
		gx_assert( funcgrad_callback != nil ) ;
		gx_assert( n>0 ) ;
		gx_assert( x != nil ) ;

		GlobalOptimizerConfig::set_newiteration_callback(newiteration_callback);
		GlobalOptimizerConfig::set_funcgrad_callback(funcgrad_callback);
		GlobalOptimizerConfig::set_N(n);

		double parameter[20] ;
        int hlbfgs_info[20] ;

		//initialize parameters and infos
		INIT_HLBFGS(parameter, hlbfgs_info) ; 
		hlbfgs_info[3] = b_m1qn3?1:0; //determines whether we use m1qn3
		hlbfgs_info[4] = max_iter ; // max iterations
        hlbfgs_info[10] = b_cg?1:0; //determines whether we use cg
		parameter[5] = 0; // disabled
		parameter[6] = epsg;
		HLBFGS(
			n,
			m,
			x, 
			evalfunc,
			0, 
			HLBFGS_UPDATE_Hessian,
			hlbfgsnewiteration,
			parameter,
			hlbfgs_info
			);

	}

	HLBFGS_HessOptimizer::HLBFGS_HessOptimizer() {
		b_m1qn3 = false; b_cg = false; T = 0;
	}

	void HLBFGS_HessOptimizer::optimize( double* x ) {
		gx_assert( newiteration_callback != nil ) ;
		gx_assert( funcgrad_callback != nil ) ;
        gx_assert( evalhessian_callback != nil);
		gx_assert( n>0 ) ;
		gx_assert( x != nil ) ;

		GlobalOptimizerConfig::set_newiteration_callback(newiteration_callback);
		GlobalOptimizerConfig::set_funcgrad_callback(funcgrad_callback);
        GlobalOptimizerConfig::set_evalhessian_callback(evalhessian_callback);
		GlobalOptimizerConfig::set_N(n);

		double parameter[20] ;
        int hlbfgs_info[20] ;

		//initialize parameters and infos
		INIT_HLBFGS(parameter, hlbfgs_info) ; 
		hlbfgs_info[4] = max_iter ; // max iterations
        hlbfgs_info[6] = T ; // update interval of hessian
        hlbfgs_info[7] = 1 ;  // 0: without hessian, 1: with accurate hessian

        HLBFGS(
			n,
			m,
			x, 
			evalfunc,
			evalfunction_h, 
			HLBFGS_UPDATE_Hessian,
			hlbfgsnewiteration,
			parameter,
			hlbfgs_info
			);
	}
	
    LMBMOptimizer::LMBMOptimizer() {
    }

    void LMBMOptimizer::optimize( double* x ) {
        gx_assert( newiteration_callback != nil ) ;
        gx_assert( funcgrad_callback != nil ) ;
        gx_assert( n>0 ) ;
        gx_assert( x != nil ) ;

        GlobalOptimizerConfig::set_newiteration_callback(newiteration_callback);
        GlobalOptimizerConfig::set_funcgrad_callback(funcgrad_callback);
        GlobalOptimizerConfig::set_N(n);

        int na = 10; //2 .. 10
        int mcu = m; // 3..20
        int mc = m;  //3..15 <= mcu
        double rpar[8];
        memset(rpar, 0, sizeof(double)*8);
        rpar[1] = -1;
        rpar[5] = 0.5;
        rpar[7] = 10;
        int ipar[7];
        memset(ipar, 0, sizeof(int)*7);
        ipar[1] = max_iter;
        ipar[2] = 2*max_iter;
        ipar[4] = -1;

        int iout[3];
        float time, rtim[2];
        time = 0.0;
        int nw = 2*(1+9*n+2*n*na+3*na+2*n*(mcu+1)+3*(mcu+2)*(mcu+1)/2+9*(mcu+1)); // multiply factor 2 
        double *w = new double[nw];
        double f = 0;
        lmbmu_(&n, &na, &mcu, &mc, &nw, x, &f, rpar, ipar, iout, &time, rtim, w);

        delete[] w;

    }
}

//-------- global functions for optimizers ----------


//----------LBFGS -------------

void funcgrad(ap::real_1d_array& x, double& f, ap::real_1d_array& g) {
	Geex::GlobalOptimizerConfig::funcgrad( x.getcontent(), f, g.getcontent() );
}


void lbfgsnewiteration(
	const ap::real_1d_array& x,
    double f,
    const ap::real_1d_array& g
){
	double gnorm2 = 0.0 ;
    for(int i=g.getlowbound(); i<=g.gethighbound(); i++) {
        gnorm2 += g(i)*g(i) ;
    }
    gnorm2 = ::sqrt(gnorm2) ;
	Geex::GlobalOptimizerConfig::newiteration( x.getcontent(), f, g.getcontent(), gnorm2);
}

void lbfgsbnewiteration(
	const ap::real_1d_array& x,
	double f,
	const ap::real_1d_array& g
){
	double gnorm2 = 0.0 ;
    for(int i=g.getlowbound(); i<=g.gethighbound(); i++) {
        gnorm2 += g(i)*g(i) ;
    }
    gnorm2 = ::sqrt(gnorm2) ;
	Geex::GlobalOptimizerConfig::newiteration( x.getcontent(), f, g.getcontent(), gnorm2);
}

//--------- HLBFGS ------

void evalfunc(int N, double* x, double *prev_x, double* f, double* g){
	Geex::GlobalOptimizerConfig::funcgrad(x, *f, g);
}

void hlbfgsnewiteration(int iter, int call_iter, double *x, double* f, double *g,  double* gnorm){
	Geex::GlobalOptimizerConfig::newiteration(x, *f, g, *gnorm);
}

void evalfunction_h(int N, double* x, double* prev_x, double* f, double* g, HESSIAN_MATRIX& m_hessian){
	Geex::GlobalOptimizerConfig::evalhessian(N, x, *f, g, m_hessian);
}

//--------- LMBM ------
int funder_(int *n, double *x, double *f, double *g, int *iterm)
{
    Geex::GlobalOptimizerConfig::funcgrad(x, *f, g);
    *iterm = 0;
    return 0;
}

int lmbmnewiter_(int *iter, int *call_iter, double *x, double* f, double *g,  double* gnorm)
{
    Geex::GlobalOptimizerConfig::newiteration(x, *f, g, sqrt(*gnorm));    
    return 0;
}
