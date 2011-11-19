
#include <xjse.h>
#include <xtk.h>
#include "xgg.h"


typedef	struct	__tag_xgg_parsectx {
	XGGCTX*		pggctx;
	JSObject*	pobjcurrent;
}	XGGPARSECTX;

/** Define Object Types
 */
xjse_result_t	xjse_xgg_define(XJSECTX* pctxa)
{
	xjse_result_t	res1;

#define	XGG_DEFINE_CLASS(classname)	{	\
	res1 = xjse_xgg_define_##classname(pctxa);	\
	if(!XJSE_IS_SUCCESS(res1)) {	\
		goto	failed;	\
	}	\
}
	XGG_DEFINE_CLASS(debug);
	XGG_DEFINE_CLASS(system);
	XGG_DEFINE_CLASS(options);
	//XGG_DEFINE_CLASS(view);
	XGG_DEFINE_CLASS(button);
	XGG_DEFINE_CLASS(div);
	XGG_DEFINE_CLASS(img);
	XGG_DEFINE_CLASS(label);
	XGG_DEFINE_CLASS(progressbar);
	XGG_DEFINE_CLASS(plugin);
	XGG_DEFINE_CLASS(pluginhelper);

	return	XJSE_SUCCESS;

failed:
	return	res1;
}

/** set all attirbutes in XML element to the js object as its js properties.
 */
void xgg_set_all_attrs_as_property(XGGCTX* pctxa, JSObject* pobja, const XML_Char** atts, char** ppszname, xjse_bool_t* pbhasmouseevent)
{
	XML_Char		**ppxc = (XML_Char**)atts;

	while((*ppxc) != 0) {
		char	*ptemp0 = (char*)JS_malloc(
			pctxa->pjsectx->pctx, xjse_strlen(0, *(ppxc + 1)) + 1);
		xjse_strcpy(0, ptemp0, *(ppxc + 1));

		// set as property
		jsval	v0 = STRING_TO_JSVAL(JS_NewString(pctxa->pjsectx->pctx,
			ptemp0, xjse_strlen(0, *(ppxc + 1))));
		JSBool	b1 = JS_SetProperty(
			pctxa->pjsectx->pctx, pobja, *(ppxc + 0), &v0);
		if(b1 != JS_TRUE) {
			XJSE_TRACE("(E) JS_SetProperty failed! (%s, %s)",
				*(ppxc + 0), *(ppxc + 1));
		}

		if(strcmp(*(ppxc + 0), "name") == 0) {
			*ppszname = ptemp0;
			//XJSE_TRACE("<x> (%s = %s)", *(ppxc + 0), ptemp0);
		}
		//XJSE_TRACE("<x> (%s = %s)", *(ppxc + 0), *(ppxc + 1));

		if(memcmp(*(ppxc + 0), "onmouse", 7) == 0) {
			if(pbhasmouseevent != 0)
				*pbhasmouseevent = XJSE_B_TRUE;
		}

		ppxc = ppxc + 2;
	}
}

/**
 * @param pdirectsiblinga (insertElement) element is being inserted before this
 */
xjse_result_t xjse_treeelem_family_set(XJSE_TREEELEM* pelema, XJSE_TREEELEM* apelemsa[], XJSE_TREEELEM* apelemstacka[], XJSE_TREEELEM* pdirectsiblinga, xjse_int_t nelemsa, xjse_int_t nelemlevela)
{
	XJSE_TREEELEM*	p0 = pelema;

	if(nelemlevela <= 0) {
		goto	cleanup;
	}

	// set parent ptr
	XJSE_TREEELEM*	pparent0 = apelemstacka[nelemlevela - 1];
	p0->pparent = pparent0;
	if(pparent0 != 0 && pparent0->pfirstchild == 0)
		pparent0->pfirstchild = p0; 

	// find the last sibling and link it to this (appendElement)
	// or, find direct sibling and linkt this to it (insertElement)
	xjse_int_t		ntemp0 = 0;
	XJSE_TREEELEM	*ptempprev0 = 0, *ptemp0 = apelemsa[ntemp0];
	while(ptemp0 != 0) {
		if(pdirectsiblinga == 0) {
			// appendElement
			if(pparent0 == ptemp0->pparent &&
				ptemp0->pnextsibling == 0) {
				ptemp0->pnextsibling = p0; 
				break;
			}
		} else {
			// insertElement
			if(ptemp0 == pdirectsiblinga) {
				//XJSE_TRACE("(X) XXXX direct sibling found!");
				if(ptempprev0 == 0) {
					p0->pnextsibling = pparent0->pfirstchild;
					pparent0->pfirstchild = p0;
				} else {
					ptempprev0->pnextsibling = p0;
					p0->pnextsibling = ptemp0;
				}
				break;
			}
		}
		ptempprev0 = ptemp0;
		ptemp0 = apelemsa[++ntemp0];
	}

cleanup:
	// stack self
	apelemstacka[nelemlevela] = p0;
	apelemsa[nelemsa] = p0;

	return	XJSE_SUCCESS;
}

