#include <Geex/symbolics/function.h>
namespace Geex {

   class Ltheta4: public Function {
      public:
      Ltheta4();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
