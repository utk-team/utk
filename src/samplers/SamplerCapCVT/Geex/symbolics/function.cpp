#include <Geex/symbolics/function.h>
#include <iostream>

namespace Geex {

    Function::Function( unsigned int nb_f, unsigned int nb_x, unsigned int nb_p)
    	: nb_f_(nb_f), nb_x_(nb_x), nb_p_(nb_p), 
        f_(nil), x_(nil), p_(nil), g_(nil), h_(nil),  
        owns_f_(false), owns_x_(false), owns_p_(false), owns_g_(false), owns_h_(false),
        cur_x_(0), cur_p_(0), valid_memory(true), alloc_h_storage(true) {}
    
    
    void Function::manage_memory(
    	double* x_storage,
    	double* p_storage,
    	double* f_storage,
    	double* g_storage,
    	double* h_storage
    	){
    	
    	// if memory is flaged as invalid, then we make sure everything is clean
    	if( !valid_memory ){
    		if(owns_x_) { delete[] x_ ; owns_x_ = false ; }
    		if(owns_p_) { delete[] p_ ; owns_p_ = false ; }
    		if(owns_f_) { delete[] f_ ; owns_f_ = false ; }
    		if(owns_g_) { delete[] g_ ; owns_g_ = false ; }
    		if(owns_h_) { delete[] h_ ; owns_h_ = false ; }
    		x_ = nil ;
    		p_ = nil ;
    		f_ = nil ;
    		g_ = nil ;
    		h_ = nil ;
    	}
    	
    	/* 
    		For each storage:
    			if the passed array is nil, then we want an internaly allocated
    			array, else we want to use the passed array.
    			
    			If we want an internally allocated array, then we only need to 
    			check if we already have it or if we need to create it.
    			
    			If we want to use the passed array, then we first check whether
    			it is the same we already have, or not. If it's not, then it's
    			either internally allocated memory, and we free it, or a 
    			previoussly passed array, and we don't bother with it. In both
    			cases we set the storage pointer at the new value.
    	*/
    	
    	if ( x_storage == nil ){ 
    		// if we don't already have it, create our own storage
    		if( !owns_x_ ){ x_ = nil ; alloc_x() ; }
    	} else {if( x_storage != x_store() ){  
    			// link with this storage 
    			if( owns_x_ ) { delete[] x_ ; x_ = nil ; owns_x_ = false ; }
    			set_x_store(x_storage);
    		}
    	}
    	
		if ( p_storage == nil ){
    		// if we don't already have it, create our own storage
    		if( !owns_p_ ){ p_ = nil ; alloc_p() ; }
    	} else {
    		if( p_storage != p_store() ){
    			// link with this storage 
    			if( owns_p_ ) { delete[] p_ ; p_ = nil ; owns_p_ = false ; }
    			set_p_store(p_storage);
    		}
    	}
    	
    	if ( f_storage == nil ){
    		// if we don't already have it, create our own storage
    		if( !owns_f_ ){ f_ = nil ; alloc_f() ; }
    	} else {
    		if( f_storage != f_store() ){
    			// link with this storage 
    			if( owns_f_ ) { delete[] f_ ; f_ = nil ; owns_f_ = false ; }
    			set_f_store(f_storage);
    		}
    	}
    	
    	if ( g_storage == nil ){
    		// if we don't already have it, create our own storage
    		if( !owns_g_ ){ g_ = nil ; alloc_g() ; }
    	} else {
    		if( g_storage != g_store() ){
    			// link with this storage 
    			if( owns_g_ ) { delete[] g_ ; g_ = nil ; owns_g_ = false ; }
    			set_g_store(g_storage);
    		}
    	}
    	
    	if ( h_storage == nil ){
    		// if we don't already have it, create our own storage, unless we don't want one
    		if( !owns_h_ ){ 
				h_ = nil ;
				if(alloc_h_storage) alloc_h() ;
			}
    	} else {
    		if( h_storage != h_store() ){
    			// link with this storage 
    			if( owns_h_ ) { delete[] h_ ; h_ = nil ; owns_h_ = false ; }
    			set_h_store(h_storage);
    		}
    	}
    	
    	valid_memory = true ;
    }
    
