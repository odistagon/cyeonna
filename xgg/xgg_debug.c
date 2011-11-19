// Google Gadget: gadget.debug

#include <xjse.h>
#include "xtk.h"
#include "xgg.h"


typedef	struct __tag_xgg_debug_priv {
	XGGPRIVHDR		hdr;		// all xgg private data must have this
}	XJSE_XGG_DEBUG_PRIV;

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

	// (gadget.debug has no properties)

	// ----8< ---- functions

static void	xgg_debug_dtor(JSContext *pcxa, JSObject *pobja)
{
}

static JSBool xgg_debug_out(char ckind, JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char		szt1[1023 + 1];
	snprintf(szt1, 1023, "[JS](%c) %s\n", ckind,
		JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0])));
	XJSE_TRACE(szt1);
	XJSECTX*	pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	if(pjsectx->fddebuglog != 0
		&& XJSE_HAS_PERMS(pjsectx, XJSEC_PERM_DEBUGLOG)) {
		xjse_fwrite(pjsectx->fddebuglog, szt1, strlen(szt1));
	}
	return	JS_TRUE;
}

JSBool xgg_debug_error(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	return	xgg_debug_out('E', pcxa, pobja, argc, pva1, rval);
}

JSBool xgg_debug_trace(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	return	xgg_debug_out('X', pcxa, pobja, argc, pva1, rval);
}

JSBool xgg_debug_warning(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	return	xgg_debug_out('W', pcxa, pobja, argc, pva1, rval);
}

#if	0
static JSBool xgg_debug_add(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_debug_add (%s)", psz1);

	XJSE_XGG_DEBUG_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSE_SAFENNP(pv0->pszurl, free);
	//pv0->pszurl = (char*)xjse_malloc(0, strlen(pszurl) + 1);
	//strcpy(pv0->pszurl, pszurl);

	return  JS_TRUE;
}
#endif

	// ----8< ---- class definition

static JSClass class_xgg_debug = {
	"debug", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_debug_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xgg_debug[] = {
	{	0,	},
};

JSFunctionSpec	funcs_xgg_debug[] = {
	{	"error",					xgg_debug_error,	1, 0, 0,	},
	{	"trace",					xgg_debug_trace,	1, 0, 0,	},
	{	"warrning",					xgg_debug_warning,	1, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_debug(XJSECTX* pctxa)
{
	JSObject	*podebug = JS_NewObject(
		pctxa->pctx, &class_xgg_debug, NULL, NULL);
	if(podebug == 0)
		goto	failed;
	JSBool	b1 = JS_DefineFunctions(pctxa->pctx, podebug, funcs_xgg_debug);
	if(b1 != JS_TRUE)
		goto	failed;
	jsval	v1 = OBJECT_TO_JSVAL(podebug);
	b1 = JS_SetProperty(pctxa->pctx, pctxa->poglobal, "debug", &v1);
	if(b1 != JS_TRUE)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

