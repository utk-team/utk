#ifdef GX_GINAC
#include <Geex/symbolics/generator.h>
#include <Geex/symbolics/optimized_ex_output.h>
#include <fstream>

namespace Geex {
    namespace Symbolic {

        Generator::Generator(
            const std::string& name,
            const std::vector<Expression>& formula,
            const std::vector<Expression>& args,
            const std::vector<Expression>& params
        ) : name_(name), formula_(formula), args_(args), params_(params) {
        }

        void Generator::generate() {
            generate_h() ;
            generate_cpp() ;
        }
        
        void Generator::generate_h() {
            std::ofstream out((name_ + ".h").c_str()) ;

            out << "#include <Geex/symbolics/function.h>" << std::endl ;
            out << "namespace Geex {" << std::endl ;
            out << std::endl ;
            out << "   class " << name_ << ": public Function {" << std::endl ;
            out << "      public:" << std::endl ;
            out << "      " << name_ << "();" << std::endl ; 
            out << "      virtual void eval(bool do_f, bool do_g, bool do_H) ;" << std::endl ;
            out << "   };" << std::endl ;
            out << "}" << std::endl ;
        }
        
        static std::string to_string(int i) {
            static char buff[300] ;
            sprintf(buff, "%d", i) ;
            return std::string(buff) ;
        }

        static void generate_ex(
            const std::string& name, std::ostream& out, const Symbolic::Expression& eqn
        ) {
            Optimized_ex_output aw ;
            aw.add(name, eqn) ;
            aw.write(out) ;
        }


        void Generator::generate_cpp() {
            std::ofstream out((name_ + ".cpp").c_str()) ;


            out << "#include \"" << name_ << ".h\"" << std::endl ;
            out << "#include <Geex/symbolics/const_pow.h>" << std::endl ;
            out << "namespace Geex {" << std::endl ;

            out << "   " << name_ << "::" << name_ << "() : Function(" 
                << formula_.size() << "," << args_.size() << "," << params_.size() << "){}" << std::endl ; 

            
            out << "   void " <<  name_ << "::eval(bool do_f, bool do_g, bool do_H) {" << std::endl ;

            out << "      if(do_f) {" << std::endl ;
            for(unsigned int i=0; i<formula_.size(); i++) {
                out << "{" << std::endl ;
                out << "#ifndef NO_SYMGEN" << std::endl ;
                generate_ex("f" + to_string(i), out, formula_[i]) ;
                out << "f(" << i << ") = f" << i << ";" << std::endl ;
                out << "#endif" << std::endl ;
                out << "}" << std::endl ;
            }
            out << "      }" << std::endl ;


            out << "      if(do_g) {" << std::endl ;
            for(unsigned int i=0; i<formula_.size(); i++) {
                for(unsigned int j=0; j<args_.size(); j++) {
                    out << "{" << std::endl ;
                    out << "#ifndef NO_SYMGEN" << std::endl ;
                    generate_ex("g" + to_string(i) + "_" + to_string(j), out, formula_[i].diff(ex_to<symbol>(args_[j]))) ;
                    out << "g(" << i << "," << j << ") = g" << i << "_" << j << ";" << std::endl ;
                    out << "#endif" << std::endl ;
                    out << "}" << std::endl ;
                }
            }
            out << "      }" << std::endl ;


            out << "   }" << std::endl ;


            out << "}" << std::endl ;
        }
        

        


    }
}
#endif
