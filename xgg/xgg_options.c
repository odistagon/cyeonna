// Google Gadget: options

#include <xjse.h>
#include <jscntxt.h>	// required for (JSContext*)->fp->flags
#include "xtk.h"
#include "xgg.h"


typedef	struct __tag_xgg_options_priv {
	XGGPRIVHDR		hdr;		// all xgg private data must have this
}	XJSE_XGG_OPTIONS_PRIV;

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

static JSBool xgg_options_propop_count_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	//XJSE_XGG_OPTIONS_PRIV	*pv0 = 0;
	//pv0 = JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xgg_options_propop_count_get. (%s)", psz1);

	return  JS_TRUE;
}

	// ----8< ---- functions

static void	xgg_options_dtor(JSContext *pcxa, JSObject *pobja) ;

static JSClass class_xgg_options = {
	"options", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub,
	JS_ConvertStub, xgg_options_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool xgg_options_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_XGG_OPTIONS_PRIV	*p0 = 0;

	if(pcxa->fp->flags & JSFRAME_CONSTRUCTING) {
		JSObject*	pobj1 = 0;
		p0 = (XJSE_XGG_OPTIONS_PRIV*)xjse_malloc(
			0, sizeof(XJSE_XGG_OPTIONS_PRIV));
		xjse_memset(0, p0, 0, sizeof(XJSE_XGG_OPTIONS_PRIV));
		JSBool	b0 = JS_SetPrivate(pcxa, pobj1, p0);
		if(b0 != JS_TRUE) {
			XJSE_TRACE("JS_SetPrivate failed!");
			goto	failed;
		}
	} else {
		// come in here when this called 
		// without new operator, just like options("...") 
		JSBool		b1 = JS_TRUE;
		jsval		voptions;
		JSObject*	poglobal = JS_GetGlobalObject(pcxa);
		b1 = JS_GetProperty(pcxa, poglobal, "options", &voptions);
		if(b1 != JS_TRUE)
			goto	failed;
		*rval = voptions;
		//XJSE_TRACE("(X) options() (%d, %p)[%s, %s]", argc, pobja,
		//	JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0])),
		//	JS_GetStringBytes(JS_ValueToString(pcxa, *rval)));
		if(argc > 0) {
			// options("xxxx") = "xxxx";	(left-hand lvalue assignment)
			JS_SetCallReturnValue2(pcxa, pva1[0]);
		}
	}

	return  JS_TRUE;

failed:
	XJSE_SAFENNP(p0, free);

	return	JS_FALSE;
}

static void	xgg_options_dtor(JSContext *pcxa, JSObject *pobja)
{
	XJSE_XGG_OPTIONS_PRIV	*pv0 = 0;
	pv0 = (XJSE_XGG_OPTIONS_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		XJSE_TRACE("xgg_options_dtor: freeing private obj ptr [0x%p]", pv0);
		//XJSE_SAFENNP(pv0->pszurl, free);
		xjse_free(0, pv0);
	}
}

static JSBool xgg_options_add(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_options_add (%s)", psz1);

	XJSE_XGG_OPTIONS_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSE_SAFENNP(pv0->pszurl, free);
	//pv0->pszurl = (char*)xjse_malloc(0, strlen(pszurl) + 1);
	//strcpy(pv0->pszurl, pszurl);

	return  JS_TRUE;
}

/**
 */
static JSBool xgg_options_exists(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	JSBool	b1, bhas;
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));

	b1 = JS_HasProperty(pcxa, pobja, psz1, &bhas);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("JS_HasProerty() failed! (%s)", psz1);
		goto	failed;
	}
	*rval = BOOLEAN_TO_JSVAL(bhas);

	return  JS_TRUE;

failed:
	return  JS_FALSE;
}

/**
 */