static void	xgg_on_startelement(void *userData, const XML_Char *name, const XML_Char **atts)
{
	XGGPARSECTX*	pparsectx = (XGGPARSECTX*)userData;
	XGGCTX*			pctx1 = pparsectx->pggctx;
	XML_Char		**ppxc = (XML_Char**)atts;
	JSObject*		pobj1 = 0;

	pctx1->nelemlevel++;
	//XJSE_TRACE("xgg_on_startelement [%d](%p, %s)",
	//	pctx1->nelemlevel, userData, name);

	if(pctx1->nelems + 1 > XGG_NMAXELEMS) {
		XJSE_TRACE("(E) elements over [%d] not supported!", XGG_NMAXELEMS);
		goto	failed;
	}

	if(xjse_strcmp(0, name, "view") == 0) {
		pobj1 = pctx1->pjsectx->poglobal;	// view == global
		pctx1->pview = pobj1;
	} else
	if(xjse_strcmp(0, name, "button") == 0) {
		char		*pszname = xjse_expat_get_attribute_linear(ppxc, "name");
		if(pszname == 0)
			pszname = "__xgg_button_temp";
		pobj1 = xjse_xgg_new_button(pctx1->pjsectx, pszname);
	} else
	if(xjse_strcmp(0, name, "div") == 0) {
		char		*pszname = xjse_expat_get_attribute_linear(ppxc, "name");
		if(pszname == 0)
			pszname = "__xgg_div_temp";
		pobj1 = xjse_xgg_new_div(pctx1->pjsectx, pszname);
	} else
	if(xjse_strcmp(0, name, "img") == 0) {
		char		*pszname = xjse_expat_get_attribute_linear(ppxc, "name");
		if(pszname == 0)
			pszname = "__xgg_img_temp";
		pobj1 = xjse_xgg_new_img(pctx1->pjsectx, pszname);
	} else
	if(xjse_strcmp(0, name, "label") == 0) {
		char		*pszname = xjse_expat_get_attribute_linear(ppxc, "name");
		if(pszname == 0)
			pszname = "__xgg_label_temp";
		pobj1 = xjse_xgg_new_label(pctx1->pjsectx, pszname);
	} else
	if(xjse_strcmp(0, name, "progressbar") == 0) {
		char		*pszname = xjse_expat_get_attribute_linear(ppxc, "name");
		if(pszname == 0)
			pszname = "__xgg_progressbar_temp";
		pobj1 = xjse_xgg_new_progressbar(pctx1->pjsectx, pszname);
	} else
	if(xjse_strcmp(0, name, "script") == 0) {
		char		*pszsrc = xjse_expat_get_attribute_linear(ppxc, "src");
		// Evaluate/ execute script
		JSBool		b1;
		jsval		rval;
		char		szjsfilename[XJSE_NMAX_PATH + 1];
		JSScript	*pscript1;

		xjse_snprintf(0, szjsfilename, (XJSE_NMAX_PATH + 1), "./%s/%s/%s",
			XJSE_PATH_XADGETS, pctx1->pjsectx->pszname, pszsrc);
		xjse_result_t	xr0 = xjse_js_compilefile(
			pctx1->pjsectx->pctx, pctx1->pjsectx->poglobal,
			szjsfilename, &pscript1);
		if(!XJSE_IS_SUCCESS(xr0) || pscript1 == 0)
			goto	failed;
		b1 = JS_ExecuteScript(
			pctx1->pjsectx->pctx, pctx1->pjsectx->poglobal, pscript1, &rval);
		if(b1 != JS_TRUE)
			goto	failed;
		JS_DestroyScript(pctx1->pjsectx->pctx, pscript1);

		//if(JS_TypeOfValue(pctx1->pjsectx->pctx, rval) != JSTYPE_VOID) {
		//XJSE_TRACE("script result: (%d) %s", (b1 == JS_TRUE),
		//	JS_GetStringBytes(JS_ValueToString(pctx1->pjsectx->pctx, rval)));
	} 

	if(pobj1 != 0) {
		XGGPRIVHDR		*ppriv0 = JS_GetPrivate(pctx1->pjsectx->pctx, pobj1);
		XJSE_TREEELEM	*p0 = &(ppriv0->te);
		p0->poself = pobj1;
		xjse_treeelem_family_set(p0, pctx1->apelems, pctx1->apelemstack,
			pctx1->pdirectsibling, pctx1->nelems++, pctx1->nelemlevel);
		//
		xjse_bool_t	bhasmouseevent = XJSE_B_FALSE;
		xgg_set_all_attrs_as_property(
			pctx1, pobj1, atts, &(p0->pszname), &bhasmouseevent);
		if(bhasmouseevent == XJSE_B_TRUE) {
			XJSE_TRACE("(X) XXXX hasmouseevent!");
			p0->uflags |= XJSEC_TREEELEMFLAG_HASMOUSEEVENT;
		}
	}

	pparsectx->pobjcurrent = pobj1;

	return;

failed:
	XJSE_TRACE("(E) xgg_on_startelement failed!");
	return;
}

