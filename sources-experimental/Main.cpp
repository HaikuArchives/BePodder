
#include "TheApp.h"
#include <stdlib.h>

int
main()
{
#ifndef ZETA
	int i = access ( "/boot/beos/system/lib/libzeta.so", F_OK );
	if ( i == 0 ){
		system("alert \"This version is for BeOS.R5/Haiku not for ZETA\nPlease download the right version!\"");
        return 0;
    }
#endif
     
	TheApp app("application/x-vnd.xeD.BePodder");
	app.Run();
	return 0;
}
