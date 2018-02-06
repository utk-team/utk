#include <Geex/symbolics/function.h>
namespace Geex {

   class L: public Function {
      public:
      L();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