static void	xgg_on_endelement(void *userData, const XML_Char *name)
{
	XGGPARSECTX*	pparsectx = (XGGPARSECTX*)userData;
	XGGCTX*			pctx1 = pparsectx->pggctx;

	//XJSE_TRACE("xgg_on_endelement [%d](%p, %s)",
	//	pctx1->nelemlevel, userData, name);
	// pop self
	pctx1->apelemstack[pctx1->nelemlevel] = 0;
	pctx1->nelemlevel--;
}

static void	xgg_on_characterdata(void *userData, const XML_Char *s, int len)
{
	XGGPARSECTX*	pparsectx = (XGGPARSECTX*)userData;
	XGGCTX*			pctx1 = pparsectx->pggctx;
	char*			psztemp = 0;

	psztemp = (char*)xjse_malloc(0, len + 1);
	strncpy(psztemp, s, len);
	psztemp[len] = 0;

	if(pparsectx->pobjcurrent != 0) {
		JSContext*	pjsctx = pctx1->pjsectx->pctx;
		XGGPRIVHDR*	phdr = (XGGPRIVHDR*)JS_GetPrivate(
			pjsctx, pparsectx->pobjcurrent);
		if(phdr != 0 && phdr->type == XGG_ETYP_LABEL) {
			// handle the undefined entity ref as the element in strings.xml
			jsval	v0 = STRING_TO_JSVAL(JS_NewStringCopyZ(pjsctx, psztemp));
			JSBool	b1 = JS_SetProperty(
				pjsctx, pparsectx->pobjcurrent, "innerText", &v0);
			if(b1 != JS_TRUE) {
				XJSE_TRACE("(E) failed to set innerText property!");
			}
		}
	}

//cleanup:
	XJSE_SAFENNP(psztemp, free);
}

static void	xgg_on_skippedentity(void *userData, const XML_Char *name, int is_parameter_entity)
{
	XGGPARSECTX*	pparsectx = (XGGPARSECTX*)userData;
	XGGCTX*			pctx1 = pparsectx->pggctx;
	JSBool			b1;
	jsval			v1;
	char*			pszval1 = 0;
	JSContext*		pjsctx = pctx1->pjsectx->pctx;

	b1 = JS_GetProperty(pjsctx, pctx1->pjsectx->poglobal, name, &v1);
	if(b1 == JS_TRUE) {
		pszval1 = JS_GetStringBytes(JS_ValueToString(pjsctx, v1));
		xgg_on_characterdata(userData, pszval1, strlen(pszval1));
		XJSE_TRACE("(*) <label> as entity ref: [%s -> %s]", name, pszval1);
	}
}

