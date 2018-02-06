#include <Geex/symbolics/function.h>
namespace Geex {

   class Ltheta16: public Function {
      public:
      Ltheta16();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
