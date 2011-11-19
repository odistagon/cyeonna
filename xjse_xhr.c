// XMLHttpRequest

#ifdef	XTPF_PSP
#include <psptypes.h>
#endif	//XTPF_PSP
#include "xjse.h"
#include "xgg/xgg.h"

typedef	struct __tag_xhrpriv {
	XJSE_XDOMPRIV		xdom;

	xjse_int_t			nresultcode;
	xjse_size_t			ncontentlength;
	xjse_int_t			nconttype;
	char				*pszurl;
	char				*pszresponsetext;
}	XJSE_XHRPRIV;

static void	xjse_xhrresstream_dtor(JSContext *pcxa, JSObject *pobja) ;

	// ----8< ---- simple data container class

static JSClass class_xhrresstream = {
	"_XHRResponseStream", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xjse_xhrresstream_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xhrresstream[] = {
//	{	"data",					21, (0),
//		xjse_xhrresstream_data, (JSPROP_READONLY),	},
	{	0,	},
};

JSFunctionSpec	funcs_xhrresstream[] = {
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

void xpnet_butback_httpresult(XJSE_XHRPRIV* pxhrpriva, const xjse_int_t nrescodea, const xjse_int_t ncontentlengtha, const xjse_int_t nconttypea, char* pszresponsetexta)
{
	XJSE_XHRPRIV*	pxhrp0 = (XJSE_XHRPRIV*)pxhrpriva;
	pxhrp0->nresultcode = nrescodea;
	pxhrp0->ncontentlength = ncontentlengtha;
	pxhrp0->nconttype = nconttypea;
	pxhrp0->pszresponsetext = pszresponsetexta;
}

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

static JSBool xjse_xhr_propop_readystate_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XJSE_XHRPRIV	*pv0 = (XJSE_XHRPRIV*)JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xhr_propop_readystate_get. (%p)", pv0->pszresponsetext);

	if(pv0->pszresponsetext == 0)
		*vp = INT_TO_JSVAL(0);
	else
		*vp = INT_TO_JSVAL(4);

	return  JS_TRUE;
}

static JSBool xjse_xhr_propop_status_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XJSE_XHRPRIV	*pv0 = (XJSE_XHRPRIV*)JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xhr_propop_status_get. (%d)", pv0->nresultcode);

	*vp = INT_TO_JSVAL(pv0->nresultcode);

	return  JS_TRUE;
}

/**
 */
static JSBool xjse_xhr_propop_responsestream_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XJSE_XHRPRIV*	pv0 = (XJSE_XHRPRIV*)JS_GetPrivate(pcxa, pobja);

	//XJSE_TRACE("(X) XXXX responseStream [%d](%d bytes)",
	//	pv0->nconttype, pv0->ncontentlength);

	JSObject*	po1 = JS_NewObject(pcxa, &class_xhrresstream, 0, pobja);
	jsval		v0 = INT_TO_JSVAL(pv0->ncontentlength);
	JS_SetProperty(pcxa, po1, "contentlength", &v0);
	v0 = STRING_TO_JSVAL(JS_NewStringCopyZ(pcxa, pv0->pszurl));
	JS_SetProperty(pcxa, po1, "uri", &v0);
	if(pv0->nconttype == XPNETC_CONTTYPE_IMGJPEG) {
		v0 = STRING_TO_JSVAL(JS_NewStringCopyZ(pcxa, "image/jpeg"));
		JS_SetProperty(pcxa, po1, "contenttype", &v0);
	}
	JS_SetPrivate(pcxa, po1, pv0->pszresponsetext);
	*vp = OBJECT_TO_JSVAL(po1);

	return	JS_TRUE;
}

/**
 */
static JSBool xjse_xhr_propop_responsetext_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XJSE_XHRPRIV*	pv0 = (XJSE_XHRPRIV*)JS_GetPrivate(pcxa, pobja);

	JSString*		pstr1 = JS_NewStringCopyZ(pcxa, pv0->pszresponsetext);
	*vp = STRING_TO_JSVAL(pstr1);

	return	JS_TRUE;
}

/**
 */