/** load main.xml.
 */
xjse_result_t xgg_load_one(XJSECTX* pjsectxa)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	char			szpath[XJSE_NMAX_PATH];
	XGGCTX*			pggctx = 0;

	pggctx = (XGGCTX*)xjse_malloc(0, sizeof(XGGCTX));
	xjse_memset(0, pggctx, 0, sizeof(XGGCTX));
	pjsectxa->pggctx = pggctx;		// cross reference XJSECTX -> XGGCTX
	pggctx->pjsectx = pjsectxa;		// cross reference XGGCTX -> XJSECTX
	pggctx->nelemlevel = -1;		// on_startelement makes this +1

	xjse_snprintf(0, szpath, sizeof(szpath) - 1, "%s/%s/main.xml",
		XJSE_PATH_XADGETS, pjsectxa->pszname);

	XJSE_EXPAT_CBS	cbs = {
		xgg_on_startelement, xgg_on_endelement,
		xgg_on_characterdata, xgg_on_skippedentity,	};
	XGGPARSECTX		parsectx = {	pggctx, 0,	};
	xr1 = xjse_parse_xmlfile(szpath, &parsectx, &cbs);
	if(!XJSE_IS_SUCCESS(xr1)) { 
		goto	failed;
	}

cleanup:
	return	xr1;

failed:
	XJSE_SAFENNP(pggctx, free);
	goto	cleanup;
}

/** load main.xml.
 */