static JSBool xgg_options_getvalue(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	JSBool	b1;
	jsval	vr1;
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));
	//XJSE_TRACE("xgg_options_getvalue (%s)", psz1);

	b1 = JS_GetProperty(pcxa, pobja, psz1, &vr1);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("JS_GetProerty() failed! (%s)", psz1);
		goto	failed;
	}
	*rval = vr1;

	return  JS_TRUE;

failed:
	return  JS_FALSE;
}

/**
 */
static JSBool xgg_options_putvalue(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	JSBool	b1;
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));
	XJSE_TRACE("xgg_options_putvalue (%s)", psz1);

	b1 = JS_SetProperty(pcxa, pobja, psz1, &(pva1[1]));
	if(b1 != JS_TRUE) {
		XJSE_TRACE("JS_SetProerty() failed! (%s)", psz1);
		goto	failed;
	}

	return  JS_TRUE;

failed:
	return  JS_FALSE;
}

/** set property value if it's not set.
 */
static JSBool xgg_options_putdefaultvalue(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	JSBool	b1;
	jsval	vr1;
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));
	//XJSE_TRACE("xgg_options_putdefaultvalue (%s)", psz1);

	b1 = JS_GetProperty(pcxa, pobja, psz1, &vr1);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("JS_GetProerty() failed! (%s)", psz1);
		goto	failed;
	}

	// see if the property is set or not
	JSType	type1 = JS_TypeOfValue(pcxa, vr1);
	if(type1 == JSTYPE_VOID) {
		b1 = JS_SetProperty(pcxa, pobja, psz1, &(pva1[1]));
		if(b1 != JS_TRUE) {
			XJSE_TRACE("JS_SetProerty() failed! (%s)", psz1);
			goto	failed;
		}
		//XJSE_TRACE("JS_SetProerty() set property: (%s)", psz1);
	}

	return  JS_TRUE;

failed:
	return  JS_FALSE;
}

	// ----8< ---- class definition

JSPropertySpec	props_xgg_options[] = {
	{	"count",		61, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xgg_options_propop_count_get, NULL,	},
	{	"defaultValue",	62, (0),	//deprecated
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	"item",			63, (0),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
//		xgg_options_propop_item_get, xgg_options_propop_item_set,	},
	{	0,	},
};

JSFunctionSpec	funcs_xgg_options[] = {
	{	"add", 					xgg_options_add,	2, 0, 0,	},
	{	"exists",				xgg_options_exists,	1, 0, 0,	},
	{	"getDefaultValue",		xjse_x_notimpl,	1, 0, 0,	},
	{	"getValue",				xgg_options_getvalue,	1, 0, 0,	},
	{	"putDefaultValue",		xgg_options_putdefaultvalue,	2, 0, 0,	},
	{	"putValue",				xgg_options_putvalue,	2, 0, 0,	},
	{	"remove",				xjse_x_notimpl,	1, 0, 0,	},
	{	"removeAll",			xjse_x_notimpl,	0, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_options(XJSECTX* pctxa)
{
#if	1
	JSObject	*pobj_proto1 = JS_InitClass(pctxa->pctx,
		pctxa->poglobal, NULL, &class_xgg_options, xgg_options_ctor, 0,
		props_xgg_options, funcs_xgg_options,
		props_xgg_options, funcs_xgg_options);	// static
	if(pobj_proto1 == 0)
		goto	failed;
#else
	JSObject*	po0 = JS_DefineFunction(
		pctxa->pctx, pctxa->poglobal, "options", xjse_x_notimpl, 1, 0);
	JS_DefineProperties(pctxa->pctx, po0, props_xgg_options);
	JS_DefineFunctions(pctxa->pctx, po0, funcs_xgg_options);
#endif

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*   xjse_xgg_new_options(XJSECTX* pctxa, const char* pszobjname)
{
    JSObject*   pobj = JS_DefineObject(pctxa->pctx, pctxa->poglobal,
        pszobjname, &class_xgg_options, 0,
        JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);

    return  pobj;
}

