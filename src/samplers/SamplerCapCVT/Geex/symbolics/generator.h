#ifdef GX_GINAC
#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <string>
#include <Geex/symbolics/expression.h>

namespace Geex {
    namespace Symbolic {

        class Generator {
        public:
            Generator(
                const std::string& name,
                const std::vector<Expression>& formula,
                const std::vector<Expression>& args,
                const std::vector<Expression>& params
            ) ;
            void generate() ;
            void generate_h() ;
            void generate_cpp() ;
        private:
            std::string name_ ;
            const std::vector<Expression>& formula_ ;
            const std::vector<Expression>& args_ ;
            const std::vector<Expression>& params_ ;
        } ;

    }
}

#endif 
#endif
