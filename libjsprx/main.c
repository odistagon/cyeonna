/*
 * copied from pspsdk sample testprx
 */
#include <pspkernel.h>
#include <stdio.h>

//PSP_MODULE_INFO("libjsprx", 0, 1, 1);
//PSP_MODULE_INFO("libjsprx", PSP_MODULE_KERNEL, 1, 1);
PSP_MODULE_INFO("libjsprx", PSP_MODULE_USER, 1, 1);
PSP_HEAP_SIZE_KB(4 * 1024);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_NO_CREATE_MAIN_THREAD();

// Exported function returns the address of module_info
void* getModuleInfo(void)
{
	return (void *) &module_info;
}

// ----8< ----

//int cb_libjsprx_thread(SceSize args, void *argp)
//{
//	printf("Hello World\n");
//
//	return 0;
//}

// Entry point
int module_start(SceSize args, void *argp)
{
//	int thid;
//
//	thid = sceKernelCreateThread(
//		"libjsprx", cb_libjsprx_thread, 7, 0x800, 0, NULL);
//	if(thid >= 0) {
//		sceKernelStartThread(thid, args, argp);
//	}
	return 0;
}

// Module stop entry
int module_stop(SceSize args, void *argp)
{
	return 0;
}
