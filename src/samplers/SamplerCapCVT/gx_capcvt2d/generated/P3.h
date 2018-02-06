#include <Geex/symbolics/function.h>
namespace Geex {

   class P3: public Function {
      public:
      P3();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
