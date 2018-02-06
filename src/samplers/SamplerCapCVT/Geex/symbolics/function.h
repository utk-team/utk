#ifndef __GEEX_FUNCTION__
#define __GEEX_FUNCTION__

#include <Geex/basics/counted.h>
#include <Geex/basics/types.h>
#include <Geex/basics/arrays.h>
#include <typeinfo>

//// Deactivate code generation for Windows,
//// compilation is too slow
//#ifndef FORCE_SYMGEN
//#ifdef WIN32
//#define NO_SYMGEN
//#endif
//#endif

namespace Geex {
    
    using Numeric::is_nan ;
   
    /** Represents a function. Its value, gradient and Hessian can be evaluated. */
    class GEEX_API Function : public Counted {
    public:
    
        /**  Constructur of a function from |R^x into |R^f.
             @param nb_f number of output values.
             @param nb_x number of input arguments.
             @param nb_p number of input fixed parameters.
        */
        Function(unsigned int nb_f, unsigned int nb_x, unsigned int nb_p ) ;

        /** Function destructor. */
        virtual ~Function() ;
        
        /**
           Evaluates the function, its gradient and its hessien on the given 
           values of x and p.
           @param do_f evaluate the function?
           @param do_g evaluate the gradient of the function?
           @param do_H evaluate the hessien of the function?
        */
        virtual void eval(bool do_f=true, bool do_g=true, bool do_H = false) = 0 ;


        /*
          Hook. Computes the result of the multiplication of the hessien evaluated on x
          by the vector y. x and p must have been set before calling mul_hessien.
          @param k index of the considered output (ake the considered sub-function).
          @param y the vector multiplied by the hessien. Must have nb_x() elements.
          @param z the resulting vector. Must have nb_x() elements.
        */
        virtual void mul_hessien( int k, const double* y, double* z ) {
            gx_assert(false); // NOT YET IMPLEMENTED
        }

		
        /** Gets the number of output values of the function.*/
        unsigned int nb_f() const { return nb_f_ ; }
        /** Gets the number of input arguments.*/
        unsigned int nb_x() const { return nb_x_ ; }
        /** Gets the number of input fixed parameters.*/
        unsigned int nb_p() const { return nb_p_ ; }
        /** Gets the number of output values of the gradient.*/
        unsigned int nb_g() const { return nb_f() * nb_x() ; }
        /** Gets the number of output values of the hessien.*/
        unsigned int nb_h() const { return nb_f() * nb_x() * nb_x() ; }

        /** Acces to a value of the function evaluation.
            @param i index of the value. */
        double& f(unsigned int i) { gx_debug_assert(i < nb_f()) ; return f_[i] ; }
        /** Acces to a value of an input argument.
            @param i index of the value. */
        double& x(unsigned int i) { gx_debug_assert(i < nb_x()) ; return x_[i] ; }
        /** Acces to a value of an input fixed parameter.
            @param i index of the value. */
        double& p(unsigned int i) { gx_debug_assert(i < nb_p()) ; return p_[i] ; }
        /** Acces to a value of the gradient of the function.  
            @param i index of the derivated output.
            @param j index of the argument with respect to wich it is derivated. */
        double& g(unsigned int i, unsigned int j) { 
            gx_debug_assert(i < nb_f() && j < nb_x()) ;
            return g_[i * nb_x() + j] ; 
        }
        /** Acces to a value of the hessien of the function.
            @param i index of the derivated output.
            @param j index of the first argument with respect to wich it is derivated.
            @param k index of the second argument with respect to wich it is derivated. */
        double& h(unsigned int i, unsigned int j, unsigned int k) {
            if(owns_h_) gx_debug_assert(alloc_h_storage) ;
            gx_debug_assert(i < nb_f() && j < nb_x() && k < nb_x()) ;
            return h_[(i * nb_x() + j) * nb_x() + k] ;
        }

        /** Allocs memory to store the input arguments values. */
        void alloc_f() {
            gx_assert(f_ == nil) ; f_ = new double[nb_f()] ; owns_f_ = true ;
        }

