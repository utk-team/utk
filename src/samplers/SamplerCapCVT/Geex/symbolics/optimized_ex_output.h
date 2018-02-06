#ifdef GX_GINAC

/***************************************************************************
                          optimized_ex_output.h  -  description
                             -------------------
    begin                : ven jun 7 2002
    copyright            : (C) 2002 by hleclerc
    email                : hleclerc@lmamef23
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef GX_GINAC
#ifndef OPTIMIZED_EX_OUTPUT_H
#define OPTIMIZED_EX_OUTPUT_H

#include <ginac/ginac.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>

/**
  * Allow to write sets of GiNaC expressions in optimized C format.
  * As example,
  *  symbol x ("x"), y ("y"), z ("z");
  *  ex A = sin (x * y * z*z) + x * x * y * z;
  *  ex B = x*x*y*y*z;
  *  Optimized_ex_output aw;
  *  aw.add ("A", A);
  *  aw.add ("B", B);
  *  aw.write (cout);
  *  cout << endl;
  *    
  *    gives
  *    
  *  double tmp0 = z;
  *  double tmp1 = y;
  *  double tmp2 = x;
  *  double tmp_gcd9 = tmp2*tmp1;
  *  double tmp_gcd8 = tmp_gcd9*tmp0;
  *  double A = tmp2*tmp_gcd8+sin(tmp0*tmp_gcd8);
  *  double B = tmp_gcd9*tmp_gcd8;
  *  
  *  In fact, it factorize expressions to allow compiler to make optimized code in a simple way.
  *@author hleclerc
  */
class Optimized_ex_output {
public: 
  /// num_type will be used for temporary variables and results
  Optimized_ex_output(const std::string &num_type="double");
  ~Optimized_ex_output();
  /// add the expression exp in the list of ex to be written
  void add (const std::string & s, const GiNaC::ex & exp);
  /** subs_tmp_use_one_time==false gives (for example)
   * A z*y*x^2+sin(z^2*y*x)
   * B z*y^2*x^2
   * double tmp0 = z;
   * double tmp1 = y;
   * double tmp2 = x;
   * double tmp_gcd9 = tmp2*tmp1;
   * double tmp_gcd8 = tmp_gcd9*tmp0;
   * double tmp3 = tmp2*tmp_gcd8;
   * double tmp4 = tmp0*tmp_gcd8;
   * double tmp5 = sin(tmp4);
   * double tmp6 = tmp3+tmp5;
   * double tmp7 = tmp_gcd9*tmp_gcd8;
   * double A = tmp6;
   * double B = tmp7;
   * 
   * subs_tmp_use_one_time==true gives
   * double tmp0 = z;
   * double tmp1 = y;
   * double tmp2 = x;
   * double tmp_gcd9 = tmp2*tmp1;
   * double tmp_gcd8 = tmp_gcd9*tmp0;
   * double A = tmp2*tmp_gcd8+sin(tmp0*tmp_gcd8);
   * double B = tmp_gcd9*tmp_gcd8;   
   * 
   * the execution time is the same but it is more eye-candy. The counter-part is the pre-calcul time
  */
    void write(std::ostream &os,unsigned nb_tabs=0,bool subs_tmp_used_one_time=true);
protected:
  /// data on temporary variables 
  struct Car_subs {
    Car_subs();
    ///
    Car_subs(const std::string &str);
    /// tmp...
    GiNaC::ex symb;
    /// times the tmp... is used
    unsigned int count;
    /// to satisfy the dependances (tm3=tmp2+1 depends on tmp2...)
    GiNaC::ex order;
  };
  /// type of subsitution list
  typedef std::map < GiNaC::ex, Car_subs, GiNaC::ex_is_less > TSubs;
  /// substitutions[expr]={symbol,counter of utilizations, order of the declaration, ...}
  TSubs substitutions;
  /// ar is the archive of expressions to be printed
  std::vector < std::pair < std::string, GiNaC::ex > > ar;
  
  /// @see find_gcd()
  class Cost_mapper;
  /// @see find_gcd()
  struct Carac_gcd;
  /// look for expressions which have the gcd!=1. If found, creation of a temporary variable, division and return true.
  /// else, return false.
  bool find_gcd(const GiNaC::ex &sub_order);
  
  
  /// used for ex.map(...). @see write()
  class Mapper;
  friend class Mapper;
  /// used to sort temporary variables. @see write()
  struct Cmp_order;
  
  /// example : "double" or "float", ... used to declare temporary variables and results
  std::string numeric_type;
};

#endif
#endif

#endif
