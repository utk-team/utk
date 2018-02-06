#include <Geex/symbolics/function.h>
namespace Geex {

   class P1: public Function {
      public:
      P1();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
