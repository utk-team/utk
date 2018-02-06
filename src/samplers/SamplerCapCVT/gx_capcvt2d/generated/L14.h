#include <Geex/symbolics/function.h>
namespace Geex {

   class L14: public Function {
      public:
      L14();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
