#include "drv.h"

#include <sys/pci.h>

void PLCReleaseLPT(void)
{
	release_region(BASE_ADDR, 2);
}

int PLCRegisterLPT(void)
{
	return(request_region(BASE_ADDR, 2, "plc_lpt") != NULL);
}


