/** xpnet: networking
 * rule: only sub thread does apcon and socket jobs.
 * rule: xpnet has xhr job slot. xpnet has a special thread. main thread only
 *   set jobs into slot, and sub thread only exec jobs and mark them completed.
 *   main thread searches completed jobs every game loop and call js with each
 *   found object.
 */

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#ifdef	XTPF_Linux
#include <pthread.h>
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#include <pspsdk.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <psputility_netmodules.h>
#endif	//XTPF_PSP
#include "xjse.h"


// remember the time last comm'ed.
#define	XPNET_UPDATELASTCOMM(pxpnctx)	xjse_time_now(&(pxpnctx->tlastcomm))

// item state
#define	XPNREQQITEMSTATE_EMPTY		(0)
#define	XPNREQQITEMSTATE_SENDREADY	(1)
#define	XPNREQQITEMSTATE_SENT		(2)
#define	XPNREQQITEMSTATE_RESPREADY	(3)
// will be disposed because profile has been changed
#define	XPNREQQITEMSTATE_OBSOLETED	(4)
#define	XPNREQQITEMSTATE_NEED_RESET(state)	\
	(state == XPNREQQITEMSTATE_SENDREADY || state == XPNREQQITEMSTATE_SENT	\
		|| state == XPNREQQITEMSTATE_RESPREADY)

/** wait until param becomes the target value.
 * @param uset 0: break when flag is cleared, !0: break when flag is set
 */
static xjse_result_t xpnet_pollwait_flag(xjse_uint_t* puflags, xjse_uint_t uflagtarget, xjse_uint_t uset, xjse_timeout_t toduration, xjse_timeout_t totimeout)
{
	xjse_uint_t	ucnt = 0;

	while(1) {
		//XJSE_TRACE(". (%d)", *puflags);

		if((*puflags & uflagtarget) == (uset == 0 ? 0 : uflagtarget)) {
			//XJSE_TRACE("flag's on!! (%d)", pxpnctx->flags);
			break;
		}

		if(totimeout > 0 && (++ucnt > totimeout / toduration)) {
			XJSE_TRACE("(W) xpnet_pollwait_flag(%d, %d) timedout.",
				uflagtarget, uset);
			break;
		}

		xjse_sleep(toduration);
	}

	return	XJSE_SUCCESS;
}

/**
 * NOTE: must be called from xpnet thread! (avoid racing)
 */
static xjse_result_t xpnet_apctl_connect(XPNETCTX* pxpnctx)
{
#ifdef	XTPF_PSP
#define	XPNET_APCTL_RETRYDURA	(1300)
#define	XPNET_APCTL_RETRYCNT	(3)
	xjse_int_t	nerrcnt = 0;
	int			nsceerr = 0;
	do {
		nsceerr = sceNetApctlConnect(pxpnctx->nap);
		if(nsceerr == 0)
			break;
		XJSE_TRACE("(X) sceNetApctlConnect() retrying apcon... (%d, %d)",
			nsceerr, nerrcnt);
		xjse_sleep(XPNET_APCTL_RETRYDURA);
	} while(++nerrcnt < XPNET_APCTL_RETRYCNT);
	if(nsceerr != 0) {
		XJSE_TRACE("(E) sceNetApctlConnect() failed!");
		goto	failed;
	}
#undef	XPNET_APCTL_RETRYDURA
#undef	XPNET_APCTL_RETRYCNT
#endif	//XTPF_PSP

	XPNET_UPDATELASTCOMM(pxpnctx);
	pxpnctx->napconstatus = XPNETAPCTLSTAT_ATTEMPT;

	return	XJSE_SUCCESS;

#ifdef	XTPF_PSP
failed:
	return	XJSE_E_UNKNOWN;
#endif	//XTPF_PSP
}

static xjse_result_t xpnet_apctl_disconnect(XPNETCTX* pxpnctx)
{
#ifdef	XTPF_PSP
	sceNetApctlDisconnect();
#endif	//XTPF_PSP
	pxpnctx->napconstatus = XPNETAPCTLSTAT_NOTCONN;

	return	XJSE_SUCCESS;
}

