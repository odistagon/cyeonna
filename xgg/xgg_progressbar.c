// Google Gadget: progressbar

#include <xjse.h>
#include <xtk.h>
#include "xgg.h"

typedef	struct __tag_xgg_progressbar_priv {
	XGGPRIVHDR		hdr;		// all xgg private data must have this
	XJSEXADGELEM	xadgelem;
	xtk_pixbuf_t*	ppbempty;	// emptyImage
	xtk_pixbuf_t*	ppbfull;	// fullImage
	xjse_int_t		nmax;
	xjse_int_t		nmin;
	xjse_int_t		nvalue;
}	XGG_PBAR_PRIV;

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

enum {	PBAR_TID_EMPTYIMAGE = 61, PBAR_TID_FULLIMAGE, PBAR_TID_MAX, PBAR_TID_MIN, PBAR_TID_ORIENTATION, PBAR_TID_THUMBDISABLEDIMAGE, PBAR_TID_THUMBDOWNIMAGE, PBAR_TID_THUMBOVERIMAGE, PBAR_TID_THUMBIMAGE, PBAR_TID_VALUE, PBAR_TID_ONCHANGE,	};

/** set numeric property (xn).
 */
JSBool xgg_pbar_propop_xn_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGPRIVHDR*		phdr = JS_GetPrivate(pcxa, pobja);
	XGG_PBAR_PRIV*	pv0 = JS_GetPrivate(pcxa, pobja);
	XJSEXADGELEM*	pelem0 = phdr->pxadgelem;
	XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	XJSE_ASSERT(pelem0 != 0);	// basicelement inheritances must have this

	xjse_result_t	xr1;
	xjse_int_t		nid = 0;//, nvalue = 0;//, n1;
	char*			psz0;
	JS_ValueToInt32(pcxa, id, &nid);
	//JS_ValueToInt32(pcxa, vp[0], &nvalue);
	//XJSE_TRACE("xgg_pbar_propop_xn_set. [%d](%d)", nid, nvalue);

	switch(nid) {
	case PBAR_TID_EMPTYIMAGE:
		psz0 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
		xr1 = xgg_xadget_reload_img(pjsectx, &(pv0->ppbempty), psz0, 0, 0);
		if(xr1 != XJSE_SUCCESS)
			goto	failed;
		if(pelem0->fcx == 0.0f)
			pelem0->fcx = (xjse_float_t)(pv0->ppbempty->nimgwidth);
		if(pelem0->fcy == 0.0f)
			pelem0->fcy = (xjse_float_t)(pv0->ppbempty->nimgheight);
		break;
	case PBAR_TID_FULLIMAGE:
		psz0 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
		xr1 = xgg_xadget_reload_img(pjsectx, &(pv0->ppbfull), psz0, 0, 0);
		if(xr1 != XJSE_SUCCESS)
			goto	failed;
		break;
	case PBAR_TID_MAX:
		JS_ValueToInt32(pcxa, vp[0], &(pv0->nmax));
		break;
	case PBAR_TID_MIN:
		JS_ValueToInt32(pcxa, vp[0], &(pv0->nmin));
		break;
	case PBAR_TID_VALUE:
		JS_ValueToInt32(pcxa, vp[0], &(pv0->nvalue));
		break;
	default:
		XJSE_TRACE("(E) unknown xn prop [property id: %d]", nid);
	}

	return	JS_TRUE;

failed:
	return	JS_FALSE;
}

	// ----8< ---- functions

xjse_result_t xgg_pbar_render(XTKCTX* ptkctxa, XJSECTX* pjsectxa, XGGPRIVHDR* phdra, void* puserdata)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	xjse_rectf_t	roffsouter, roffsinner;
	xjse_float_t	fframescale;
	xjse_opacity_t	opacity;
	xgg_xadget_apply_trans(ptkctxa, pjsectxa, phdra,
		&roffsouter, &roffsinner, &fframescale, &opacity);

	// focused?
	xjse_bool_t bfocused = (pjsectxa->pggctx->pfocusedelem == &(phdra->te)
		? XJSE_B_TRUE : XJSE_B_FALSE);

	// render if it has image
	void*			ptarget = 0;
	XGG_PBAR_PRIV*	pv0 = (XGG_PBAR_PRIV*)phdra;
	XJSEXADGELEM*	pelem1 = phdra->pxadgelem;
	if(pv0->ppbempty != 0)
		xr1 = xtk_pixbuf_draw(ptarget, pelem1, pv0->ppbempty,
			&roffsouter, &roffsinner, 0, fframescale, opacity);
	if(pv0->ppbfull != 0) {
		xjse_float_t	frate =
			((xjse_float_t)pv0->nvalue / (xjse_float_t)(pv0->nmax - pv0->nmin));
		xjse_rectf_t	r1 = {	0.0f, 0.0f, frate, 1.0f,	};
		xr1 = xtk_pixbuf_draw(ptarget, pelem1, pv0->ppbfull,
			&roffsouter, &roffsinner, &r1, fframescale, opacity);
	}

	return  xr1;
}

