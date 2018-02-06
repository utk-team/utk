#include <Geex/symbolics/function.h>
namespace Geex {

   class L2: public Function {
      public:
      L2();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