        /** Allocs memory to store the output values. */
        void alloc_x() {
            gx_assert(x_ == nil) ; x_ = new double[nb_x()] ; owns_x_ = true ;
        }

        /** Allocs memory to store the input fixed parameters values. */
        void alloc_p() {
            gx_assert(p_ == nil) ; p_ = new double[nb_p()] ; owns_p_ = true ; 
        }

        /** Allocs memory to store the gradient values. */
        void alloc_g() {
            gx_assert(g_ == nil) ; g_ = new double[nb_g()] ; owns_g_ = true ; 
        }

        /** Allocs memory to store the hessien values. */
        void alloc_h() {
            gx_assert(alloc_h_storage);
            gx_assert(h_ == nil) ; h_ = new double[nb_h()] ; owns_h_ = true ;
        }


        /** Sets all gradient values to zero. */
        void clear_g() {
            for(unsigned int i=0; i<nb_g(); i++) {
                g_[i] = 0.0 ;
            }
        }
        
        /** Sets all hessien values to zero. */
        void clear_h() {
            if(owns_h_) gx_assert(alloc_h_storage) ;
            for(unsigned int i=0; i<nb_h(); i++) {
                h_[i] = 0.0 ;
            }
        }
        
        /** Begin setting values of arguments and parameters. */
        void begin_xp() { manage_memory(x_store(),p_store(),f_store(),g_store(),h_store()); cur_x_ = 0 ; cur_p_ = 0 ; }
        /** Begin setting values of arguments. */
        void begin_x() { manage_memory(x_store(),p_store(),f_store(),g_store(),h_store()); cur_x_ = 0 ; }
        /** Begin setting values of parameters. */
        void begin_p() { manage_memory(x_store(),p_store(),f_store(),g_store(),h_store()); cur_p_ = 0 ; }
        /** Ends setting values of arguments and parameters. Performs security checks. */
        void end_xp() { end_x() ; end_p() ; }
        /** Ends setting values of arguments. Performs security checks. */
        void end_x() { gx_debug_assert(cur_x_ == nb_x()) ; }
        /** Ends setting values of parameters. Performs security checks. */
        void end_p() { gx_debug_assert(cur_p_ == nb_p()) ; }
        /** Sets the value of the next argument. Independant from setting value of parameters. */
        void add_x(double x_in) { gx_debug_assert(cur_x_ < nb_x()) ; x(cur_x_) = x_in ; cur_x_++ ; }
        /** Sets the value of the next parameter. Independant of setting values of arguments. */
        void add_p(double p_in) { gx_debug_assert(cur_p_ < nb_p()) ; p(cur_p_) = p_in ; cur_p_++ ; } 
        
        /** Gets the array of output values. */
        double* f_store() { return f_ ; }
        /** Gets the array of input argument values. */
        double* x_store() { return x_ ; }
        /** Gets the array of input fixed parameter values. */
        double* p_store() { return p_ ; }
        /** Gets the array of gradient values. */
        double* g_store() { return g_ ; }
        /** Gets the array of hessien values. */
        double* h_store() { return h_ ; }

        /** Sets the array of output values. */
        void set_f_store(double* ptr) { gx_assert(f_ == nil) ; f_ = ptr ; }
        /** Sets the array of input argument values. */
        void set_x_store(double* ptr) { gx_assert(x_ == nil) ; x_ = ptr ; }
        /** Sets the array of input fixed parameter values. */
        void set_p_store(double* ptr) { gx_assert(p_ == nil) ; p_ = ptr ; }
        /** Sets the array of gradient values. */
        void set_g_store(double* ptr) { gx_assert(g_ == nil) ; g_ = ptr ; }
        /** Sets the array of hessien values. */
        void set_h_store(double* ptr) { gx_assert(h_ == nil) ; h_ = ptr ; }

        /** Prints the function to std::cerr. */
        void print() ;

