#include <Geex/symbolics/function.h>
namespace Geex {

   class P0: public Function {
      public:
      P0();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
