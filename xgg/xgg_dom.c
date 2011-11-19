// DOM

#ifdef	XTPF_PSP
#include <psptypes.h>
#endif	//XTPF_PSP
#include <xjse.h>
#include "xgg.h"

#ifdef	NODELISTISCLASS
typedef	struct __tag_nodelistpriv {
	xjse_int_t	dummy;
}	XJSE_NODELISTPRIV;
#endif	//NODELISTISCLASS


	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

#if	0
static JSBool xgg_nodelist_propop_dummy_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XJSE_NODELISTPRIV	*pv0 = (XJSE_NODELISTPRIV*)JS_GetPrivate(pcxa, pobja);
	XJSE_TRACE("xgg_nodelist_propop_dummy_get. (%p)", pv0->pszresponsetext);

	return  JS_TRUE;
}
#endif

	// ----8< ---- functions

static JSBool xgg_domdocument_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_XDOMPRIV	*p0;
	p0 = (XJSE_XDOMPRIV*)xjse_malloc(0, sizeof(XJSE_XDOMPRIV));
	xjse_memset(0, p0, 0, sizeof(XJSE_XDOMPRIV));
	p0->pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	p0->pothis = pobja;
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}

	return  JS_TRUE;
}

static void	xgg_domdocument_dtor(JSContext *pcxa, JSObject *pobja)
{
	XJSE_ELEMENTPRIV	*pv0 = (XJSE_ELEMENTPRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		//XJSE_TRACE("xgg_element_dtor: freeing private obj ptr [0x%p]", pv0);
	}
}

static JSBool xgg_element_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_ELEMENTPRIV	*p0;
	p0 = (XJSE_ELEMENTPRIV*)xjse_malloc(0, sizeof(XJSE_ELEMENTPRIV));
	xjse_memset(0, p0, 0, sizeof(XJSE_ELEMENTPRIV));
	p0->te.poself = pobja;
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}

	return  JS_TRUE;
}

static void	xgg_element_dtor(JSContext *pcxa, JSObject *pobja)
{
	XJSE_ELEMENTPRIV	*pv0 = (XJSE_ELEMENTPRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		//XJSE_TRACE("xgg_element_dtor: freeing private obj ptr [0x%p]", pv0);
	}
}

static JSBool xgg_nodelist_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
#ifdef	NODELISTISCLASS
	XJSE_NODELISTPRIV	*p0;
	p0 = (XJSE_NODELISTPRIV*)xjse_malloc(0, sizeof(XJSE_NODELISTPRIV));
	xjse_memset(0, p0, 0, sizeof(XJSE_NODELISTPRIV));
	JSBool	b0 = JS_SetPrivate(pcxa, pobja, p0);
	if(b0 != JS_TRUE) {
		XJSE_TRACE("JS_SetPrivate failed!");
		return	JS_FALSE;
	}
#endif	//NODELISTISCLASS

	return  JS_TRUE;
}

static void	xgg_nodelist_dtor(JSContext *pcxa, JSObject *pobja)
{
#ifdef	NODELISTISCLASS
	XJSE_NODELISTPRIV	*pv0 = (XJSE_NODELISTPRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		//XJSE_TRACE("xgg_nodelist_dtor: freeing private obj ptr [0x%p]", pv0);
	}
#endif	//NODELISTISCLASS
}

/**
 */
static JSBool xgg_element_appendchild(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	return	JS_TRUE;
}

/** returns element's attribute (implemented as property value)
 */
static JSBool xgg_element_getattribute(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*pszattrname = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));
	jsval	v1;

	JSBool	b1 = JS_GetProperty(pcxa, pobja, pszattrname, &v1);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_GetProperty() failed!");
		goto	failed;
	}
	*rval = v1;

	return	JS_TRUE;

failed:
	return	JS_FALSE;
}

xjse_result_t xjse_xdom_parsexml(JSContext *pcxa, JSObject *pobja, const char* pszxmla, int nxmllena) ;
/** 
 */
static JSBool xgg_domdocument_loadxml(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	JSBool		b1 = JS_TRUE;
	char*		pszxml = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));

	xjse_result_t	xr1 = xjse_xdom_parsexml(
		pcxa, pobja, pszxml, strlen(pszxml));
	if(!XJSE_IS_SUCCESS(xr1)) {
		goto	failed;
	}

