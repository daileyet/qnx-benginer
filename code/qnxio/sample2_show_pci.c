#include <stdlib.h>
#include <stdio.h>
#include <hw/pci.h>
#include <errno.h>
#include <string.h>

int  display_info( int class, int index )
{
	void *hdl;
	struct pci_dev_info pci_info;
	int i;
	
	memset(&pci_info, 0, sizeof pci_info );
	pci_info.Class = class;

    printf("looking for class %#x, index %d\n", class, index );
	
	hdl = pci_attach_device( NULL, PCI_SEARCH_CLASS|PCI_SHARE|PCI_INIT_ALL,
	           index, &pci_info );
	if(NULL == hdl )
	{
		int ret;
		if( (ret = (errno == EBUSY )))
		{
			printf("device in use\n");
		} else {
			perror( "attach_device");
		}
		return ret;
	}
	printf("Found a card with vendor id %#x, deviced: %#x\n", 
	        pci_info.VendorId, pci_info.DeviceId );
	
	printf("It has interrupt %#x (%d)\n", pci_info.Irq, pci_info.Irq);

    for (i = 0; i < 6; i++ )
    {
    	if( pci_info.BaseAddressSize[i] )
    	{
    		if (PCI_IS_IO( pci_info.CpuBaseAddress[i] ) )
    		{
    			printf("IO space at %#llx for %#x bytes\n", 
    			       PCI_IO_ADDR( pci_info.CpuBaseAddress[i] ), pci_info.BaseAddressSize[i] );
    		}
    		if (PCI_IS_MEM( pci_info.CpuBaseAddress[i] ) )
    		{
    			printf("Memory space at %#llx for %#x bytes\n", 
    			       PCI_MEM_ADDR( pci_info.CpuBaseAddress[i] ), pci_info.BaseAddressSize[i] );
    		}
    	}
    }
    pci_detach_device( hdl );
    
    return 1;
    
}
int main(int argc, char *argv[]) {
	int pd;
	int i;
	
	pd = pci_attach(0 );
	if( -1 == pd )
	{
		perror("pci_attach");
		return EXIT_FAILURE;
	}
	
	printf("\n ethernet drivers... \n");
	for (i = 0; display_info( PCI_CLASS_NETWORK|PCI_SUBCLASS_NETWORK_ETHERNET, i ); i++ )
	{
		printf("\n");
	} 
	printf("\n vga display device... \n");
	for (i = 0; display_info( PCI_CLASS_DISPLAY|PCI_SUBCLASS_DISPLAY_VGA, i ); i++ )
	{
		printf("\n");
	} 
		
	printf("\n Audio device... \n");
	for (i = 0; display_info( PCI_CLASS_MULTIMEDIA|PCI_SUBCLASS_MULTIMEDIA_AUDIO, i ); i++ )
	{
		printf("\n");
	} 
	
	pci_detach(pd);
	return EXIT_SUCCESS;
}
