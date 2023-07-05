#include "fmg.hh"
#include <iostream>

using namespace std;

/* should begin with init guess set in high level and forces (non-homogeneous term) */ 
void Field_MGN(TensorField *tf, int iters, int ncycle) {

  int depth = tf->get_depth() ;

  tf->set_init_guess() ;
  
  //for(int up_to_level = depth - 1 ; up_to_level > 0 ; up_to_level--) {

  int up_to_level = depth - 1 ;

   for(int nc = 0 ; nc < ncycle ; nc++) {
    
    /* Going down */

    for(int level = 0 ; level < up_to_level ; level ++) {

      for(int i = 0 ; i < iters ; i++) {
	tf->smooth(level) ; // error, residual 
      }

      tf->calc_next_level_residual(level) ; // calc next level resudual by restricting the current residual & set init guess is 0)

      tf->zero_next_level(level) ;  // Next level's init guess
    }

    /* End of goind down */
    
    for(int i = 0 ; i < iters * 50 ; i++) {
      tf->smooth(up_to_level) ; // error, residual 
    }
    
    /* Going up*/
    
    for(int level = up_to_level-1 ; level >= 0 ; level --) {

      tf->add_prolonged_prev_level(level) ;
	
      for(int i = 0 ; i < iters ; i++) {
	tf->smooth(level) ; // error, residual 
      }
    }

    /* End of going up */
   }
   
}

void Field_TMG(TensorField *tf, int iters, int ncycle) {
  cout << "Starting Full V-Cycle MultiGrid..." << endl ;

  int depth = tf->get_depth() ;

  tf->set_init_guess() ;
  
  for(int up_to_level = depth - 1 ; up_to_level > 0 ; up_to_level--) {

    // int up_to_level = depth - 1 ;

    for(int nc = 0 ; nc < ncycle ; nc++) {
    
    /* Going down */

    for(int level = 0 ; level < up_to_level ; level ++) {

      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "\\\t" ;
  
      cout << "[" << tf->level_len_x(level) << "," << tf->level_len_y(level) << "]\t" ;

      for(int i = 0 ; i < iters ; i++) {
	tf->smooth(level) ; // error, residual 
      }

      tf->calc_next_level_residual(level) ; // calc next level resudual by restricting the current residual & set init guess is 0)

      tf->zero_next_level(level) ;  // Next level's init guess

      cout << endl ;
    }

    /* End of goind down */
    
    for(int i = 0 ; i < up_to_level ; i++)
      cout << ' ' ;
    cout << ">\t" ;

    cout << "[" << tf->level_len_x(up_to_level) << "," << tf->level_len_y(up_to_level) << "]\t" ;
    
    for(int i = 0 ; i < iters ; i++) {
      tf->smooth(up_to_level) ; // error, residual 
    }
    
    cout << endl ;
    
    /* Going up*/
    
    for(int level = up_to_level-1 ; level >= 0 ; level --) {
      
      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "/\t" ;
      
      cout << "[" << tf->level_len_x(level) << "," << tf->level_len_y(level) << "]\t" ;

      tf->add_prolonged_prev_level(level) ;
	
      for(int i = 0 ; i < iters ; i++) {
	tf->smooth(level) ; // error, residual 
      }
      
      cout << endl ;
    }

    /* End of going up */
    }
  }
}
void Field_MG(TensorField *tf, int iters, int ncycle) {
  cout << "Starting Full V-Cycle MultiGrid..." << endl ;

  int depth = tf->get_depth() ;

  tf->set_init_guess() ;
  
  //for(int up_to_level = depth - 1 ; up_to_level > 0 ; up_to_level--) {

  int up_to_level = depth - 1 ;

    for(int nc = 0 ; nc < ncycle ; nc++) {
    
    /* Going down */

    for(int level = 0 ; level < up_to_level ; level ++) {

      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "\\\t" ;
  
      cout << "[" << tf->level_len_x(level) << "," << tf->level_len_y(level) << "]\t" ;

      for(int i = 0 ; i < iters ; i++) {
	tf->smooth(level) ; // error, residual 
      }

      tf->calc_next_level_residual(level) ; // calc next level resudual by restricting the current residual & set init guess is 0)

      tf->zero_next_level(level) ;  // Next level's init guess

      cout << endl ;
    }

    /* End of goind down */
    
    for(int i = 0 ; i < up_to_level ; i++)
      cout << ' ' ;
    cout << ">\t" ;

    cout << "[" << tf->level_len_x(up_to_level) << "," << tf->level_len_y(up_to_level) << "]\t" ;
    
    for(int i = 0 ; i < iters ; i++) {
      tf->smooth(up_to_level) ; // error, residual 
    }
    
    cout << endl ;
    
    /* Going up*/
    
    for(int level = up_to_level-1 ; level >= 0 ; level --) {
      
      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "/\t" ;
      
      cout << "[" << tf->level_len_x(level) << "," << tf->level_len_y(level) << "]\t" ;

      tf->add_prolonged_prev_level(level) ;
	
      for(int i = 0 ; i < iters ; i++) {
	tf->smooth(level) ; // error, residual 
      }
      
      cout << endl ;
    }

    /* End of going up */
    }
    //  }
}

