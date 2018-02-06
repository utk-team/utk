#include <Geex/symbolics/ginacFunction.h>

#if WITH_GINAC

namespace Geex {

    GinacFunction::GinacFunction( const exvector& exs, const lst& args, const lst& params) 
        : Function( exs.size(), args.nops(), params.nops()),
        exs_(exs), args_(args), params_(params) {
            if(print__all){
            //std::cerr << "GinacFunction = ";					/**/
            //for(unsigned int i = 0; i < exs_.size(); i++ ){     /**/
            //    std::cerr << "[f("<<i<<") ="<<exs_[i] << "] " ; /**/
            //}													/**/
            //std::cerr << std::endl;								/**/
        	}
        }
        
    void GinacFunction::eval(bool do_f, bool do_g, bool do_H){
        
        lst argsAndParams;
        lst::const_iterator args_it = args_.begin();
        lst::const_iterator params_it = params_.begin();
        
        for( unsigned int i = 0; args_it != args_.end();  args_it++, i++ ){
            argsAndParams.append( ex_to<symbol>(*args_it) == x(i));
        }
        for( unsigned int i = 0; params_it != params_.end(); params_it++, i++ ){
            argsAndParams.append( ex_to<symbol>(*params_it) == p(i));
        }
        //std::cerr << "NEXT FUNC"<< std::endl;           /**/
        
        if (do_f){
            for(unsigned int i = 0; i < exs_.size(); i++ ){
                f(i) = ex_to<numeric>(exs_[i].subs( argsAndParams )).to_double();
                //std::cerr << "f("<<i<<") ="<<f(i)<< std::endl; /**/
            }
        }
        if (do_g){
            clear_g();
            //std::cerr << "G = " << std::endl;           /**/
            for(unsigned int i = 0; i < exs_.size(); i++ ){
                //std::cerr << " ";                       /**/
                args_it = args_.begin();
                for( unsigned int j = 0; args_it != args_.end(); args_it++, j++ ){
                    g(i,j) = ex_to<numeric>( 
                        exs_[i].diff(ex_to<symbol>(*args_it)).subs( argsAndParams )
                        ).to_double();
                    //std::cerr <<g(i,j)<< "\t" ;         /**/
                }
                //std::cerr << std::endl;                 /**/
            }
        }
        if (do_H){
            clear_h();
            lst::const_iterator args1_it;
            lst::const_iterator args2_it;
            
            //std::cerr << "H = " << std::endl;           /**/
            for(unsigned int i = 0; i < exs_.size(); i++ ){
                args1_it = args_.begin();
                //std::cerr << "(H"<<i<<")"<<std::endl;   /**/
                for( unsigned int j = 0; args1_it != args_.end(); args1_it++, j++ ){
                    args2_it = args_.begin();
                    //std::cerr<<" ";                     /**/
                    for( unsigned int k = 0; args2_it != args_.end(); args2_it++, k++ ){
                        h(i,j,k) = ex_to<numeric>( 
                            exs_[i].diff(ex_to<symbol>(*args1_it)).diff(ex_to<symbol>(*args2_it)).subs( argsAndParams )
                            ).to_double();
                        //std::cerr <<h(i,j,k)<<"\t";     /**/
                    }
                    //std::cerr << std::endl;             /**/
                }
            }
        }
    }

	void GinacFunction::mul_hessien( int k, const double* y, double* z ){
		gx_assert(false);
		// TODO
	}

   //--------------------------------------------------------------------------
   
   
    GinacConstFunction::GinacConstFunction( const exvector& exs, const lst& params) 
        : Function( exs.size(), 0, params.nops()),
        exs_(exs), params_(params) {
            if(print__all){
            //std::cerr << "GinacConstFunction = ";					/**/
            //for(unsigned int i = 0; i < exs_.size(); i++ ){     /**/
            //    std::cerr << "[f("<<i<<") ="<<exs_[i] << "] " ; /**/
            //}													/**/
            //std::cerr << std::endl;								/**/
        	//}
        }
        
    void GinacConstFunction::eval(bool do_f, bool do_g, bool do_H){
        
        lst argsAndParams;
        lst::const_iterator params_it = params_.begin();
        
        for( unsigned int i = 0; params_it != params_.end(); params_it++, i++ ){
            argsAndParams.append( ex_to<symbol>(*params_it) == p(i));
        }
        //std::cerr << "NEXT FUNC"<< std::endl;           /**/
        
        if (do_f){
            for(unsigned int i = 0; i < exs_.size(); i++ ){
                f(i) = ex_to<numeric>(exs_[i].subs( argsAndParams )).to_double();
                //std::cerr << "f("<<i<<") ="<<f(i)<< std::endl; /**/
            }
        }
        
        if (do_g){
            clear_g();
        }
        
        if (do_H){
            clear_h();
        }
    }


	void GinacConstFunction::mul_hessien( int k, const double* y, double* z ){
		// A constant function has a hessien with all coefficients equal to zero.
		for(unsigned int i=0; i<nb_x(); i++) {
			z[i] = 0.0 ;
        }
	}

}

#endif