static xjse_result_t xpnet_pollwait_apcon(XPNETCTX* pxpnctx)
{
#define	XPNET_APCTLCONN_TIMEDOUT	(12000)
#define	XPNET_APCTLCONN_POLLDURA	(300)
#ifdef	XTPF_Linux
	// simulate apcon wait
	xjse_time_t	tnow;
	while(1) {
		xjse_time_now(&tnow);
		if(tnow > pxpnctx->tlastcomm + 3 * 1000) {
			XJSE_TRACE("(X) exiting hoax apcon wait.(%lld, %lld)",
				tnow, pxpnctx->tlastcomm);
			break;
		}
		xjse_sleep(XPNET_APCTLCONN_POLLDURA);
	}
	pxpnctx->napconstatus = XPNETAPCTLSTAT_CONNED;
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	xjse_int_t	ncnt = 0, nsceerr = 0, nstatelast = -1;
	while(1) {
		nsceerr = sceNetApctlGetState(&(pxpnctx->napconstatus));
		if(nsceerr != 0) {
			XJSE_TRACE("(E) sceNetApctlGetState() failed!");
			goto	failed;
		}
		if(pxpnctx->napconstatus > nstatelast) {
			XJSE_TRACE("(X) connecting... (%d -> %d)",
				nstatelast, pxpnctx->napconstatus);
			nstatelast = pxpnctx->napconstatus;
		}
		if(pxpnctx->napconstatus == XPNETAPCTLSTAT_CONNED)
			break;	// connected
		sceKernelDelayThread(XPNET_APCTLCONN_POLLDURA * 1000);

		if(ncnt++ > (XPNET_APCTLCONN_TIMEDOUT / XPNET_APCTLCONN_POLLDURA)) {
			XJSE_TRACE("(E) connection timed out!");
			goto	failed;
		}
	}

#if	1
	char	szipaddr[32];
	if(sceNetApctlGetInfo(8, szipaddr) != 0) {
		XJSE_TRACE("(E) no ip address!");
	} else {
		XJSE_TRACE("(X) ip address retrieved: [%s]", szipaddr);
	}
#endif
#endif	//XTPF_PSP

	return	XJSE_SUCCESS;

#ifdef	XTPF_PSP
failed:
	pxpnctx->napconstatus = XPNETAPCTLSTAT_NOTCONN;

	return	XJSE_E_UNKNOWN;
#endif	//XTPF_PSP
}

/**
 */
static xjse_result_t xjse_xpnet_cb_header(char* pszdata, xjse_int_t nbytes, xjse_int_t nrescode, xjse_int_t nconttype, void* puserdata)
{
	XPNETREQQITEM*	pitem0 = (XPNETREQQITEM*)puserdata;
	pitem0->nrescode = nrescode;
	pitem0->nconttype = nconttype;

//cleanup:
	return	XJSE_SUCCESS;
}

/**
 */
static xjse_result_t xjse_xpnet_cb_data(char* pszdata, xjse_int_t nbytes, xjse_int_t nrestsize, void* puserdata)
{
	XPNETREQQITEM	*pitem0 = (XPNETREQQITEM*)puserdata;

	if(nbytes == 0) {
		pitem0->ncontentlength = pitem0->sbytesread;
		XJSE_TRACE("loading done, %d bytes.", pitem0->ncontentlength);
		//XJSE_TRACE(pitem0->pszresponsetext);

		// send back result data to xhr private object
		xpnet_butback_httpresult(pitem0->puserdata,
			pitem0->nrescode, pitem0->ncontentlength,
			pitem0->nconttype, pitem0->pszresponsetext);

		{	//TODO: critical section
			if(pitem0->state == XPNREQQITEMSTATE_OBSOLETED)
				pitem0->state = XPNREQQITEMSTATE_EMPTY;
			else
				pitem0->state = XPNREQQITEMSTATE_RESPREADY;
		}
		//XJSE_TRACE("(X) XXXX pitem (%p)[%d]", pitem0, pitem0->state);
		goto	cleanup;
	}

	// extending string (unefficient)
	if(pitem0->pszresponsetext != 0) {
		char*		psz0 = (char*)malloc(pitem0->sbytesread + nbytes + 1);
		memcpy(psz0, pitem0->pszresponsetext, pitem0->sbytesread);
		memcpy(psz0 + pitem0->sbytesread, pszdata, nbytes);
		psz0[pitem0->sbytesread + nbytes] = 0;
		free(pitem0->pszresponsetext);
		pitem0->pszresponsetext = psz0;
	} else {
		pitem0->pszresponsetext = (char*)malloc(nbytes + 1);
		memcpy(pitem0->pszresponsetext, pszdata, nbytes);
		pitem0->pszresponsetext[nbytes] = 0;
	}

	pitem0->sbytesread += nbytes;
	//XJSE_TRACE("(X) xhr: xjse_xpnet_cb_data() [%d]", nbytes);

cleanup:
	return	XJSE_SUCCESS;

//failed:
//	goto	cleanup;
}