xjse_result_t xgg_exec_onopen(XJSECTX* pjsectxa)
{
	if(pjsectxa->pggctx->pview == 0) {
		XJSE_TRACE("(W) pview is not initialized!");
		goto	failed;
	}

	// exec onopen
	jsval	v1, vr1;
	JSBool	b1 = JS_GetProperty(
		pjsectxa->pctx, pjsectxa->pggctx->pview, "onopen", &v1);
	if(b1 != JS_TRUE)
		goto	failed;

	char*	pszonopen = JS_GetStringBytes(
		JS_ValueToString(pjsectxa->pctx, v1));
	//char*	pszonopen = JS_GetStringBytes(JSVAL_TO_STRING(v1));	// X
	XJSE_TRACE("exec'ing onopen... [%s]", pszonopen);
	b1 = JS_EvaluateScript(pjsectxa->pctx, pjsectxa->poglobal,
		pszonopen, xjse_strlen(0, pszonopen), "__xgg_call_onopen",
		(uint32)0, &vr1);

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

xjse_result_t xgg_cleanup_one(XJSECTX* pjsectxa)
{
	XJSE_SAFENNP(pjsectxa->pggctx, free);

	return	XJSE_SUCCESS;
}


/** focus select - find the next end node.
 */
void xgg_find_nextendnode(XJSE_TREEELEM* pnodea, XJSE_TREEELEM** ppcurrenta, XJSE_TREEELEM** ppfounda, xjse_uint_t ucondflags)
{
	XJSE_TREEELEM	*pe1 = pnodea;

	while(pe1 != 0) {
		XJSE_TREEELEM	*pe0 = 0;
		//XJSE_TRACE("(X) XXXX searching... 1 [%s](%p, %p)",
		//	pe1->pszname, *ppcurrenta, pe1->pfirstchild);
		if(pe1->pfirstchild != 0) {
			xgg_find_nextendnode(pe1->pfirstchild, ppcurrenta, &pe0, ucondflags);
			if(pe0 != 0)
				pe1 = pe0;
		}
		//XJSE_TRACE("(X) XXXX searching... 2 [%s](%p, %p)",
		//	pe1->pszname, *ppcurrenta, pe1);
		if(*ppcurrenta == 0 && pe1 != 0) {
			if(ucondflags == 0 || (pe1->uflags && ucondflags) != 0)
				break;
		}
		if(pe1 == *ppcurrenta) {
			*ppcurrenta = 0;
		}
		pe1 = pe1->pnextsibling;
	}

	*ppfounda = pe1;
}


JSBool xgg_global_strings(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval)
{
	return	JS_TRUE;
}

/** read the first argument of setTimer() or beginAnimation() and convert
 * the argument jsval to JSFunction* or char*.
 */
static xjse_result_t xgg_global_read_settimer_argf(JSContext *pcxa, jsval varg1a, JSFunction **ppfa, char** ppszfa)
{
	JSFunction	*pfexec = 0;
	char		*pszexec = 0;

	if(JSVAL_IS_STRING(varg1a)) {
		char	*psz0 = JS_GetStringBytes(JS_ValueToString(pcxa, varg1a));
		pszexec = (char*)xjse_malloc(0, strlen(psz0) + 1);
		xjse_strcpy(0, pszexec, psz0);
	} else
	if(JSVAL_IS_OBJECT(varg1a)) {
		pfexec = JS_ValueToFunction(pcxa, varg1a);
		if(pfexec == 0) {
			XJSE_TRACE("(X) beginAnimation(): the first argument object "
				"is not Function!");
			goto	failed;
		}
	} else {
		XJSE_TRACE("(X) beginAnimation(): the first argument is not "
			"a String nor a Function!");
		goto	failed;
	}

	if(ppfa != 0)
		*ppfa = pfexec;
	if(ppszfa != 0)
		*ppszfa = pszexec;

	return	JS_TRUE;

failed:
	return	JS_FALSE;
}

static JSBool xgg_global_settimer_with_flags(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval, xjse_uint_t uflags)
{
	xjse_int_t	ntimeout = 0, ntimerid = 0;
	//JSFunction	*pf0 = JS_ValueToFunction(pcxa, pva1[0]);
	JSBool		b0 = JS_TRUE;
	//char		*pszarg1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));
	//char		*pszarg1 = JS_GetStringBytes(JSVAL_TO_STRING(pva1[0]));	// X
	JSFunction	*pfexec = 0;
	char		*pszexec = 0;

	b0 = JS_ValueToInt32(pcxa, pva1[1], &ntimeout);
	if(b0 != JS_TRUE)
		goto	failed;

	xjse_result_t	xr1 = xgg_global_read_settimer_argf(
		pcxa, pva1[0], &pfexec, &pszexec);
	if(b0 != JS_TRUE)
		goto	failed;

	//XJSE_TRACE("SetTimeout (%p, %p)(%d, %s)", pszexec, pfexec, ntimeout, JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0])));
	xr1 = xjse_timer_add((XJSECTX*)JS_GetContextPrivate(pcxa),
		(xjse_timeout_t)ntimeout, 0, pszexec, pfexec, 0, 0, uflags, &ntimerid);
	if(!XJSE_IS_SUCCESS(xr1)) {
		b0 = JS_FALSE;
		goto	failed;
	}

	*rval = INT_TO_JSVAL(ntimerid);

    return  JS_TRUE;

failed:
	return	b0;
}

// SetTimeout(callback-function(void), timeout-msec)
JSBool xgg_global_settimeout(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval)
{
	return	xgg_global_settimer_with_flags(pcxa, pobja, argc, pva1, rval,
		0);
}
JSBool xgg_global_setinterval(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval)
{
	return	xgg_global_settimer_with_flags(pcxa, pobja, argc, pva1, rval,
		XJSE_TIMERFLAGS_RECURR);
}

/** find pobja from apelems.
 * @param pptedirectsibling returns the just elder one, if exists
 */
static xjse_result_t xgg_apelems_find(XJSECTX* pjsectx, JSObject* poparenta, XJSE_TREEELEM** pptea, xjse_int_t* pnlevela, XJSE_TREEELEM** pptedirectsibling)
{
	xjse_int_t		i;
	XGGPRIVHDR*		pggpriv = JS_GetPrivate(pjsectx->pctx, poparenta);
	XJSE_TREEELEM	*pe0, *pe1 = &(pggpriv->te), *pefound = 0;
	XJSE_TREEELEM	*peds1 = 0;

	for(i = 0; i < XGG_NMAXELEMS; i++) {
		pe0 = pjsectx->pggctx->apelems[i];
		if(pe1 == pe0) {
			pefound = pe1;
			//XJSE_TRACE("(X) element found! [%d, %p]",
			//	pggpriv->type, pefound->pparent);
		}
		if(pe1->pnextsibling == pe0) {
			peds1 = pe1;
		}
	}
	if(pefound == 0) {
		XJSE_TRACE("(X) element not found! [%d]", pggpriv->type);
		goto	failed;
	}
	// count level of the found elem
	pe0 = pefound;
	for(i = 0; pe0 != 0; i++) {
		if(pe0->pparent == 0)
			break;
		pe0 = pe0->pparent;
	}
	//XJSE_TRACE("(X) level: [%d]", i);

	//
	if(pptea != 0)
		*pptea = pefound;
	if(pnlevela != 0)
		*pnlevela = i;
	if(pptedirectsibling != 0)
		*pptedirectsibling = peds1;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

#ifdef	XDELETED20080417	//-> xgg_global_insertelement
JSBool xgg_global_appendelement(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval)
{
	XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	char*			pszxml = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));

	{
		xjse_int_t		nlevel;
		XJSE_TREEELEM*	pe1;
		xgg_apelems_find(pjsectx, pobja, &pe1, &nlevel, 0);

		// prepare parsing context
		pjsectx->pggctx->nelemlevel = nlevel;
		pjsectx->pggctx->apelemstack[nlevel] = pe1;
	}

	XJSE_TRACE("(X) appendElement parsing: [%s]", pszxml);
	XJSE_EXPAT_CBS	cbs = {
		xgg_on_startelement, xgg_on_endelement,
		xgg_on_characterdata, xgg_on_skippedentity,	};
	XGGPARSECTX		parsectx = {	pjsectx->pggctx, 0,	};
	xjse_result_t	xr1 = xjse_parse_xmlstring(pszxml, &parsectx, &cbs);
	if(!XJSE_IS_SUCCESS(xr1)) {
		XJSE_TRACE("(E) xjse_parse_xmlstring() failed!");
		goto	failed;
	}

	//XJSE_TRACE("(X) obj: [%p]", parsectx.pobjcurrent);
	*rval = OBJECT_TO_JSVAL(parsectx.pobjcurrent);

	return	JS_TRUE;

failed:
	return	JS_FALSE;
}
#endif	//XDELETED20080417