static JSBool xgg_pbar_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XGG_PBAR_PRIV	*p0;
	p0 = (XGG_PBAR_PRIV*)xjse_malloc(0, sizeof(XGG_PBAR_PRIV));
	xjse_memset(0, p0, 0, sizeof(XGG_PBAR_PRIV));
	p0->hdr.type = XGG_ETYP_PROGRESSBAR;
	p0->hdr.pxadgelem = &(p0->xadgelem);
	p0->hdr.pfrender = xgg_pbar_render;
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}
	xgg_xadgelem_setdefaultvalues(pcxa, pobja, &(p0->xadgelem));
	p0->nmax = 100;
	p0->nmin = 0;
	p0->nvalue = 0;

	return	JS_TRUE;
}

static void	xgg_pbar_dtor(JSContext *pcxa, JSObject *pobja)
{
	XGG_PBAR_PRIV	*pv0 = 0;
	pv0 = (XGG_PBAR_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		XJSE_TRACE("xgg_pbar_dtor: freeing private obj ptr [0x%p]", pv0);
		xjse_free(0, pv0);
	}
}

#if	0
static JSBool xgg_pbar_setsrcsize(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_pbar_setsrcsize (%s)", psz1);

	XGG_PBAR_PRIV	*pv0 = 0;
	pv0 = JS_GetPrivate(pcxa, pobja);
	//XJSE_SAFENNP(pv0->pszurl, free);
	//pv0->pszurl = (char*)xjse_malloc(0, strlen(pszurl) + 1);
	//strcpy(pv0->pszurl, pszurl);

	return	JS_TRUE;
}
#endif

	// ----8< ---- class definition

static JSClass class_xgg_pbar = {
	"progressbar", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_pbar_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xgg_pbar[] = {
	// basic element
	XGG_BE_JSPROPSPEC_PARTIAL
	// progressbar specific
	{	"emptyImage",			PBAR_TID_EMPTYIMAGE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	"fullImage",			PBAR_TID_FULLIMAGE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	"max",					PBAR_TID_MAX, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	"min",					PBAR_TID_MIN, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	"orientation",			PBAR_TID_ORIENTATION, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	"thumbDisabledImage",	PBAR_TID_THUMBDISABLEDIMAGE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	"thumbDownImage",		PBAR_TID_THUMBDOWNIMAGE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	"thumbOverImage",		PBAR_TID_THUMBOVERIMAGE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	"thumbImage",			PBAR_TID_THUMBIMAGE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	"value",				PBAR_TID_VALUE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	// events
	{	"onchange",				PBAR_TID_ONCHANGE, (JSPROP_ENUMERATE),
		JS_PropertyStub, xgg_pbar_propop_xn_set,	},
	{	0,	},
};

JSFunctionSpec	funcs_xgg_pbar[] = {
	// basic element
	XGG_BE_JSFUNCSPEC_PARTIAL
	// progressbar specific
//	{	"setSrcSize",				xgg_pbar_setsrcsize,	2, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_progressbar(XJSECTX* pctxa)
{
	JSObject	*pobj_proto1 = JS_InitClass(pctxa->pctx,
		pctxa->poglobal, NULL, &class_xgg_pbar, xgg_pbar_ctor, 0,
		props_xgg_pbar, funcs_xgg_pbar, props_xgg_pbar, funcs_xgg_pbar);
	if(pobj_proto1 == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*	xjse_xgg_new_progressbar(XJSECTX* pctxa, const char* pszobjname)
{
	// create proto
	//JSObject*	pobjbase = xjse_xgg_new_basicelement(pctxa, "tempbe01");
	//if(pobjbase == 0)
	//	return	0;
	JSObject*	pobjbase = 0;
	//
	JSObject*	pobj = JS_DefineObject(pctxa->pctx, pctxa->poglobal,
		pszobjname, &class_xgg_pbar, pobjbase,
		JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);
	//
	JSBool	b1 = xgg_pbar_ctor(pctxa->pctx, pobj, 0, 0, 0);	// args, rval
	if(b1 != JS_TRUE) {
		XJSE_TRACE("error: xgg_pbar_ctor failed!");
	}
	//
	return	pobj;
}