    Function::~Function() {
    	// Delete only internally allocated memory
        if(owns_f_) { delete[] f_ ; }
        if(owns_x_) { delete[] x_ ; }
        if(owns_p_) { delete[] p_ ; }
        if(owns_g_) { delete[] g_ ; }
        if(owns_h_ && alloc_h_storage) { delete[] h_ ; }
    }
    
    void Function::print() {

        std::cerr << "F: " << typeid(*this).name() << std::endl ;

        std::cerr << "f = [" ;
        for(unsigned int i=0; i<nb_f(); i++) {
            std::cerr << f(i) << " " ;
        }
        std::cerr << "]" << std::endl ;

        std::cerr << "x = [" ;
        for(unsigned int i=0; i<nb_x(); i++) {
            std::cerr << x(i) << " " ;
        }
        std::cerr << "]" << std::endl ;

        std::cerr << "p = [" ;
        for(unsigned int i=0; i<nb_p(); i++) {
            std::cerr << p(i) << " " ;
        }
        std::cerr << "]" << std::endl ;

    }

    //_______________________________________________________________________

    Identity::Identity( unsigned int nb )
    	: Function(nb, nb, 0) {
    }
    
    void Identity::eval(bool do_f, bool do_g, bool do_H) {
    	/* 
    		Creates a direct mapping of the input arguments to the output
    	*/
        if(do_f) {
            for(unsigned int i=0; i<nb_f(); i++) {
                f(i) = x(i) ;
            }
        }
        if(do_g) {
            for(unsigned int i=0; i<nb_f(); i++) {
                for(unsigned int j=0; j<nb_x(); j++) {
                    g(i,j) = (i==j) ? 1.0 : 0.0 ;
                }
            }
        }
        if(do_H  ){
			clear_h();
		}
    }

	void Identity::mul_hessien( int k, const double* y, double* z ){
		// Identity has a hessien with all coefficients equal to zero.
		for(unsigned int i=0; i<nb_x(); i++) {
			z[i] = 0.0 ;
        }
	}

    //_______________________________________________________________________

    ConstantMapping::ConstantMapping( unsigned int nb ) 
    	: Function(nb, 0, nb) {
    
    }
    
    void ConstantMapping::eval(bool do_f, bool do_g, bool do_H) {
        /* 
    		Creates a direct mapping of the input parameters to the output
    	*/
        if(do_f) {
            for(unsigned int i=0; i<nb_f(); i++) {
                f(i) = p(i) ;
            }
        }
        if(do_g) {
            clear_g();
        }
        if(do_H) {
			clear_h();
        }
    }

	void ConstantMapping::mul_hessien( int k, const double* y, double* z ){
		// A constant function has a hessien with all coefficients equal to zero.
		for(unsigned int i=0; i<nb_x(); i++) {
			z[i] = 0.0 ;
        }
	}

    //_______________________________________________________________________

    CompositeFunction::CompositeFunction( 
        Function* F, const std::vector<Function_var>& G
    ) : Function(
        	F->nb_f(),
        	sum_nb_x(G),
        	F->nb_p() + sum_nb_p(G)
    	), 
        F_(F), G_((size_t)G.size()) {
        
        // all outputs of the G functions are the input args of F so their
        // number must match
        gx_assert(sum_nb_f(G) == F->nb_x()) ;
        
        for(unsigned int i=0; i<G.size(); i++) {
            G_(i) = G[i] ;
        }
    }
    
    CompositeFunction::~CompositeFunction(){
    	// invalidate children's memory in case they may be used again
    	F_->invalidate_memory();
    	for(unsigned int i=0; i<G_.size(); i++) {
            G_(i)->invalidate_memory();
        }
    }

