#include <Geex/symbolics/function.h>
namespace Geex {

   class L12: public Function {
      public:
      L12();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
