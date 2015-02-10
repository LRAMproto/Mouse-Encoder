/* Simplified mouse reading functions*/

#include "mousefunctions.h"


//Buffer for mouse data
static 	UInt32	MouseBytes = 4;
int KensingtonBuffer[4][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};

int findKensington(USBmouse* pKensington, int* pKenCount)
{
	//Initialize variables
	kern_return_t			err;  //Kernel error returns
    CFMutableDictionaryRef 	matchingDictionary = 0;		// requires <IOKit/IOKitLib.h>
    SInt32					idVendor = 0x047d;	//Vendor ID for the kensington mice
    SInt32					idProduct = 0x1032;	//Product ID for the kensington mice
    CFNumberRef				numberRef;
    io_iterator_t			usbDeviceList = 0;			//List of usb devices
    io_service_t			usbDeviceRef;				//specific usb device
//	int i;

	/////////////////////////
		
	//////////////////////
	
	//Get the matching dictionary
    matchingDictionary = IOServiceMatching(kIOUSBDeviceClassName);	// requires <IOKit/usb/IOUSBLib.h>
	
    if (!matchingDictionary)
    {
        printf("SnakeMouse:mousefunctions could not create matching dictionary\n");
        return -1;
    }
	else
	{
		//printf("USBSimpleExample: created matching dictionary\n");
	}
	
	/////////////////////
	
	//Create a CFNumberRef for the vendor
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &idVendor);
	
    if (!numberRef)
    {
        printf("SnakeMouse:mousefunctions could not create CFNumberRef for vendor\n");
        return -1;
    }
	
	//Add the number corresponding to the vendor ID to the dictionary
    CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBVendorID), numberRef);
	
	//Release numberRef, which is a holding value
    CFRelease(numberRef);
	
	//Reset numberRef so it can be reused cleanly
    numberRef = 0;
	
	/////////////////////
	
	//Create a reference number for the product
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &idProduct);
	
    if (!numberRef)
    {
        printf("SnakeMouse:mousefunctions could not create CFNumberRef for product\n");
        return -1;
    }
	
	//Add the product number to the dictionary
    CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBProductID), numberRef);
	
	//Release nuberRef
    CFRelease(numberRef);
	
	//Reset numberRef for clean reuse
    numberRef = 0;
	
	///////////////////////
	
	
	//Build a list of all devices matching vendor and product ID to the kensington mouse
	 err = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDictionary, &usbDeviceList);
	 
	 if (err)
	 {
        printf("SnakeMouse:mousefunctions could not build a list of devices\n");
        return -1;
	 }
	 
	 //Reset matching dictionary
	 matchingDictionary = 0;			// this was consumed by the above call
	 
	 ///////////////////
	 
	 //Iterate over all matching devices found, storing count in KenCount
	 int DeviceCount = 0;
	 
	 while ( (usbDeviceRef = IOIteratorNext(usbDeviceList)) )
	 {
		//Print the number of the device being worked with
		printf("Found device %p\n", (void*)usbDeviceRef);
		
		//Add one to the number of mice found
		DeviceCount++;
		
		//Add the new device to the device field for the unfilled element of temp
		pKensington[DeviceCount-1].Device = usbDeviceRef;
		pKensington[DeviceCount-1].mouseNum = DeviceCount-1;
		
	 }
	 
	//Export the number of devices found. 
	*pKenCount = DeviceCount;
	
//	/////////////
//	//Cleanup
//	
	//Release the device list
	IOObjectRelease(usbDeviceList);
    usbDeviceList = 0;
			
	return 0;
}


