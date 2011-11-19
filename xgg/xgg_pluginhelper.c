// Google Gadget: pluginHelper

#include <xjse.h>
#include <jscntxt.h>	// required for (JSContext*)->fp->flags
#include "xtk.h"
#include "xgg.h"


typedef	struct __tag_xgg_pluginhelper_priv {
	XGGPRIVHDR		hdr;		// all xgg private data must have this
}	XJSE_XGG_PLUGINHELPER_PRIV;

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

static JSBool xgg_pluginhelper_propop_onaddcustommenuitems_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	//XJSE_XGG_PLUGINHELPER_PRIV	*pv0 = 0;
	//pv0 = JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xgg_pluginhelper_propop_onaddcustommenuitems_get. (%s)", psz1);

	return  JS_TRUE;
}

	// ----8< ---- functions

static void	xgg_pluginhelper_dtor(JSContext *pcxa, JSObject *pobja) ;

static JSClass class_xgg_pluginhelper = {
	"pluginHelper", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub,
	JS_ConvertStub, xgg_pluginhelper_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool xgg_pluginhelper_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_XGG_PLUGINHELPER_PRIV	*p0 = 0;

	if(pcxa->fp->flags & JSFRAME_CONSTRUCTING) {
		JSObject*	pobj1 = 0;
		p0 = (XJSE_XGG_PLUGINHELPER_PRIV*)xjse_malloc(
			0, sizeof(XJSE_XGG_PLUGINHELPER_PRIV));
		xjse_memset(0, p0, 0, sizeof(XJSE_XGG_PLUGINHELPER_PRIV));
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

static void	xgg_pluginhelper_dtor(JSContext *pcxa, JSObject *pobja)
{
	XJSE_XGG_PLUGINHELPER_PRIV	*pv0 = 0;
	pv0 = (XJSE_XGG_PLUGINHELPER_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		XJSE_TRACE("xgg_pluginhelper_dtor: freeing private obj ptr [0x%p]", pv0);
		//XJSE_SAFENNP(pv0->pszurl, free);
		xjse_free(0, pv0);
	}
}

#if	0
static JSBool xgg_pluginhelper_add(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_pluginhelper_add (%s)", psz1);

	XJSE_XGG_PLUGINHELPER_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSE_SAFENNP(pv0->pszurl, free);
	//pv0->pszurl = (char*)xjse_malloc(0, strlen(pszurl) + 1);
	//strcpy(pv0->pszurl, pszurl);

	return  JS_TRUE;
}
#endif	//0

	// ----8< ---- class definition

JSPropertySpec	props_xgg_pluginhelper[] = {
	{	"onAddCustomMenuItems",		61, (JSPROP_ENUMERATE),
		xgg_pluginhelper_propop_onaddcustommenuitems_get, NULL,	},
	{	0,	},
};

JSFunctionSpec	funcs_xgg_pluginhelper[] = {
//	{	"RemoveMe",				xjse_x_notimpl,	1, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_pluginhelper(XJSECTX* pctxa)
{
	JSObject	*pobj_proto1 = JS_InitClass(pctxa->pctx,
		pctxa->poglobal, NULL, &class_xgg_pluginhelper, xgg_pluginhelper_ctor, 0,
		props_xgg_pluginhelper, funcs_xgg_pluginhelper,
		props_xgg_pluginhelper, funcs_xgg_pluginhelper);	// static
	if(pobj_proto1 == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*   xjse_xgg_new_pluginhelper(XJSECTX* pctxa, const char* pszobjname)
{
    JSObject*   pobj = JS_DefineObject(pctxa->pctx, pctxa->poglobal,
        pszobjname, &class_xgg_pluginhelper, 0,
        JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);

    return  pobj;
}