/** xpnet thread function
 * (platform dependent callback function type)
 */
#ifdef	XTPF_Linux
static void* xpnet_cb_thread(void* argp)
{
	XPNETCTX*	pxpnctx = (XPNETCTX*)argp;
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
static xjse_int_t xpnet_cb_thread(SceSize args, void* argp)
{
	XPNETCTX*	pxpnctx = *((XPNETCTX**)argp);
#endif	//XTPF_PSP
	xjse_result_t	xr1;
	xjse_int_t		i;
	xjse_time_t		tnow;
	XJSE_TRACE("(*) XXXX xpnet thread started. thread id: (%d, %p)",
		(xjse_int_t)pxpnctx->threadid, pxpnctx);

	if((pxpnctx->flags & XPNETFLAG_INITDONE) == 0) {
#if	XTPF_PSP
		int	nsceerr = pspSdkInetInit();
		if(nsceerr != 0) {
			XJSE_TRACE("(E) pspSdkInetInit() failed! (%d)", nsceerr);
			goto	failed;
		}
#endif
	}
	pxpnctx->flags |= XPNETFLAG_INITDONE;

//pxpnctx->flags |= XPNETFLAG_INITCONN;	//TODO: read from .conf (for debug)
	if((pxpnctx->flags & XPNETFLAG_INITCONN) != 0
		&& pxpnctx->nap != 0
		&& pxpnctx->napconstatus == XPNETAPCTLSTAT_NOTCONN) {
		xr1 = xpnet_apctl_connect(pxpnctx);
		if(!XJSE_IS_SUCCESS(xr1))
			goto	failed;
	}

	while(1) {
		for(i = 0; i < XPNETC_NMAXREQQITEMS; i++) {
			XPNETREQQITEM*	pitem = &(pxpnctx->xpnetreqq[i]);
			//XJSE_TRACE("(X) state (%d, %p)[%d]", i, pitem, pitem->state);

			switch(pitem->state) {
			case XPNREQQITEMSTATE_SENDREADY:
//XJSE_TRACE("(X) xxx 1.0 state (%d, %p)[%d]", i, pitem, pitem->state);
				// do apcon if not connected yet
				if(pxpnctx->nap == 0) {
					XJSE_TRACE("(X) http req (%d) is canceled (offline)", i);
					pitem->state = XPNREQQITEMSTATE_EMPTY;
					continue;
				}

				if(pxpnctx->napconstatus == XPNETAPCTLSTAT_NOTCONN) {
					xr1 = xpnet_apctl_connect(pxpnctx);
					if(!XJSE_IS_SUCCESS(xr1))
						goto	failed;
				}
//XJSE_TRACE("(X) xxx 1.1 state (%d, %p)[%d]", i, pitem, pitem->state);
				xr1 = xpnet_pollwait_apcon(pxpnctx);
				if(!XJSE_IS_SUCCESS(xr1))
					goto	failed;

//XJSE_TRACE("(X) xxx 1.2 state (%d, %p)[%d]", i, pitem, pitem->state);
				{	//TODO: critical section
					if(pitem->state == XPNREQQITEMSTATE_OBSOLETED) {
						pitem->state = XPNREQQITEMSTATE_EMPTY;
						continue;
					}
					// (state might be changed until here, by main thread)
//XJSE_TRACE("(X) xxx 2 state (%d, %p)[%d]", i, pitem, pitem->state);
					pitem->state = XPNREQQITEMSTATE_SENT;
				}

				XJSE_XPNET_CALLBACKS	afcbs = {
					xjse_xpnet_cb_header, xjse_xpnet_cb_data,
				};
				xr1 = xjse_xpnet_httprequest(pitem->pszurl, &afcbs, pitem);
				if(!XJSE_IS_SUCCESS(xr1)) {
					XJSE_TRACE("xjse_xpnet_httprequest failed!");
					pitem->state = XPNREQQITEMSTATE_EMPTY;
					goto	failed;
				}
				XPNET_UPDATELASTCOMM(pxpnctx);
				break;
			}
		}

		// wait exit (flag will be set by main thread)
		if((pxpnctx->flags & XPNETFLAG_EXIT) != 0)
			break;

		// apcon timeout? (specified time elapsed after last comm?)
		xjse_time_now(&tnow);
		if(pxpnctx->napconstatus == XPNETAPCTLSTAT_CONNED &&
			pxpnctx->tlastcomm + 5 * 1000 < tnow) {	//TODO: read from .config
			//XJSE_TRACE("(X) apc timed out. going offline... [%d](%lld, %lld)",
			//	pxpnctx->napconstatus, pxpnctx->tlastcomm, tnow);
			XPNET_UPDATELASTCOMM(pxpnctx);
			xr1 = xpnet_apctl_disconnect(pxpnctx);
		}

		xjse_sleep(400);
	}

	XJSE_TRACE("(X) xpnet thread is being shut down...");
	pxpnctx->flags ^= XPNETFLAG_EXIT;	// main thread waits this

failed:
	XJSE_TRACE("(E) xpnet thread is being shut down with error...");

	return	0;
}

/**
 */
static xjse_result_t	xjse_xpnet_startthread(XPNETCTX* pxpnctx)
{
	XJSE_TRACE("(*) XXXX starting xpnet thread: (%p)", pxpnctx);
	// create xpnet controll thread
#ifdef	XTPF_Linux
	xjse_int_t	n1 = pthread_create(&(pxpnctx->threadid), 0,
		xpnet_cb_thread, pxpnctx);
	if(n1 != 0) {
		XJSE_TRACE("(E) pthread_create() failed!");
		goto	failed;
	}
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	pxpnctx->threadid = sceKernelCreateThread("xpnet_thread", xpnet_cb_thread,
		32, 10 * 1024, PSP_THREAD_ATTR_USER, 0);
	if(pxpnctx->threadid < 0) {
		XJSE_TRACE("(E) sceKernelCreateThread() failed!");
		goto	failed;
	}
	sceKernelStartThread(pxpnctx->threadid, sizeof(XPNETCTX**), &pxpnctx);
#endif	//XTPF_PSP

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

/**
 */
xjse_result_t	xjse_xpnet_init(XPNETCTX** ppxpnctx, const xjse_int_t napa)
{
	XPNETCTX*	pctx1 = (XPNETCTX*)xjse_malloc(0, sizeof(XPNETCTX));
	memset(pctx1, 0, sizeof(XPNETCTX));

	memset(&(pctx1->xpnetreqq), 0,
		sizeof(XPNETREQQITEM) * XPNETC_NMAXREQQITEMS);
	//
	pctx1->nap = napa;	// default ap

#ifdef	XTPF_PSP
	sceUtilityLoadNetModule(1);
	sceUtilityLoadNetModule(3);
#endif

	xjse_result_t	xr1 = xjse_xpnet_startthread(pctx1);
	if(!XJSE_IS_SUCCESS(xr1))
		goto	failed;

	*ppxpnctx = pctx1;

	XJSE_TRACE("(X) xjse_xpnet_init successfully done!");
	return	XJSE_SUCCESS;

failed:
	XJSE_TRACE("(E) xjse_xpnet_init failed!");
	XJSE_SAFENNP(pctx1, free);
	return	XJSE_E_UNKNOWN;
}

/** queue the request.
 */
xjse_result_t	xjse_xpnet_httpreq_send_queue(XPNETCTX* pxpnctx, char* pszurl, JSContext* pjsctx, JSObject* poxhr, void* puserdata)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	xjse_int_t		i;
	XPNETREQQITEM*	pitem = 0;

	// find empty item slot in queue
	for(i = 0; i < XPNETC_NMAXREQQITEMS; i++) {
		if(pxpnctx->xpnetreqq[i].state == XPNREQQITEMSTATE_EMPTY) {
			pitem = &(pxpnctx->xpnetreqq[i]);
			break;
		}
	}
	if(pitem == 0) {
		xr1 = XJSE_E_NOEMPTYREQQ;
		goto	failed;
	}

	// set requetst parameters
	memset(pitem, 0, sizeof(XPNETREQQITEM));
	pitem->pszurl = pszurl;
	pitem->pjsctx = pjsctx;
	pitem->poxhr = poxhr;
	pitem->puserdata = puserdata;
	pitem->state = XPNREQQITEMSTATE_SENDREADY;

cleanup:
	return	xr1;

failed:
	goto	cleanup;
}

/** process requests in the queue.
 */
xjse_result_t	xjse_xpnet_do_queue(XPNETCTX* pxpnctx)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	xjse_int_t		i;

	for(i = 0; i < XPNETC_NMAXREQQITEMS; i++) {
		XPNETREQQITEM*	pitem = &(pxpnctx->xpnetreqq[i]);
//XJSE_TRACE("(X) state (%d, %p)[%d]", i, pitem, pitem->state);

		switch(pitem->state) {
		case XPNREQQITEMSTATE_RESPREADY:
		{
			// response has been loaded, exec onreadystatechange.
			jsval	v1;
			JSBool	b1 = JS_GetProperty(
				pitem->pjsctx, pitem->poxhr, "onreadystatechange", &v1);
			if(b1 == JS_TRUE) {
				jsval	rv0;
				b1 = JS_CallFunctionValue(
					pitem->pjsctx, pitem->poxhr, v1, 0, 0, &rv0);
				if(b1 != JS_TRUE) {
					XJSE_TRACE("(E) JS_CallFunctionValue() failed!");
					pitem->state = XPNREQQITEMSTATE_EMPTY;
					goto	failed;
				}
XJSE_TRACE("(X) xjse_xhr_send() JS_CallFunctionValue() returned (%s).", JS_GetStringBytes(JS_ValueToString(pitem->pjsctx, rv0)));
			}
			//
			pitem->state = XPNREQQITEMSTATE_EMPTY;
		}
			break;
		case XPNREQQITEMSTATE_OBSOLETED:
			XJSE_TRACE("(X) xjse_xpnet_do_queue(): obs'ed req detected. "
				"(idx: %d)", i);
			pitem->state = XPNREQQITEMSTATE_EMPTY;
			break;
		}
	}

	return	XJSE_SUCCESS;

failed:
	return	xr1;
}

