#include <Geex/symbolics/function.h>
namespace Geex {

   class L4: public Function {
      public:
      L4();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
