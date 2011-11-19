// Google Gadget: button

#include <xjse.h>
#include <xtk.h>
#include "xgg.h"

typedef	struct __tag_xgg_button_priv {
	XGGPRIVHDR		hdr;		// all xgg private data must have this
	XJSEXADGELEM	xadgelem;
	xtk_pixbuf_t*	ppixbuf;
}	XGG_BUTTON_PRIV;

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

static JSBool xgg_button_propop_image_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_BUTTON_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xgg_button_propop_src_get. (%s)",
		JS_GetStringBytes(JS_ValueToString(pcxa, vp[0])));

	return	JS_TRUE;
}

static JSBool xgg_button_propop_image_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_BUTTON_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	XJSEXADGELEM*	pe0 = pv0->hdr.pxadgelem;
	XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	JSType			type1 = JS_TypeOfValue(pcxa, vp[0]);

	if(type1 == JSTYPE_VOID)	// "undefined"
		goto	cleanup;

	char*	psz0 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	xjse_result_t	xr1 = xgg_xadget_reload_img(
		pjsectx, &(pv0->ppixbuf), psz0, 0, 0);
	if(xr1 != XJSE_SUCCESS)
		goto	failed;

	if(pe0->fcx == 0.0f)
		pe0->fcx = (xjse_float_t)(pv0->ppixbuf->nimgwidth);
	if(pe0->fcy == 0.0f)
		pe0->fcy = (xjse_float_t)(pv0->ppixbuf->nimgheight);

cleanup:
	return	JS_TRUE;

failed:
	XJSE_SAFENNP(pv0->ppixbuf, xtk_pixbuf_delete);
	return	JS_FALSE;
}

	// ----8< ---- functions

xjse_result_t xgg_button_render(XTKCTX* ptkctxa, XJSECTX* pjsectxa, XGGPRIVHDR* phdra, void* puserdata)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	void*			ptarget = 0;
	XGG_BUTTON_PRIV*	ppriv = (XGG_BUTTON_PRIV*)phdra;
	XJSEXADGELEM*	pelem1 = phdra->pxadgelem;

	if(pelem1->bvisible == XJSE_B_FALSE)
		goto	cleanup;

	xjse_rectf_t	roffsouter, roffsinner;
	xjse_float_t	fframescale;
	xjse_opacity_t	opacity;
	xgg_xadget_apply_trans(ptkctxa, pjsectxa, phdra,
		&roffsouter, &roffsinner, &fframescale, &opacity);

	// focused?
	xjse_bool_t bfocused = (pjsectxa->pggctx->pfocusedelem == &(phdra->te)
		? XJSE_B_TRUE : XJSE_B_FALSE);

	// render if it has image
	if(ppriv->ppixbuf != 0)
		xr1 = xtk_pixbuf_draw(ptarget, pelem1, ppriv->ppixbuf,
			&roffsouter, &roffsinner, 0, fframescale, opacity);

	xr1 = xtk_wireframe_draw(ptarget, pelem1,
            &roffsouter, &roffsinner, fframescale, bfocused);

cleanup:
	return	xr1;
}

static JSBool xgg_button_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XGG_BUTTON_PRIV	*p0;
	p0 = (XGG_BUTTON_PRIV*)xjse_malloc(0, sizeof(XGG_BUTTON_PRIV));
	xjse_memset(0, p0, 0, sizeof(XGG_BUTTON_PRIV));
	p0->hdr.type = XGG_ETYP_BUTTON;
	p0->hdr.pxadgelem = &(p0->xadgelem);
	p0->hdr.pfrender = xgg_button_render;	//xgg_xadget_render_generic;
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}
	xgg_xadgelem_setdefaultvalues(pcxa, pobja, &(p0->xadgelem));

	return	JS_TRUE;
}

static void	xgg_button_dtor(JSContext *pcxa, JSObject *pobja)
{
	XGG_BUTTON_PRIV	*pv0 = 0;
	pv0 = (XGG_BUTTON_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		//XJSE_TRACE("xgg_button_dtor: freeing private obj ptr [0x%p]", pv0);
		XJSE_SAFENNP(pv0->ppixbuf, xtk_pixbuf_delete);
		xjse_free(0, pv0);
	}
}

	// ----8< ---- class definition

static JSClass class_xgg_button = {
	"button", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_button_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xgg_button[] = {
	// basic element
	XGG_BE_JSPROPSPEC_PARTIAL
	// button specific
	{	"disabledImage",	61, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	"downImage",		62, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	"image",			63, (JSPROP_ENUMERATE),
		xgg_button_propop_image_get, xgg_button_propop_image_set,	},
	{	"overImage",		64, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	0,	},
};

JSFunctionSpec	funcs_xgg_button[] = {
	// basic element
	XGG_BE_JSFUNCSPEC_PARTIAL
	// button specific
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_button(XJSECTX* pctxa)
{
	JSObject	*pobj_proto1 = JS_InitClass(pctxa->pctx,
		pctxa->poglobal, NULL, &class_xgg_button, xgg_button_ctor, 0,
		props_xgg_button, funcs_xgg_button,
		props_xgg_button, funcs_xgg_button);
	if(pobj_proto1 == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*	xjse_xgg_new_button(XJSECTX* pctxa, const char* pszobjname)
{
	// create proto
	//JSObject*	pobjbase = xjse_xgg_new_basicelement(pctxa, "tempbe01");
	//if(pobjbase == 0)
	//	return	0;
	JSObject*	pobjbase = 0;
	//
	JSObject*	pobj = JS_DefineObject(pctxa->pctx, pctxa->poglobal,
		pszobjname, &class_xgg_button, pobjbase,
		JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);
	//
	JSBool	b1 = xgg_button_ctor(pctxa->pctx, pobj, 0, 0, 0);	// args, rval
	if(b1 != JS_TRUE) {
		XJSE_TRACE("error: xgg_button_ctor failed!");
	}
	//
	return	pobj;
}

