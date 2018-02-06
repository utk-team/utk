#include <Geex/symbolics/function.h>
namespace Geex {

   class Ltheta_rho12: public Function {
      public:
      Ltheta_rho12();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