/** appendElement/ insertElement
 * arg[0]: xml string that represents the element being inserted/ appended
 * arg[1]: object that the element being inserted/ appended before
 */
JSBool xgg_global_insertelement(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval)
{
	char*		pszxml = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));
	JSObject*	poref1 = (argc > 1 ? JSVAL_TO_OBJECT(pva1[1]) : 0);
	XJSECTX*	pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);

	{
		xjse_int_t		nlevel1, nlevel2;
		XJSE_TREEELEM	*pe1, *pe2 = 0;

		xgg_apelems_find(pjsectx, pobja, &pe1, &nlevel1, 0);
		if(poref1 != 0) {
			xgg_apelems_find(pjsectx, poref1, &pe2, &nlevel2, 0);
			XJSE_TRACE("(X) insertElement [%s], (%p, %d)",
				JS_GetStringBytes(JS_ValueToString(pcxa,
				OBJECT_TO_JSVAL(poref1))), pe2, nlevel2);
		}

		// prepare parsing context
		pjsectx->pggctx->nelemlevel = nlevel1;
		pjsectx->pggctx->apelemstack[nlevel1] = pe1;
		pjsectx->pggctx->pdirectsibling = pe2;
	}

	XJSE_TRACE("(X) insertElement parsing: [%s]", pszxml);
	XJSE_EXPAT_CBS	cbs = {
		xgg_on_startelement, xgg_on_endelement,
		xgg_on_characterdata, xgg_on_skippedentity,	};
	XGGPARSECTX		parsectx = {	pjsectx->pggctx, 0,	};
	xjse_result_t	xr1 = xjse_parse_xmlstring(pszxml, &parsectx, &cbs);
	if(!XJSE_IS_SUCCESS(xr1)) {
		XJSE_TRACE("(E) xjse_parse_xmlstring() failed!");
		goto	failed;
	}

	//XJSE_TRACE("(X) obj: [%p]", parsectx.pobjcurrent);
	*rval = OBJECT_TO_JSVAL(parsectx.pobjcurrent);

	return	JS_TRUE;

