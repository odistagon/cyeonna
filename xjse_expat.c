
#include "xjse.h"

void	xjse_expat_error_report(XML_Parser parser, const XML_Char *filename)
{
	enum XML_Error	code = XML_GetErrorCode(parser);
	const XML_Char	*message = XML_ErrorString(code);
	if(message) {
		XJSE_TRACE("%s:%lu:%lu: %s",
			filename,
			XML_GetErrorLineNumber(parser), XML_GetErrorColumnNumber(parser),
			message);
	} else {
		XJSE_TRACE("%s: (unknown message %d)", filename, code);
	}
}

char*	xjse_expat_get_attribute_linear(XML_Char** ppattrs, const char* pszattrname)
{
	XML_Char	**ppxc = ppattrs;
	while((*ppxc) != 0) {
		if(xjse_strcmp(0, pszattrname, *(ppxc + 0)) == 0) {
			return	*(ppxc + 1);
		}
		ppxc += 2;
	}
	return	0;
}

void xjse_expat_on_startelement(void *userData, const XML_Char *name, const XML_Char **atts) {
	XJSE_TRACE("xjse_expat_on_startelement (%p, %s)", userData, name);
}

void xjse_expat_on_endelement(void *userData, const XML_Char *name) {
	XJSE_TRACE("xjse_expat_on_endelement (%p, %s)", userData, name);
}

void test_expat()
{
	char		buf[1024];
	XML_Parser	parser1 = 0;

	xjse_size_t	size1 = 0;
	xjse_fd_t	fd1 = xjse_fopen("./samples/ggsample.xml", XJSE_B_FALSE);
	if(!XJSE_FD_IS_VALID(fd1)) {
		XJSE_TRACE("xjse_fopen failed! [%s]", "filename");
		goto	failed;
	}

	parser1 = XML_ParserCreate(0);
	if(parser1 == 0) {
		XJSE_TRACE("XML_ParserCreate failed!");
		goto	failed;
	}
	XML_SetElementHandler(parser1,
		xjse_expat_on_startelement, xjse_expat_on_endelement);

	do {
		size1 = xjse_fread(fd1, buf, sizeof(buf));
		XJSE_TRACE("read %d bytes from %p", size1, (void*)fd1);
		if(!XML_Parse(parser1, buf, size1, (size1 < sizeof(buf)))) {
			xjse_expat_error_report(parser1, "filenamexxxx");
		}
	} while(size1 > 0 && size1 == sizeof(buf));

failed:
	XJSE_SAFENNP(fd1, xjse_fclose);
	XJSE_SAFENNP(parser1, XML_ParserFree);

	return;
}

