// Google Gadget: label

#include <xjse.h>
#include <xtk.h>
#include "xgg.h"

typedef	struct __tag_xgg_label_priv {
	XGGPRIVHDR		hdr;			// all xgg private data must have this
	XJSEXADGELEM	xadgelem;
	xjse_uint8_t	align;
	xjse_color_t	color;
	char*			pszinnertext;
	xjse_float_t	fsize;
}	XGG_LABEL_PRIV;

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

static JSBool xgg_label_propop_align_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_LABEL_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSEXADGELEM*	pe0 = pv0->hdr.pxadgelem;
	//XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);

	if(JSVAL_IS_VOID(vp[0]))
		goto	cleanup;

	char		*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	if(*psz1 == 'c')	// center
		pv0->align = XTK_TEXTALIGN_CENTER;
	else if(*psz1 == 'l')	// left
		pv0->align = XTK_TEXTALIGN_LEFT;
	else if(*psz1 == 'r')	// right
		pv0->align = XTK_TEXTALIGN_RIGHT;
	else
		goto	failed;

cleanup:
	return	JS_TRUE;

failed:
	return	JS_FALSE;
}

static JSBool xgg_label_propop_bold_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	//XGG_LABEL_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSEXADGELEM*	pe0 = pv0->hdr.pxadgelem;
	//XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);

	JSType			type1 = JS_TypeOfValue(pcxa, vp[0]);
	if(type1 == JSTYPE_VOID)	// "undefined"
		goto	cleanup;

cleanup:
	return	JS_TRUE;
}

static JSBool xgg_label_propop_color_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_LABEL_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSEXADGELEM*	pe0 = pv0->hdr.pxadgelem;
	//XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);

	if(JSVAL_IS_VOID(vp[0]))
		goto	cleanup;

	char		*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	if(*psz1 != '#') {
		XJSE_TRACE("(X) color not start with '#' is not supported yet.");
		goto	failed;
	}
	psz1++;

	xjse_argb_to_color(&(pv0->color), psz1);

cleanup:
	return	JS_TRUE;

failed:
	return	JS_FALSE;
}

static JSBool xgg_label_propop_font_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	//XGG_LABEL_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSEXADGELEM*	pe0 = pv0->hdr.pxadgelem;
	//XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);

	//if(JSVAL_IS_VOID(vp[0]))
	//	goto	cleanup;
	//char		*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));

//cleanup:
	return	JS_TRUE;
}

static JSBool xgg_label_propop_innertext_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_LABEL_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSEXADGELEM*	pe0 = pv0->hdr.pxadgelem;
	//XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);

	if(JSVAL_IS_VOID(vp[0])) {
		XJSE_TRACE("(X) assigning non-text to innerText prop. (%d)",
			JS_TypeOfValue(pcxa, vp[0]));
		goto	cleanup;
	}

	char		*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	//XJSE_TRACE("(X) label.innerText: [%s]", psz1);
	xjse_int_t	n0 = xjse_strlen(0, psz1);
	XJSE_SAFENNP(pv0->pszinnertext, free);
	pv0->pszinnertext = (char*)malloc(n0 + 1);
	memcpy(pv0->pszinnertext, psz1, n0);
	pv0->pszinnertext[n0] = 0;

cleanup:
	return	JS_TRUE;
}

static JSBool xgg_label_propop_size_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_LABEL_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSEXADGELEM*	pe0 = pv0->hdr.pxadgelem;
	//XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);

	if(JSVAL_IS_VOID(vp[0]))
		goto	cleanup;

	char		*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	// rate from 0.0f (min.) to 1.0f (max.: XTK_BASEFONTPIXS)
	pv0->fsize = (((xjse_float_t)atoi(psz1)) / XTK_BASEFONTPIXS);

cleanup:
	return	JS_TRUE;
}

	// ----8< ---- functions

xjse_result_t xgg_label_render(XTKCTX* ptkctxa, XJSECTX* pjsectxa, XGGPRIVHDR* phdra, void* puserdata)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;

	xjse_rectf_t	roffsouter, roffsinner;
	xjse_float_t	fscale;
	xjse_opacity_t	opacity;
	xgg_xadget_apply_trans(ptkctxa, pjsectxa, phdra,
		&roffsouter, &roffsinner, &fscale, &opacity);

	//
	XGG_LABEL_PRIV*	pv0 = (XGG_LABEL_PRIV*)phdra;
	if(pv0->pszinnertext != 0) {
		XGG_LABEL_PRIV*	ppriv = (XGG_LABEL_PRIV*)phdra;
		XJSEXADGELEM*	pelem = phdra->pxadgelem;
		xjse_float_t
			fleft0 = roffsouter.fleft + (roffsinner.fleft + pelem->fx) * fscale,
			ftop0 = roffsouter.ftop + (roffsinner.ftop + pelem->fy) * fscale;
		if(ppriv->align == XTK_TEXTALIGN_CENTER)
			fleft0 +=  (pelem->fcx * fscale / 2.0f);
#ifdef	XTPF_PSP
		if(ppriv->align == XTK_TEXTALIGN_RIGHT)
			fleft0 +=  (pelem->fcx * fscale);
#endif	//XTPF_PSP
		xjse_color_t	cshadow;
		XJSE_COLOR_SET(&cshadow, 0x00000000);
		xtk_string_draw(ptkctxa, pv0->pszinnertext, &(ppriv->color), &cshadow,
			fleft0, ftop0, ppriv->fsize, ppriv->align, fscale);
	}

	return	xr1;
}

