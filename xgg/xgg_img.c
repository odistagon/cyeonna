// Google Gadget: img

#include <xjse.h>
#include <xtk.h>
#include "xgg.h"

typedef	struct __tag_xgg_img_priv {
	XGGPRIVHDR		hdr;		// all xgg private data must have this
	XJSEXADGELEM	xadgelem;
	xtk_pixbuf_t*	ppixbuf;
}	XGG_IMG_PRIV;

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

static JSBool xgg_img_propop_src_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_IMG_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xgg_img_propop_src_get. (%s)",
		JS_GetStringBytes(JS_ValueToString(pcxa, vp[0])));

	return	JS_TRUE;
}

static JSBool xgg_img_propop_src_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_IMG_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	XJSEXADGELEM*	pe0 = pv0->hdr.pxadgelem;
	XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	JSType			type1 = JS_TypeOfValue(pcxa, vp[0]);
	char*			pszname = 0;
	xjse_uint8_t*	pdata = 0;
	xjse_size_t		datasize = 0;

	if(type1 == JSTYPE_VOID)	// "undefined"
		goto	cleanup;

	if(type1 == JSTYPE_OBJECT) {	// for xhr.responseStream property
		JSBool			b0;
		jsval			v0;
		JSObject*		po0 = JSVAL_TO_OBJECT(vp[0]);
		b0 = JS_GetProperty(pcxa, po0, "contentlength", &v0);
		if(b0 != JS_TRUE)
			goto	failed;
		datasize = (xjse_size_t)JSVAL_TO_INT(v0);
		b0 = JS_GetProperty(pcxa, po0, "uri", &v0);
		if(b0 != JS_TRUE)
			goto	failed;
		pszname = JS_GetStringBytes(JS_ValueToString(pcxa, v0));
		//b0 = JS_GetProperty(pcxa, po0, "contenttype", &v0);//"image/jpeg"
		//v0 = STRING_TO_JSVAL(JS_NewStringCopyZ(pcxa, "temp.jpg"));
		pdata = (xjse_uint8_t*)JS_GetPrivate(pcxa, po0);
	} else
	if(type1 == JSTYPE_STRING) {	// for direct path parameter
		pszname = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	} else {
		XJSE_TRACE("(E) undefined img src type.");
		goto	failed;
	}
	xjse_result_t	xr1 = xgg_xadget_reload_img(
		pjsectx, &(pv0->ppixbuf), pszname, pdata, datasize);
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

xjse_result_t xgg_img_render(XTKCTX* ptkctxa, XJSECTX* pjsectxa, XGGPRIVHDR* phdra, void* puserdata)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	void*			ptarget = 0;
	XGG_IMG_PRIV*	ppriv = (XGG_IMG_PRIV*)phdra;
	XJSEXADGELEM*	pelem1 = phdra->pxadgelem;

	if(pelem1->bvisible == XJSE_B_FALSE)
		goto	cleanup;

	xjse_rectf_t	roffsouter, roffsinner;
	xjse_float_t	fframescale;
	xjse_opacity_t	opacity;
	xgg_xadget_apply_trans(ptkctxa, pjsectxa, phdra,
		&roffsouter, &roffsinner, &fframescale, &opacity);

	// focused?
	xjse_bool_t	bfocused = (pjsectxa->pggctx->pfocusedelem == &(phdra->te)
		? XJSE_B_TRUE : XJSE_B_FALSE);

	// render if it has the image data
	if(ppriv->ppixbuf != 0) {
		xr1 = xtk_pixbuf_draw(ptarget, pelem1, ppriv->ppixbuf,
			&roffsouter, &roffsinner, 0, fframescale, opacity);
		xr1 = xtk_wireframe_draw(ptarget, pelem1,
			&roffsouter, &roffsinner, fframescale, bfocused);
	}

cleanup:
	return	xr1;
}

static JSBool xgg_img_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XGG_IMG_PRIV	*p0;
	p0 = (XGG_IMG_PRIV*)xjse_malloc(0, sizeof(XGG_IMG_PRIV));
	xjse_memset(0, p0, 0, sizeof(XGG_IMG_PRIV));
	p0->hdr.type = XGG_ETYP_IMG;
	p0->hdr.pxadgelem = &(p0->xadgelem);
	p0->hdr.pfrender = xgg_img_render;	//xgg_xadget_render_generic;
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}
	xgg_xadgelem_setdefaultvalues(pcxa, pobja, &(p0->xadgelem));

	return	JS_TRUE;
}

static void	xgg_img_dtor(JSContext *pcxa, JSObject *pobja)
{
	XGG_IMG_PRIV	*pv0 = 0;
	pv0 = (XGG_IMG_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		//XJSE_TRACE("xgg_img_dtor: freeing private obj ptr [0x%p]", pv0);
		XJSE_SAFENNP(pv0->ppixbuf, xtk_pixbuf_delete);
		xjse_free(0, pv0);
	}
}

static JSBool xgg_img_setsrcsize(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_img_setsrcsize (%s)", psz1);

	XGG_IMG_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSE_SAFENNP(pv0->pszurl, free);
	//pv0->pszurl = (char*)xjse_malloc(0, strlen(pszurl) + 1);
	//strcpy(pv0->pszurl, pszurl);

	return	JS_TRUE;
}

	// ----8< ---- class definition

static JSClass class_xgg_img = {
	"img", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_img_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xgg_img[] = {
	// basic element
	XGG_BE_JSPROPSPEC_PARTIAL
	// img specific
	{	"enabled",		61, (0),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	"src",			62, (0),
		xgg_img_propop_src_get, xgg_img_propop_src_set,	},
	{	"srcHeight",	63, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	"srcWidth",		64, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	0,	},
};

JSFunctionSpec	funcs_xgg_img[] = {
	// basic element
	XGG_BE_JSFUNCSPEC_PARTIAL
	{	"setSrcSize",				xgg_img_setsrcsize,	2, 0, 0,	},
	// img specific
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_img(XJSECTX* pctxa)
{
	JSObject	*pobj_proto1 = JS_InitClass(pctxa->pctx,
		pctxa->poglobal, NULL, &class_xgg_img, xgg_img_ctor, 0,
		props_xgg_img, funcs_xgg_img,
		props_xgg_img, funcs_xgg_img);
	if(pobj_proto1 == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*	xjse_xgg_new_img(XJSECTX* pctxa, const char* pszobjname)
{
	// create proto
	//JSObject*	pobjbase = xjse_xgg_new_basicelement(pctxa, "tempbe01");
	//if(pobjbase == 0)
	//	return	0;
	JSObject*	pobjbase = 0;
	//
	JSObject*	pobj = JS_DefineObject(pctxa->pctx, pctxa->poglobal,
		pszobjname, &class_xgg_img, pobjbase,
		JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);
	//
	JSBool	b1 = xgg_img_ctor(pctxa->pctx, pobj, 0, 0, 0);	// args, rval
	if(b1 != JS_TRUE) {
		XJSE_TRACE("error: xgg_img_ctor failed!");
	}
	//
	return	pobj;
}

