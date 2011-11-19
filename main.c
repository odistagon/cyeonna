
#ifdef	XTPF_Linux
#include <sys/time.h>
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#include <pspkernel.h>
#include <psprtc.h>
#include <pspdebug.h>
PSP_MODULE_INFO("TESTJS01", 0, 1, 0);	// 2nd param: 0: usermode
PSP_HEAP_SIZE_KB(16 * 1024);
//PSP_HEAP_SIZE_MAX();
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif	//XTPF_PSP

#include "xjse.h"
#include "xtk/xtk.h"
#include "xgg/xgg.h"

char*	g_pszvers = 0;


#ifdef	XTPF_PSP
int		g_pspdone = 0;
/** Exit callback
 */
int exit_callback(int arg1, int arg2, void *common)
{
	g_pspdone = 1;
	return	0;
}

/** Callback thread
 */
int CallbackThread(SceSize args, void *argp)
{
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return	0;
}

/** Sets up the callback thread and returns its thread id
 */
int SetupCallbacks(void)
{
	int	thid = sceKernelCreateThread("update_thread", CallbackThread,
		0x11, 0xFA0, 0, 0);
	if(thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}
#endif	//XTPF_PSP


/**
 */
static xjse_result_t xjse_xadget_render(XTKCTX* ptkctxa, XJSECTX* pjsectx)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	xjse_int_t		i;
	XGGCTX*			pggctx = pjsectx->pggctx;

	for(i = 0; i < pggctx->nelems; i++) {
		XGGPRIVHDR*		phdr1 = (XGGPRIVHDR*)pggctx->apelems[i];

		if(phdr1->pxadgelem == 0)
			continue;

		if(phdr1->pfrender != 0) {
			xr1 = phdr1->pfrender(ptkctxa, pjsectx, phdr1, 0);
		}
	}

	return	xr1;
}

/** called every time in loop
 * + find timed-out timer objects
 * ? rendering and others should be separated?
 */
xjse_int_t	cb_once_on_loop(XTKCTX* ptkctxa, void* pdata)
{
	xjse_int_t		i;
	xjse_result_t	xr1;
	XJSECTX*		pjx = 0;
	XJSETIMER*		ptimer1 = 0;
	XJSECTX**		ppjsectx = (XJSECTX**)pdata;

	// render, fire timers of each xadget
	for(i = 0; i < XJSE_NMAXXADGETS; i++) {
		pjx = ppjsectx[i];
		if(pjx == 0)
			continue;

		// render
		//XJSE_TRACE("rendering [%s]", pjsectx->pszname);
		xjse_xadget_render(ptkctxa, pjx);

		//
		xr1 = xjse_timer_find_timedout(pjx, &ptimer1);
		if(!XJSE_IS_SUCCESS(xr1))
			goto	failed;
		if(ptimer1 == 0)
			continue;
		xjse_int_t	nindex1 = (ptimer1 - pjx->timers);

		// beginAnimation(): prepare event.value, recurring test
		if((ptimer1->flags & XJSE_TIMERFLAGS_ANIM) != 0) {
			jsval	v0;
			JSBool	b0 = JS_GetProperty(pjx->pctx, pjx->poglobal, "event", &v0);
			if(b0 != JS_TRUE) {
				XJSE_TRACE("(X) JS_GetProperty(\"event\") failed!");
				goto	failed;
			}
			JSObject	*poevent = JSVAL_TO_OBJECT(v0);
			// calcurate current value
			xjse_time_t		tnow;
			xjse_time_now(&tnow);
			xjse_float_t	frate0 =
				(tnow > (ptimer1->tstart + ptimer1->toduration) ?
				1.0f : (((xjse_float_t)(tnow - ptimer1->tstart))
					/ ((xjse_float_t)(ptimer1->toduration))));
			xjse_int_t	nval0 = (ptimer1->nstartval
				+ ((xjse_int_t)(((xjse_float_t)(ptimer1->nendval
					- ptimer1->nstartval)) * frate0)));
			v0 = INT_TO_JSVAL(nval0);
			//
			JS_SetProperty(pjx->pctx, poevent, "value", &v0);
			//XJSE_TRACE("(X) XXXX anim timer: (%f, %lld)[%d -> %d]",
			//	frate0, (tnow - ptimer1->tstart), nval0, ptimer1->nendval);

			// stop recurring?
			if(tnow > (ptimer1->tstart + ptimer1->toduration)) {
				// time elapsed, clear recurring flag.
				ptimer1->flags ^= XJSE_TIMERFLAGS_RECURR;
				//XJSE_TRACE("(X) XXXX anim timer: cleared!");
			}
		}

		// execute script
		jsval	rval1;
		if(ptimer1->pszexec != 0) {
			//XJSE_TRACE("(X) executing timer script (%p)[%s]",
			//	ptimer1, ptimer1->pszexec);
			JSBool	b0 = JS_EvaluateScript(pjx->pctx, pjx->poglobal,
				ptimer1->pszexec, xjse_strlen(0, ptimer1->pszexec),
				"__xjse_call_ontimer", (uint32)0, &rval1);
			if(b0 != JS_TRUE) {
				XJSE_TRACE("(W) JS_EvaluateScript() failed!");
			}
		}
		if(ptimer1->pfexec != 0) {
			JS_CallFunction(pjx->pctx, pjx->poglobal,
				ptimer1->pfexec, 0, 0, &rval1);
		}

		//
		ptimer1->ucntexec++;
		if(ptimer1->ucntexec % 30 == 0) {
			XJSE_TRACE("timed out: [xadget %d, timer %d] executed. cnt: [%d]",
				i, nindex1, ptimer1->ucntexec);
		}

		// reset (setInterval) or clear (setTimeout) the timer
		if((ptimer1->flags & XJSE_TIMERFLAGS_RECURR) != 0) {
			//XJSE_TRACE("on_timerout1: resetting timer [%d]", nindex1);
			xjse_timer_reset(ptimer1);
		} else {
			//XJSE_TRACE("on_timerout1: clearing timer [%d]", nindex1);
			xjse_timer_destroy(pjx, nindex1);
		}
	}

failed:
	return	0;
}

