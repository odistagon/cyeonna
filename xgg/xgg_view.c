// Google Gadget: view

#include <xjse.h>
#include <xtk.h>
#include "xgg.h"

typedef	struct __tag_xgg_view_priv {
	XGGPRIVHDR		hdr;		// all xgg private data must have this
	XGGESPEC_VIEW	espec;		//
	JSObject*		poevent;	// event property
}	XGG_VIEW_PRIV;

	// ----8< ---- properties

static JSBool xgg_view_propop_caption_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	//XGG_VIEW_PRIV*	pv0 = (XGG_VIEW_PRIV*)JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xgg_view_propop_caption_get.");

	return  JS_TRUE;
}

static JSBool xgg_view_propop_caption_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	//XGG_VIEW_PRIV*	pv0 = (XGG_VIEW_PRIV*)JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xgg_view_propop_caption_set.");

	return  JS_TRUE;
}

JSBool xgg_global_children(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	//XGG_VIEW_PRIV*	pv1 = ((XGG_VIEW_PRIV*)JS_GetPrivate(pcxa, pobja);
	//XGGPRIVHDR*		phdr1 = &(pv1->hdr);
	XGGPRIVHDR*		phdr1 = (XGGPRIVHDR*)JS_GetPrivate(pcxa, pobja);
	char*			pszarg1 = (argc != 0 ?
			JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0])) : 0);
	JSObject*		poret = 0;
	xjse_int_t		nidx = 0;

	//XJSE_TRACE("(X) XXXX xgg_global_children() 1 (%p)[%d, %s](%d)",
	//	phdr1->te.pfirstchild, argc, pszarg1, phdr1->type);
	if(argc == 0) {
		poret = JS_NewArrayObject(pcxa, 0, 0);
	}

	XJSE_TREEELEM*	pe0 = phdr1->te.pfirstchild;
	while(pe0 != 0) {
		if(pszarg1 != 0) {
			if(pe0->pszname != 0 && strcmp(pszarg1, pe0->pszname) == 0) {
				poret = pe0->poself;
				break;
			}
		} else {
			jsval	v0 = OBJECT_TO_JSVAL(pe0->poself);
			JS_SetElement(pcxa, poret, nidx++, &v0);
		}

		if(pe0->pnextsibling == 0)
			break;
		pe0 = pe0->pnextsibling;
	}

	*rval = OBJECT_TO_JSVAL(poret);

	return  JS_TRUE;
}

JSBool xgg_global_propop_children_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	//XGG_VIEW_PRIV*	pv1 = ((XGG_VIEW_PRIV*)JS_GetPrivate(pcxa, pobja);
	//XGGPRIVHDR*		phdr1 = &(pv1->hdr);
	XGGPRIVHDR*		phdr1 = (XGGPRIVHDR*)JS_GetPrivate(pcxa, pobja);
	JSBool			b1;

	JSFunction*	pf1 = JS_NewFunction(pcxa, xgg_global_children, 1, 0, 0, "");
	JSObject*	poar1 = JS_GetFunctionObject(pf1);
	if(poar1 == 0) {
		XJSE_TRACE("(E) JS_GetFunctionObject() failed!");
		goto	failed;
	}
	b1 = JS_SetParent(pcxa, poar1, pobja);
	if(poar1 == 0) {
		XJSE_TRACE("(E) JS_SetParent() failed!");
		goto	failed;
	}

	XJSE_TREEELEM*	pe0 = phdr1->te.pfirstchild;
	while(pe0 != 0) {
		jsval		v1 = OBJECT_TO_JSVAL(pe0->poself);
		if(pe0->pszname != 0) {
			b1 = JS_SetProperty(pcxa, poar1, pe0->pszname, &v1);
			if(b1 != JS_TRUE) {
				XJSE_TRACE("(E) JS_SetElement() failed!");
				goto	failed;
			}
		}
		if(pe0->pnextsibling == 0)
			break;
		pe0 = pe0->pnextsibling;
	}

	*vp = OBJECT_TO_JSVAL(poar1);

	return  JS_TRUE;

failed:
	return  JS_FALSE;
}

static JSBool xgg_view_propop_event_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_VIEW_PRIV*	pv0 = (XGG_VIEW_PRIV*)JS_GetPrivate(pcxa, pobja);

	if(pv0->poevent == 0) {
		pv0->poevent = JS_NewObject(pcxa, (JSClass*)0, (JSObject*)0, pobja);
		jsval	vevent = OBJECT_TO_JSVAL(pv0->poevent);
		JS_SetProperty(pcxa, pobja, "event", &vevent);
	}

	*vp = OBJECT_TO_JSVAL(pv0->poevent);

	return  JS_TRUE;
}

static JSBool xgg_view_propop_width_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_VIEW_PRIV*	pv0 = (XGG_VIEW_PRIV*)JS_GetPrivate(pcxa, pobja);
	XGGESPEC_VIEW*	pespec = (XGGESPEC_VIEW*)pv0->hdr.pespec;

	JS_ValueToInt32(pcxa, vp[0], &(pespec->nwidth));

	return  JS_TRUE;
}

static JSBool xgg_view_propop_height_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_VIEW_PRIV*	pv0 = (XGG_VIEW_PRIV*)JS_GetPrivate(pcxa, pobja);
	XGGESPEC_VIEW*	pespec = (XGGESPEC_VIEW*)pv0->hdr.pespec;

	JS_ValueToInt32(pcxa, vp[0], &(pespec->nheight));

	return  JS_TRUE;
}

	// ----8< ---- functions

