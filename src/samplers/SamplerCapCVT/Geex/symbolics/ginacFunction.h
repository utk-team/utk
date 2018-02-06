#ifndef __GINACFUNCTION__
#define __GINACFUNCTION__

#if WITH_GINAC

#include <ginac/ginac.h>
using namespace GiNaC;

#include "function.h"
namespace Geex {

	/** Symbolic function implementation of Function. Allows to
	define a Function using symbolic expressions using GiNaC, and to use GiNaC's
	derivation mechanisme to define the gradient and Hessian. Evaluation is
	performed using GiNaC's built-in evaluation mechanism. */
    class GEEX_API GinacFunction : public Function {
      public:
        GinacFunction( const exvector& exs, const lst& args, const lst& params);
        virtual void eval(bool do_f, bool do_g, bool do_H) ;
		virtual void mul_hessien( int k, const double* y, double* z );
	
	protected:
        const exvector& exs_;
        const lst& args_;
        const lst& params_;
    };
    
    /** Symbolic constant function implementation of Function, using GiNaC.
    	Allows to define expressions using the parameters of the Function. 
    	Evaluation is performed using GiNaC's built-in evaluation mechanism. */
    class GinacConstFunction : public Function {
      public:
        GinacConstFunction( const exvector& exs, const lst& params);
        virtual void eval(bool do_f, bool do_g, bool do_H) ;
		virtual void mul_hessien( int k, const double* y, double* z );

      protected:
        const exvector& exs_;
        const lst& params_;
    };
    
}

#endif // WITH_GINAC
#endif