void findInterfaces(USBmouse* pKensington, int KenCount)
{
    IOReturn						err;
    IOCFPlugInInterface				**iodev;		// requires <IOKit/IOCFPlugIn.h>
    IOUSBDeviceInterface			**dev;
    SInt32							score;
    UInt8							numConf;
    IOUSBConfigurationDescriptorPtr	confDesc;
    IOUSBFindInterfaceRequest		interfaceRequest;
    io_iterator_t					iterator;
    io_service_t					usbInterfaceRef;
    io_service_t			usbDeviceRef;				//specific usb device
    int i;
	
	for (i=0;i<KenCount;i++)
	{
	
		usbDeviceRef = pKensington[i].Device;
		
		err = IOCreatePlugInInterfaceForService(usbDeviceRef, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &iodev, &score);
		if (err || !iodev)
		{
			printf("dealWithDevice: unable to create plugin. ret = %08x, iodev = %p\n", err, iodev);
			return;
		}
		err = (*iodev)->QueryInterface(iodev, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (void**)&dev);
		IODestroyPlugInInterface(iodev);				// done with this

		if (err || !dev)
		{
			printf("dealWithDevice: unable to create a device interface. ret = %08x, dev = %p\n", err, dev);
			return;
		}
		err = (*dev)->USBDeviceOpen(dev);
		if (err)
		{
			printf("dealWithDevice: unable to open device. ret = %08x\n", err);
			return;
		}
		err = (*dev)->GetNumberOfConfigurations(dev, &numConf);
		if (err || !numConf)
		{
			printf("dealWithDevice: unable to obtain the number of configurations. ret = %08x\n", err);
			(*dev)->USBDeviceClose(dev);
			(*dev)->Release(dev);
			return;
		}
		printf("dealWithDevice: found %d configurations\n", numConf);
		err = (*dev)->GetConfigurationDescriptorPtr(dev, 0, &confDesc);			// get the first config desc (index 0)
		if (err)
		{
			printf("dealWithDevice:unable to get config descriptor for index 0\n");
			(*dev)->USBDeviceClose(dev);
			(*dev)->Release(dev);
			return;
		}
		err = (*dev)->SetConfiguration(dev, confDesc->bConfigurationValue);
		if (err)
		{
			printf("dealWithDevice: unable to set the configuration\n");
			(*dev)->USBDeviceClose(dev);
			(*dev)->Release(dev);
			return;
		}
		
		interfaceRequest.bInterfaceClass = kIOUSBFindInterfaceDontCare;		// requested class
		interfaceRequest.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;		// requested subclass
		interfaceRequest.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;		// requested protocol
		interfaceRequest.bAlternateSetting = kIOUSBFindInterfaceDontCare;		// requested alt setting
		
		//Create an interface iterator
		err = (*dev)->CreateInterfaceIterator(dev, &interfaceRequest, &iterator);
		if (err)
		{
			printf("dealWithDevice: unable to create interface iterator\n");
			(*dev)->USBDeviceClose(dev);
			(*dev)->Release(dev);
			return;
		}
		
		//Only expecting one interface
		(usbInterfaceRef = IOIteratorNext(iterator));
		printf("found interface: %p\n", (void*)usbInterfaceRef);
		
		//Store the interface reference
		pKensington[i].Interface = usbInterfaceRef;

		
		//Store the device tag
		(pKensington[i].DevTag) = dev;
			
	//		dealWithInterface(usbInterfaceRef);
	//		IOObjectRelease(usbInterfaceRef);				// no longer need this reference
	
		
		IOObjectRelease(iterator);
		iterator = 0;
	}
	
}


void findPipes(USBmouse* pKensington, int KenCount)
{
    IOReturn					err;
    IOCFPlugInInterface 		**iodev;		// requires <IOKit/IOCFPlugIn.h>
    IOUSBInterfaceInterface 	**intf;
    SInt32						score;
    UInt8						numPipes;
    io_service_t					usbInterfaceRef;
	int i;

	for(i=0;i<KenCount;i++)
	{
		usbInterfaceRef = pKensington[i].Interface;
		
		//Create a plugin for the interface
		err = IOCreatePlugInInterfaceForService(usbInterfaceRef, kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID, &iodev, &score);
		if (err || !iodev)
		{
			printf("dealWithInterface: unable to create plugin. ret = %08x, iodev = %p\n", err, iodev);
			return;
		}
		
		//Create a device interface
		err = (*iodev)->QueryInterface(iodev, CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID), (void**)&intf);
		IODestroyPlugInInterface(iodev);				// done with this
		if (err || !intf)
		{
			printf("dealWithInterface: unable to create a device interface. ret = %08x, intf = %p\n", err, intf);
			return;
		}
		
		//Open the interface
		err = (*intf)->USBInterfaceOpen(intf);
		if (err)
		{
			printf("dealWithInterface: unable to open interface. ret = %08x\n", err);
			return;
		}
		
		//Get the number of endpoints
		err = (*intf)->GetNumEndpoints(intf, &numPipes);
		if (err)
		{
			printf("dealWithInterface: unable to get number of endpoints. ret = %08x\n", err);
			(*intf)->USBInterfaceClose(intf);
			(*intf)->Release(intf);
			return;
		}
		//Print the number of pipes (pipe and endpoint are roughly equivalent)
		printf("dealWithInterface: found %d pipes\n", numPipes);
		
		//If no pipes were found
		if (numPipes == 0)
		{
			// try alternate setting 1
			err = (*intf)->SetAlternateInterface(intf, 1);
			if (err)
			{
				printf("dealWithInterface: unable to set alternate interface 1. ret = %08x\n", err);
				(*intf)->USBInterfaceClose(intf);
				(*intf)->Release(intf);
				return;
			}
			err = (*intf)->GetNumEndpoints(intf, &numPipes);
			if (err)
			{
				printf("dealWithInterface: unable to get number of endpoints - alt setting 1. ret = %08x\n", err);
				(*intf)->USBInterfaceClose(intf);
				(*intf)->Release(intf);
				return;
			}
			numPipes = 13;  		// workaround. GetNumEndpoints does not work after SetAlternateInterface
		}
		
		//numPipes could be changed by the if structure, so this is a second if, not an else
		if (numPipes)
		{
			pKensington[i].InterfaceTag = intf;
			//Hardcoding the pipe to 1
			pKensington[i].Pipe = 1;
		}
	}
	
}