/**
 */
xjse_result_t	xjse_xpnet_cleanup(XPNETCTX* pxpnctx)
{
	// terminate xpnet thread
	pxpnctx->flags |= XPNETFLAG_EXIT;
	// wait until thread exits
	xpnet_pollwait_flag(&(pxpnctx->flags), XPNETFLAG_EXIT, 0, 100, 4000);
	xpnet_apctl_disconnect(pxpnctx);
#ifdef	XTPF_PSP
	sceNetInetTerm();
	//sceUtilityUnloadNetModule(3);	// these lines causes crash
	//sceUtilityUnloadNetModule(1);	// these lines causes crash
#endif	//XTPF_PSP
	XJSE_SAFENNP(pxpnctx, free);

	return	XJSE_SUCCESS;
}

/**
 */
xjse_result_t	xjse_xpnet_reset(XPNETCTX* pxpnctx)
{
// marking items is likely not enough, so terminate and restart threads.
#define	TERMINATEXPNETTHREADONSWITHPROF
#ifdef	TERMINATEXPNETTHREADONSWITHPROF
	// terminate xpnet thread
	pxpnctx->flags |= XPNETFLAG_EXIT;
	// wait until thread exits
	xpnet_pollwait_flag(&(pxpnctx->flags), XPNETFLAG_EXIT, 0, 100, 4000);
	//pxpnctx->flags ^= XPNETFLAG_EXIT;	// this should have been done by thread
#endif	//TERMINATEXPNETTHREADONSWITHPROF

	xjse_int_t	i;
	for(i = 0; i < XPNETC_NMAXREQQITEMS; i++) {
		XPNETREQQITEM*	pitem = &(pxpnctx->xpnetreqq[i]);
		if(XPNREQQITEMSTATE_NEED_RESET(pitem->state)) {
			pitem->state = XPNREQQITEMSTATE_OBSOLETED;
			pitem->pszurl = 0;
XJSE_TRACE("(X) reset(): req [%d] marked as obs'ed, (%d)", i, pitem->state);
		}
	}

#define	TERMINATEXPNETTHREADONSWITHPROF
#ifdef	TERMINATEXPNETTHREADONSWITHPROF
	//
	xjse_result_t	xr1 = xjse_xpnet_startthread(pxpnctx);
XJSE_TRACE("(X) xpnet thread reastarted.(%d)", xr1);
	if(!XJSE_IS_SUCCESS(xr1))
		goto	failed;

failed:
	return	XJSE_E_UNKNOWN;
#endif	//TERMINATEXPNETTHREADONSWITHPROF

	return	XJSE_SUCCESS;
}

