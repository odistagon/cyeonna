
#include "xjse.h"
#include <jscntxt.h>	// required for (JSContext*)->fp->flags


#define	XJSE_C_TO_X(c, x)	{	\
	xjse_int_t	ntempxxxx = ((c) - '0');	\
	if(ntempxxxx > 9)	\
		ntempxxxx = ((c) - 'A') + 10;	\
	if(ntempxxxx > 15)	\
		ntempxxxx = ((c) - 'a') + 10;	\
	if(ntempxxxx > 15) {	\
		XJSE_TRACE("(X) E (%c, %d)", (c), ntempxxxx);	\
	}	\
	(x) = ntempxxxx;	\
}

/** convert "FF" -> float (0.0f - 1.0f)
 */
xjse_float_t xjse_xx_to_float(const char* psza)
{
	xjse_int_t	nr1, nr2;
	XJSE_C_TO_X(*(psza + 0), nr1);
	XJSE_C_TO_X(*(psza + 1), nr2);
	return	((xjse_float_t)nr1 * 16.0f + (xjse_float_t)nr2) / 255.0f;
}

/** glut: convert "FFFFFFFF" -> xjse_color4f_t(1.0f, ...)
 * psp: convert "FFFFFFFF" -> 0xFFFFFFFF
 */
void xjse_argb_to_color(xjse_color_t* pcolor, const char* pszargb)
{
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	xjse_int_t	n1 = 0;
	if(xjse_strlen(0, pszargb) == 8) {
		pcolor->fa = xjse_xx_to_float(pszargb + n1);	n1 += 2;
	} else {
		pcolor->fa = 1.0f;
	}
	pcolor->fr = xjse_xx_to_float(pszargb + n1);	n1 += 2;
	pcolor->fg = xjse_xx_to_float(pszargb + n1);	n1 += 2;
	pcolor->fb = xjse_xx_to_float(pszargb + n1);	n1 += 2;
	//XJSE_TRACE("(X) color converted: [%s](%f, %f, %f, %f)",
	//	psz1, pv0->color.fa, pv0->color.fr, pv0->color.fg, pv0->color.fb);
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	char*			psz1 = (char*)pszargb;
	xjse_ulong_t	u0, ur = 0;
	xjse_int_t		n1 = 0;
	if(xjse_strlen(0, pszargb) < 8) {
		n1 += 2;
		ur |= 0x00000FF0;
	}
	do {
		XJSE_C_TO_X(*psz1, u0);
		ur |= u0;
		if(++n1 == 8)
			break;
		psz1++;
		ur <<= 4;
	} while(1);
	*pcolor = ur;
	//XJSE_TRACE("(X) color converted: [%s](%x)", psz1, (unsigned int)pv0->color);
#endif	//XTPF_PSP
}

/** convert width, height string to value.
 * @param brelative shows if it's relative value (includes % sign)
 */
xjse_result_t xjse_whstr_to_val(const char* pszvaluea, xjse_int_t* pnresult, xjse_bool_t* pbrelative)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	xjse_int_t		n1 = 0; 
	char			*pszt1 = 0, sztemp[32];
	xjse_bool_t		brelative = XJSE_B_FALSE;

	for(pszt1 = (char*)pszvaluea; *pszt1 != 0; pszt1++) {
		if(*pszt1 < '0' || *pszt1 > '9') {
			if(*pszt1 == '%')
				brelative = XJSE_B_TRUE;
			else
				goto	failed;
		} else {
			sztemp[n1++] = *pszt1;
		}
		if(n1 == sizeof(sztemp) - 1)
			break;
	}
	sztemp[n1] = 0;

	*pbrelative = brelative;
	*pnresult = xjse_atoi(0, sztemp);

cleanup:
	return	xr1;

failed:
	xr1 = XJSE_E_UNKNOWN;
	goto	cleanup;
}

#ifdef	XXXXOBSOLETED
// use JS_DefineObject()
JSBool	xjse_jsutil_declobj(JSContext *pcxa, JSObject *poglobal, const char* psztypename, const char* pszobjname)
{
	JSBool	b1;
	jsval	rval;
	char	szscript[1024];
	xjse_snprintf(0, szscript, 1024, "var %s = new %s() ;",
		pszobjname, psztypename);
	b1 = JS_EvaluateScript(pcxa, poglobal, szscript, xjse_strlen(0, szscript),
		"tempscript0", (uint32)0, &rval);
	//XJSE_TRACE("script result: (%d) %s", (b1 == JS_TRUE), JS_GetStringBytes(JS_ValueToString(pcxa, rval)));
	// is it possible to do this without writing js script?
	//JSObject  *ptemp0 = JS_NewObject(pcxa, &class_xgg_img, pobj_proto1, 0);
	return  b1;
}

