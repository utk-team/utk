#ifndef __GINACJITFUNCTION__
#define __GINACJITFUNCTION__

#if WITH_GINAC

#include <ginac/ginac.h>
using namespace GiNaC;

#include "ginacFunction.h"
namespace Geex {

    class GEEX_API GinacJITFunction : public GinacFunction {
      public:
        GinacJITFunction( const exvector& exs, const lst& args, const lst& params, bool initJIT);
        virtual void eval(bool do_f, bool do_g, bool do_H) ;
		virtual void mul_hessien( int k, const double* y, double* z );
	
    };
    
    class GinacConstJITFunction : public GinacConstFunction {
      public:
        GinacConstJITFunction( const exvector& exs, const lst& params, bool initJIT);
        virtual void eval(bool do_f, bool do_g, bool do_H) ;
		virtual void mul_hessien( int k, const double* y, double* z );
	
    };
    
}

#endif // WITH_GINAC
#endif

