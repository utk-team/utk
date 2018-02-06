#include <Geex/symbolics/function.h>
namespace Geex {

   class L16: public Function {
      public:
      L16();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