/** called when menu item is selected
 */
xjse_int_t	cb_on_menuitem(XTKCTX* ptkctxa, void* pdata)
{
	XTK_MENUITEM*	pmenuitems = ptkctxa->apmenuitems[ptkctxa->umenulevel];
	XTK_MENUITEM*	pmi = &(pmenuitems[ptkctxa->aumenuitem[ptkctxa->umenulevel]]);

	switch(pmi->cmd) {
	case XTK_MICMD_CLICK:
		break;
	case XTK_MICMD_ENUMPROFS:
	{
		XTK_MENUITEM*	pmi0 = 0;
		xjse_result_t	xr1 = xtk_menu_profiles_milist(ptkctxa, &pmi0);
		if(xr1 != XJSE_SUCCESS)
			;
		xtk_menulist_set(ptkctxa, 2, pmi0);
		xtk_menu_toggle(ptkctxa, +1);
	}
		break;
	case XTK_MICMD_ENUMWLANAPS:
	{
		XPNETCTX*	pxpnctx = ((XJSECTX**)pdata)[0]->pxpnetctx;
		XTK_MENUITEM*	pmi0 = 0;
		xjse_result_t	xr1 = xtk_menu_wlanaps_milist(
			ptkctxa, &pmi0, pxpnctx->nap);
		if(xr1 != XJSE_SUCCESS)
			;
		xtk_menulist_set(ptkctxa, 2, pmi0);
		xtk_menu_toggle(ptkctxa, +1);
	}
		break;
	case XTK_MICMD_SELECTPROF:
		XJSE_TRACE("(X) switching profile [%s]", pmi->pszcaption);
	{
		XJSECTX**		ppjsectx = (XJSECTX**)pdata;
		xjse_int_t		i;
		xjse_result_t	xr1;
		JSRuntime*		pjsrt = 0;
		XPNETCTX*		pxpnctx = 0;
		// cleanup all jsectx's first
		for(i = 0; i < XJSE_NMAXXADGETS; i++) {
			if(ppjsectx[i] == 0)
				continue;

			// reuse JSRuntime, xpnet ctx
			if(pjsrt == 0) {
				pjsrt = JS_GetRuntime(ppjsectx[i]->pctx);
				pxpnctx = ppjsectx[i]->pxpnetctx;
			}

			xjse_timer_destroy_all(ppjsectx[i]);
			xgg_cleanup_one(ppjsectx[i]);
			xjse_ctx_delete(ppjsectx[i]);
			ppjsectx[i] = 0;
		}
		xjse_xpnet_reset(pxpnctx);
		// reload profile and xadgets
		xr1 = xjse_xadgprof_load(
			pjsrt, ppjsectx, ptkctxa, pxpnctx, pmi->pszcaption);
		if(!XJSE_IS_SUCCESS(xr1))
			goto	failed;
	}
		break;
	case XTK_MICMD_SELECTWLANAP:
		XJSE_TRACE("(X) switching wlan ap (%d)[%s]", pmi->narg, pmi->pszcaption);
	{
		XPNETCTX*	pxpnctx = ((XJSECTX**)pdata)[0]->pxpnetctx;
		pxpnctx->nap = pmi->narg;
	}
		break;
	case XTK_MICMD_TEST01:
		break;
	case XTK_MICMD_EXIT:
		ptkctxa->uflags |= XTKCTXFLG_EXIT;
		break;
	default:
		XJSE_TRACE("(X) on menu item ! (%d)", pmi->cmd);
	}

failed:
	return	0;
}

