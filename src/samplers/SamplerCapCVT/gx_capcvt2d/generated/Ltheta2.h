#include <Geex/symbolics/function.h>
namespace Geex {

   class Ltheta2: public Function {
      public:
      Ltheta2();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