	void CompositeFunction::manage_memory(
    	double* x_storage,
    	double* p_storage,
    	double* f_storage,
    	double* g_storage,
    	double* h_storage
    	){
    	
    	// if memory is not valid, then make sure we will fix the children's too
    	if(!valid_memory){
			F_->invalidate_memory();
			for(unsigned int i=0; i<G_.size(); i++) {
		        G_(i)->invalidate_memory();
		    }
        }
        
        // manage my memory
    	Function::manage_memory(
    		x_storage,
    		p_storage,
    		f_storage,
    		g_storage,
    		h_storage );
    	
    	/*
    	Manage my children's memory :
    	F.x belongs to F
    	F.p belongs to the composite
    	F.f belongs to the composite
    	F.g belongs to F
    	F.h belongs to F
    	*/
    	F_->manage_memory( nil, this->p_store(), this->f_store() );
    	
    	/*
    	G.x belongs to the composite
    	G.p belongs to the composite
    	G.f belongs to F (F.x)
    	G.g belongs to G
    	G.h belongs to G
    	*/
        unsigned int cur_f = 0 ;
        unsigned int cur_x = 0 ;
        unsigned int cur_p = F_->nb_p() ;
        for(unsigned int i=0; i<G_.size(); i++) {
            G_(i)->manage_memory(
            			this->x_store() + cur_x,
            			this->p_store() + cur_p,
            			  F_->x_store() + cur_f );
            
            cur_f += G_(i)->nb_f() ;
            cur_x += G_(i)->nb_x() ;
            cur_p += G_(i)->nb_p() ;
        }
        gx_assert(cur_f ==   F_->nb_x()) ;
        gx_assert(cur_x == this->nb_x()) ;
        gx_assert(cur_p == this->nb_p()) ;
    }
     
     
    void CompositeFunction::eval(bool do_f, bool do_g, bool do_H) {
        /*
        	To compute f, we need only to eval f in the children
        	To compute g, we need to eval f and g in the children
        	To compute h, we need f, g and h in the children
        	
        	That translates into : 
        	we eval f in the children if we want f, g or h,
        	we eval g in the children if we want g or h
        	we eval h in the children if we want h.
        */
        bool subfunction_do_f = do_f || do_g || do_H;
        bool subfunction_do_g =         do_g || do_H;
        bool subfunction_do_H =                 do_H;
        

        /* 
        	Sub-functions evaluation: we need to eval all the G first, for they
        	set F's input args.
        */
        for(unsigned int i=0; i<G_.size(); i++) {
            G_(i)->eval(subfunction_do_f, subfunction_do_g, subfunction_do_H) ;
        }
        /* Evaluating F sets the result of the evaluation of the compose's f */
        F_->eval(subfunction_do_f, subfunction_do_g, subfunction_do_H) ;
        
        
        /*
        	Then we evaluate the gradient of the composite function.
        	
        	We perform the matrix product between the gradient matrices of 
        	F and G, that we will call respectively Jf and Jg.
        	
        		Jcompo = Jf x Jg
        	
        	As G is in fact divided in many functions G[i], Jg is the block
        	triangular matrix where each block is the gradient matrix Jg_i of 
        	the Function G[i].
        	      _                   _
        	Jg = | Jg_1   0  ...   0   |
        	     |  0   Jg_2 `-.   :   |
        	     |  :   `-.  `-.   0   |
        		 |_ 0   ...   0  Jg_n _|
        		 
        	As we don't want to use most of our time multiplying by zero, we
        	first iterate on the G[i] functions to treat each block.
        	
        	The block Jg_i is the gradient of G[i], so it is a 
        	G[i].nb_f()  x  G[i].nb_x()  matrix. We have to iterate on each
        	element, and we use the variable k for the rows and l for the
        	columns. With the variables cur_Fx and cur_x, we keep track of the
        	offset of Jg_i in Jg, so that you always have :
        	
        		Jg_i(k,l) == Jg(k+cur_Fx, l+cur_x)
        		cur_x = sum for z<i of G[z].nb_x() 
        	    cur_Fx = sum for z<i of G[z].nb_f()
        	
        	Basically, it means that the position of the first element of the
        	Jg_i block you are currently using in Jg is (cur_Fx, cur_x). 
        	
        	               cur_x 
        	                 |
        	      _          v             _
        	Jg = | Jg_1                     |       ^
        	     |      `-.                 |       |
      cur_Fx ->  |          Jg_i            |    G.nb_f()
        		 |               `-.        |       |
			     |_                   Jg_n _|       v
                 
                  <------- G.nb_x() ------->

        Once we know that, we can perform the matrix multiplication.
        	
        	J_compo(J, L) = sum for  K<G.nb_f()  of (
        											  Jf(J, K) * Jg(K, L)
        											)
        
        which becomes, using J = j, L = cur_x+l, K = cur_Fx+k:
        	
        	J_compo(j, cur_x + l) = sum for cur_Fx+k<G.nb_f() of (
        	                           Jf(j, cur_Fx+k) * Jg(cur_Fx+k, cur_x+l)
        	                        )
        	
        and finally, considering Jg_i instead of Jg :
        	
        	J_compo(j, cur_x + l) = sum for k<G[i].nb_f() of (
        							   Jf(j, cur_Fx+k) * Jg_i(k, l)
        							)
        	
        The three last loops are permutable, but we organised it this way to
        save a few recalculations.
        */
        
        if(do_g) {
            clear_g() ;
            
            unsigned int cur_x = 0 ;
            unsigned int cur_Fx = 0 ;                   
            for(unsigned int i=0; i<G_.size(); i++) {
                Function* Gi = G_(i) ;
                for(unsigned int j=0; j<F_->nb_f(); j++){
                    for(unsigned int k=0; k<Gi->nb_f(); k++) {
                        double dFdG = F_->g(j, cur_Fx + k) ;
                        for(unsigned int l=0; l<Gi->nb_x(); l++) {
                            double dGdx = Gi->g(k,l) ;
                            g(j,cur_x + l) += dFdG * dGdx ;
                        }
                    }
                }
                cur_Fx += Gi->nb_f() ;
                cur_x += Gi->nb_x() ;
            } ;
            
            gx_debug_assert(cur_x == nb_x()) ;
            gx_debug_assert(cur_Fx == F_->nb_x()) ;
            
        }
        
        
        /*
        	Finally, we evaluate the hessian of the composed function.
        	The Hessian can be seen as the sum of two matrix operations.
        	
        	Hcompo is like a pile of Hessian matrices, where Hcompo(i) is the
        	Hessian matrix related to the i^th function output of the composite.
        
        	Hcompo(i,j,k) = H1(i,j,k) + H2(i,j,k)
        	
        	H1(i,j,k) = 	sum for w<nb_f() of ( Jf(i,w) * Hg(w,j,k) )
        	H2(i,j,k) = 	sum for x<nb_f() of (
        							sum for y<nb_f() of (
        								Hf(i,x,y)  *  Jg(x,j) * Jg(y,k)
        							)
        						)
        	
        	For H1:
        		For the first part of the equation, we sum the matrices
        		resulting from the multiplication of the Hessian matrix 
        		Hg(k) with the scalar Jf(i,k) to obtain the 
        		H1(i) matrix, and we do it for each function output i.
				
				See the documentation for a graphical representation that might
				clarify the organisation of Hg.
				
				We can see Hg(w) as a matrix where the only non-zero block is
				an Hessian matrix of Hg_i, positioned at (cur_x1,cur_x2).
				We have:
				
					Hg(w,x,t) = Hg(curFx+k, cur_x1+l, cur_x2+m )
        	    			  = G[i]->h(k,l,m)
        	    	cur_Fx = sum for z<i of G[z].nb_f() // first dimension
					cur_x1 = sum for z<i of G[z].nb_x() // second dimension
        	    	cur_x2 = sum for z<i of G[z].nb_x() // third dimension
				
				Then the principle is simlar to the one used to calculate
				J_compo.
        		
        	For H2:
        		For the second part, we just perform two matrix multiplications.
        		
        		As Jg is still a block function, and this time it apears twice
        		in the equation (including once as a transpose), we have to do
        		the same trick as before. We use temporary variables cur_X1 and
        		cur_FX1 for the transpose version (that is pointed to by Gy->g)
        		and cur_X2 and cur_FX2 for the original (pointed to by Gz->g).
        	
        		Those temporary variables are managed independantly from the
        		ones used for H1 for clarity, even if some are always equal.
			        	
        */
        
        if (do_H){
			clear_h();
            
            // accumulators for Hcompo_1
            // we have cur_x1 == cur_x2, but we keep it for code clarity
            unsigned int cur_x1, cur_x2;
            unsigned int cur_Fx;
            
            // accumulators for Hcompo_2
            // we have cur_X1 == cur_x1 too, but, again...
            unsigned int cur_X1, cur_X2;
            unsigned int cur_FX1, cur_FX2;
            
            for(unsigned int w = 0; w < F_->nb_f(); w++ ){
                cur_x1 = 0 ;
                cur_x2 = 0 ;
                cur_Fx = 0 ;
                
                cur_X1 = 0 ;
                cur_FX1 = 0 ;
                
                for(unsigned int y=0; y<G_.size(); y++) {
                    Function* Gy = G_(y) ;
                    
                    // first part of H computing : Sum(k, Jf(w,k) * Hx(k,i,j))
                    for(unsigned int k = 0; k < Gy->nb_f(); k++){
                        double dFdG = F_->g(w,cur_Fx + k);
                        for(unsigned int l = 0; l < Gy->nb_x(); l++ ){
                            for(unsigned int m = 0; m < Gy->nb_x(); m++ ){
                                h(w, cur_x1 + l, cur_x2 + m ) += Gy->h(k,l,m)*dFdG;
                            }
                        }
                    }
                    cur_Fx += Gy->nb_f();
                    cur_x1 += Gy->nb_x() ;
                    cur_x2 += Gy->nb_x() ;
                    
                    // second part of h computing : sum(k, sum(l, Hf(w,k,l)*Jx(k,i)*Jx(l,j)  ))
                    cur_X2 = 0 ;
                    cur_FX2 = 0 ;
                    for(unsigned int z=0; z<G_.size(); z++){
                        Function* Gz = G_(z) ;
                        
                        for(unsigned int k = 0; k<Gy->nb_f() ; k++ ){
                            for(unsigned int l = 0; l<Gz->nb_f() ; l++ ){
                                double dFdG1dG2 = F_->h(w, cur_FX1+k, cur_FX2+l);
                                
                                for(unsigned int i = 0; i < Gy->nb_x(); i++){
                                    double dGdx1 = Gy->g(k,i);
                                    for(unsigned int j= 0; j< Gz->nb_x(); j++){
                                        h(w, cur_X1+i, cur_X2+j ) += dGdx1 * Gz->g(l,j) * dFdG1dG2 ;
                                    }
                                }
                                
                            }
                        }
                        cur_FX2 += Gz->nb_f();
                        cur_X2 += Gz->nb_x();
                    }
                    
                    cur_FX1 += Gy->nb_f();
                    cur_X1 += Gy->nb_x();
                    
                }
            }
            
        }
    }

	void CompositeFunction::mul_hessien( int k, const double* y, double* z ){
		gx_assert(false);
		// TODO
	}

    unsigned int CompositeFunction::sum_nb_f(const std::vector<Function_var>& G) {
        unsigned int result = 0 ;
        for(unsigned int i=0; i<G.size(); i++) {
            result += G[i]->nb_f() ;
        }
        return result ;
    }
     
    unsigned int CompositeFunction::sum_nb_x(const std::vector<Function_var>& G) {
        unsigned int result = 0 ;
        for(unsigned int i=0; i<G.size(); i++) {
            result += G[i]->nb_x() ;
        }
        return result ;
    }
    
    unsigned int CompositeFunction::sum_nb_p(const std::vector<Function_var>& G) {
        unsigned int result = 0 ;
        for(unsigned int i=0; i<G.size(); i++) {
            result += G[i]->nb_p() ;
        }
        return result ;
    }
    


}