static JSBool xgg_view_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XGG_VIEW_PRIV	*p0;
	p0 = (XGG_VIEW_PRIV*)xjse_malloc(0, sizeof(XGG_VIEW_PRIV));
	xjse_memset(0, p0, 0, sizeof(XGG_VIEW_PRIV));
	p0->hdr.type = XGG_ETYP_VIEW;
	p0->hdr.pespec = &(p0->espec);
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}

	return  JS_TRUE;
}

static void	xgg_view_dtor(JSContext *pcxa, JSObject *pobja)
{
	XGG_VIEW_PRIV*	pv0 = (XGG_VIEW_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		XJSE_TRACE("xgg_view_dtor: freeing private obj ptr [0x%p]", pv0);
		//XJSE_SAFENNP(pv0->pszurl, free);
		xjse_free(0, pv0);
	}
}

static JSBool xgg_view_alert(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_view_alert (%s)", psz1);

	//XGG_VIEW_PRIV*	pv0 = (XGG_VIEW_PRIV*)JS_GetPrivate(pcxa, pobja);

	return  JS_TRUE;
}

	// ----8< ---- class definition

static JSClass class_xgg_view = {
	"view", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_view_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xgg_view[] = {
	{	"caption",		61, (0),
		xgg_view_propop_caption_get, xgg_view_propop_caption_set,	},
	{	"children",		62, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xgg_global_propop_children_get, 0,	},
	{	"event",		63, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xgg_view_propop_event_get, 0,	},
	{	"height",		64, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xgg_view_propop_height_set,	},
	{	"width",		65, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xgg_view_propop_width_set,	},
	{	"resizable",	66, (0),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	"showCaptionAlways",67, (0),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	0,	},
};

JSFunctionSpec	funcs_xgg_view[] = {
	{	"alert",					xgg_view_alert,	1, 0, 0,	},
	{	"appendElement",			xgg_global_insertelement,	1, 0, 0,	},
	{	"beginAnimation",			xgg_global_beginanimation,	4, 0, 0,	},
	{	"cancelAnimation",			xjse_x_notimpl,	1, 0, 0,	},
	{	"clearInterval",			xgg_global_clearinterval,	1, 0, 0,	},
	{	"clearTimeout",				xjse_x_notimpl,	1, 0, 0,	},
	{	"confirm",					xjse_x_notimpl,	1, 0, 0,	},
//	{	"debug",					xjse_debugprint,	1, 0, 0,	},
	{	"insertElement",			xgg_global_insertelement,	2, 0, 0,	},
	{	"removeElement",			xgg_global_removeelement,	1, 0, 0,	},
	{	"resizeBy",					xjse_x_notimpl,	2, 0, 0,	},
	{	"resizeTo",					xjse_x_notimpl,	2, 0, 0,	},
	{	"setTimeout",				xgg_global_settimeout,	2, 0, 0,	},
	{	"SetTimeout",				xgg_global_settimeout,	2, 0, 0,	},
	{	"setInterval",				xgg_global_setinterval,	2, 0, 0,	},
	{	"SetInterval",				xgg_global_setinterval,	2, 0, 0,	},
	{	"strings",					xgg_global_strings,	0, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_view(XJSECTX* pctxa)
{
	JSObject	*pobj_proto1 = JS_InitClass(pctxa->pctx,
		pctxa->poglobal, NULL, &class_xgg_view, xgg_view_ctor, 0,
		props_xgg_view, funcs_xgg_view,
		props_xgg_view, funcs_xgg_view);
	if(pobj_proto1 == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*	xjse_xgg_new_view(XJSECTX* pctxa, const char* pszobjname)
{
	JSBool		b1;
#define XTEMP_VIEWISGLOBAL
#ifndef XTEMP_VIEWISGLOBAL
	JSObject*	pobj = JS_DefineObject(pctxa->pctx, pctxa->poglobal,
		pszobjname, &class_xgg_view, 0,
		JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);
#else //XTEMP_VIEWISGLOBAL
XJSE_TRACE("(X) XXXX cp 1.0 (%x)", pctxa->pctx);
	JSObject*	pobj = JS_NewObject(pctxa->pctx, &class_xgg_view, 0, 0);
	if(pobj == 0)
		goto	failed;
XJSE_TRACE("(X) XXXX cp 2.0 (%x)", pobj);
	b1 = JS_DefineProperties(pctxa->pctx, pobj, props_xgg_view);
	if(b1 != JS_TRUE)
		goto	failed;
XJSE_TRACE("(X) XXXX cp 3.0");
	b1 = JS_DefineFunctions(pctxa->pctx, pobj, funcs_xgg_view);
	if(b1 != JS_TRUE)
		goto	failed;
XJSE_TRACE("(X) XXXX cp 4.0");
#endif //XTEMP_VIEWISGLOBAL
	//
	b1 = xgg_view_ctor(pctxa->pctx, pobj, 0, 0, 0);	// args, rval
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) xgg_view_ctor failed!");
	}

cleanup:
	return	pobj;

failed:
	XJSE_TRACE("(E) xgg_view_ctor failed!");

	goto	cleanup;
}