failed:
	return	JS_FALSE;
}

/** removeElement()
 */
JSBool xgg_global_removeelement(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval)
{
	XJSECTX*	pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	XJSE_TREEELEM	*pte1 = 0, *pte2 = 0;
	xjse_int_t		nlevel = 0;

	//XJSE_TRACE("(X) XXXX removeElement() called!");
	xgg_apelems_find(pjsectx, pobja, &pte1, &nlevel, &pte2);
	if(pte1 == 0) {
		XJSE_TRACE("(E) removeElement(): object not found!");
		goto	failed;
	}

	// operate XJSE_TEEELEM tree
	if(pte1->pparent != 0 && pte1->pparent->pfirstchild == pte1) {
		XJSE_TRACE("(X) removeElement(): splitting from parent...");
		pte1->pparent->pfirstchild = pte1->pnextsibling;
	}
	if(pte2 != 0) {
		//XJSE_TRACE("(X) removeElement(): splitting from sibling...");
		pte2->pnextsibling = pte1->pnextsibling;
	}

	// JSObject should be deleted, but it does not have relation to something...
	JSObject*	poparent = JS_GetParent(pcxa, pobja);
	if(poparent != 0) {
		XJSE_TRACE("(X) removeElement(): parent [%s]", 
			JS_GetStringBytes(JS_ValueToString(pcxa, OBJECT_TO_JSVAL(poparent))));
	}

	return	JS_TRUE;

failed:
	return	JS_FALSE;
}

JSBool xgg_global_beginanimation(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval)
{
	xjse_int_t	ntimeout, nstartval, nendval, ntimerid = 0;
	JSBool		b0 = JS_TRUE;
	xjse_uint_t	uflags = (XJSE_TIMERFLAGS_ANIM | XJSE_TIMERFLAGS_RECURR);
	JSFunction	*pfexec = 0;
	char		*pszexec = 0;

	b0 = JS_ValueToInt32(pcxa, pva1[1], &nstartval);
	if(b0 != JS_TRUE)
		goto	failed;
	b0 = JS_ValueToInt32(pcxa, pva1[2], &nendval);
	if(b0 != JS_TRUE)
		goto	failed;
	b0 = JS_ValueToInt32(pcxa, pva1[3], &ntimeout);
	if(b0 != JS_TRUE)
		goto	failed;

	b0 = xgg_global_read_settimer_argf(pcxa, pva1[0], &pfexec, &pszexec);
	if(b0 != JS_TRUE)
		goto	failed;

	XJSE_TRACE("(X) XXXX beginAnimation(%d, %d -> %d)",
		ntimeout, nstartval, nendval);
	xjse_timeout_t	tointerval = 100;
	xjse_result_t	xr1 = xjse_timer_add((XJSECTX*)JS_GetContextPrivate(pcxa),
		tointerval, (xjse_timeout_t)ntimeout, pszexec, pfexec,
		nstartval, nendval, uflags, &ntimerid);
	if(!XJSE_IS_SUCCESS(xr1)) {
		goto	failed;
	}

	*rval = INT_TO_JSVAL(ntimerid);

cleanup:
    return  b0;

failed:
	// what should be returned at error?
	//*rval = INT_TO_JSVAL(0);
	b0 = JS_FALSE;

	goto	cleanup;
}

// ClearInterval(timer identifier)
JSBool xgg_global_clearinterval(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval)
{
	XJSECTX*	pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	xjse_int_t	ntimerid = 0;
	JSBool		b0 = JS_TRUE;

	b0 = JS_ValueToInt32(pcxa, pva1[0], &ntimerid);
	if(b0 != JS_TRUE)
		goto	failed;

	xjse_timer_destroy(pjsectx, ntimerid);

    return  JS_TRUE;

failed:
	return	b0;
}

