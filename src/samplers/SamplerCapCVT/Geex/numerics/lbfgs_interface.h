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

#ifndef __GEEX_NUMERICS_LBFGS_ADAPTER__
#define __GEEX_NUMERICS_LBFGS_ADAPTER__

#include <Geex/basics/types.h>
#include <Geex/basics/assert.h>

#include "optimizer.h"


namespace Geex {

	enum optimizer_enum {
		LBFGS_ID,
		LBFGSB_ID,
		HLBFGS_ID,
		HLBFGS_Hess_ID,
        LMBM_ID
	};

	//--------------- LBFGS -------------


	class GEEX_API LBFGSOptimizer : public Optimizer {
	public:

		LBFGSOptimizer(){
		}

		virtual void optimize( double* x );

		virtual int get_optimizer_id(){
			return (int)LBFGS_ID;
		}


	protected:
	};

	// --------------  LBFGSB  ----------------

	class GEEX_API LBFGSBOptimizer : public LBFGSOptimizer {
	public:

		LBFGSBOptimizer(): LBFGSOptimizer(), nbd(nil), l(nil), u(nil) {
		}

		~LBFGSBOptimizer(){
			if(nbd) delete [] nbd;
			if(l) delete [] l;
			if(u) delete [] u;
		}

		virtual void optimize( double* x );

		virtual int get_optimizer_id(){
			return (int)LBFGSB_ID;
		}

		void set_nbd(int nb, int* rhs);
		void set_l(int nb, double* rhs);
		void set_u(int nb, double* rhs);
		
	protected:
		int* nbd;
		double* l;
		double* u;
	};


	// ---------------- HLBFGS ------------------

	class GEEX_API HLBFGSOptimizer : public Optimizer {
	public:

		HLBFGSOptimizer();

		virtual void optimize( double* x );

		virtual int get_optimizer_id(){
			return (int)HLBFGS_ID;
		}

		void set_m1qn3(bool _m1qn3){b_m1qn3 = _m1qn3;}
        void set_cg(bool _cg){b_cg = _cg;}
	protected:
		bool b_m1qn3; 
        bool b_cg;
	};


	class GEEX_API HLBFGS_HessOptimizer : public HLBFGSOptimizer {
	public:

		HLBFGS_HessOptimizer();

		virtual void optimize( double* x );

		int get_optimizer_id(){
			return (int)HLBFGS_Hess_ID;
		}

        void set_T(unsigned int mT) {T = mT; } 
    protected:
        unsigned int T;
	};


    //LMBM
    class GEEX_API LMBMOptimizer : public Optimizer {
    public:

        LMBMOptimizer();

        virtual void optimize( double* x );

        virtual int get_optimizer_id(){
            return (int)LMBM_ID;
        }
    };

}




#endif