/** parse and break url down to fqdn and uri.
 */
static xjse_result_t xjse_xpnet_parseurl(char* pszfqdn, char* pszuri, char* pszsrc)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	//xjse_int_t		i = 0;

	if(strncmp(pszsrc, "http://", 7) != 0) {
		xr1 = XJSE_E_URLNOTSPRTED;
		goto	failed;
	}
	pszsrc += 7;

	// FQDN
	while(*pszsrc != 0 && *pszsrc != ':' && *pszsrc != '/') {
		*pszfqdn++ = *pszsrc++;
	}
	*pszfqdn = 0;

	// path, params
	while(*pszsrc != 0) {
		*pszuri++ = *pszsrc++;
	}
	*pszuri = 0;

failed:
	return	xr1;
}

/**
 */
static xjse_result_t xjse_xpnet_parsehttpheader(const char* pszdata, xjse_size_t* pnheaderbytes, xjse_int_t* pnrescode, xjse_int_t* pnconttype)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	char*			psz1 = (char*)pszdata;

	psz1[9 + 3] = 0;
//	if(memcmp(psz1 + 9, "200", 3) != 0)	// HTTP/1.x 200 OK
//		;
	XJSE_TRACE("(X) http result is: [%s]", psz1 + 9);
	*pnrescode = atoi(psz1 + 9);

	do {
		while(*psz1 != '\n' && *psz1 != 0)
			psz1++;

		psz1++;
		if(*(psz1 + 1) == '\n') {
		//if(*(psz1 + 0) == '\r' && *(psz1 + 1) == '\n') {
			*pnheaderbytes = (psz1 - pszdata + 2);
			break;
		}

		if(memcmp(psz1, "Content-Type: ", 14) == 0) {
			char	szct[XPNETC_NMAX_CONTTYPE + 1];
			xjse_int_t	i;
			for(i = 0; i < XPNETC_NMAX_CONTTYPE; i++) {
				char	c0 = psz1[14 + i];
				if(c0 == '\r' || c0 == '\n' || c0 == 0)
					break;
				szct[i] = c0;
			}
			szct[i] = 0;
			psz1 += i;
			XJSE_TRACE("(X) Content-Type: [%s]", szct);
			if(memcmp(szct, "text/xml", 8) == 0) {
				*pnconttype = XPNETC_CONTTYPE_TEXTXML;
			} else
			if(memcmp(szct, "image/jpeg", 10) == 0) {
				*pnconttype = XPNETC_CONTTYPE_IMGJPEG;
			}
		}
		if(memcmp(psz1, "Set-Cookie: ", 12) == 0) {
			XJSE_TRACE("(X) Set-Cookie found!");
		}
	} while(1);

	return	xr1;
}

