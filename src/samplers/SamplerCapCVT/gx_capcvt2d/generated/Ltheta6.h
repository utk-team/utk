#include <Geex/symbolics/function.h>
namespace Geex {

   class Ltheta6: public Function {
      public:
      Ltheta6();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