static JSBool xjse_xhr_propop_responsexml_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	JSBool			b1 = JS_TRUE;
	XJSE_XHRPRIV*	pv0 = (XJSE_XHRPRIV*)JS_GetPrivate(pcxa, pobja);
	XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);

	xjse_result_t	xr1 = xjse_xdom_parsexml(
		pcxa, pobja, pv0->pszresponsetext, pv0->ncontentlength);
	if(!XJSE_IS_SUCCESS(xr1)) {
		goto	failed;
	}

	jsval	vrootelem;
	b1 = JS_GetElement(pjsectx->pctx, pv0->xdom.ponodelist, 0, &vrootelem);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_GetElement() failed!");
		goto	failed;
	}
	*vp = vrootelem;

cleanup:
	return  b1;

failed:
	goto	cleanup;
}

	// ----8< ---- functions

static JSBool xjse_xhrresstream_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	return  JS_TRUE;
}

static void	xjse_xhrresstream_dtor(JSContext *pcxa, JSObject *pobja)
{
	void	*pv0 = (void*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		xjse_free(0, pv0);
	}
}

static JSBool xjse_xhr_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_TRACE("xhr_ctor!");

	XJSECTX*	pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	if(!XJSE_HAS_PERMS(pjsectx, XJSEC_PERM_XHR)) {
		XJSE_TRACE("(W) context has no perm for xhr.");
		goto	failed;
	}

	XJSE_XHRPRIV	*p0;
	p0 = (XJSE_XHRPRIV*)xjse_malloc(0, sizeof(XJSE_XHRPRIV));
	xjse_memset(0, p0, 0, sizeof(XJSE_XHRPRIV));
	p0->xdom.pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	p0->xdom.pothis = pobja;
	p0->xdom.nelemlevel = -1;	// on_startelement makes this +1
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}

	return  JS_TRUE;

failed:
	return  JS_FALSE;
}

static void	xjse_xhr_dtor(JSContext *pcxa, JSObject *pobja)
{
	XJSE_XHRPRIV	*pv0 = (XJSE_XHRPRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		XJSE_TRACE("xhr_dtor: freeing private obj ptr [0x%p]", pv0);
		xjse_free(0, pv0->pszurl);
		xjse_free(0, pv0);
	}
}

static JSBool xjse_xhr_abort(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	return  JS_TRUE;
}

static JSBool xjse_xhr_open(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*pszurl = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xhr_open (%s)", pszurl);

	XJSE_XHRPRIV	*pv0 = (XJSE_XHRPRIV*)JS_GetPrivate(pcxa, pobja);
	xjse_free(0, pv0->pszurl);
	pv0->pszurl = (char*)xjse_malloc(0, xjse_strlen(0, pszurl) + 1);
	xjse_strcpy(0, pv0->pszurl, pszurl);

	return  JS_TRUE;
}

#if	0
/**
 */
static xjse_result_t xjse_xpnet_cb_header(char* pszdata, xjse_int_t nbytes, xjse_int_t nrescode, void* puserdata)
{
	XJSE_XHRPRIV	*pv0 = (XJSE_XHRPRIV*)puserdata;

	pv0->nresultcode = nrescode;

//cleanup:
	return  XJSE_SUCCESS;
}

/**
 */
static xjse_result_t xjse_xpnet_cb_data(char* pszdata, xjse_int_t nbytes, xjse_int_t nrestsize, void* puserdata)
{
	XJSE_XHRPRIV	*pv0 = (XJSE_XHRPRIV*)puserdata;

	if(nbytes == 0) {
		pv0->ncontentlength = strlen(pv0->pszresponsetext);
		XJSE_TRACE("loading done, %d bytes.", pv0->ncontentlength);
		//XJSE_TRACE(pv0->pszresponsetext);
		goto	cleanup;
	}

	pszdata[nbytes] = 0;

	// extending string (unefficient)
	if(pv0->pszresponsetext != 0) {
		xjse_int_t	n0 = strlen(pv0->pszresponsetext);
		char*		psz0 = (char*)malloc(n0 + nbytes + 1);
		memcpy(psz0, pv0->pszresponsetext, n0);
		memcpy(psz0 + n0, pszdata, nbytes);
		psz0[n0 + nbytes] = 0;
		free(pv0->pszresponsetext);
		pv0->pszresponsetext = psz0;
	} else {
		pv0->pszresponsetext = (char*)malloc(nbytes + 1);
		memcpy(pv0->pszresponsetext, pszdata, nbytes);
		pv0->pszresponsetext[nbytes] = 0;
	}
	//XJSE_TRACE("(X) xhr: xjse_xpnet_cb_data() [%s]", pszdata);

cleanup:
	return  XJSE_SUCCESS;

//failed:
//	goto	cleanup;
}
#endif	//0


