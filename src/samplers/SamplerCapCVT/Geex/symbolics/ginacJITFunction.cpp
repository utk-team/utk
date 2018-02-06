#include <Geex/symbolics/ginacJITFunction.h>

#if WITH_GINAC

namespace Geex{
	GinacJITFunction::GinacJITFunction( const exvector& exs, const lst& args, const lst& params, bool initJIT)
		: GinacFunction(exs,args,params) {
		gx_assert(false);
		// TODO if initJIT, init the whole JIT mechanisme...
	}

	void GinacJITFunction::eval(bool do_f, bool do_g, bool do_H){
		gx_assert(false);
		//TODO eval using JITnac
	}

	void GinacJITFunction::mul_hessien( int k, const double* y, double* z ){
		gx_assert(false);
		// TODO
	}


	GinacConstJITFunction::GinacConstJITFunction( const exvector& exs, const lst& params, bool initJIT)
		: GinacConstFunction(exs,params){
		gx_assert(false);
		// TODO if initJIT, init the whole JIT mechanisme...
	}

	void GinacConstJITFunction::eval(bool do_f, bool do_g, bool do_H){
		gx_assert(false);
		// TODO eval using JITnac
	}

	void GinacConstJITFunction::mul_hessien( int k, const double* y, double* z ){
		// A constant function has a hessien with all coefficients equal to zero.
		for(unsigned int i=0; i<nb_x(); i++) {
			z[i] = 0.0 ;
        }
	}

}
#endif
