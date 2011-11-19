
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/stat.h>
#ifdef	XP_PSP
#include <pspiofilemgr.h>
#endif	//XP_PSP
#include "xjse.h"

// wrapper for portability

void*	xjse_malloc(XJSE_MEMCTX* pvctxa, xjse_size_t sizea)
{
	return	malloc(sizea);
}

void	xjse_free(XJSE_MEMCTX* pvctxa, void* pva1)
{
	if(pva1 != 0)
		free(pva1);
}

void	xjse_memset(XJSE_MEMCTX* pvctxa, void* pva1, xjse_int_t na1, xjse_size_t sizea) {
	memset(pva1, na1, sizea);
}

xjse_size_t	xjse_strlen(XJSE_MEMCTX* pvctxa, const char* psza)
{
	return	(xjse_size_t)strlen(psza);
}

char*	xjse_strcpy(XJSE_MEMCTX* pvctxa, char* pszdst, const char* pszsrc)
{
	return	strcpy(pszdst, pszsrc);
}

xjse_int_t	xjse_strcmp(XJSE_MEMCTX* pvctxa, const char* psz1, const char* psz2)
{
	return	(xjse_int_t)strcmp(psz1, psz2);
}

xjse_int_t	xjse_atoi(XJSE_MEMCTX* pvctxa, const char* psz1)
{
	return	(xjse_int_t)atoi(psz1);
}

//void	xjse_snprintf(char *pszdst, const char *pszformat, ...)
//{
//	snprintf(pszdst, pszformat, __VA_ARGS__);
//}


// wrapper for fileio portability

xjse_fd_t	xjse_fopen(const char* pszpath, xjse_bool_t bwritea)
{
#ifdef	XP_PSP
	return	(xjse_fd_t)sceIoOpen(pszpath,
		(bwritea ? (PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC) : PSP_O_RDONLY),
		0777);
#else
	return	(xjse_fd_t)fopen(pszpath, (bwritea ? "wb" : "rb"));
#endif
}

xjse_size_t	xjse_fread(xjse_fd_t fda, char* bufa, xjse_size_t sizea)
{
#ifdef	XP_PSP
	return	(xjse_size_t)sceIoRead(fda, bufa, sizea);
#else
	return	(xjse_size_t)fread(bufa, 1, sizea, fda);
#endif
}

xjse_size_t	xjse_fwrite(xjse_fd_t fda, const char* bufa, xjse_size_t sizea)
{
#ifdef	XP_PSP
	return	(xjse_size_t)sceIoWrite(fda, bufa, sizea);
#else
	return	(xjse_size_t)fwrite(bufa, 1, sizea, fda);
#endif
}