        /** Permits to inhibate storage allocation for the hessien if you don't plan to use it */
        void set_alloc_h_storage(bool alloc_h){
            /* 
               If we had an allocated array for h and we want to set 
               alloc_h_storage to false, then delete h_. Every other case
               is taken care of by manage_memory().
            */
            if(owns_h_ && alloc_h_storage && !alloc_h && h_ != nil ) { delete[] h_ ; owns_h_ = false ; h_ = nil; }
            alloc_h_storage = alloc_h ;
        }
        bool get_alloc_h_storage(){ return alloc_h_storage ; }

        /** Sets the memory of the input and output, and ensures that they 
            are valid.
            @param x_storage array for the input arguments. Internally allocated if nil.
            @param p_storage array for the input parameters. Internally allocated if nil.
            @param f_storage array for the function output. Internally allocated if nil.
            @param g_storage array for the gradient output. Internally allocated if nil.
            @param h_storage array for the Hessian output. Internally allocated if nil and alloc_h_storage is true (true by default).
        */
        virtual void manage_memory(
            double* x_storage = nil,
            double* p_storage = nil,
            double* f_storage = nil,
            double* g_storage = nil,
            double* h_storage = nil
        );
    	
		
    	/** Invalidates the memory of the compose so that it is cleaned up at 
            the next manage_memory() call. Use only if you want to reset 
            completely the memory allocation. */
    	void invalidate_memory() { valid_memory = false; }

    protected:
        bool valid_memory; // TODO find better name. True if all is ok with the memory allocation. Must pass at false when something changes in the function tree...
        bool alloc_h_storage;	// sometimes, we don't want a storage for h to be allocated
        // so (owns_h_ && !alloc_h_storage) => no valid storage for h
		
    private:
        unsigned int nb_f_ ; 
        unsigned int nb_x_ ;
        unsigned int nb_p_ ;
        double* f_ ;
        double* x_ ;
        double* p_ ;
        double* g_ ;
        double* h_ ;
        bool owns_f_ ;
        bool owns_x_ ;
        bool owns_p_ ;
        bool owns_g_ ;
        bool owns_h_ ;
        unsigned int cur_x_ ;
        unsigned int cur_p_ ;
        
    private:
        Function(const Function& rhs) ;
        Function& operator=(const Function& rhs) ;
    } ;

    /** A counted-reference pointer to the Function. */
    typedef SmartPointer<Function> Function_var ;

    //--------------------------------------------------

    /** An implentation of the identity function with Function. Directly maps
        input arguments to the function output. */
    class GEEX_API Identity : public Function {
    public:
        Identity(unsigned int nb) ;
        virtual void eval(bool do_f=true, bool do_g=true, bool do_H = false) ;
        virtual void mul_hessien( int k, const double* y, double* z );
    } ;

    //--------------------------------------------------

    /** An implentation of a constant function with Function. Directly maps
        input parameters to the function output. */
    class GEEX_API ConstantMapping : public Function {
        ConstantMapping(unsigned int nb_p);
        virtual void eval(bool do_f=true, bool do_g=true, bool do_H = false);
        virtual void mul_hessien( int k, const double* y, double* z );
    } ;
    
    //--------------------------------------------------

    class GEEX_API CompositeFunction : public Function {
    public:
        CompositeFunction(Function* F, const std::vector<Function_var>& G) ;
        virtual ~CompositeFunction() ;
        virtual void eval(bool do_f=true, bool do_g=true, bool do_H=false) ;
        virtual void mul_hessien( int k, const double* y, double* z );
        virtual void manage_memory(
            double* x_storage = nil,
            double* p_storage = nil,
            double* f_storage = nil,
            double* g_storage = nil,
            double* h_storage = nil
        );
        int nb_composed() { return G_.size() ; }
        Function* composed(int i) { return G_[i] ; }
		
    protected:
        static unsigned int sum_nb_f(const std::vector<Function_var>& G) ;
        static unsigned int sum_nb_x(const std::vector<Function_var>& G) ;
        static unsigned int sum_nb_p(const std::vector<Function_var>& G) ;
        
        Function_var F_ ;
        Array1d<Function_var> G_ ;
    } ;

}


#endif
