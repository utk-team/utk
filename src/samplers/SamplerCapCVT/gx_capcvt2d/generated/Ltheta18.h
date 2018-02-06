#include <Geex/symbolics/function.h>
namespace Geex {

   class Ltheta18: public Function {
      public:
      Ltheta18();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
