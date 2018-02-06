#include <Geex/symbolics/function.h>
namespace Geex {

   class L6: public Function {
      public:
      L6();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
