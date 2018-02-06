#include <Geex/symbolics/function.h>
namespace Geex {

   class P2tet: public Function {
      public:
      P2tet();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