static JSBool xgg_label_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XGG_LABEL_PRIV	*p0;
	p0 = (XGG_LABEL_PRIV*)xjse_malloc(0, sizeof(XGG_LABEL_PRIV));
	xjse_memset(0, p0, 0, sizeof(XGG_LABEL_PRIV));
	p0->hdr.type = XGG_ETYP_LABEL;
	p0->hdr.pxadgelem = &(p0->xadgelem);
	p0->hdr.pfrender = xgg_label_render;
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}
	// default properties
	xgg_xadgelem_setdefaultvalues(pcxa, pobja, &(p0->xadgelem));
	p0->align = XTK_TEXTALIGN_CENTER;
	p0->fsize = 1.0f;
	XJSE_COLOR_SETA(&(p0->color));

	return  JS_TRUE;
}

static void	xgg_label_dtor(JSContext *pcxa, JSObject *pobja)
{
	XGG_LABEL_PRIV	*pv0 = 0;
	pv0 = (XGG_LABEL_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		//XJSE_TRACE("xgg_label_dtor: freeing private obj ptr [0x%p]", pv0);
		XJSE_SAFENNP(pv0->pszinnertext, free);
		xjse_free(0, pv0);
	}
}

#if	0
static JSBool xgg_label_setsrcsize(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_label_setsrcsize (%s)", psz1);

	XGG_LABEL_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSE_SAFENNP(pv0->pszurl, free);
	//pv0->pszurl = (char*)xjse_malloc(0, strlen(pszurl) + 1);
	//strcpy(pv0->pszurl, pszurl);

	return  JS_TRUE;
}
#endif

	// ----8< ---- class definition

static JSClass class_xgg_label = {
	"label", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_label_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

enum {	LABEL_TID_ALIGN = 61, LABEL_TID_BOLD, LABEL_TID_COLOR, LABEL_TID_FONT, LABEL_TID_INNERTEXT, LABEL_TID_ITALIC, LABEL_TID_SIZE, LABEL_TID_STRIKEOUT, LABEL_TID_TRIMMING, LABEL_TID_UNDERLINE, LABEL_TID_VALIGN, LABEL_TID_WORDWRAP,	};

JSPropertySpec	props_xgg_label[] = {
	// basic element
	XGG_BE_JSPROPSPEC_PARTIAL
	// label specific
    {   "align",		LABEL_TID_ALIGN, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_label_propop_align_set,	},
	{	"bold",			LABEL_TID_BOLD, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_label_propop_bold_set,	},
	{	"color",		LABEL_TID_COLOR, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_label_propop_color_set,	},
	{	"font",			LABEL_TID_FONT, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_label_propop_font_set,	},
	{	"innerText",	LABEL_TID_INNERTEXT, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_label_propop_innertext_set,	},
	{	"italic",		LABEL_TID_ITALIC, (JSPROP_ENUMERATE),
		JS_PropertyStub, xjse_x_propop_notimpl,	},
	{	"size",			LABEL_TID_SIZE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_label_propop_size_set,	},
	{	"strikeout",	LABEL_TID_STRIKEOUT, (JSPROP_ENUMERATE),
		JS_PropertyStub, xjse_x_propop_notimpl,	},
	{	"trimming",		LABEL_TID_TRIMMING, (JSPROP_ENUMERATE),
		JS_PropertyStub, xjse_x_propop_notimpl,	},
	{	"underline",	LABEL_TID_UNDERLINE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xjse_x_propop_notimpl,	},
	{	"vAlign",		LABEL_TID_VALIGN, (JSPROP_ENUMERATE),
		JS_PropertyStub, xjse_x_propop_notimpl,	},
	{	"wordWrap",		LABEL_TID_WORDWRAP, (JSPROP_ENUMERATE),
		JS_PropertyStub, xjse_x_propop_notimpl,	},
	{	0,	},
};

JSFunctionSpec	funcs_xgg_label[] = {
	// basic element
	XGG_BE_JSFUNCSPEC_PARTIAL
	// label specific
//	{	"setSrcSize",				xgg_label_setsrcsize,	2, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_label(XJSECTX* pctxa)
{
	JSObject	*pobj_proto1 = JS_InitClass(pctxa->pctx,
		pctxa->poglobal, NULL, &class_xgg_label, xgg_label_ctor, 0,
		props_xgg_label, funcs_xgg_label, props_xgg_label, funcs_xgg_label);
	if(pobj_proto1 == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*	xjse_xgg_new_label(XJSECTX* pctxa, const char* pszobjname)
{
	// create proto
	//JSObject*	pobjbase = xjse_xgg_new_basicelement(pctxa, "tempbe01");
	//if(pobjbase == 0)
	//	return	0;
	JSObject*	pobjbase = 0;
	//
	JSObject*	pobj = JS_DefineObject(pctxa->pctx, pctxa->poglobal,
		pszobjname, &class_xgg_label, pobjbase,
		JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);
	//
	JSBool	b1 = xgg_label_ctor(pctxa->pctx, pobj, 0, 0, 0);	// args, rval
	if(b1 != JS_TRUE) {
		XJSE_TRACE("error: xgg_label_ctor failed!");
	}
	//
	return	pobj;
}

