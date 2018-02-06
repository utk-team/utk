
/*
 * Simple macros to wrap the GL functions that are
 * not in GLsdk yet...
 * Note: there is a performance penalty, should be
 * only used for functions called at setup, not at
 * rendering.
 * Bruno Levy, project ALICE - INRIA, Nov 14 2006.
 */

#ifndef __GL_WRAP__
#define __GL_WRAP__

#ifdef WIN32
#  define glWrapGetProcAddress(x) wglGetProcAddress(x)
#else
extern "C" {
/*   extern void* glXGetProcAddress (const GLubyte *) ; */
#  define glWrapGetProcAddress(x) glXGetProcAddress((const GLubyte *)x)
}
#  ifndef APIENTRY
#  define APIENTRY
#  endif
#endif

#define CHECK_PFN(pfn, name) \
    if(pfn == NULL) { printf("Missing GL function: %s\n", name) ; }

#define WRAP_GL_PROC(name, args, call) \
   void name args { \
      typedef void (APIENTRY *PFN) args ; \
      static PFN pfn = NULL ; \
      if(pfn == NULL) { pfn = (PFN)glWrapGetProcAddress(#name) ; } \
      CHECK_PFN(pfn, #name) ; \
      if(pfn == NULL) { abort() ; } \
      pfn call ; \
   }

#define WRAP_GL_FUNC(rettype, name, args, call)  \
   rettype name args { \
      typedef rettype (APIENTRY *PFN)args ; \
      static PFN pfn = NULL ; \
      if(pfn == NULL) { pfn = (PFN)glWrapGetProcAddress(#name) ; } \
      CHECK_PFN(pfn, #name) ; \
      if(pfn == NULL) { abort() ; } \
      return pfn call ; \
   }

#define WRAP_GL_PROC_NON_FATAL(name, args, call) \
   void name args { \
      typedef void (APIENTRY *PFN) args ; \
      static PFN pfn = NULL ; \
      if(pfn == NULL) { pfn = (PFN)glWrapGetProcAddress(#name) ; } \
      CHECK_PFN(pfn, #name) ; \
      if(pfn == NULL) { return ; } \
      pfn call ; \
   }

#define WRAP_GL_FUNC_NON_FATAL(rettype, name, args, call)  \
   rettype name args { \
      typedef rettype (APIENTRY *PFN)args ; \
      static PFN pfn = NULL ; \
      if(pfn == NULL) { pfn = (PFN)glWrapGetProcAddress(#name) ; } \
      CHECK_PFN(pfn, #name) ; \
      if(pfn == NULL) { return 0 ; } \
      return pfn call ; \
   }

#endif