cleanup:
	return	b1;

failed:
	goto	cleanup;
}

static xjse_result_t xgg_getelemsbytagname(JSContext *pcxa, JSObject* ponodelista, XJSE_TREEELEM* pelema, const char* psztgt, xjse_uint_t* pucount)
{
	XJSE_TREEELEM*	pv0 = pelema;

	while(pv0 != 0) {
		jsval	vname;
		JSBool	b1 = JS_GetProperty(pcxa, pv0->poself, "__elemname", &vname);
		if(b1 != JS_TRUE) {
			XJSE_TRACE("(E) JS_GetProperty failed: [%d]", b1);
			goto	failed;
		}
		char	*pszname = JS_GetStringBytes(JS_ValueToString(pcxa, vname));
		//XJSE_TRACE("(X) comparing names... [%s, %s]", psztgt, pszname);
		if(strcmp(psztgt, pszname) == 0) {
			jsval	velement = OBJECT_TO_JSVAL(pv0->poself);
			//XJSE_TRACE("(X) XXXX found!: [%s]", pszname);
			b1 = JS_SetElement(pcxa, ponodelista, (*pucount)++, &velement);
			if(b1 != JS_TRUE) {
				XJSE_TRACE("(E) JS_GetElement failed: [%d]", b1);
				goto	failed;
			}
		}

		if(pv0->pfirstchild != 0) {
			xgg_getelemsbytagname(
				pcxa, ponodelista, pv0->pfirstchild, psztgt, pucount);
		}

		pv0 = pv0->pnextsibling;
	}

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}
/** returns name-matched NodeList.
 */
static JSBool xgg_element_getelementsbytagname(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSECTX*			pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	XJSE_ELEMENTPRIV*	pv1 = (XJSE_ELEMENTPRIV*)JS_GetPrivate(pcxa, pobja);
	xjse_uint_t		ucount = 0;
	char			*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));

	JSObject*	ponodelist1 = xgg_new_nodelist(pjsectx, pobja);
	if(ponodelist1 == NULL) {
		XJSE_TRACE("xgg_new_nodelist() failed!");
		goto	failed;
	}

	xjse_result_t	xr1 = xgg_getelemsbytagname(
		pcxa, ponodelist1, pv1->te.pfirstchild, psz1, &ucount);
	if(!XJSE_IS_SUCCESS(xr1)) {
		XJSE_TRACE("(X) xgg_getelemsbytagname() failed!");
		goto	failed;
	}

	if(ucount == 0) {
		XJSE_TRACE("(W) name [%s] not found!", psz1);
	}

cleanup:
	*rval = OBJECT_TO_JSVAL(ponodelist1);

	return  JS_TRUE;

failed:
	return	JS_FALSE;
}
/**
 */
static JSBool xgg_xdom_getelementsbytagname(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	JSBool				b1 = JS_TRUE;
	XJSECTX*			pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	XJSE_XDOMPRIV*		pv1 = (XJSE_XDOMPRIV*)JS_GetPrivate(pcxa, pobja);
	jsval				vrootelem;

	b1 = JS_GetElement(pjsectx->pctx, pv1->ponodelist, 0, &vrootelem);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_GetElement() failed!");
		goto	failed;
	}

	JSObject	*porootelem = 0;
	b1 = JS_ValueToObject(pcxa, vrootelem, &porootelem);
	if(b1 != JS_TRUE || porootelem == 0) {
		XJSE_TRACE("(E) JS_ValueToObject() failed!");
		goto	failed;
	}

	return	xgg_element_getelementsbytagname(
		pcxa, porootelem, argc, pva1, rval);

failed:
	return	JS_FALSE;
}

	// ----8< ---- class definition