JSBool  xjse_jsutil_declset(JSContext *pcxa, JSObject *poglobal, const char* pszobjname, const char* pszvalue)
{
	jsval	rval;
	char	szscript[1024];
	xjse_snprintf(0, szscript, 1024, "var %s = %s;", pszobjname, pszvalue);
	return	JS_EvaluateScript(pcxa, poglobal,
		szscript, xjse_strlen(0, szscript), "tempscript0", (uint32)0, &rval);
}

// use JS_SetProperty
JSBool  xjse_jsutil_propset(JSContext *pcxa, JSObject *poglobal, const char* pszobjname, const char* pszpropname, const char* pszvalue)
{
	jsval	rval;
	char	szscript[1024];
	xjse_snprintf(0, szscript, 1024, "%s.%s = '%s';",
		pszobjname, pszpropname, pszvalue);
	return	JS_EvaluateScript(pcxa, poglobal,
		szscript, xjse_strlen(0, szscript), "tempscript0", (uint32)0, &rval);
}
#endif	//XXXXOBSOLETED


// ----8< ---- Enumerator

typedef struct	__tag_xjse_enumerator_priv {
	JSObject*		pocollection;
	xjse_int_t		ncurrent;
}	XJSE_ENUMERATOR_PRIV;

static JSBool xjse_enumerator_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_ENUMERATOR_PRIV*	pv0 = 0;

	if(pcxa->fp->flags & JSFRAME_CONSTRUCTING) {
		JSObject*	poarg = 0;

		JSType	t0 = JS_TypeOfValue(pcxa, pva1[0]);
		if(t0 == JSTYPE_OBJECT) {
			poarg = JSVAL_TO_OBJECT(pva1[0]);
		} else
		if(t0 == JSTYPE_FUNCTION) {
			JSObject*	pop0 = JS_GetParent(pcxa, JSVAL_TO_OBJECT(pva1[0]));
			JSFunction*	pf0 = JS_ValueToFunction(pcxa, pva1[0]);
			jsval	rv0;
			JSBool	b0 = JS_CallFunction(pcxa, pop0, pf0, 0, 0, &rv0);
			if(b0 != JS_TRUE) {
				XJSE_TRACE("(E) Enumerator(): calling arg Function failed!");
				goto	failed;
			}
			poarg = JSVAL_TO_OBJECT(rv0);
			if(poarg == 0) {
				XJSE_TRACE("(E) Enumerator(): arg Function returned nothing!");
				goto	failed;
			}
		} else {
			XJSE_TRACE("(E) Enumerator(): arg is not Object nor Function!");
			goto	failed;
		}

		JSBool		b1 = JS_IsArrayObject(pcxa, poarg);
		if(b1 != JS_TRUE) {
			XJSE_TRACE("(E) Enumerator initialized with non-array object!");
			goto	failed;
		}
		pv0 = (XJSE_ENUMERATOR_PRIV*)xjse_malloc(
			0, sizeof(XJSE_ENUMERATOR_PRIV));
		xjse_memset(0, pv0, 0, sizeof(XJSE_ENUMERATOR_PRIV));
		pv0->pocollection = poarg;
		JSBool	b0 = JS_SetPrivate(pcxa, pobja, pv0);
		if(b0 != JS_TRUE) {
			XJSE_TRACE("(E) JS_SetPrivate() failed!");
			goto	failed;
		}
	}

	return	JS_TRUE;

failed:
	XJSE_SAFENNP(pv0, free);
	return	JS_FALSE;
}

static void xjse_enumerator_dtor(JSContext* pcxa, JSObject* pobja)
{
	XJSE_ENUMERATOR_PRIV*	pv0 =
		(XJSE_ENUMERATOR_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		xjse_free(0, pv0);
	}
}

static JSBool xjse_enumerator_atend(JSContext* pcxa, JSObject* pobja, uintN argc, jsval* pva1, jsval* rval)
{
	JSBool	b1;
	jsuint	ulen = 0;
	XJSE_ENUMERATOR_PRIV*	pv0 =
		(XJSE_ENUMERATOR_PRIV*)JS_GetPrivate(pcxa, pobja);

	b1 = JS_GetArrayLength(pcxa, pv0->pocollection, &ulen);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_GetArrayLength() failed!");
		goto	failed;
	}

	//XJSE_TRACE("(X) XXXX atEnd(%d, %d)[%d]",
	//	ulen, pv0->ncurrent, (ulen == (jsuint)(pv0->ncurrent + 0)));
	*rval = BOOLEAN_TO_JSVAL(ulen == (jsuint)(pv0->ncurrent + 0));

	return	JS_TRUE;