/**
 */
//#define	XPNETLOCALTEXT
#ifndef	XPNETLOCALTEXT
xjse_result_t	xjse_xpnet_httprequest(char* pszurl, XJSE_XPNET_CALLBACKS* pfcbs, void* puserdata)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	xjse_int_t		nerr = 0, mFd = 0, nrecv = 0, nrescode = 0, nconttype = 0;
	struct sockaddr_in	s1;
	struct hostent	*phe1 = 0;
	char			szfqdn[128], szuri[1024], szreq[2048];
	XJSE_TRACE("(X) XXXX xjse_xpnet_httprequest start! [%s]", pszurl);

	xr1 = xjse_xpnet_parseurl(szfqdn, szuri, pszurl);
	if(!XJSE_IS_SUCCESS(xr1)) {
		XJSE_TRACE("(E) xhr: parseurl failed: [%s][%s, %s]",
			pszurl, szfqdn, szuri);
		goto	failed;
	}
	XJSE_TRACE("(X) XXXX url parsed: [%s, %s]", szfqdn, szuri);
	//XJSE_TRACE("(X) XXXX gethostbyname(%s)...", szfqdn);
	//phe1 = gethostbyname("www.google.com");
	phe1 = gethostbyname(szfqdn);
	if(phe1 == 0) {
		XJSE_TRACE("(E) XXXX gethostbyname failed!");
		//if(XJSE_IS_IPADDRESS()) {
		//}
		goto	failed;
	}
	XJSE_TRACE("(X) XXXX gethostbyname success!");

	//mFd = sceNetInetSocket(PF_INET, SOCK_STREAM, 0);
	mFd = socket(PF_INET, SOCK_STREAM, 0);
	if(mFd < 0)
		goto	failed;
	//XJSE_TRACE("(X) XXXX socket created: [%d]", mFd);

	memset(&s1, 0, sizeof(s1));
	s1.sin_family = AF_INET;
	s1.sin_port = htons(80);
	memcpy(&(s1.sin_addr),
		(struct in_addr**)phe1->h_addr_list[0], sizeof(struct in_addr));
	//inet_aton("66.249.89.147", &s1.sin_addr);	// www.google.com
	//inet_aton("127.0.0.1", &s1.sin_addr);
	//nerr = sceNetInetConnect(mFd, (struct sockaddr*)&s1, sizeof(s1));
	nerr = connect(mFd, (struct sockaddr*)&s1, sizeof(s1));
	//XJSE_TRACE("(X) XXXX connect returned: [%d]", nerr);

	//strcpy(szreq, "GET / HTTP/1.0\n\n");
	xjse_snprintf(0, szreq, sizeof(szreq) - 1, "GET %s HTTP/1.0\r\n\r\n", szuri);
	//nerr = sceNetInetSend(mFd, szreq, strlen(szreq), 0);
	nerr = send(mFd, szreq, strlen(szreq), 0);
	XJSE_TRACE("(X) XXXX send returned: [%d]", nerr);

	memset(szreq, 0, sizeof(szreq));
	do {
		//nerr = sceNetInetRecv(mFd, szreq, sizeof(szreq) - 1, 0);
		nerr = recv(mFd, szreq, sizeof(szreq) - 1, 0);
		szreq[nerr] = 0;

		// header parse & callback
		//TODO: header size > buffer size (when cannot read at once)
		xjse_size_t	nbytesheader = 0;
		if(nrecv == 0) {
			xjse_xpnet_parsehttpheader(szreq, &nbytesheader, &nrescode, &nconttype);
			if(pfcbs != 0 && pfcbs->onheader)
				pfcbs->onheader(szreq, nbytesheader, nrescode, nconttype, puserdata);
		}

		// data callback
		if(pfcbs != 0 && pfcbs->ondata
			&& !(nrecv == 0 && nerr <= nbytesheader)) {	//first packet == header
			pfcbs->ondata(
				szreq + nbytesheader, nerr - nbytesheader, -1, puserdata);
		}

		nrecv += nerr;
		//XJSE_TRACE("(X) XXXX recv returned: (%d)[%s]", nerr, szreq);
	} while(nerr != 0);

