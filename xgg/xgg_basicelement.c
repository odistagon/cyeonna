// Google Gadget: basicElement

#ifdef	XTPF_Linux
#include <math.h>
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#include <floatonly.h>
#endif	//XTPF_PSP
#include <xjse.h>
#include <xtk.h>
#include "xgg.h"

#define	XGGBE_PREPAREPRIV	\
	XGGPRIVHDR		*pv0 = JS_GetPrivate(pcxa, pobja);	\
	XJSEXADGELEM	*pelem0 = pv0->pxadgelem;	\
	XJSE_ASSERT(pelem0 != 0);	// basicelement inheritances must have this

	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

/** set default property values.
 */
void xgg_xadgelem_setdefaultvalues(JSContext* pcxa, JSObject* pobja, XJSEXADGELEM* pelema)
{
	jsval	v1;
	pelema->opacity = XTKC_OPACITY_MAX;
	v1 = INT_TO_JSVAL(pelema->opacity);
	JS_SetProperty(pcxa, pobja, "opacity", &v1);
	pelema->bvisible = XJSE_B_TRUE;
	v1 = BOOLEAN_TO_JSVAL(pelema->bvisible);
	JS_SetProperty(pcxa, pobja, "visible", &v1);
}

JSBool xgg_be_propop_cursor_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	//XGGBE_PREPAREPRIV
	//XJSE_TRACE("xgg_be_propop_count_get. url: (%s)", pv0->pszurl);

	return  JS_TRUE;
}

JSBool xgg_be_propop_cursor_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	//XGGBE_PREPAREPRIV
	//XJSE_TRACE("xgg_be_propop_count_set. url: (%s)", pv0->pszurl);

	return  JS_TRUE;
}

JSBool xgg_be_propop_x_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGBE_PREPAREPRIV

	JS_NewNumberValue(pcxa, (jsdouble)pelem0->fx, vp);	// x -> vp[0]
	// confirm set
	xjse_int_t		n1 = 0, n2 = 0;
	JS_ValueToInt32(pcxa, id, &n1);
	JS_ValueToInt32(pcxa, vp[0], &n2);
	XJSE_TRACE("xgg_be_propop_x_get. [%s, %d](%f, %d)", JS_GetStringBytes(JS_ValueToString(pcxa, id)), n1, pelem0->fx, n2);

	return  JS_TRUE;
}

#if	0
JSBool xgg_be_propop_pinx_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGBE_PREPAREPRIV

	JS_ValueToInt32(pcxa, vp[0], &(pelem0->rotorgx));
	XJSE_TRACE("xgg_be_propop_piny_set. 1 [%p](%d)", pelem0, pelem0->rotorgx);

	return  JS_TRUE;
}

JSBool xgg_be_propop_piny_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGBE_PREPAREPRIV

	JS_ValueToInt32(pcxa, vp[0], &(pelem0->rotorgy));
	XJSE_TRACE("xgg_be_propop_piny_set. 1 [%p](%d)", pelem0, pelem0->rotorgy);

	return  JS_TRUE;
}

JSBool xgg_be_propop_x_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGBE_PREPAREPRIV

	JS_ValueToInt32(pcxa, vp[0], &(pelem0->x));
	XJSE_TRACE("xgg_be_propop_x_set. 1 [%p](%d)", pelem0, pelem0->x);

	return  JS_TRUE;
}

JSBool xgg_be_propop_y_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGBE_PREPAREPRIV

	JS_ValueToInt32(pcxa, vp[0], &(pelem0->y));
	XJSE_TRACE("xgg_be_propop_y_set. 1 [%p](%d)", pelem0, pelem0->y);

	return  JS_TRUE;
}

JSBool xgg_be_propop_width_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGBE_PREPAREPRIV

	JS_ValueToInt32(pcxa, vp[0], &(pelem0->cx));
	XJSE_TRACE("xgg_be_propop_y_set. 1 [%p](%d)", pelem0, pelem0->cx);

	return  JS_TRUE;
}

JSBool xgg_be_propop_height_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGBE_PREPAREPRIV

	JS_ValueToInt32(pcxa, vp[0], &(pelem0->cy));
	XJSE_TRACE("xgg_be_propop_y_set. 1 [%p](%d)", pelem0, pelem0->cy);

	return  JS_TRUE;
}

JSBool xgg_be_propop_rotation_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGBE_PREPAREPRIV

	JS_ValueToInt32(pcxa, vp[0], &(pelem0->rotation));
	XJSE_TRACE("xgg_be_propop_y_set. 1 [%p](%d)", pelem0, pelem0->rotation);

	return  JS_TRUE;
}
#endif	//0

/** set numeric property (xn).
 */
