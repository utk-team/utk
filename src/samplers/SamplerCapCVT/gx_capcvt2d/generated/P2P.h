#include <Geex/symbolics/function.h>
namespace Geex {

   class P2P: public Function {
      public:
      P2P();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