xjse_result_t	xjse_fsize(const char* pszpath, xjse_size_t* psizea)
{
	xjse_int_t	nr;
#ifdef	XP_PSP
	SceIoStat	st1;
#else	//XP_PSP
	struct stat	st1;
#endif	//XP_PSP

#ifdef	XP_PSP
	nr = sceIoGetstat(pszpath, &st1);
#else	//XP_PSP
	nr = stat(pszpath, &st1);
#endif	//XP_PSP
	if(nr != 0)
		goto	failed;

	*psizea = st1.st_size;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

void	xjse_fclose(xjse_fd_t fda)
{
#ifdef	XP_PSP
	sceIoClose(fda);
#else
	fclose(fda);
#endif
}


#define	XJSE_STRRES_NMAX_ELEMNAME	(100)
#define	XJSE_STRRES_NMAX_ELEMVAL	(1000)

typedef	struct	__xjse_strres_load_ctx {
	XJSECTX*	pjsectx;
	char		szelemname[XJSE_STRRES_NMAX_ELEMNAME + 1];
	xjse_int_t	nelemval;
	char		szelemval[XJSE_STRRES_NMAX_ELEMVAL + 1];
}	XJSE_STRLCTX;

/**
 */
static void xjse_strres_cb_startelement(void* userData, const XML_Char* name, const XML_Char** atts)
{
	XJSE_STRLCTX*	pstrlctx = (XJSE_STRLCTX*)userData;
	//XML_Char**	ppxc = (XML_Char**)atts;

	if(xjse_strcmp(0, name, "strings") == 0) {
		pstrlctx->szelemname[0] = 0;
		pstrlctx->nelemval = 0;
		return;
	}

	strncpy(pstrlctx->szelemname, name, XJSE_STRRES_NMAX_ELEMNAME);
	pstrlctx->nelemval = 0;
}

static void xjse_strres_cb_endelement(void* userData, const XML_Char* name)
{
	XJSE_STRLCTX*	pstrlctx = (XJSE_STRLCTX*)userData;
	XJSECTX*		pjsectx = pstrlctx->pjsectx;

	if(xjse_strcmp(0, name, "strings") == 0) {
		goto	cleanup;
	}
	//XJSE_TRACE("(X) setting strres: [%s, %s]",
	//	pstrlctx->szelemname, pstrlctx->szelemval);

	// set as global's property
	jsval	v1 = STRING_TO_JSVAL(
		JS_NewStringCopyZ(pjsectx->pctx, pstrlctx->szelemval));
	JSBool	b1 = JS_SetProperty(pjsectx->pctx, pjsectx->poglobal,
		pstrlctx->szelemname, &v1);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_SetProperty() failed!");
		goto	cleanup;
	}

	// also set as property strings.<symbol>
	jsval	vstrings;
	b1 = JS_GetProperty(pjsectx->pctx, pjsectx->poglobal, "strings", &vstrings);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_GetProperty() failed!");
		goto	failed;
	}
	b1 = JS_SetProperty(pjsectx->pctx, JSVAL_TO_OBJECT(vstrings),
		pstrlctx->szelemname, &v1);
	if(b1 != JS_TRUE) {
		XJSE_TRACE("(E) JS_SetProperty() failed!");
		goto	cleanup;
	}

cleanup:
	pstrlctx->szelemname[0] = 0;
	pstrlctx->nelemval = 0;

	return;

failed:
	goto	cleanup;
}

static void xjse_strres_cb_characterdata(void* userData, const XML_Char* s, int len)
{
	XJSE_STRLCTX*	pstrlctx = (XJSE_STRLCTX*)userData;

	if((len + pstrlctx->nelemval) > XJSE_STRRES_NMAX_ELEMVAL) {
		XJSE_TRACE("(E) too long strres!");
		return;
	}

	memcpy(pstrlctx->szelemval + pstrlctx->nelemval, s, len);
	pstrlctx->szelemval[pstrlctx->nelemval + len] = 0;
	pstrlctx->nelemval += len;
}

/**
 */
xjse_result_t xjse_strres_load(XJSECTX* pjsectxa, char* pszxadgetname, char* pszlang)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	char			szpath[XJSE_NMAX_PATH];
	XJSE_STRLCTX	strlctx;

	memset(&strlctx, 0, sizeof(XJSE_STRLCTX));
	strlctx.pjsectx = pjsectxa;

	xjse_snprintf(0, szpath, sizeof(szpath) - 1, "%s/%s/%s/strings.xml",
		XJSE_PATH_XADGETS, pjsectxa->pszname, pszlang);
	XJSE_EXPAT_CBS	cbs = {
		xjse_strres_cb_startelement, xjse_strres_cb_endelement,
		xjse_strres_cb_characterdata,
	};
	xr1 = xjse_parse_xmlfile(szpath, &strlctx, &cbs);
	return	xr1;
}


#define	XTEMP_CREATEXMLPARSER_CBS(parser1, pfcbs)	{	\
	parser1 = XML_ParserCreate("UTF-8");	\
	if(parser1 == 0) {	\
		XJSE_TRACE("(E) XML_ParserCreate() failed!");	\
		goto	failed;	\
	}	\
	XML_SetUserData(parser1, puserdata);	\
	XML_UseForeignDTD(parser1, XML_TRUE);/* allows undefined entity ref */	\
	if(pfcbs->pfstart != 0 && pfcbs->pfend != 0)	\
		XML_SetElementHandler(parser1, pfcbs->pfstart, pfcbs->pfend);	\
	if(pfcbs->pfcharacterdata != 0)	\
		XML_SetCharacterDataHandler(parser1, pfcbs->pfcharacterdata);	\
	if(pfcbs->pfskippedentity != 0)	\
		XML_SetSkippedEntityHandler(parser1, pfcbs->pfskippedentity);	\
}

