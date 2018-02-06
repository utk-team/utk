#include <Geex/symbolics/function.h>
namespace Geex {

   class L10: public Function {
      public:
      L10();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