JSBool xgg_be_propop_xn_set(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XGGBE_PREPAREPRIV

	xjse_result_t	xr1 = XJSE_SUCCESS;
	xjse_int_t		n1, nid = 0, ntemp0;
	xjse_bool_t		btemp0;
	char*			psz0 = 0;
	JS_ValueToInt32(pcxa, id, &nid);
	//JS_ValueToInt32(pcxa, vp[0], &nvalue);
	//XJSE_TRACE("xgg_be_propop_xn_set. [%d](%d)", nid, nvalue);

	switch(nid) {
	case XGG_BE_PID_CHILDREN:
		break;
	case XGG_BE_PID_CURSOR:
		break;
	case XGG_BE_PID_DROPTARGET:
		break;
	case XGG_BE_PID_ENABLED:
		break;
	case XGG_BE_PID_HEIGHT:
		xr1 = xjse_whstr_to_val(
			JS_GetStringBytes(JS_ValueToString(pcxa, vp[0])), &ntemp0, &btemp0);
		if(btemp0 == XJSE_B_TRUE) {
			// relative value (%): query parent's value
			if(!(pv0->te.pparent != 0 &&
				TREEELEM_TO_XADGELEM(pv0->te.pparent) != 0))
				goto	failed;
			pelem0->fcy = (xjse_float_t)(
				(TREEELEM_TO_XADGELEM(pv0->te.pparent))->fcy
				* (xjse_float_t)ntemp0 / 100.0f);
		} else {
			pelem0->fcy = (xjse_float_t)ntemp0;
		}
		break;
	case XGG_BE_PID_HITTEST:
		break;
	case XGG_BE_PID_MASK:
		break;
	case XGG_BE_PID_NAME:
		break;
	case XGG_BE_PID_OFFSETHEIGHT:
		break;
	case XGG_BE_PID_OFFSETWIDTH:
		break;
	case XGG_BE_PID_OFFSETX:
		break;
	case XGG_BE_PID_OFFSETY:
		break;
	case XGG_BE_PID_OPACITY:
		JS_ValueToInt32(pcxa, vp[0], &n1);
		pelem0->opacity = (xjse_opacity_t)n1;
		//XJSE_TRACE("(X) XXXX opacity set: [%s, %d]",
		//	JS_GetStringBytes(JS_ValueToString(pcxa, vp[0])), n1);
		break;
	case XGG_BE_PID_PARENTELEMENT:
		break;
	case XGG_BE_PID_PINX:
		JS_ValueToInt32(pcxa, vp[0], &n1);
		pelem0->frotorgx = (xjse_float_t)n1;
		break;
	case XGG_BE_PID_PINY:
		JS_ValueToInt32(pcxa, vp[0], &n1);
		pelem0->frotorgy = (xjse_float_t)n1;
		break;
	case XGG_BE_PID_ROTATION:
		JS_ValueToInt32(pcxa, vp[0], &(pelem0->rotation));
		break;
	case XGG_BE_PID_TAGNAME:
		break;
	case XGG_BE_PID_TOOLTIP:
		break;
	case XGG_BE_PID_WIDTH:
		xr1 = xjse_whstr_to_val(
			JS_GetStringBytes(JS_ValueToString(pcxa, vp[0])), &ntemp0, &btemp0);
		if(btemp0 == XJSE_B_TRUE) {
			// relative value (%): query parent's value
			if(!(pv0->te.pparent != 0 &&
				TREEELEM_TO_XADGELEM(pv0->te.pparent) != 0)) {
				goto	failed;
			}
			pelem0->fcx = (xjse_float_t)(
				(TREEELEM_TO_XADGELEM(pv0->te.pparent))->fcx
				* (xjse_float_t)ntemp0 / 100.0f);
		} else {
			pelem0->fcx = (xjse_float_t)ntemp0;
		}
		break;
	case XGG_BE_PID_VISIBLE:
		psz0 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
		pelem0->bvisible = (*psz0 == 't' ? XJSE_B_TRUE : XJSE_B_FALSE);
		break;
	case XGG_BE_PID_X:
		JS_ValueToInt32(pcxa, vp[0], &n1);
		pelem0->fx = (xjse_float_t)n1;
		break;
	case XGG_BE_PID_Y:
		JS_ValueToInt32(pcxa, vp[0], &n1);
		pelem0->fy = (xjse_float_t)n1;
		break;
	default:
		XJSE_TRACE("(E) unknown xn prop [property id: %d]", nid);
	}

	return  JS_TRUE;

failed:
	return	JS_FALSE;
}

	// ----8< ---- functions

