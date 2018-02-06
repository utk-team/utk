#include "optimizer.h"



namespace Geex{


	Optimizer::Optimizer() 
			: n(0), m(0), max_iter(1000),
			newiteration_callback(nil),
			funcgrad_callback(nil),
            evalhessian_callback(nil),
			epsg(0), epsf(0), epsx(0),
			verbose(true)
	{
	}

	Optimizer::~Optimizer(){
	}


}