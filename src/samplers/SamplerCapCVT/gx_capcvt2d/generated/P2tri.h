#include <Geex/symbolics/function.h>
namespace Geex {

   class P2tri: public Function {
      public:
      P2tri();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
