#include "drv.h"

#include <sys/pci.h>

#define DEV_NAME "PCI device 13fe:1750"

void	*FindPCI1750Card(void)
{
	int		iFound = 0;
	struct pci_dev	*pDevice = NULL;

	if (pcibios_present())
	{
		while((pDevice = pci_find_device(0x13fe, 1750, pDevice)))
		{
			pci_setup_device(pDevice);

			if (pci_request_region(pDevice, 2, "plc_labcard") == 0)
				iFound = 1;

			break;
		}
	}


	if (iFound)
		return ((void *)pDevice);
	
	return NULL;
}

u32	GetPCI1750BaseAddr(void *pDevice)
{
	if (pDevice == NULL)
		return 0;

	return	(((struct pci_dev *)pDevice)->resource[2].start);
}

void	ReleasePCI1750Card(void *pDevice)
{
	if (pDevice != NULL)
		pci_release_region((struct pci_dev *)pDevice, 2);
}

