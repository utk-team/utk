#ifdef GX_GINAC

/***************************************************************************
                          optimized_ex_output.cpp  -  description
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
#include "optimized_ex_output.h"
#include <set>
#include <float.h>
//#include <strstream>
#include <sstream>


using namespace GiNaC ;
using namespace std ;

// -----------------------------------------------------------------------------
Optimized_ex_output::Optimized_ex_output(const string &num_type):numeric_type(num_type) {
}

// -----------------------------------------------------------------------------
Optimized_ex_output::~Optimized_ex_output() {
}

// -----------------------------------------------------------------------------
void Optimized_ex_output::add (const string & s, const GiNaC::ex & exp) {
  ar.push_back (make_pair (s, exp));
}


// -----------------------------------------------------------------------------
Optimized_ex_output::Car_subs::Car_subs():count(0) {}
Optimized_ex_output::Car_subs::Car_subs(const string &str):symb(symbol(str)),count(0) {}

// -----------------------------------------------------------------------------
/// a map_function to expand expressions
class Optimized_ex_output::Mapper : public GiNaC::map_function {
public:
  Mapper() { cpt=0; }
  ex operator()(const ex &expr) {
    if (is_exactly_a<numeric>(expr))
      return expr;
        
    ex new_expr;
      
    if (is_exactly_a<power>(expr) && is_exactly_a<numeric>(expr.op(1)) && 
            ex_to<numeric>(expr.op(1)).is_integer() && (expr.op(1)!=1) && (expr.op(1)!=-1) ) {
      if (expr.op(1)>1)
        return expr.map(*this);
      else {
      	new_expr=new_subs(expr.op(0).map(*this),false);
      	new_expr=new_subs(pow(new_expr,-1),false);
		new_expr=pow(new_expr,-expr.op(1));
      }
    }
    else
      new_expr=expr.map(*this);

    return new_subs(new_expr);
  }
  ex new_subs(const ex &sub_ex,bool new_count=true) {
    TSubs::iterator iter=w->substitutions.find(sub_ex);
    if (iter==w->substitutions.end()) {
        ostringstream s;
        s << "tmp_" << ++cpt ; // << ends;
      Car_subs cs(s.str());
      cs.order=cpt;
      w->substitutions[sub_ex]=cs;
      return cs.symb;
    }
    iter->second.count+=new_count;
    return iter->second.symb;
  }
  Optimized_ex_output *w;
  unsigned int cpt;
};

// -----------------------------------------------------------------------------
class Optimized_ex_output::Cost_mapper : public GiNaC::map_function {
public:
  Cost_mapper():res(-1.0) {}
  ex operator()(const ex &expr) {
    if (!is_exactly_a<numeric>(expr))
      res+=1.0;      
    return expr.map(*this);
  }
  double res;
};

// -----------------------------------------------------------------------------
struct Optimized_ex_output::Carac_gcd {
  Carac_gcd():cost(0.0),order_min(pow(ex(2),100)) {}
  typedef set<GiNaC::ex,GiNaC::ex_is_less> Tset_ex;
  Tset_ex lst_ex;
  double cost;
  ex order_min;
};



// -----------------------------------------------------------------------------
static bool good_for_gcd(const ex &expr) {
/*
  if (is_exactly_a<mul>(expr))
    return true;
  if (is_exactly_a<power>(expr) && is_exactly_a<numeric>(expr.op(1)) && 
         ex_to<numeric>(expr.op(1)).is_integer() && (expr.op(1)>1) )
    return true;
*/
  return false ;
}

