#include <Geex/symbolics/function.h>
namespace Geex {

   class L18: public Function {
      public:
      L18();
      virtual void eval(bool do_f, bool do_g, bool do_H) ;
   };
}
