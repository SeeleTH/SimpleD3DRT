#ifndef MACROHELPER_H
#define MACROHELPER_H

#include <assert.h>

#define N_ASSERT(_EXP_) assert(_EXP_)

#define N_DEBUG(__CMD__) __CMD__

#if defined(_DEBUG)
#   define N_NEW  new//new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#	define N_NEW  new
#endif

#if !defined(N_DELETE)
#define N_DELETE(x) if(x) delete x; x=NULL;
#endif

#if !defined(N_DELETE_SIZE)
#define N_DELETE_SIZE(x,y) if(x) delete(x,y); x=NULL;
#endif

#if !defined(N_DELETE_ARRAY)
#define N_DELETE_ARRAY(x) if (x) delete [] x; x=NULL; 
#endif

#if !defined(N_RELEASE)
#define N_RELEASE(x) if(x) x->Release(); x=NULL;
#endif

#if !defined(N_DEBUG_COUT)
#if _DEBUG
#define N_DEBUG_COUT(MSG) std::cout << "[" << __FILE__ <<":" << __LINE__ << "] " << MSG << std::endl;
#else
#define N_DEBUG_COUT(MSG)
#endif
#endif

#endif