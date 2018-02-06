#include <Geex/symbolics/function.h>
namespace Geex {

   class P2: public Function {
      public:
      P2();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