static JSBool xjse_xhr_send(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	xjse_result_t	b1;
	XJSE_XHRPRIV	*pv0 = (XJSE_XHRPRIV*)JS_GetPrivate(pcxa, pobja);

	if(pv0 == 0 || (pv0 != 0 && pv0->pszurl == 0)) {
		XJSE_TRACE("url is not set! (call Open() before Send())");
		return	JS_FALSE;
	}

	// here, just queue the request. result will be polled in each game loop.
	b1 = xjse_xpnet_httpreq_send_queue(
		pv0->xdom.pjsectx->pxpnetctx, pv0->pszurl,
		pv0->xdom.pjsectx->pctx, pobja, pv0);

#if	0
	XJSE_XPNET_CALLBACKS	afcbs = {
		xjse_xpnet_cb_header, xjse_xpnet_cb_data,
	};
	b1 = xjse_xpnet_httprequest(pv0->pszurl, &afcbs, pv0);
	if(!XJSE_IS_SUCCESS(b1)) {
		XJSE_TRACE("xjse_xpnet_httprequest failed!");
	}

	// assume response has been loaded, exec onreadystatechange.
	jsval	v1;
	b1 = JS_GetProperty(pcxa, pobja, "onreadystatechange", &v1);
	if(b1 == JS_TRUE) {
		jsval	rv0;
		b1 = JS_CallFunctionValue(pcxa, pobja, v1, 0, 0, &rv0);
XJSE_TRACE("(X) xjse_xhr_send() JS_CallFunctionValue() returned (%s).", JS_GetStringBytes(JS_ValueToString(pcxa, rv0)));
	}

cleanup:
#endif	//0
	return  JS_TRUE;
}

	// ----8< ---- class definitions

static JSClass class_xhr = {
	"XMLHttpRequest", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xjse_xhr_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xhr[] = {
	{	"onreadystatechange",	21, (0),
		JS_PropertyStub, JS_PropertyStub,	},
	{	"readyState",			22, (JSPROP_READONLY),
		xjse_xhr_propop_readystate_get, NULL,	},
	{	"responseBody",			23, (JSPROP_READONLY),
		xjse_x_propop_notimpl, NULL,	},
	{	"responseStream",		24, (JSPROP_READONLY),
		xjse_xhr_propop_responsestream_get, NULL,	},
	{	"responseText",			25, (JSPROP_READONLY),
		xjse_xhr_propop_responsetext_get, NULL,	},
	{	"responseXML",			26, (JSPROP_READONLY),
		xjse_xhr_propop_responsexml_get, NULL,	},
	{	"status",				27, (JSPROP_READONLY),
		xjse_xhr_propop_status_get, NULL,	},
	{	"statusText",			28, (JSPROP_READONLY),
		xjse_x_propop_notimpl, NULL,	},
	{	0,	},
};

JSFunctionSpec	funcs_xhr[] = {
	{	"abort", 					xjse_xhr_abort,	0, 0, 0,	},
	{	"getAllResponseHeaders",	xjse_x_notimpl,	1, 0, 0,	},
	{	"getResponseHeader",		xjse_x_notimpl,	1, 0, 0,	},
	{	"open",						xjse_xhr_open,	3, 0, 0,	},
	{	"send",						xjse_xhr_send,	1, 0, 0,	},
	{	"setRequestHeader",			xjse_x_notimpl,	1, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xhr_define(JSContext *pcxa, JSObject *poglobal)
{
	JSObject	*pobj_proto2 = JS_InitClass(pcxa,
		poglobal, NULL, &class_xhrresstream, xjse_xhrresstream_ctor, 0,
		props_xhrresstream, funcs_xhrresstream, props_xhrresstream, funcs_xhrresstream);
	if(pobj_proto2 == 0)
		goto	failed;

	JSObject	*pobj_proto1 = JS_InitClass(pcxa,
		poglobal, NULL, &class_xhr, xjse_xhr_ctor, 0,
		props_xhr, funcs_xhr, props_xhr, funcs_xhr);
	if(pobj_proto1 == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