failed:
	return	JS_FALSE;
}

static JSBool xjse_enumerator_movenext(JSContext* pcxa, JSObject* pobja, uintN argc, jsval* pva1, jsval* rval)
{
	XJSE_ENUMERATOR_PRIV*	pv0 =
		(XJSE_ENUMERATOR_PRIV*)JS_GetPrivate(pcxa, pobja);

	pv0->ncurrent++;

	return	JS_TRUE;
}

static JSBool xjse_enumerator_item(JSContext* pcxa, JSObject* pobja, uintN argc, jsval* pva1, jsval* rval)
{
	JSBool	b1;
	XJSE_ENUMERATOR_PRIV*	pv0 =
		(XJSE_ENUMERATOR_PRIV*)JS_GetPrivate(pcxa, pobja);

	jsval	v1;
	b1 = JS_GetElement(pcxa, pv0->pocollection, pv0->ncurrent, &v1);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_GetElement() failed!");
		goto	failed;
	}

	*rval = v1;

	return	JS_TRUE;

failed:
	return	JS_FALSE;
}

static JSClass class_xjse_enumerator = {
	"Enumerator", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub,
	JS_ConvertStub, xjse_enumerator_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xjse_enumerator[] = {
	{	"item",			63, (0),
		JS_PropertyStub, JS_PropertyStub,	},
	{	0,	},
};

JSFunctionSpec  funcs_xjse_enumerator[] = {
	{	"atEnd",		xjse_enumerator_atend,		0, 0, 0,	},
	{	"moveNext",		xjse_enumerator_movenext,	0, 0, 0,	},
	{	"item",			xjse_enumerator_item,		0, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

xjse_result_t	xjse_enumerator_define(JSContext* pcxa, JSObject* poglobal)
{
	JSObject	*poenumerator = JS_InitClass(pcxa,
		poglobal, NULL, &class_xjse_enumerator, xjse_enumerator_ctor, 0,
		props_xjse_enumerator, funcs_xjse_enumerator,
		props_xjse_enumerator, funcs_xjse_enumerator);  // static
	if(poenumerator == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}


#ifdef	XXXXTESTCOLLECTION20080412
// ----8< ---- test Collection

typedef struct	__tag_xjse_collection_priv {
	JSObject*		pocollection;
}	XJSE_COLLECTION_PRIV;

static JSBool xjse_collection_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_COLLECTION_PRIV*	pv0 = 0;

	if(pcxa->fp->flags & JSFRAME_CONSTRUCTING) {
		pv0 = (XJSE_COLLECTION_PRIV*)xjse_malloc(
			0, sizeof(XJSE_COLLECTION_PRIV));
		xjse_memset(0, pv0, 0, sizeof(XJSE_COLLECTION_PRIV));
		//pv0->pocollection = poarg;
		JSBool	b0 = JS_SetPrivate(pcxa, pobja, pv0);
		if(b0 != JS_TRUE) {
			XJSE_TRACE("(E) JS_SetPrivate() failed!");
			goto	failed;
		}
	} else {
		XJSE_TRACE("(X) xjse_collection_ctor() called!");
	}

	return	JS_TRUE;

failed:
	XJSE_SAFENNP(pv0, free);
	return	JS_FALSE;
}

static void xjse_collection_dtor(JSContext* pcxa, JSObject* pobja)
{
	XJSE_COLLECTION_PRIV*	pv0 =
		(XJSE_COLLECTION_PRIV*)JS_GetPrivate(pcxa, pobja);
	if(pv0 != 0) {
		xjse_free(0, pv0);
	}
}

static JSClass class_xjse_collection = {
	"Collection", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub,
	JS_ConvertStub, xjse_collection_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

JSPropertySpec	props_xjse_collection[] = {
//	{	"item",			63, (0),
//		JS_PropertyStub, JS_PropertyStub,	},
	{	0,	},
};

JSFunctionSpec  funcs_xjse_collection[] = {
//	{	"item",			xjse_collection_item,		0, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

xjse_result_t	xjse_collection_define(JSContext* pcxa, JSObject* poglobal)
{
	JSObject	*poarray = JS_NewArrayObject(pcxa, 0, 0);
	if(poarray == 0) {
		goto	failed;
	}

	JSObject	*pocollection = JS_InitClass(pcxa,
		poglobal, poarray, &class_xjse_collection, xjse_collection_ctor, 0,
		props_xjse_collection, funcs_xjse_collection,
		props_xjse_collection, funcs_xjse_collection);  // static
	if(pocollection == 0)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}
#endif	//XXXXTESTCOLLECTION20080412

