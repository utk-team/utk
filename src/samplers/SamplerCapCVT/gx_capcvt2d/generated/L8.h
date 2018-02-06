#include <Geex/symbolics/function.h>
namespace Geex {

   class L8: public Function {
      public:
      L8();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