#ifdef	XTPF_PSP
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif	//XTPF_PSP
/**
 */
int main(int argc, char** argv)
{
	xjse_result_t	xr1;
	xjse_int_t		i = 0;
	XTKCTX			*ptkctx = 0;
	JSRuntime		*pjsrt = 0;		// sole Runtime object per process
	XJSECTX*		apjsectx[XJSE_NMAXXADGETS];
	XPNETCTX*		pxpnctx;

	// version string
	{
		xjse_int_t	n0 = 0;
		g_pszvers = (char*)malloc(XJSE_VERS_NMAX + 1);
		snprintf(g_pszvers, XJSE_VERS_NMAX, "%s %s.%s Build %s",
			XJSE_VERS_NAM, XJSE_VERS_MAJ, XJSE_VERS_MIN, XJSE_VERS_EXT);
		do {
			n0 += g_pszvers[i++];
		} while(g_pszvers[i] != 0);
		if(XJSE_VERS_NSUM == 0) {
			XJSE_TRACE("(X) checksum: [%s](%d)", g_pszvers, n0);
		} else {
			if(n0 != XJSE_VERS_NSUM) {
				XJSE_TRACE("(E) version string altered.");
				goto	failed;
			}
		}
	}
	XJSE_TRACE("Cyonna version %s", g_pszvers);

	// load external prx
#ifdef	XTPF_PSP
	{
		xjse_int_t			nflags = 0;
//		SceUID	uidprx = sceKernelLoadModule("libjs.prx", nflags,
//			0);	// &opt0 to load in the kernel part.
		SceUID	uidprx = pspSdkLoadStartModule("libjs.prx",
			PSP_MEMORY_PARTITION_USER);
		XJSE_TRACE("(X) prx [%p]", uidprx);
		if(uidprx < 0) {
			XJSE_TRACE("(X) failed to load module [libjs.prx]");
			goto	failed;
		}
		// Let's test the module's exports
		XJSE_TRACE("(X) Module Info [%p]", (void*)getModuleInfo());
	}
#endif	//XTPF_PSP

	memset(apjsectx, 0, sizeof(XJSECTX*) * XJSE_NMAXXADGETS);

	XJSE_TRACE("starting up (0)...");
#ifdef	XTPF_PSP
	pspDebugScreenInit();
	SetupCallbacks();
#endif	//XTPF_PSP

	// Initialize Toolkit
	xr1 = xtk_init(&ptkctx, argc, argv);
	if(!XJSE_IS_SUCCESS(xr1))
		goto	failed;
	xtk_eventcb_set(ptkctx, XTKC_CBTYPE_ONCEONLOOP, cb_once_on_loop, apjsectx);
	xtk_eventcb_set(ptkctx, XTKC_CBTYPE_MENUITEM, cb_on_menuitem, apjsectx);

	// Initialize Network
	xr1 = xjse_xpnet_init(&pxpnctx, 1);
	if(!XJSE_IS_SUCCESS(xr1))
		goto	failed;

	// initialize JSRuntime
	xr1 = xjse_init(&pjsrt);
	if(!XJSE_IS_SUCCESS(xr1))
		goto	failed;

	// load profile and xadgets
	xr1 = xjse_xadgprof_load(pjsrt, apjsectx, ptkctx, pxpnctx, "default.xml");
	if(!XJSE_IS_SUCCESS(xr1))
		goto	failed;

	// main loop
	xtk_mainloop(ptkctx, apjsectx, pxpnctx);

	goto	cleanup;

failed:
	XJSE_TRACE("something went wrong, cleaning up...");

cleanup:
	XJSE_SAFENNP(pxpnctx, xjse_xpnet_cleanup);
	XJSE_SAFENNP(ptkctx, xtk_cleanup);
	for(i = 0; i < XJSE_NMAXXADGETS; i++) {
		if(apjsectx[i] == 0)
			continue;
		xjse_timer_destroy_all(apjsectx[i]);
		xgg_cleanup_one(apjsectx[i]);
		xjse_ctx_delete(apjsectx[i]);
	}
	XJSE_SAFENNP(pjsrt, xjse_cleanup);
#ifdef	XTPF_PSP
	if(0) {
		xjse_time_t	tnow = 0, ttowait = 0;
		XJSE_TRACE("waiting 3sec. (ttowait)");
		xjse_time_now(&tnow);
		ttowait = (tnow + 3000);
		do {
			xjse_time_now(&tnow);
		} while(tnow < ttowait);
		XJSE_TRACE("ok, exit...");
	}

	sceKernelExitDeleteThread(0);	// delete prx thread? (20081212)
	sceKernelExitGame();
#endif	//XTPF_PSP
	XJSE_SAFENNP(g_pszvers, free);

	return	0;
}