static JSClass class_domdocument = {
	"DOMDocument", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_domdocument_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_domdocument[] = {
	{	0,	},
};

JSFunctionSpec	funcs_domdocument[] = {
	{	"loadXML", 				xgg_domdocument_loadxml,	1, 0, 0,	},
	{	"getElementsByTagName",	xgg_xdom_getelementsbytagname,	1, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

static JSClass class_element = {
	"Element", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_element_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_element[] = {
	{	0,	},
};

JSFunctionSpec	funcs_element[] = {
	{	"appendChild", 			xgg_element_appendchild,	1, 0, 0,	},
	{	"getAttribute",			xgg_element_getattribute,	1, 0, 0,	},
	{	"getElementsByTagName",	xgg_element_getelementsbytagname,	1, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

static JSClass class_nodelist = {
	"NodeList", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_nodelist_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_nodelist[] = {
	{	"item",		21, (0),
		JS_PropertyStub, JS_PropertyStub,	},
	{	"length",	22, (0),
		JS_PropertyStub, JS_PropertyStub,	},
	{	0,	},
};

JSFunctionSpec	funcs_nodelist[] = {
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xgg_dom_define(JSContext *pcxa, JSObject *poglobal)
{
	JSObject	*pobj_proto_domdocument = JS_InitClass(pcxa,
		poglobal, NULL, &class_domdocument, xgg_domdocument_ctor, 0,
		props_domdocument, funcs_domdocument, props_domdocument, funcs_domdocument);
	if(pobj_proto_domdocument == 0)
		goto	failed;

	JSObject	*pobj_proto_element = JS_InitClass(pcxa,
		poglobal, NULL, &class_element, xgg_element_ctor, 0,
		props_element, funcs_element, props_element, funcs_element);
	if(pobj_proto_element == 0)
		goto	failed;

	JSObject	*pobj_proto_nodelist = JS_InitClass(pcxa,
		poglobal, NULL, &class_nodelist, xgg_nodelist_ctor, 0,
		props_nodelist, funcs_nodelist, props_nodelist, funcs_nodelist);
	if(pobj_proto_nodelist == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

JSObject*	xgg_new_element(XJSECTX* pctxa, JSObject* poparent)
{
//	JSObject*	pobjbase = 0;
//	JSObject*	pobj = JS_DefineObject(pctxa->pctx, poparent,
//		pszobjname, &class_element, pobjbase,
//		JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);
	JSObject*	pobj = JS_NewObject(
		pctxa->pctx, &class_element, (JSObject*)NULL, poparent);
	JSBool	b1 = xgg_element_ctor(pctxa->pctx, pobj, 0, 0, 0);	// args, rval
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) xgg_new_element(ctor) failed!");
	}

	return	pobj;
}

JSObject*	xgg_new_nodelist(XJSECTX* pctxa, JSObject* poparent)
{
	JSBool	b1;
#ifdef	NODELISTISCLASS
	JSObject*	pobjbase = 0;
	JSObject*	pobj = JS_DefineObject(pctxa->pctx, poparent,
		pszobjname, &class_nodelist, pobjbase,
		JSPROP_PERMANENT | JSPROP_READONLY | JSPROP_ENUMERATE);
#else	//NODELISTISCLASS
	JSObject*	pobj = JS_NewArrayObject(pctxa->pctx, 0, NULL);
	if(pobj == 0) {
		XJSE_TRACE("(E) JS_NewArrayObject() failed!");
		goto	failed;
	}
#if	0
	b1 = JS_DefineFunctions(pctxa->pctx, pobj, funcs_nodelist);
	if(b1 != JS_TRUE)
		goto	failed;
	b1 = JS_DefineProperties(pctxa->pctx, pobj, props_nodelist);
	if(b1 != JS_TRUE)
		goto	failed;
#endif
#endif	//NODELISTISCLASS
	//
	b1 = xgg_nodelist_ctor(pctxa->pctx, pobj, 0, 0, 0);    //args, rval
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) xgg_nodelist_ctor failed!");
	}
	//
	return  pobj;

failed:
	return	0;
}
// XMLHttpRequest

	// ----8< ---- XDOM

static void xjse_xdom_on_startelement(void *userData, const XML_Char *name, const XML_Char **atts)
{
	XJSE_XDOMPRIV*	pv0 = (XJSE_XDOMPRIV*)userData;
	JSBool			b1;

	pv0->nelemlevel++;
	if(pv0->nelems + 1 > XGG_NMAXELEMS
		|| pv0->nelemlevel > XGG_NMAXELEMLEVEL) {
		XJSE_TRACE("(E) elems excessed limit! (%d, %d)",
			pv0->nelems, pv0->nelemlevel);
		goto	failed;
	}

	//XJSE_TRACE("(X) startelement <%s>", name);
	JSObject*	ponode = xgg_new_element(pv0->pjsectx, pv0->pothis);
	if(ponode == NULL) {
		XJSE_TRACE("(E) xgg_new_element() failed!");
		goto	failed;
	}
	// element name -> name property
	jsval	v1 = STRING_TO_JSVAL(JS_NewStringCopyZ(pv0->pjsectx->pctx, name));
	b1 = JS_SetProperty(pv0->pjsectx->pctx, ponode, "__elemname", &v1);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_SetProperty() failed!");
		goto	failed;
	}
	//XJSE_TRACE("(*) element (%d, %d)<%s>", pv0->nelems, pv0->nelemlevel, name);

	// attributes -> property
	xgg_set_all_attrs_as_property(pv0->pjsectx->pctx, ponode, atts, 0);

	// set the element as an array element
	jsval	vnode = OBJECT_TO_JSVAL(ponode);
	b1 = JS_SetElement(
		pv0->pjsectx->pctx, pv0->ponodelist, pv0->nelems, &vnode);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_SetElement() failed!");
		goto	failed;
	}

	//
	{
		XJSE_ELEMENTPRIV*	ppriv0 = JS_GetPrivate(pv0->pjsectx->pctx, ponode);
		XJSE_TREEELEM*		p0 = &(ppriv0->te);
		xjse_treeelem_family_set(p0, pv0->apelems, pv0->apelemstack, 0,
			pv0->nelems++, pv0->nelemlevel);
	}

	return;

failed:
	XJSE_TRACE("(E) xjse_xdom_on_startelement() failed!");
	return;
}

static void xjse_xdom_on_endelement(void *userData, const XML_Char *name)
{
	XJSE_XDOMPRIV*	pv0 = (XJSE_XDOMPRIV*)userData;
	//XJSE_TRACE("(X) endelement <%s>", name);
	// pop self
	pv0->apelemstack[pv0->nelemlevel] = 0;
	pv0->nelemlevel--;
}

void xjse_xdom_on_startelement(void *userData, const XML_Char *name, const XML_Char **atts) ;
/**
 * (o) pv0->ponodelist
 */
xjse_result_t xjse_xdom_parsexml(JSContext *pcxa, JSObject *pobja, const char* pszxmla, int nxmllena)
{
	XJSE_XDOMPRIV*	pv0 = (XJSE_XDOMPRIV*)JS_GetPrivate(pcxa, pobja);
	XJSECTX*		pjsectx = (XJSECTX*)JS_GetContextPrivate(pcxa);
	XML_Parser		xmlparser;

	if(pszxmla == 0) {
		goto	failed;
	}

	if(pv0->nelems != 0) {
		pv0->ponodelist = 0;
		pv0->nelems = 0;
	}

	JSObject*	ponodelist = xgg_new_nodelist(pjsectx, pobja);
	if(ponodelist == NULL) {
		XJSE_TRACE("xgg_new_nodelist() failed!");
		goto	failed;
	}
	pv0->ponodelist = ponodelist;

	xmlparser = XML_ParserCreate("UTF-8");
	if(xmlparser == 0) {
		XJSE_TRACE("XML_ParserCreate failed!");
		goto	failed;
	}
	XML_SetUserData(xmlparser, pv0);
	XML_SetElementHandler(xmlparser,
		xjse_xdom_on_startelement, xjse_xdom_on_endelement);

	if(!XML_Parse(xmlparser, pszxmla, nxmllena, 0)) {
		xjse_expat_error_report(xmlparser, "xhr");
		goto	failed;
	}

	xjse_uint_t	ulen = 0;
	JSBool  b1 = JS_GetArrayLength(pjsectx->pctx, ponodelist, &ulen);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_GetArrayLength failed: [%d]", b1);
		goto	failed;
	}
	if(ulen == 0) {
		XJSE_TRACE("(W) nothing's contained in NodeList.");
	} else {
		XJSE_TRACE("(X) array length: [%u]", ulen);
	}

cleanup:
	XJSE_SAFENNP(xmlparser, XML_ParserFree);
	return  XJSE_SUCCESS;

failed:
	goto	cleanup;
}

