// Google Gadget: plugin

#include <xjse.h>
#include <jscntxt.h>	// required for (JSContext*)->fp->flags
#include "xtk.h"
#include "xgg.h"


typedef	struct __tag_xgg_plugin_priv {
	XGGPRIVHDR		hdr;		// all xgg private data must have this
}	XJSE_XGG_PLUGIN_PRIV;

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

static JSBool xgg_plugin_propop_flags_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	//XJSE_XGG_PLUGIN_PRIV	*pv0 = 0;
	//pv0 = JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xgg_plugin_propop_flags_get. (%s)", psz1);

	return  JS_TRUE;
}

static JSBool xgg_plugin_propop_on_x_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XJSE_TRACE("xgg_plugin_propop: setting callback: (%s)",
		JS_GetStringBytes(JS_ValueToString(pcxa, id)));

	return  JS_TRUE;
}

	// ----8< ---- functions

static void	xgg_plugin_dtor(JSContext *pcxa, JSObject *pobja) ;

static JSClass class_xgg_plugin = {
	"plugin", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub,
	JS_ConvertStub, xgg_plugin_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool xgg_plugin_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_XGG_PLUGIN_PRIV	*p0 = 0;

	if(pcxa->fp->flags & JSFRAME_CONSTRUCTING) {
		JSObject*	pobj1 = 0;
		p0 = (XJSE_XGG_PLUGIN_PRIV*)xjse_malloc(
			0, sizeof(XJSE_XGG_PLUGIN_PRIV));
		xjse_memset(0, p0, 0, sizeof(XJSE_XGG_PLUGIN_PRIV));
		JSBool	b0 = JS_SetPrivate(pcxa, pobj1, p0);
		if(b0 != JS_TRUE) {
			XJSE_TRACE("JS_SetPrivate failed!");
			goto	failed;
		}
	}

	return  JS_TRUE;

failed:
	XJSE_SAFENNP(p0, free);

	return	JS_FALSE;
}

static void	xgg_plugin_dtor(JSContext *pcxa, JSObject *pobja)
{
	XJSE_XGG_PLUGIN_PRIV	*pv0 = 0;
	pv0 = (XJSE_XGG_PLUGIN_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		XJSE_TRACE("xgg_plugin_dtor: freeing private obj ptr [0x%p]", pv0);
		//XJSE_SAFENNP(pv0->pszurl, free);
		xjse_free(0, pv0);
	}
}

#if	0
static JSBool xgg_plugin_add(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_plugin_add (%s)", psz1);

	XJSE_XGG_PLUGIN_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSE_SAFENNP(pv0->pszurl, free);
	//pv0->pszurl = (char*)xjse_malloc(0, strlen(pszurl) + 1);
	//strcpy(pv0->pszurl, pszurl);

	return  JS_TRUE;
}
#endif	//0

	// ----8< ---- class definition

JSPropertySpec	props_xgg_plugin[] = {
	{	"plugin_flags",		61, (JSPROP_ENUMERATE),
		xgg_plugin_propop_flags_get, NULL,	},
	//{	"window_height",	62, (JSPROP_ENUMERATE | JSPROP_READONLY),	// depr
	//	xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	//{	"window_width",		63, (JSPROP_ENUMERATE | JSPROP_READONLY),	// depr
	//	xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	// events
	{	"onShowOptionsDlg",		71, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xgg_plugin_propop_on_x_set,	},
	{	"onAddCustomMenuItems",	72, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xgg_plugin_propop_on_x_set,	},
	{	"onCommand",			73, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xgg_plugin_propop_on_x_set,	},
	{	"onDisplayStateChange",	74, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xgg_plugin_propop_on_x_set,	},
	{	"onDisplayTargetChange",74, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xgg_plugin_propop_on_x_set,	},
	{	0,	},
};

JSFunctionSpec	funcs_xgg_plugin[] = {
	{	"RemoveMe",				xjse_x_notimpl,	1, 0, 0,	},
	{	"ShowDetailsView",		xjse_x_notimpl,	4, 0, 0,	},
	{	"ClearDetailsView",		xjse_x_notimpl,	0, 0, 0,	},
	{	"ShowOptionsDialog",	xjse_x_notimpl,	0, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_plugin(XJSECTX* pctxa)
{
	JSObject	*pobj_proto1 = JS_InitClass(pctxa->pctx,
		pctxa->poglobal, NULL, &class_xgg_plugin, xgg_plugin_ctor, 0,
		props_xgg_plugin, funcs_xgg_plugin,
		props_xgg_plugin, funcs_xgg_plugin);	// static
	if(pobj_proto1 == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*   xjse_xgg_new_plugin(XJSECTX* pctxa, const char* pszobjname)
{
    JSObject*   pobj = JS_DefineObject(pctxa->pctx, pctxa->poglobal,
        pszobjname, &class_xgg_plugin, 0,
        JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);

    return  pobj;
}

