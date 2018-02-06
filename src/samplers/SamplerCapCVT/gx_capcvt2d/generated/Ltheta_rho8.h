#include <Geex/symbolics/function.h>
namespace Geex {

   class Ltheta_rho8: public Function {
      public:
      Ltheta_rho8();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
