// Google Gadget: div

#include <xjse.h>
#include <xtk.h>
#include "xgg.h"

typedef	struct __tag_xgg_div_priv {
	XGGPRIVHDR		hdr;		// all xgg private data must have this
	XJSEXADGELEM	xadgelem;
}	XGG_DIV_PRIV;

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

static JSBool xgg_div_propop_bold_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGG_DIV_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xgg_div_propop_bold_get. (%s)",
		JS_GetStringBytes(JS_ValueToString(pcxa, vp[0])));

	return  JS_TRUE;
}

static JSBool xgg_div_propop_bold_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	//XGG_DIV_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSEXADGELEM*	pe0 = pv0->hdr.pxadgelem;
	//XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	JSType			type1 = JS_TypeOfValue(pcxa, vp[0]);

	if(type1 == JSTYPE_VOID)	// "undefined"
		goto	cleanup;

cleanup:
	return	JS_TRUE;

//failed:
//	return	JS_FALSE;
}

	// ----8< ---- functions

static xjse_result_t xgg_div_render(XTKCTX* ptkctxa, XJSECTX* pjsectxa, XGGPRIVHDR* phdra, void* puserdata)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	void*			ptarget = 0;
	XGG_DIV_PRIV*	ppriv = (XGG_DIV_PRIV*)phdra;
	XJSEXADGELEM*	pelem1 = phdra->pxadgelem;

	xjse_rectf_t	roffsouter, roffsinner;
	xjse_float_t	fframescale;
	xjse_opacity_t	opacity;
	xgg_xadget_apply_trans(ptkctxa, pjsectxa, phdra,
		&roffsouter, &roffsinner, &fframescale, &opacity);

	// focused?
	xjse_bool_t bfocused = (pjsectxa->pggctx->pfocusedelem == &(phdra->te)
		? XJSE_B_TRUE : XJSE_B_FALSE);

	// render if it has mouse events
	if((XGGPRIVHDR_TO_TREEELEM((XGGPRIVHDR*)ppriv)->uflags
		&& XJSEC_TREEELEMFLAG_HASMOUSEEVENT) != 0) {
		xr1 = xtk_wireframe_draw(ptarget, pelem1,
			&roffsouter, &roffsinner, fframescale, bfocused);
	}

//cleanup:
	return	xr1;
}

static JSBool xgg_div_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XGG_DIV_PRIV	*p0;
	p0 = (XGG_DIV_PRIV*)xjse_malloc(0, sizeof(XGG_DIV_PRIV));
	xjse_memset(0, p0, 0, sizeof(XGG_DIV_PRIV));
	p0->hdr.type = XGG_ETYP_DIV;
	p0->hdr.pxadgelem = &(p0->xadgelem);
	p0->hdr.pfrender = xgg_div_render;
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}
	xgg_xadgelem_setdefaultvalues(pcxa, pobja, &(p0->xadgelem));
	//XJSE_TRACE("(X) new <div>: %p, %p", p0, p0->hdr.pxadgelem);

	return  JS_TRUE;
}

static void	xgg_div_dtor(JSContext *pcxa, JSObject *pobja)
{
	XGG_DIV_PRIV	*pv0 = 0;
	pv0 = (XGG_DIV_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		//XJSE_TRACE("xgg_div_dtor: freeing private obj ptr [0x%p]", pv0);
		xjse_free(0, pv0);
	}
}

#if	0
static JSBool xgg_div_setsrcsize(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_div_setsrcsize (%s)", psz1);

	XGG_DIV_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSE_SAFENNP(pv0->pszurl, free);
	//pv0->pszurl = (char*)xjse_malloc(0, strlen(pszurl) + 1);
	//strcpy(pv0->pszurl, pszurl);

	return  JS_TRUE;
}
#endif

	// ----8< ---- class definition

static JSClass class_xgg_div = {
	"div", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_div_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xgg_div[] = {
	// basic element
	XGG_BE_JSPROPSPEC_PARTIAL
	// div specific
    {   "autoscroll",			122, (JSPROP_ENUMERATE),
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	"background",			123, (JSPROP_ENUMERATE),
		xgg_div_propop_bold_get, xgg_div_propop_bold_set,	},
	{	0,	},
};

JSFunctionSpec	funcs_xgg_div[] = {
	// basic element
	XGG_BE_JSFUNCSPEC_PARTIAL
	// div specific
//	{	"setSrcSize",				xgg_div_setsrcsize,	2, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_div(XJSECTX* pctxa)
{
	JSObject	*pobj_proto1 = JS_InitClass(pctxa->pctx,
		pctxa->poglobal, NULL, &class_xgg_div, xgg_div_ctor, 0,
		props_xgg_div, funcs_xgg_div, props_xgg_div, funcs_xgg_div);
	if(pobj_proto1 == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*	xjse_xgg_new_div(XJSECTX* pctxa, const char* pszobjname)
{
	// create proto
	//JSObject*	pobjbase = xjse_xgg_new_basicelement(pctxa, "tempbe01");
	//if(pobjbase == 0)
	//	return	0;
	JSObject*	pobjbase = 0;
	//
	JSObject*	pobj = JS_DefineObject(pctxa->pctx, pctxa->poglobal,
		pszobjname, &class_xgg_div, pobjbase,
		JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);
	//
	JSBool	b1 = xgg_div_ctor(pctxa->pctx, pobj, 0, 0, 0);	// args, rval
	if(b1 != JS_TRUE) {
		XJSE_TRACE("error: xgg_div_ctor failed!");
	}
	//
	return	pobj;
}