cleanup:
	//XJSE_SAFENNP(mFd, sceNetInetClose);
	XJSE_SAFENNP(mFd, close);
	return	xr1;

failed:
	XJSE_TRACE("(X) XXXX something went wrong!");
	xr1 = XJSE_E_UNKNOWN;
	goto	cleanup;
}
#else	//XPNETLOCALTEXT
// use local file as http response data (for debug)
xjse_result_t	xjse_xpnet_httprequest(char* pszurl, XJSE_XPNET_CALLBACKS* pfcbs, void* puserdata)
{
	xjse_fd_t	fd1;
	xjse_int_t	nrecv = 0, nrescode = 0, nconttype = 0;
	xjse_size_t	size1 = 0;
	char		buff[4096];

	fd1 = xjse_fopen("xhrsample.xml", XJSE_B_FALSE);
	//if(!XJSE_FD_IS_VALID(fd1))
	//	goto	failed;
	do {
		size1 = xjse_fread(fd1, buff, sizeof(buff));

		// header parse & callback
		xjse_size_t	nbytesheader = 0;
		//TODO: header size > buffer size (when cannot read not at once)
		if(nrecv == 0) {
			xjse_xpnet_parsehttpheader(buff, &nbytesheader, &nrescode, &nconttype);
			if(pfcbs != 0 && pfcbs->onheader)
				pfcbs->onheader(buff, nbytesheader, nrescode, nconttype, puserdata);
		}
		nrecv += size1;

		// data callback
		if(pfcbs != 0 && pfcbs->ondata)
			pfcbs->ondata(
				buff + nbytesheader, size1 - nbytesheader, -1, puserdata);
	} while(size1 != 0);

cleanup:
	xjse_fclose(fd1);

	return	XJSE_SUCCESS;
}
#endif	//XPNETLOCALTEXT