void Field_FMG(TensorField *tf, int iters, int ncycle) {
  cout << "Starting Full V-Cycle MultiGrid..." << endl ;

  int depth = tf->get_depth() ;

  tf->set_init_guess() ;

  for(int ii = 0 ; ii < 3 ; ii++){
  for(int level = 0 ; level < depth-1 ; level ++) {
    
    for(int i = 0 ; i < level ; i++)
      cout << ' ' ;
    cout << "\\\t" ;
    
    cout << "[" << tf->level_len_x(level) << "," << tf->level_len_y(level) << "]\t" ;
    
    for(int i = 0 ; i < iters ; i++) {
      tf->smooth(level) ; // error, residual 
    }
    
    tf->calc_next_level_residual(level) ; // calc next level resudual by restricting the current residual & set init guess is 0)
    
    tf->zero_next_level(level) ;  // Next level's init guess
    
    cout << endl ;
  }

  
  for(int up_to_level = depth - 2 ; up_to_level >= 0 ; up_to_level--) 
    {
      		
    for(int nc = 0 ; nc < ncycle ; nc++) {
    
      for(int level = up_to_level+1 ; level < depth-1 ; level++) { 

	/* Going down */
	
      for(int i = 0 ; i < level ; i++)
	cout << ' ' ;
      cout << "\\\t" ;
  
      cout << "[" << tf->level_len_x(level) << "," << tf->level_len_y(level) << "]\t" ;

      for(int i = 0 ; i < iters ; i++) {
	tf->smooth(level) ; // error, residual 
      }

      tf->calc_next_level_residual(level) ; // calc next level resudual by restricting the current residual & set init guess is 0)

      tf->zero_next_level(level) ;  // Next level's init guess

      cout << endl ;
    }

    /* End of goind down */
    
    for(int i = 0 ; i < depth-1 ; i++)
      cout << ' ' ;
    cout << ">\t" ;

    cout << "[" << tf->level_len_x(depth-1) << "," << tf->level_len_y(depth-1) << "]\t" ;
    
    for(int i = 0 ; i < iters ; i++) {
      tf->smooth(depth-1) ; // error, residual 
    }
    
    cout << endl ;
    
    /* Going up*/
    
    for(int l = depth-2 ; l > up_to_level ; l--) {
      
      for(int i = 0 ; i < l ; i++)
	cout << ' ' ;
      cout << "/\t" ;
      
      cout << "[" << tf->level_len_x(l) << "," << tf->level_len_y(l) << "]\t" ;

      tf->add_prolonged_prev_level(l) ;
	
      for(int i = 0 ; i < iters ; i++) {
	tf->smooth(l) ; // error, residual 
      }
      
      cout << endl ;
    }
    
    /* End of going up */
    }

    int  l = up_to_level ;
    for(int i = 0 ; i < l ; i++)
      cout << ' ' ;
    cout << "/\t" ;
    
    cout << "[" << tf->level_len_x(l) << "," << tf->level_len_y(l) << "]\t" ;
    
    tf->add_prolonged_prev_level(l) ;
    
    for(int i = 0 ; i < iters ; i++) {
      tf->smooth(l) ; // error, residual 
    }
    
    cout << endl ;
    }
  }
}
