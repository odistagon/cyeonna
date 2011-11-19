
#include <sys/time.h>
#ifdef	XTPF_Linux
#include <time.h>	//nanosleep
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#include <psptypes.h>
#include <psprtc.h>
#include <psputils.h>
#include <pspthreadman.h>
#endif	//XTPF_PSP
#include "xjse.h"
#include "xtk/xtk.h"
#include "xgg/xgg.h"


/**
 */
xjse_result_t xjse_init(JSRuntime** ppjsrt)
{
	JSRuntime*	pjsrt = 0;

	/* You always need:
	 *    a runtime per process,
	 *    a context per thread,
	 *    a global object per context,
	 *    standard classes (e.g. Date).
	 */
	pjsrt = JS_NewRuntime(1024L * 1024L);
	if(pjsrt == 0)
		goto	failed;
	*ppjsrt = pjsrt;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

/**
 */
xjse_result_t xjse_ctx_new(JSRuntime* pjsrt, XJSECTX** ppctxa)
{
	JSBool			b1;
	JSContext		*pcx1 = 0;
	JSObject		*poglobal = 0;	// global
	xjse_result_t	xb1 = XJSE_SUCCESS;
	XJSECTX			*pctx = 0;

	//
	pctx = (XJSECTX*)malloc(sizeof(XJSECTX));
	memset(pctx, 0, sizeof(XJSECTX));
	//memset(&(pctx->timers), 0, sizeof(XJSETIMER) * XJSE_NMAXTIMERS);

	pcx1 = JS_NewContext(pjsrt, 4096);	// usually 8192 is recommended
	if(pcx1 == 0)
		goto	failed;
	pctx->pctx = pcx1;	// set foward reference (XJSECTX* -> JSContext*)
	JS_SetContextPrivate(pcx1, pctx);	// set reverse reference from JSContext*
	//
	poglobal = xjse_xgg_new_view(pctx, "__xgg_view");
	if(poglobal == 0)
		goto	failed;
	jsval	vview = OBJECT_TO_JSVAL(poglobal);
	b1 = JS_SetProperty(pcx1, poglobal, "view", &vview);
	if(b1 != JS_TRUE)
		goto	failed;
	pctx->poglobal = poglobal;
	//
	b1 = JS_InitStandardClasses(pcx1, poglobal);
	if(b1 != JS_TRUE)
		goto	failed;
	JS_SetErrorReporter(pcx1, printJSError);

	// define DOM, XMLHttpRequest (put here because this is not .gg specific)
	xb1 = xgg_dom_define(pcx1, poglobal);
	if(!XJSE_IS_SUCCESS(xb1)) {
		goto	failed;
	}
	xb1 = xjse_xhr_define(pcx1, poglobal);
	if(!XJSE_IS_SUCCESS(xb1)) {
		goto	failed;
	}
	xb1 = xjse_enumerator_define(pcx1, poglobal);
	if(!XJSE_IS_SUCCESS(xb1)) {
		goto	failed;
	}
#ifdef	XXXXTESTCOLLECTION20080412
	xb1 = xjse_collection_define(pcx1, poglobal);
	if(!XJSE_IS_SUCCESS(xb1)) {
		goto	failed;
	}
#endif	//XXXXTESTCOLLECTION20080412

	*ppctxa = pctx;

cleanup:
	return	xb1;

failed:
	xjse_ctx_delete(pctx);

	goto	cleanup;
}

/** get current time in xjse_time_t form (msec).
 */
xjse_result_t xjse_time_now(xjse_time_t* ptimea)
{
#ifdef	XTPF_Linux
	int				n1;
	struct timeval	tv1;

	n1 = gettimeofday(&tv1, 0);
	if(n1 != 0)
		goto	failed;

	if(ptimea != 0)
		*ptimea = (xjse_time_t)(tv1.tv_sec * 1000 + tv1.tv_usec / 1000);
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#if	0
	int				n1;
	struct timeval	tv1;
	n1 = sceKernelLibcGettimeofday(&tv1, 0);	// does not return date data
	if(ptimea != 0)
		*ptimea = (xjse_time_t)(tv1.tv_sec * 1000 + tv1.tv_usec / 1000);
#else	//1
	xjse_uint64_t		tick = 0;
	const xjse_uint64_t	tickspersec = 1000000;	// sceRtcGetTickResolution()
	sceRtcGetCurrentTick(&tick);
	if(ptimea != 0)
		*ptimea = (xjse_time_t)(tick / (tickspersec / 1000));
#endif	//1
#endif	//XTPF_PSP

	return	XJSE_SUCCESS;

#ifdef	XTPF_Linux
failed:
	return	XJSE_E_UNKNOWN;
#endif	//XTPF_Linux
}

/** reset the timer. (for recurring)
 */
xjse_result_t xjse_timer_reset(XJSETIMER* ptimera)
{
	xjse_result_t	r1 = XJSE_E_UNKNOWN;
	xjse_time_t		tnow;
	r1 = xjse_time_now(&tnow);

	ptimera->nexttime = (tnow + ptimera->timeout);
	//XJSE_TRACE("timer next: (%llu + %llu)", tnow, ptimera->timeout);

	return	XJSE_SUCCESS;
}

xjse_result_t xjse_timer_destroy(XJSECTX* pctxa, xjse_int_t indexa)
{
	XJSETIMER*	ptimer = (XJSETIMER*)(&(pctxa->timers[indexa]));
	if(XJSE_TIMER_IS_VALID(ptimer)) {
		XJSE_SAFENNP(ptimer->pszexec, free);
		memset(ptimer, 0, sizeof(XJSETIMER));
	}
	//XJSE_TRACE("timer destroyed: [%d]", indexa);

	return	XJSE_SUCCESS;
}

xjse_result_t xjse_timer_destroy_all(XJSECTX* pctxa)
{
	xjse_int_t	n1;
	for(n1 = 0 ; n1 < XJSE_NMAXTIMERS ; n1++) {
		xjse_timer_destroy(pctxa, n1);
	}
	return	XJSE_SUCCESS;
}

/**
 * @param pntimerid
 */
xjse_result_t xjse_timer_add(XJSECTX* pctxa, xjse_timeout_t timeouta, xjse_timeout_t toduration, char* pszexeca, JSFunction* pfexeca, xjse_int_t nstartvala, xjse_int_t nendvala, xjse_uint_t uflags, xjse_int_t* pntimerid)
{
	xjse_result_t	r1 = XJSE_E_UNKNOWN;
	xjse_int_t		n1, naddedindex = -1;
	xjse_time_t		tnow = 0;

	// Search empty slot and put it there
	for(n1 = 0 ; n1 < XJSE_NMAXTIMERS ; n1++) {
		//XJSE_TRACE("(X) timer [%p](%d, %lld)[%s]", &(pctxa->timers[n1]),
		//	n1, pctxa->timers[n1].timeout, pctxa->timers[n1].pszexec);
		if(XJSE_TIMER_IS_VALID(&(pctxa->timers[n1])))
			continue;

		if(tnow == 0) {
			r1 = xjse_time_now(&tnow);
			if(!XJSE_IS_SUCCESS(r1))
				goto	failed;
		}

		// limit minimum timeout value
		if(timeouta < 1000)
			timeouta = 1000;

		memset(&(pctxa->timers[n1]), 0, sizeof(XJSETIMER));
		pctxa->timers[n1].nexttime = (tnow + timeouta);
		pctxa->timers[n1].timeout = timeouta;
		pctxa->timers[n1].pszexec = pszexeca;
		pctxa->timers[n1].pfexec = pfexeca;
		pctxa->timers[n1].flags = uflags;
		pctxa->timers[n1].tstart = tnow;
		pctxa->timers[n1].nstartval = nstartvala;
		pctxa->timers[n1].nendval = nendvala;
		pctxa->timers[n1].toduration = toduration;
		naddedindex = n1;
		XJSE_TRACE("timer [%d] next: (%llu + %llu)[%s]", n1, tnow, pctxa->timers[n1].timeout, pctxa->timers[n1].pszexec);

		break;
	}

	if(naddedindex < 0) {
		XJSE_TRACE("no valid timer(s) found!");
		return	XJSE_E_TIMERTOOMANY;
	}

	if(pntimerid != 0)
		*pntimerid = naddedindex;

	return	XJSE_SUCCESS;

failed:
	return	r1;
}

/** find timed out timer.
 */
xjse_result_t xjse_timer_find_timedout(XJSECTX* pctxa, XJSETIMER** pptimera)
{
	xjse_result_t	r1 = XJSE_E_UNKNOWN;
	xjse_int_t	n1;
	xjse_time_t	tnow = 0;

	r1 = xjse_time_now(&tnow);
	if(!XJSE_IS_SUCCESS(r1))
		goto	failed;

	*pptimera = 0;

	for(n1 = 0 ; n1 < XJSE_NMAXTIMERS ; n1++) {
		if(!XJSE_TIMER_IS_VALID(&(pctxa->timers[n1])))
			continue;

//XJSE_TRACE("timer [%d] timedout? (%lld, next: %lld, %dsec.)", n1, tnow, pctxa->timers[n1].nexttime, XJSE_TIMEOUT_TO_SEC(pctxa->timers[n1].nexttime - tnow));
		if(XJSE_TIMER_IS_TIMEDOUT(&(pctxa->timers[n1]), tnow)) {
			*pptimera = &(pctxa->timers[n1]);
			break;
		}
	}

	return	XJSE_SUCCESS;

failed:
	return	r1;
}

xjse_result_t xjse_ctx_delete(XJSECTX* pctxa)
{
	xjse_int_t	n1;

	// Cleanup timer objects
	for(n1 = 0 ; n1 < XJSE_NMAXTIMERS ; n1++) {
		if(XJSE_TIMER_IS_VALID(&(pctxa->timers[n1]))) {
			xjse_timer_destroy(pctxa, n1);
		}
	}

	if(pctxa != 0) {
       	XJSE_SAFENNP(pctxa->fddebuglog, xjse_fclose);
		XJSE_SAFENNP(pctxa->pszname, free);
		XJSE_SAFENNP(pctxa->pctx, JS_DestroyContext);
		XJSE_SAFENNP(pctxa, free);
	}

	return	XJSE_SUCCESS;
}

xjse_result_t xjse_cleanup(JSRuntime* pjsrt)
{
	// Before exiting the application, free the JS run time
	XJSE_SAFENNP(pjsrt, JS_DestroyRuntime);

	return	XJSE_SUCCESS;
}

xjse_result_t xjse_sleep(xjse_timeout_t tsleep)
{
#ifdef	XTPF_Linux
	struct timespec t0;
	t0.tv_sec = 0;  t0.tv_nsec = (tsleep * 1000 * 1000);
	nanosleep(&t0, 0);
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	sceKernelDelayThread((SceUInt)tsleep * 1000);	//usec.
#endif	//XTPF_PSP

	return	XJSE_SUCCESS;
}

/** load a w/gagdet. (generates one JSContext for earch)
 */
static xjse_result_t xjse_prepare_jsectx(JSRuntime* pjsrta, XJSECTX** ppjsectx, const char* pszname, XTKCTX* ptkctx, xjse_uint_t upermsa)
{
	xjse_result_t	xr1;
	XJSECTX*		pjsectx = 0;

	if(pjsrta == 0) {
		xr1 = XJSE_E_INVALIDPARAM;
		goto	failed;
	}

	// new JS context
	xr1 = xjse_ctx_new(pjsrta, &pjsectx);
	if(!XJSE_IS_SUCCESS(xr1)) {
		xr1 = XJSE_E_UNKNOWN;
		goto	failed;
	}
    XJSE_SET_PERMS(pjsectx, upermsa);

	// debug log
    if(XJSE_HAS_PERMS(pjsectx, XJSEC_PERM_DEBUGLOG)) {
    	XJSE_ASSERT(pjsectx->fddebuglog == 0);
       	xjse_fd_t   fd0 = xjse_fopen("cyeonna_debug.log", XJSE_B_TRUE);
       	if(!XJSE_FD_IS_VALID(fd0)) {
           	XJSE_TRACE("(W) failed to open debuglog!");
       	} else {
       		pjsectx->fddebuglog = fd0;
       	}
    }

	// Define Google Desktop Widgets
	xr1 = xjse_xgg_define(pjsectx);
	if(!XJSE_IS_SUCCESS(xr1)) {
		xr1 = XJSE_E_UNKNOWN;
		goto	failed;
	}

	// set xadgets' name
	{
		xjse_int_t	nlen = strlen(pszname);
		pjsectx->pszname = (char*)malloc(nlen + 1);
		strcpy(pjsectx->pszname, pszname);
	}

	// Load String Resource (TODO: use configured language as arg);
	xr1 = xjse_strres_load(pjsectx, pjsectx->pszname, "1033");
	if(!XJSE_IS_SUCCESS(xr1)) {
		xr1 = xjse_strres_load(pjsectx, pjsectx->pszname, "en");
	}

	pjsectx->ptkctx = ptkctx;

	*ppjsectx = pjsectx;

	return	XJSE_SUCCESS;

failed:
	if(pjsectx != 0) {
		XJSE_SAFENNP(pjsectx->pszname, free);
		xjse_ctx_delete(pjsectx);
	}

	return	xr1;
}

/** find the next focus target elem.
 * @param pcurrenta current focused elem. skip search to this element.
 */
xjse_result_t xjse_xadget_focus_next(XJSECTX** ppjsectx, xjse_int_t nflipoffset, XJSE_TREEELEM** ppnewfocused)
{   
	XGGCTX			*pggctx = ppjsectx[nflipoffset]->pggctx;
	XJSECTX			*pjsectx = pggctx->pjsectx;
	XJSE_TREEELEM	*pcurrent = pggctx->pfocusedelem;
	XJSE_TREEELEM	*pe1, *pe2 = 0;

	pe1 = pggctx->apelems[0];
	//XJSE_TRACE("(X) XXXX xjse_focus_next() (%p, %p)", pe1, pcurrent);
	xgg_find_nextendnode(pe1, &pcurrent, &pe2, XJSEC_TREEELEMFLAG_HASMOUSEEVENT);

	// 0 will be set if not found or search reached end.
	pggctx->pfocusedelem = pe2;
	XJSE_TRACE("(X) xjse_focus_next() found (%p)[%s]", pe2,
		(pe2 != 0 ? pe2->pszname : "----"));

	// fire mouse focus event
	if(pe2 != 0) {
		//XGGPRIVHDR*	phdr = TREEELEM_TO_XGGPRIVHDR(pe2);
		jsval		v0, vr0;
		JSBool		b0 = JS_GetProperty(pjsectx->pctx,
			pe2->poself, "onmouseover", &v0);
		char*		pszonmover = JS_GetStringBytes(
			JS_ValueToString(pjsectx->pctx, v0));
		XJSE_TRACE("(X) property onmouseover 1: (%d)[%s]", b0, pszonmover);
		b0 = JS_EvaluateScript(pjsectx->pctx, pjsectx->poglobal,
			pszonmover, xjse_strlen(0, pszonmover),
			"__xgg_call_onmouseover", (uint32)0, &vr0);
//		JSObject*	po0 = JSVAL_TO_OBJECT(v0);
//		b0 = JS_ObjectIsFunction(pjsectx->pctx, po0);
	}

	return	XJSE_SUCCESS;
}

/** fire click event.
 */
xjse_result_t xjse_xadget_click(XJSECTX** ppjsectx, xjse_int_t nflipoffset)
{   
	XGGCTX			*pggctx = ppjsectx[nflipoffset]->pggctx;
	XJSECTX			*pjsectx = pggctx->pjsectx;
	XJSE_TREEELEM	*pcurrent = pggctx->pfocusedelem;

	if(pcurrent == 0)
		goto	cleanup;

	jsval		v0, vr0;
	JSBool		b0 = JS_GetProperty(pjsectx->pctx,
		pcurrent->poself, "onclick", &v0);
	char*		psz1 = JS_GetStringBytes(JS_ValueToString(pjsectx->pctx, v0));
	XJSE_TRACE("(X) property onclick: (%d)[%s]", b0, psz1);
	b0 = JS_EvaluateScript(pjsectx->pctx, pjsectx->poglobal,
		psz1, xjse_strlen(0, psz1), "__xgg_call_onclick", (uint32)0, &vr0);
	//if(b0 != JS_TRUE)
	XJSE_TRACE("(X) property onclick: returned (%d)", b0);

cleanup:
	return	XJSE_SUCCESS;
}


// xadgprof

typedef	struct	__tag_xadgprof_readctx {
	XTKCTX*			ptkctx;
	xjse_uint_t		uidxframeset;
	xjse_uint_t		uidxframe;
	xjse_rectf_t	arframesets[XJSE_NMAXFRAMESETS][XJSE_NMAXXADGETS];
	xjse_uint_t		uidxxadget;

	JSRuntime*		pjsrt;
	XJSECTX*		apxjsectx[XJSE_NMAXXADGETS];
}	XADGPROFREADCTX;

static void xadgprof_cb_startelement(void *userData, const XML_Char *name, const XML_Char **atts)
{
	XADGPROFREADCTX*	pxctx = (XADGPROFREADCTX*)userData;
	XML_Char**			ppxc = (XML_Char**)atts;
	char				*pszt1 = 0, *pszt2 = 0;

	if(xjse_strcmp(0, name, "xadget") == 0) {
		char	*pszname = 0;
		pszname = xjse_expat_get_attribute_linear(ppxc, "name");
		if(pxctx->uidxxadget == XJSE_NMAXXADGETS) {
			XJSE_TRACE("(W) ignoring xadget [%s]: "
				"max number of xadgets excessed.", pszname);
			goto	cleanup;
		}
		XJSE_TRACE("(X) xadget: (%d)[%s]", pxctx->uidxxadget, pszname);

		xjse_uint_t	uperms = 0;
		pszt1 = xjse_expat_get_attribute_linear(ppxc, "perms");
		if(pszt1 != 0) {
			if(strstr(pszt1, "dlog") != 0)
				uperms |= XJSEC_PERM_DEBUGLOG;
			if(strstr(pszt1, "xhr") != 0)
				uperms |= XJSEC_PERM_XHR;
		}

		XJSECTX*	pjsectx = 0;
		xjse_result_t	xr1 = xjse_prepare_jsectx(
			pxctx->pjsrt, &pjsectx, pszname, pxctx->ptkctx, uperms);
		if(!XJSE_IS_SUCCESS(xr1)) {
			XJSE_TRACE("(X) xjse_prepare_jsectx() failed!");
			goto	failed;
		}
		XJSE_TRACE("(X) ctx %d: %s, %p loaded.",
			pxctx->uidxxadget, pjsectx->pszname, pjsectx->pctx);
		pxctx->apxjsectx[pxctx->uidxxadget] = pjsectx;
	} else
	if(xjse_strcmp(0, name, "option") == 0) {
		// set configuration as options.xxxx value
		pszt1 = xjse_expat_get_attribute_linear(ppxc, "name");
		pszt2 = xjse_expat_get_attribute_linear(ppxc, "value");
		XJSE_TRACE("(X) option [%s -> %s]", pszt1, pszt2);
		XJSECTX*	pjsectx = pxctx->apxjsectx[pxctx->uidxxadget];
		if(pjsectx == 0) {
			XJSE_TRACE("(X) misconfiguration: <option> out of <xadget> found!");
			goto	failed;
		}
		jsval	v0, voptions;
		JSBool	b1;
		b1 = JS_GetProperty(
			pjsectx->pctx, pjsectx->poglobal, "options", &voptions);
		if(b1 != JS_TRUE) {
			XJSE_TRACE("(E) JS_GetProperty() failed!");
			goto	failed;
		}
		JSObject*	pooptions = JSVAL_TO_OBJECT(voptions);
		v0 = STRING_TO_JSVAL(JS_NewStringCopyZ(pjsectx->pctx, pszt2));
		b1 = JS_SetProperty(pjsectx->pctx, pooptions, pszt1, &v0);
		if(b1 != JS_TRUE) {
			XJSE_TRACE("(E) JS_SetProperty() failed!");
			goto	failed;
		}
	} else
	if(xjse_strcmp(0, name, "frame") == 0) {
		if(pxctx->uidxframe == XJSE_NMAXXADGETS) {
			XJSE_TRACE("(W) ignoring frame: max number of frames excessed.");
			goto	cleanup;
		}
		xjse_rectf_t*	pr0 =
			&(pxctx->arframesets[pxctx->uidxframeset][pxctx->uidxframe]);
		pszt1 = xjse_expat_get_attribute_linear(ppxc, "left");
		pr0->fleft = atof(pszt1);
		pszt1 = xjse_expat_get_attribute_linear(ppxc, "top");
		pr0->ftop = atof(pszt1);
		pszt1 = xjse_expat_get_attribute_linear(ppxc, "right");
		pr0->fright = atof(pszt1);
		pszt1 = xjse_expat_get_attribute_linear(ppxc, "bottom");
		pr0->fbottom = atof(pszt1);
		XJSE_TRACE("(*) frame: (%d, %d)[%5.1f, %5.1f, %5.1f, %5.1f]", pxctx->uidxframeset, pxctx->uidxframe, pr0->fleft, pr0->ftop, pr0->fright, pr0->fbottom);
		pxctx->uidxframe++;
	}

cleanup:
	return;

failed:
	return;
}

static void xadgprof_cb_endelement(void *userData, const XML_Char *name)
{
	XADGPROFREADCTX*	pxctx = (XADGPROFREADCTX*)userData;

	if(xjse_strcmp(0, name, "xadget") == 0) {
		// Load XML
		xgg_load_one(pxctx->apxjsectx[pxctx->uidxxadget]);
		pxctx->uidxxadget++;
	} else
	if(xjse_strcmp(0, name, "frameset") == 0) {
		if(pxctx->uidxframeset == (XJSE_NMAXFRAMESETS - 1)) {
			XJSE_TRACE("(W) # of framesets reached max.");
			goto	cleanup;
		}
		pxctx->uidxframe = 0;
		pxctx->uidxframeset++;
	}

cleanup:
	return;
}

static void xjse_xadgprof_ctx_cleanup(XADGPROFREADCTX* pxctx)
{
	xjse_int_t	i;

	for(i = 0; i < XJSE_NMAXXADGETS; i++) {
	}
}

/**
 */
xjse_result_t xjse_xadgprof_load(JSRuntime* pjsrt, XJSECTX* apjsectx[], XTKCTX* ptkctx, XPNETCTX* pxpnctx, char* pszfilename)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	char			szpath[XJSE_NMAX_PATH];
	XADGPROFREADCTX	xctx;

	memset(&xctx, 0, sizeof(XADGPROFREADCTX));
	xctx.ptkctx = ptkctx;
	xctx.pjsrt = pjsrt;
	memset(&(xctx.apxjsectx), 0, sizeof(XJSECTX*) * XJSE_NMAXXADGETS);

	// load profiles and xadgets
	xjse_snprintf(0, szpath, XJSE_NMAX_PATH,
		"%s/%s", XJSE_PATH_PROFILES, pszfilename);
	XJSE_EXPAT_CBS	cbs = {
		xadgprof_cb_startelement, xadgprof_cb_endelement,	};
	xr1 = xjse_parse_xmlfile(szpath, &xctx, &cbs);
	if(!XJSE_IS_SUCCESS(xr1)) {
		//XJSE_E_FOPEN;
		goto	failed;
	}

	// additional xtk configurations
	xjse_int_t	i, ii;
	ptkctx->nxadgconfs = xctx.uidxframeset;
	ptkctx->ncurrxadgconf = 0;
	XJSEXADGCONF**	ppconfs = (XJSEXADGCONF**)xjse_malloc(0,
		sizeof(XJSEXADGCONF*) * ptkctx->nxadgconfs);
	for(i = 0; i < ptkctx->nxadgconfs; i++) {
		ppconfs[i] = (XJSEXADGCONF*)xjse_malloc(
			0, sizeof(XJSEXADGCONF) * XJSE_NMAXXADGETS);
		for(ii = 0; ii < XJSE_NMAXXADGETS; ii++) {
			xjse_rectf_t*	pr0 = &(ppconfs[i][ii].rectinsc);
			memcpy(pr0, &(xctx.arframesets[i][ii]), sizeof(xjse_rectf_t));
			//XJSE_TRACE("(X) ppconfs[%d, %d][%f, %f, %f, %f]", i, ii,
			//	pr0->fleft, pr0->ftop, pr0->fright, pr0->fbottom);
		}
	}
	ptkctx->ppxadgconfs = ppconfs;

	// copy xtk configs to xadgets
	for(i = 0; i < XJSE_NMAXXADGETS; i++) {
		XJSECTX*	pjsectx = xctx.apxjsectx[i];
		if(pjsectx == 0)
			continue;
		memcpy(&(pjsectx->xadgconf),
			&(ptkctx->ppxadgconfs[ptkctx->ncurrxadgconf][i]),
			sizeof(XJSEXADGCONF));

		//pjsectx->ptkctx = ptkctx;
		pjsectx->pxpnetctx = pxpnctx;

		// exec onopen
		xr1 = xgg_exec_onopen(pjsectx);
		if(!XJSE_IS_SUCCESS(xr1)) {
			XJSE_TRACE("(E) xgg_exec_onopen() failed!");
		}

		apjsectx[i] = pjsectx;
	}

cleanup:
	xjse_xadgprof_ctx_cleanup(&xctx);

	return  xr1;

failed:
	XJSE_TRACE("(E) xjse_xadgprof_load() failed!");
	goto	cleanup;
}