JSBool xgg_be_appendelement(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	//XGGBE_PREPAREPRIV

	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[1]));
	XJSE_TRACE("xgg_be_appendelement (%s)", psz1);

	//XJSE_SAFENNP(pelem0->pszurl, free);
	//pelem0->pszurl = (char*)xjse_malloc(0, strlen(pszurl) + 1);
	//xjse_strcpy(pelem0->pszurl, pszurl);

	return  JS_TRUE;
}

#if	0	//20080416 -> xjse_global_propop_children_get
JSBool xgg_be_children(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	XGGPRIVHDR		*pv1 = JS_GetPrivate(pcxa, pobja);
	xjse_uint_t		ucount = 0;
	char			*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));
	//XJSE_TRACE("xgg_be_children [%s](%s)", JS_GetStringBytes(
	//	JS_ValueToString(pcxa, OBJECT_TO_JSVAL(pobja))), psz1);

	JSObject*	ponodelist1 = xgg_new_nodelist(pjsectx, pobja);
	if(ponodelist1 == NULL) {
		XJSE_TRACE("xjse_new_nodelist() failed!");
		goto	failed;
	}

	if(pv1->te.pfirstchild == 0) {
		XJSE_TRACE("(X) xgg_be_children(): no children.");
		goto	cleanup;
	}

	XJSE_TREEELEM*  pv0 = pv1->te.pfirstchild;
	while(pv0 != 0) {
		jsval	vname;
		JSBool	b1 = JS_GetProperty(pcxa, pv0->poself, "name", &vname);
		if(b1 != JS_TRUE) {
			XJSE_TRACE("(E) JS_GetProperty failed: [%d]", b1);
			goto	failed;
		}
		char	*pszname = JS_GetStringBytes(JS_ValueToString(pcxa, vname));
		XJSE_TRACE("(X) comparing names... [%s, %s](%s)", psz1, pszname, JS_GetStringBytes(JS_ValueToString(pcxa, OBJECT_TO_JSVAL(pv0->poself))));
		if(strcmp(psz1, pszname) == 0) {
			jsval	velement = OBJECT_TO_JSVAL(pv0->poself);
			XJSE_TRACE("(X) XXXX found!: [%s](%s)", pszname, JS_GetStringBytes(JS_ValueToString(pcxa, OBJECT_TO_JSVAL(pv0->poself))));
			b1 = JS_SetElement(pcxa, ponodelist1, ucount++, &velement);
			if(b1 != JS_TRUE) {
				XJSE_TRACE("(E) JS_GetElement failed: [%d]", b1);
				goto	failed;
			}
		}
		pv0 = pv0->pnextsibling;
	}
	if(ucount == 0) {
		XJSE_TRACE("(W) name [%s] not found!", psz1);
	}

cleanup:
	*rval = OBJECT_TO_JSVAL(ponodelist1);

	return  JS_TRUE;

failed:
	return  JS_TRUE;
}
#endif	//20080416 -> xjse_global_propop_children_get


/** apply translation, rotation before drawing
 * @param proffsoutera cood form screen 0, 0
 * @param proffsinnera cood form xadget's 0, 0
 * @param pfscale scaling rate (xadget frame : original xadget size)
 */
