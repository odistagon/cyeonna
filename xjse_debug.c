
#include <jsapi.h>
#include "xjse.h"

void printJSError(JSContext *cx, const char *message, JSErrorReport *report)
{
	char		ckind = 'X';
	XJSECTX*	pjsectx = 0;
	char		szt1[1023 + 1];

	pjsectx = (XJSECTX*)JS_GetContextPrivate(cx);

	if(JSREPORT_IS_EXCEPTION(report->flags))
		ckind = 'E';
	else if(JSREPORT_IS_WARNING(report->flags))
		ckind = 'W';
	else if(JSREPORT_IS_STRICT(report->flags))
		ckind = 'S';

	snprintf(szt1, 1023, "JSError (%c) %d: %s:%d\n    %s\n", ckind,
		report->errorNumber, (report->filename ? report->filename : "NULL"),
		report->lineno, message);
	fputs(szt1, stderr);
	if(pjsectx->fddebuglog != 0
		&& XJSE_HAS_PERMS(pjsectx, XJSEC_PERM_DEBUGLOG)) {
		xjse_fwrite(pjsectx->fddebuglog, szt1, strlen(szt1));
	}
	if(report->linebuf != 0)
		fprintf(stderr, "    %s\n", report->linebuf);
	if(report->tokenptr != 0)
		fprintf(stderr, "    %s\n", report->tokenptr);
}

JSBool xjse_debugprint(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_TRACE("debug: %s", JS_GetStringBytes(JS_ValueToString(pcxa, *pva1)));

	return	JS_TRUE;
}

#include <stdarg.h>
/** XJSE_TRACE()
 * concat to one string before output so to avoid thread racing corruption
 */
void xjse_debug_trace(const char* const pszfile, const xjse_int_t nline, char* pszfa, ...)
{
	char	szHEAD[31 + 1], szTEMP[255 + 1], szTA[286 + 1];
	snprintf(szHEAD, 31, "[%s:%d](x) ", pszfile, nline);
	va_list	val0;
	va_start(val0, pszfa);
	vsnprintf(szTEMP, 255, pszfa, val0);
	va_end(val0);
	snprintf(szTA, 278, "%s%s\n", szHEAD, szTEMP);
	fprintf(stderr, szTA);
}

	// ----8< ---- functions for not-implemented functions/ properties

JSBool xjse_x_propop_notimpl(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	XJSE_TRACE("xjse_x_propop_notimpl. (%s, %s)",
		JS_GetStringBytes(JS_ValueToString(pcxa, id)),
		JS_GetStringBytes(JS_ValueToString(pcxa, vp[0])));
	return	JS_TRUE;
}

JSBool xjse_x_notimpl(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	XJSE_TRACE("xjse_x_notimpl. (%s)",
		JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0])));

	return	JS_TRUE;
}

