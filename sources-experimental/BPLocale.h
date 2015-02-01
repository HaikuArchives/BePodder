#ifndef _BPLocale_H
#define _BPLocale_H

#ifdef ZETA
	#include <locale/Locale.h>
	#ifndef _TT
		#define _TT(A) _T(A)
	#endif
#else
	#ifndef _T
		#include "R5Locale.h"
		#define _TT(A) findKey(A)
		#define _T(A) A
	#endif
#endif

#endif

//end.