// -----------------------------------------------------------------------------
bool Optimized_ex_output::find_gcd(const ex &sub_order) {
  // we construct a list of common greatest divisor
  typedef map<ex,Carac_gcd,GiNaC::ex_is_less> Tlstgcd;
  Tlstgcd lst_gcd;
  bool have_found_a_gcd_eq_to_tmp=false;
  for(TSubs::iterator iter1=substitutions.begin();(iter1!=substitutions.end())&&(have_found_a_gcd_eq_to_tmp==false);++iter1) {
    if ( ! good_for_gcd(iter1->first) )
      continue;
    for(TSubs::iterator iter2=substitutions.begin();iter2!=iter1;++iter2) {
      if ( ! good_for_gcd(iter2->first) )
          continue;

      ex g=gcd(iter1->first,iter2->first);
      if ((g==1)||is_exactly_a<symbol>(g))
        continue;
      Carac_gcd &cg=lst_gcd[g];
      if (cg.lst_ex.size()==0) {
        Cost_mapper cm;
        cm(g);
        cg.cost=cm.res;
      }
      cg.lst_ex.insert(iter1->first);
      cg.lst_ex.insert(iter2->first);
      cg.order_min=min(cg.order_min,iter1->second.order);
      cg.order_min=min(cg.order_min,iter2->second.order);
        
      // if gcd==tmp_...
      if (g==iter1->first) {
        have_found_a_gcd_eq_to_tmp=true;
        cg.cost=DBL_MAX;
      }
    }
  }
  if (lst_gcd.size()==0)
    return false;

  // we search the higher cost
  Tlstgcd::iterator iter_max=lst_gcd.begin();
  Tlstgcd::iterator iter_gcd=iter_max;
  ++iter_gcd;
  for(;iter_gcd!=lst_gcd.end();++iter_gcd)
    if (iter_gcd->second.cost*(iter_gcd->second.lst_ex.size()-1)>iter_max->second.cost*(iter_max->second.lst_ex.size()-1))
      iter_max=iter_gcd;

  // 
  static unsigned int cpt=0;
  ostringstream s;
  s << "tmp_gcd_" << cpt++ ; // << ends;
  Car_subs cs(s.str());
  cs.order=iter_max->second.order_min-sub_order;
  cs.count=iter_max->second.lst_ex.size();
  for(Carac_gcd::Tset_ex::iterator iter_ex=iter_max->second.lst_ex.begin();iter_ex!=iter_max->second.lst_ex.end();++iter_ex) {
    substitutions[ *iter_ex / iter_max->first * cs.symb ]=substitutions[ *iter_ex ];
    substitutions.erase( *iter_ex );
  }
  substitutions[ iter_max->first ]=cs;
  return true;
}

///
struct Optimized_ex_output::Cmp_order {
  typedef pair<GiNaC::ex, Car_subs> Telem;
  bool operator()(const Telem &e1,const Telem &e2) { return e1.second.order<e2.second.order; }
};

///
void Optimized_ex_output::write(std::ostream &os,unsigned nb_tabs,bool subs_tmp_used_one_time) {
  try {
  /// subsitutions
  Mapper my_map;
  my_map.w=this;

  vector<GiNaC::ex> res;
  for (unsigned cpt = 0; cpt < ar.size (); ++cpt)
    res.push_back(my_map(ar[cpt].second));
    
  /// gcd
  ex sub_find_gcd=1;
  while (find_gcd(sub_find_gcd/=2));
    
  /// print    
  typedef set<Cmp_order::Telem,Cmp_order> Tset;
  Tset vec;
  for(TSubs::const_iterator iter=substitutions.begin();iter!=substitutions.end();++iter)
    vec.insert(Cmp_order::Telem(iter->first,iter->second));

  lst lst_subs;
  for(Tset::const_iterator iter=vec.begin();iter!=vec.end();++iter) {
    if ( subs_tmp_used_one_time && (iter->second.count==0) )
      lst_subs.append(iter->second.symb==iter->first.subs(lst_subs));
    else {
      for(unsigned n=0;n<nb_tabs;++n)
        os << "\t";
      os << numeric_type << " " << iter->second.symb << " = ";
      iter->first.subs(lst_subs).print(print_csrc_double(os));
      os << ";";
      //os << "// " << iter->second.count;
	  os << endl;
//      os << "gx_assert(!is_nan(" << iter->second.symb << "));" << std::endl ;
    }
  }
      
  for (unsigned cpt = 0; cpt < ar.size (); ++cpt) {
    for(unsigned n=0;n<nb_tabs;++n)
      os << "\t";
    os << numeric_type << " " << ar[cpt].first << " = ";
    res[cpt].subs(lst_subs).print(print_csrc_double(os));
    os << ";" << endl;
//    os << "gx_assert(!is_nan(" << ar[cpt].first << "));" << std::endl ;
  }
  
  } catch (std::exception &e) { cerr << e.what() << endl; }
}

#endif

#endif
