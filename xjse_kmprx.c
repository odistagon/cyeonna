/** for PSP, kernel module prx
 */

#include <stdio.h>
#include <pspsdk.h>
#include <pspnet_inet.h>

PSP_MODULE_INFO("xjse_kmprx", 0x1000, 1, 1);
PSP_HEAP_SIZE_KB(2 * 1024);
//PSP_MAIN_THREAD_ATTR();


/** Giant hack snprintf to deal with doing this :)
 * snprintf(&args[argpos], MAX_ARGS-argpos, "%s", argv[i]);
 */
int snprintf(char *str, size_t size, const char *format, ...)
{
	// After coding this to do something I realized it will never be called
	// when loading the wifi prxs, lmao.
	// Perhaps other prxs will need it, so here it is...
#if 0
	char* x;
	va_list ap;

	va_start(ap, format);
	x = va_arg(ap, char*);

	int a = 0;
	for(; a < size-1; a++) {
		if(x[a] == '\0')
			break;
		str[a] = x[a];
	}
	str[a] = '\0';

	va_end(ap);
	return a;
#else
    return 0;
#endif
}

int xpnet_mainthread(SceSize args, void *argp)
{
	int err = 0;

#if	0
#if	1
    int modID;
    modID = pspSdkLoadStartModule(
		"flash0:/kd/ifhandle.prx", PSP_MEMORY_PARTITION_KERNEL);
    if (modID < 0)
        return modID;
printf("pspSdkLoadStartModule() 1 returns %i\n", modID);
    modID = pspSdkLoadStartModule(
		"flash0:/kd/pspnet.prx", PSP_MEMORY_PARTITION_USER);
    if (modID < 0)
        return modID;
//    else
//        pspSdkFixupImports(modID);
printf("pspSdkLoadStartModule() 1 returns %i\n", modID);
    modID = pspSdkLoadStartModule(
		"flash0:/kd/pspnet_inet.prx", PSP_MEMORY_PARTITION_USER);
    if (modID < 0)
        return modID;
//    else
//        pspSdkFixupImports(modID);
printf("pspSdkLoadStartModule() 1 returns %i\n", modID);
#if	0
    modID = pspSdkLoadStartModule(
		"flash0:/kd/pspnet_apctl.prx", PSP_MEMORY_PARTITION_USER);
    if (modID < 0)
        return modID;
//    else
//        pspSdkFixupImports(modID);
printf("pspSdkLoadStartModule() 1 returns %i\n", modID);
    modID = pspSdkLoadStartModule(
		"flash0:/kd/pspnet_resolver.prx", PSP_MEMORY_PARTITION_USER);
    if (modID < 0)
        return modID;
//    else
//        pspSdkFixupImports(modID);
printf("pspSdkLoadStartModule() 1 returns %i\n", modID);
#endif
#else
	if((err = pspSdkLoadInetModules())) {
		printf(": Error, could not load modules %08X\n", err);
	}
#endif
	printf("pspSdkLoadInetModules returns %i (want 1)\n", err);

	//err = connectToApctl(1);
	//printf("connectToApctl returns %i (want 1)\n", err);
#endif
	printf("xjse_km.prx exit...\n");

	sceKernelExitDeleteThread(0);

	return	0;
}

// Entry point

int module_start(SceSize args, void *argp)
{
	SceUID thid;
	thid = sceKernelCreateThread(
		"xjse_kmprx_main", xpnet_mainthread, 8, 0x10000, 0, NULL);
	if(thid >= 0)
		sceKernelStartThread(thid,args,argp);

	return	0;
}

int module_stop(void)
{
	return	0;
}

