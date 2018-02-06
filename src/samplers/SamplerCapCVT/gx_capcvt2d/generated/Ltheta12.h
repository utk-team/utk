#include <Geex/symbolics/function.h>
namespace Geex {

   class Ltheta12: public Function {
      public:
      Ltheta12();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