void CopyOneMouseBuffer(int mouseNum, char* OneMouseData)
{
	UInt32					i;
//    IOUSBInterfaceInterface 	**intf;
//	UInt8					inPipeRef;		

//	printf("Copied ");
	for (i=0; i < MouseBytes; i++)
	{
//		printf("%d ", KensingtonBuffer[mouseNum][i]);
		OneMouseData[i] = KensingtonBuffer[mouseNum][i];
	}
//	printf("\n");

}


void *MouseReadLoop(void *pKensingtonIv)
{
	IOUSBInterfaceInterface 	**intf;
	UInt8					inPipeRef;
	IOReturn						err;
	USBmouse* pKensingtonI;
	int mouseNum;
	UInt32 numBytes = MouseBytes;
	char KensingtonRead[5];
	int i;
	FILE * pBufferRecord;
	
	pKensingtonI = (USBmouse*) pKensingtonIv;
	
	intf = pKensingtonI->InterfaceTag;
	inPipeRef = pKensingtonI->Pipe;
	mouseNum = pKensingtonI->mouseNum;

	pBufferRecord = fopen("BufferRecord.txt","w");

	while(!MouseStopFlag)
	{
//		printf("MouseStopFlag In MouseReadLoop: %d\n",MouseStopFlag);
		//This is an acknowledged race condition
		{
	//		printf("Running ReadPipe on mouse %d \n",mouseNum);
			err = (*intf)->ReadPipe(intf, inPipeRef, KensingtonRead, &numBytes);
	//		printf("Finished ReadPipe on mouse %d: ",mouseNum);
	//		for (i=0; i < MouseBytes; i++)
	//		{
	//			printf("%d ", KensingtonBuffer[mouseNum][i]);
	//		}
	//		printf("\n");
		
			for (i=0; i < MouseBytes; i++)
			{
				KensingtonBuffer[mouseNum][i] += KensingtonRead[i];
			}
			
//			switch(mouseNum)
//			{
//				case 0:
//					for (i=0; i < MouseBytes; i++)
//					{
//						fprintf(pBufferRecord," %d ",KensingtonBuffer[mouseNum][i]);
//					}
//					
//					fprintf(pBufferRecord," 0 0 0 0 \n");
//					break;
//				
//				case 1:
//					fprintf(pBufferRecord," 0 0 0 0 ");
//					for (i=0; i < MouseBytes; i++)
//					{
//						fprintf(pBufferRecord," %d ",KensingtonBuffer[mouseNum][i]);
//					}
//					fprintf(pBufferRecord,"\n");
//					break;
//			}
			
			
		}
		
	
	}
	
	fclose(pBufferRecord);
	
	pthread_exit( (void*)0 );
	return (void*)0;
}

//void ReadMouse(IndexedUSBmouse* pKensington)
//{
//
//	IOUSBInterfaceInterface 	**intf;
//	UInt8					inPipeRef;
//	IOReturn						err;
//	int mouseNum;
//	
//	//Extract device-specific information
//	intf = (pKensington)->Mouse.InterfaceTag;
//	inPipeRef = pKensington->Mouse.Pipe;
//	mouseNum = pKensington->mouseNum;
//	
//	printf("Calling ReadPipeAsync \n");
//	err = (*intf)->ReadPipeAsync(intf, inPipeRef, KensingtonBuffer[mouseNum], MouseBytes, ReadMouseCallback, (void*)pKensington);
//    if (kIOReturnSuccess != err)
//    {
//        printf("unable to do async interrupt read (%08x)\n", err);
//		return;
//	}
//	
//
//}
//
//void ReadMouseCallback(void *refCon, IOReturn result, void *arg0)
//{
//
//	printf("In ReadCompletion \n");
//	
//	if (result != kIOReturnSuccess)
//	{
//		printf("readKensingtonData: ReadPipe failed, result = %08x\n", result);
//		return;
//	}
//	else if(!MouseStop)
//	{
//		printf("Collected data \n");
//		ReadMouse((IndexedUSBmouse*) refCon);
//	}
//	else
//	{
//		printf("Recieved mousestop signal\n");
//		return;
//	}
//
//}

void MouseCleanup(USBmouse* pKensington, int KenCount)
{
	IOUSBDeviceInterface			**dev;
	IOUSBInterfaceInterface			**intf;
    IOReturn						err;
	int i;
	
	
	for(i=0;i<KenCount;i++)
	{
		dev = pKensington[i].DevTag;
		intf = pKensington[i].InterfaceTag;
		
		//Close and release the interface
		err = (*intf)->USBInterfaceClose(intf);
		if (err)
		{
			printf("dealWithInterface: unable to close interface. ret = %08x\n", err);
			return;
		}
		err = (*intf)->Release(intf);
		if (err)
		{
			printf("dealWithInterface: unable to release interface. ret = %08x\n", err);
			return;
		}

		
		//Close and release the device
		err = (*dev)->USBDeviceClose(dev);
		if (err)
		{
			printf("dealWithDevice: error closing device - %08x\n", err);
			(*dev)->Release(dev);
			return;
		}
		err = (*dev)->Release(dev);
		if (err)
		{
			printf("dealWithDevice: error releasing device - %08x\n", err);
			return;
		}
	}
}