xjse_result_t xgg_xadget_apply_trans(XTKCTX* ptkctxa, XJSECTX* pjsectxa, XGGPRIVHDR* phdra, xjse_rectf_t* proffsoutera, xjse_rectf_t* proffsinnera, xjse_float_t* pfscale, xjse_opacity_t* popacity)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;

	// calc animated rectangle from conf and animation time
	// this part must be out of rendering function?
	XJSEXADGCONF	conf1;
	memcpy(&conf1, &(pjsectxa->xadgconf), sizeof(XJSEXADGCONF));
	if(ptkctxa->tflipfrom != 0) {
		xjse_time_t	tnow;
		xjse_time_now(&tnow);
		// apply flip animation
		xjse_float_t	fflip =
			sinf(((xjse_float_t)(tnow - (ptkctxa)->tflipfrom)
			/ (xjse_float_t)((ptkctxa)->tflipdura)) * M_PI / 2.0f);
		xjse_rectf_t*	rect1 = &(conf1.rectinsc);
		xjse_rectf_t*	rect2 = &(pjsectxa->pxadgconfflipto->rectinsc);
		rect1->fleft += (rect2->fleft - rect1->fleft) * fflip;
		rect1->ftop += (rect2->ftop - rect1->ftop) * fflip;
		rect1->fright += (rect2->fright - rect1->fright) * fflip;
		rect1->fbottom += (rect2->fbottom - rect1->fbottom) * fflip;
	}

	xjse_rectf_t	roffsouter = {
		conf1.rectinsc.fleft, conf1.rectinsc.ftop,
		conf1.rectinsc.fright, conf1.rectinsc.fbottom,	};
	xjse_rectf_t	roffsinner = {	0.0f, 0.0f, 0.0f, 0.0f,	};
	xjse_opacity_t	opacity = XTKC_OPACITY_MAX;

	// calc scaling rate <view>:conf
	xjse_float_t	fframescale = 1.0f;
	{
		XGGPRIVHDR	*proot = phdra;
		do {
			proot = (XGGPRIVHDR*)proot->te.pparent;
		} while(proot->te.pparent != 0);
		if(proot->type == XGG_ETYP_VIEW) {
			XGGESPEC_VIEW*	pespec0 = (XGGESPEC_VIEW*)proot->pespec;
			xjse_float_t	fh0 = ((roffsouter.fright - roffsouter.fleft) /
				(xjse_float_t)(pespec0->nwidth));
			xjse_float_t	fv0 = ((roffsouter.fbottom - roffsouter.ftop) /
				(xjse_float_t)(pespec0->nheight));
#define	XGG_CENTER_XAGDETS	// centering the xadget in frame
			if(fh0 < fv0) {
				fframescale = fh0;
#ifdef	XGG_CENTER_XAGDETS
				xjse_float_t	f0 = ((roffsouter.fbottom - roffsouter.ftop)
					- (xjse_float_t)(pespec0->nheight) * fframescale) / 2.0f;
				roffsouter.ftop += f0;
				roffsouter.fbottom += f0;
#endif	//XGG_CENTER_XAGDETS
			} else {
				fframescale = fv0;
#ifdef	XGG_CENTER_XAGDETS
				xjse_float_t	f0 = ((roffsouter.fright - roffsouter.fleft)
					- (xjse_float_t)(pespec0->nwidth) * fframescale) / 2.0f;
				roffsouter.fleft += f0;
				roffsouter.fright += f0;
#endif	//XGG_CENTER_XAGDETS
			}
		} else {
			XJSE_TRACE("(E) root element is not <view>!");
		}
	}

	// apply parent's cood offset, opacity
	{
		XJSE_TREEELEM*	pte0 = phdra->te.pparent;
		while(pte0 != 0) {
			XJSEXADGELEM*	pep0 = ((XGGPRIVHDR*)pte0)->pxadgelem;
			if(pep0 != 0) {
				roffsinner.fleft += pep0->fx;
				roffsinner.ftop += pep0->fy;
				roffsinner.fright += pep0->fx;
				roffsinner.fbottom += pep0->fy;
				opacity = ((pep0->opacity * opacity) / XTKC_OPACITY_MAX);
//XJSE_TRACE("(X) opacity applied: [%d -> %d](%p)", (xjse_int_t)pep0->opacity, (xjse_int_t)opacity, pte0->pparent);
			}

			pte0 = pte0->pparent;
		}
	}

	memcpy(proffsoutera, &roffsouter, sizeof(xjse_rectf_t));
	memcpy(proffsinnera, &roffsinner, sizeof(xjse_rectf_t));
	*popacity = opacity;
	*pfscale = fframescale;

	return	xr1;
}

/** load or reload image to xtk_pixbuf_t.
 * @param pdata img data. (can be 0)
 */
xjse_result_t xgg_xadget_reload_img(XJSECTX* pjsectx, xtk_pixbuf_t** pppixbuf, const char* pszname, xjse_uint8_t* pdata, xjse_size_t datasize)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	xtk_pixbuf_t	*ppbcurr = *pppixbuf, *ppb1 = 0;

	char	szpath[XJSE_NMAX_PATH + 1];
	if(strncmp(pszname, "http://", 7) == 0) {
		strncpy(szpath, pszname, XJSE_NMAX_PATH);
	} else {
		xjse_snprintf(0, szpath, (XJSE_NMAX_PATH + 1), "%s/%s/%s",
			XJSE_PATH_XADGETS, pjsectx->pszname, pszname);
	}
	//XJSE_TRACE("(X) path: [%s]", szpath);

	// do nothing if it's identical with previous
	if(ppbcurr != 0 && strcmp(ppbcurr->pszfilename, szpath) == 0) {
		XJSE_TRACE("(X) reusing identical img: [%s]", szpath);
		goto	cleanup;
	}

	xr1 = xtk_pixbuf_load(pjsectx->ptkctx, &ppb1, szpath, pdata, datasize);
	if(!XJSE_IS_SUCCESS(xr1)) {
		XJSE_TRACE("loading img failed: (%d)[%s, %s]", xr1, pszname, szpath);
		goto	failed;
	}

	// replace image data
	XJSE_SAFENNP(ppbcurr, xtk_pixbuf_delete);	// delete previous one
	*pppixbuf = ppb1;	// set new one

cleanup:
	return	xr1;

failed:
	goto	cleanup;
}