/** generic xml reader fucntion.
 */
xjse_result_t xjse_parse_xmlfile(const char* pszfilename, void* puserdata, XJSE_EXPAT_CBS* pfcbs)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	xjse_size_t		size1 = 0;
	char			buf[XJSE_NREADUNIT];
	xjse_fd_t		fd1 = 0;
	XML_Parser		parser1 = 0;

	XJSE_TRACE("(X) loading [%s]...", pszfilename);
	fd1 = xjse_fopen(pszfilename, XJSE_B_FALSE);
	if(!XJSE_FD_IS_VALID(fd1)) {
		XJSE_TRACE("xjse_fopen failed! [%s]", pszfilename);
		xr1 = XJSE_E_FOPEN;
		goto	failed;
	}

	XTEMP_CREATEXMLPARSER_CBS(parser1, pfcbs);

	do {
		size1 = xjse_fread(fd1, buf, sizeof(buf));
		XJSE_TRACE("read %d bytes from %d", size1, (xjse_int_t)fd1);
		if(!XML_Parse(parser1, buf, size1, (size1 < sizeof(buf)))) {
			xjse_expat_error_report(parser1, "filenamexxxx");
		}
	} while(size1 > 0 && size1 == sizeof(buf));

cleanup:
	XJSE_SAFENNP(fd1, xjse_fclose);
	XJSE_SAFENNP(parser1, XML_ParserFree);

	return	xr1;

failed:
	goto	cleanup;
}

/** generic xml parse function.
 */
xjse_result_t xjse_parse_xmlstring(const char* pszxml, void* puserdata, XJSE_EXPAT_CBS* pfcbs)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	XML_Parser		parser1 = 0;

	XTEMP_CREATEXMLPARSER_CBS(parser1, pfcbs);

	if(!XML_Parse(parser1, pszxml, strlen(pszxml), JS_TRUE)) {
		xjse_expat_error_report(parser1, "filenamexxxx");
	}

cleanup:
	XJSE_SAFENNP(parser1, XML_ParserFree);

	return	xr1;

failed:
	goto	cleanup;
}

xjse_result_t xjse_js_compilefile(JSContext* pctxa, JSObject* poglobala, const char* pszfilename, JSScript** ppscript)
{
	xjse_result_t	xr0;
	char			*pszt0 = 0;
	xjse_size_t		size0, size00;
	JSScript*		pscript1 = 0;

	xr0 = xjse_fsize(pszfilename, &size0);
	if(xr0 != XJSE_SUCCESS) {
		XJSE_TRACE("(E) failed to stat %s.", pszfilename);
		goto	failed; 
	}   
	pszt0 = (char*)malloc(size0 + 1);
	xjse_fd_t	fd0 = xjse_fopen(pszfilename, XJSE_B_FALSE);
	if(!XJSE_FD_IS_VALID(fd0)) {
		XJSE_TRACE("(E) failed to open %s.", pszfilename);
		goto	failed;
	}
	size00 = xjse_fread(fd0, pszt0, size0);
	if(size00 != size0) {
		XJSE_TRACE("(E) failed to read from %s (%d / %d).",
			pszfilename, size00, size0);
		goto	failed;
	}
	xjse_fclose(fd0);
	pszt0[size0] = 0;
	//
	size00 = 0;
	const char	BOM[] = {	0xEF, 0xBB, 0xBF,	};
	if(memcmp(pszt0, BOM, sizeof(BOM)) == 0) {
		XJSE_TRACE("(X) BOM detected! [%s](%d)", pszfilename, size0);
		size00 = sizeof(BOM);	// adjustment
	}
	pscript1 = JS_CompileScript(pctxa, poglobala,
		pszt0 + size00, size0 - size00, pszfilename, 0);

	*ppscript = pscript1;

cleanup:
	XJSE_SAFENNP(pszt0, free);

	return	XJSE_SUCCESS;

failed:
	goto	cleanup;
}

