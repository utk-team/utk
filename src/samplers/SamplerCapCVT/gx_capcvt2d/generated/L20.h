#include <Geex/symbolics/function.h>
namespace Geex {

   class L20: public Function {
      public:
      L20();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
