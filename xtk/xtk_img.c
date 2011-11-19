
#include <malloc.h>
#include <png.h>
#ifdef	XTPF_Linux
#include <math.h>
#ifdef	ENABLEGLUT
#include <GL/glut.h>
#else	//ENABLEGLUT
#include <gdk-pixbuf/gdk-pixbuf-transform.h>
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#include <pspgu.h>
#include <pspgum.h>
#include "floatonly.h"
#endif	//XTPF_PSP
#include <xjse.h>
#include "xtk.h"

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
#else	//ENABLEGLUT
xjse_result_t xtk_pixbuf_rotate(GdkPixbuf *src, GdkPixbuf *dest, gint r, gfloat mx, gfloat my);
#endif	//ENABLEGLUT
#endif	//XTPF_Linux

/**
 */
static void xtk_cb_user_warning(png_structp png_ptr, png_const_charp warning_msg)
{
}

/**
 */
static int xtk_get_next_power2(xjse_int_t width)
{
	xjse_int_t	n, b = width;

	for(n = 0; b != 0; n++)
		b >>= 1;

	b = 1 << n;
	if(b == 2 * width)
		b >>= 1;

	return	b;
}

// ----8< ---- (jpeg mem) ----8< ---- ----8< ---- ----8< ---- ----8< ----
#include <jpeglib.h>
//#include <setjmp.h>

// from http://yarimit.blog.so-net.ne.jp/2005-06-03
typedef struct {
	struct jpeg_source_mgr	pub;	// public fields
	JOCTET*					buffer;
	unsigned long			buffer_length;
}	xtkjpg_mem_source_mgr;
typedef xtkjpg_mem_source_mgr*	memory_src_ptr;

METHODDEF(void) xtkjpg_mem_init_source(j_decompress_ptr cinfo)
{
}

METHODDEF(boolean) xtkjpg_mem_fill_input_buffer(j_decompress_ptr cinfo)
{
	memory_src_ptr src = (memory_src_ptr)cinfo->src;
	src->buffer[0] = (JOCTET) 0xFF;
	src->buffer[1] = (JOCTET) JPEG_EOI;
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = 2;
    return	TRUE;
}

METHODDEF(void) xtkjpg_mem_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	memory_src_ptr src = (memory_src_ptr)cinfo->src;

	if(num_bytes > 0) {
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

METHODDEF(void) xtkjpg_mem_term_source(j_decompress_ptr cinfo)
{
}

GLOBAL(void) xtkjpg_mem_src(j_decompress_ptr cinfo, void* data, unsigned long len)
{
	memory_src_ptr src;

	if(cinfo->src == NULL) {	// first time for this JPEG object?
		cinfo->src = (struct jpeg_source_mgr*)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof(xtkjpg_mem_source_mgr));
		src = (memory_src_ptr)cinfo->src;
		src->buffer = (JOCTET*)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			len * sizeof(JOCTET));
	}
	src = (memory_src_ptr)cinfo->src;
	src->pub.init_source = xtkjpg_mem_init_source;
	src->pub.fill_input_buffer = xtkjpg_mem_fill_input_buffer;
	src->pub.skip_input_data = xtkjpg_mem_skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart;// use default method
	src->pub.term_source = xtkjpg_mem_term_source;
	src->pub.bytes_in_buffer = len;
	src->pub.next_input_byte = (JOCTET*)data;
}
// ----8< ---- (jpeg mem) ----8< ---- ----8< ---- ----8< ---- ----8< ----

typedef struct _tag_xtkjpegerr {
	struct jpeg_error_mgr	pub;			// "public" fields
	jmp_buf					setjmp_buffer;	// for return to caller
}	XTKJPEGERR;

/**
 */
static xjse_result_t xtk_load_jpeg_mem(xtk_pixbuf_t* ppba, xjse_uint8_t* pdata, xjse_size_t datasize)
{
	xjse_result_t		xr1 = XJSE_SUCCESS;
	struct jpeg_decompress_struct	cinfo, *pcinfo = 0;
	XTKJPEGERR			jerr;
	JSAMPARRAY			buffer;		// array of pointers to scanlines
	xjse_int_t			row_stride;	// physical row width in output buffer

	cinfo.err = jpeg_std_error(&jerr.pub);
//	jerr.pub.error_exit = my_error_exit;
//	if(setjmp(jerr.setjmp_buffer)) {
//		// If we get here, the JPEG code has signaled an error.
//		jpeg_destroy_decompress(&cinfo);
//		fclose(pf1);
//		xr1 = XJSE_E_UNKNOWN;
//		goto	failed;
//	}
	jpeg_create_decompress(&cinfo);
	pcinfo = &cinfo;

	xtkjpg_mem_src(&cinfo, pdata, datasize);
	(void) jpeg_read_header(&cinfo, TRUE);
	(void) jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	//XJSE_TRACE("(X) load_jpeg: output will be (%d, %d)(%d)",
	//	cinfo.output_width, cinfo.output_height, row_stride);

	xtk_pixbuf_t*	ppixbuf = ppba;
	ppixbuf->nimgwidth = cinfo.output_width;
	ppixbuf->nimgheight = cinfo.output_height;
	ppixbuf->ntexwidth = xtk_get_next_power2(cinfo.output_width);
	ppixbuf->ntexheight = xtk_get_next_power2(cinfo.output_height);
	ppixbuf->urefcnt = 1;
	ppixbuf->datasize = 
		(ppixbuf->ntexwidth * ppixbuf->ntexheight * sizeof(xjse_uint_t));
	ppixbuf->pdata = (xjse_uint_t*)memalign(16, ppixbuf->datasize);
	if(ppixbuf->datasize > 400 * 1000) {
		XJSE_TRACE("(E) img size too large! (%d bytes)", ppixbuf->datasize);
		goto	failed;
	}
	if(!ppixbuf->pdata)
		goto	failed;
	memset(ppixbuf->pdata, 0xFF, ppixbuf->datasize);

	xjse_int_t	n0, nbp = cinfo.output_components;//bytes per pix
	while(cinfo.output_scanline < cinfo.output_height) {
		//xjse_int_t	nread0 =
			jpeg_read_scanlines(&cinfo, buffer, 1);
		for(n0 = 0; n0 < cinfo.output_width; n0++) {
			xjse_uint8_t*	put0 = 
				(xjse_uint8_t*)(ppixbuf->pdata + (ppixbuf->ntexwidth
				* (cinfo.output_scanline - 1) + n0));
			xjse_uint8_t*	put1 = (xjse_uint8_t*)((JSAMPROW*)(buffer[0]));
			*((xjse_uint_t*)put0) = (xjse_uint_t)(
				((xjse_uint_t)put1[n0 * nbp + 0]) << 0 |
				((xjse_uint_t)put1[n0 * nbp + 1]) << 8 |
				((xjse_uint_t)put1[n0 * nbp + 2]) << 16 |
				((xjse_uint_t)0xFF) << 24);
		}
	}

cleanup:
	if(pcinfo != 0) {
		(void) jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
	}

	return	xr1;

failed:
	goto	cleanup;
}
static xjse_result_t xtk_load_png(xtk_pixbuf_t* ppba, const char* pszfilepath)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	png_structp		png_ptr = 0;
	png_infop		ppnginfo;
    xjse_uint_t		*pline = 0, sig_read = 0;
    png_uint_32		width, height;
    xjse_int_t		bit_depth, color_type, interlace_type, x, y;
    FILE			*pf = 0;
	xtk_pixbuf_t*	ppixbuf = ppba;

	if((pf = fopen(pszfilepath, "rb")) == NULL)
		goto	failed;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL)
		goto	failed;
	png_set_error_fn(png_ptr,
		(png_voidp)NULL, (png_error_ptr)NULL, xtk_cb_user_warning);
	ppnginfo = png_create_info_struct(png_ptr);
	if(ppnginfo == NULL)
		goto	failed;
	png_init_io(png_ptr, pf);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, ppnginfo);
	png_get_IHDR(png_ptr, ppnginfo, &width, &height,
		&bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
	if(width > 512 || height > 512)
		goto	failed;
	ppixbuf->nimgwidth = width;
	ppixbuf->nimgheight = height;
	ppixbuf->ntexwidth = xtk_get_next_power2(width);
	ppixbuf->ntexheight = xtk_get_next_power2(height);
	ppixbuf->urefcnt = 1;
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);
	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_gray_1_2_4_to_8(png_ptr);
    if(png_get_valid(png_ptr, ppnginfo, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	ppixbuf->datasize = 
		(ppixbuf->ntexwidth * ppixbuf->ntexheight * sizeof(xjse_uint_t));
	ppixbuf->pdata = (xjse_uint_t*)memalign(16, ppixbuf->datasize);
	if(ppixbuf->datasize > 400 * 1000) {
		XJSE_TRACE("(E) img size too large! (%d bytes)", ppixbuf->datasize);
		goto	failed;
	}
	if(!ppixbuf->pdata)
		goto	failed;
	memset(ppixbuf->pdata, 0, ppixbuf->datasize);
	pline = (xjse_uint_t*)malloc(width * 4);
	if(!pline)
		goto	failed;
	for(y = 0; y < height; y++) {
		png_read_row(png_ptr, (xjse_uint8_t*)pline, png_bytep_NULL);
		for (x = 0; x < width; x++) {
			xjse_uint_t color = pline[x];
			ppixbuf->pdata[x + y * ppixbuf->ntexwidth] =  color;
		}
	}

//cleanup:
	XJSE_SAFENNP(pline, free);
	if(png_ptr != 0) {
		png_read_end(png_ptr, ppnginfo);
		png_destroy_read_struct(&png_ptr, &ppnginfo, png_infopp_NULL);
	}
	XJSE_SAFENNP(pf, fclose);

	return	xr1;

failed:
	if(png_ptr != 0) {
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		png_ptr = 0;
	}
	XJSE_SAFENNP(pf, fclose);

	xr1 = XJSE_E_UNKNOWN;
	return	xr1;
}

/**
 */
static xjse_result_t xtk_pixbuf_load_from_file(xtk_pixbuf_t** pppba, const char* pszfilename, xjse_uint8_t* pdata, xjse_size_t datasize)
{
	xjse_result_t	xr1 = XJSE_SUCCESS;
	if(pppba == 0)
		return	XJSE_E_INVALIDPARAM;

	xtk_pixbuf_t*	ppixbuf = (xtk_pixbuf_t*)malloc(sizeof(xtk_pixbuf_t));
	if(ppixbuf == 0)
		return	XJSE_E_MALLOC;
	memset(ppixbuf, 0, sizeof(xtk_pixbuf_t));
	ppixbuf->pszfilename = (char*)xjse_malloc(0, strlen(pszfilename) + 1);
	if(ppixbuf->pszfilename == 0)
		return	XJSE_E_MALLOC;
	strcpy(ppixbuf->pszfilename, pszfilename);

	xjse_int_t	n0 = strlen(pszfilename);
	if(pszfilename[n0 - 4] == '.' && pszfilename[n0 - 3] == 'j'
		&& pszfilename[n0 - 2] == 'p' && pszfilename[n0 - 1] == 'g') {
		if(pdata == 0) {
#if	1
			//TODO:
			XJSE_TRACE("(W) jpeg from file: not supported yet");
			goto	failed;
#else
			if((pf1 = fopen(pszfilepath, "rb")) == NULL) {
				XJSE_TRACE("(E) can't open %s\n", pszfilepath);
				xr1 = XJSE_E_UNKNOWN;
				goto	failed;
			}
			fread
			fclose
#endif
		}
		XJSE_TRACE("(X) XXXX jpeg: loading from mem!");
		xr1 = xtk_load_jpeg_mem(ppixbuf, pdata, datasize);
	} else
	if(pszfilename[n0 - 4] == '.' && pszfilename[n0 - 3] == 'p'
		&& pszfilename[n0 - 2] == 'n' && pszfilename[n0 - 1] == 'g') {
		xr1 = xtk_load_png(ppixbuf, pszfilename);
	} else {
		XJSE_TRACE("(E) img load failed: not known type [%s]", pszfilename);
		goto	failed;
	}


#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	glGenTextures(1, &(ppixbuf->ntex1));
	glBindTexture(GL_TEXTURE_2D, ppixbuf->ntex1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		ppixbuf->ntexwidth, ppixbuf->ntexheight, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, ppixbuf->pdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif	//ENABLEGLUT
#endif	//XTPF_Linux

	*pppba = ppixbuf;

cleanup:
	return	xr1;

failed:
	XJSE_SAFENNP(ppixbuf, xtk_pixbuf_delete);
	xr1 = XJSE_E_UNKNOWN;
	goto	cleanup;
}

/**
 */
xjse_result_t xtk_pixbuf_load(XTKCTX* ptkctxa, xtk_pixbuf_t** pppba, const char* pszuri, xjse_uint8_t* pdata, xjse_size_t datasize)
{
	xjse_result_t	xr1 = XJSE_E_UNKNOWN;

#if	0
	if(strncmp(pszuri, "http://", 7) == 0) {
		XJSE_TRACE("(W) loading from http:// data is not supported yet. [%s]",
			pszuri);
		goto	failed;
	}
#endif

	// find from xtk img cache (yes, linear search)
	xjse_int_t	i, nfirstemptyidx = -1;
	for(i = 0; i < XTKC_NMAX_PBCACHE; i++) {
		xtk_pixbuf_t*	ppb0 = ptkctxa->appixbufcache[i];
		if(ppb0 == 0) {
			if(nfirstemptyidx < 0)
				nfirstemptyidx = i;
			continue;
		}
		if(strcmp(pszuri, ppb0->pszfilename) == 0) {
			ppb0->urefcnt++;
			//XJSE_TRACE("(X) img [%s] refcnt: %d", pszuri, ppb0->urefcnt);
			*pppba = ppb0;
			xr1 = XJSE_SUCCESS;
			goto	cleanup;
		}
	}

	xr1 = xtk_pixbuf_load_from_file(pppba, pszuri, pdata, datasize);
	if(!XJSE_IS_SUCCESS(xr1)) {
		goto	failed;
	}
	(*pppba)->ptkctx = ptkctxa;

	i = nfirstemptyidx;
	if(i < 0) {
		XJSE_TRACE("(E) img cache full!");
		goto	failed;
	}

	ptkctxa->appixbufcache[i] = *pppba;
	//XJSE_TRACE("(X) img [%s](%dth, +%d bytes) cached.",
	//	pszuri, i, (*pppba)->datasize);

cleanup:

failed:
	return	xr1;
}

void xtk_pixbuf_delete(xtk_pixbuf_t* ppba)
{
	if(ppba == 0)
		return;

	ppba->urefcnt -= 1;
	if(ppba->urefcnt > 0)
		return;

	// delete from cache (temporary way)
	xjse_int_t	i;
	for(i = 0; i < XTKC_NMAX_PBCACHE; i++) {
		if(ppba == ppba->ptkctx->appixbufcache[i]) {
			ppba->ptkctx->appixbufcache[i] = 0;
		}
	}

#if	0
	XTKCTX*	ptkctxa = ppba->ptkctx;
	// dump info (debug)
	xjse_size_t	size0 = 0;
	for(i = 0; i < XTKC_NMAX_PBCACHE; i++) {
		xtk_pixbuf_t*	ppb0 = ptkctxa->appixbufcache[i];
		if(ppb0 == 0)
			continue;
		XJSE_TRACE("(X) img [%d, %s](%d, %d) %db ref: %d",
			i, ppb0->pszfilename, ppb0->ntexwidth, ppb0->ntexheight,
			ppb0->datasize, ppb0->urefcnt);
		size0 += ppb0->datasize;
	}
	XJSE_TRACE("(X) img cache total: %d bytes.", size0);
#endif

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	XJSE_SAFENNP(ppba->pdata, free);
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
	XJSE_SAFENNP(ppba->pszfilename, free);
	XJSE_SAFENNP(ppba->pdata, free);
	XJSE_SAFENNP(ppba, free);
}

/** show boundary box of focused object
 */
xjse_result_t xtk_wireframe_draw(void* ptarget, XJSEXADGELEM* pelema, xjse_rectf_t* proffsouter, xjse_rectf_t* proffsinner, xjse_float_t fscalea, xjse_bool_t bfocuseda)
{
	xjse_float_t	fscalex = 1.0f, fscaley = 1.0f;
	xjse_rectf_t	ruv1 = {	0.0f, 0.0f, 1.0f, 1.0f,	};
	xjse_float_t	fl = ruv1.fleft * fscalex, ft = ruv1.ftop * fscaley,
		fr = pelema->fcx * ruv1.fright * fscalex,
		fb = pelema->fcy * ruv1.fbottom * fscaley;

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	{
		glTranslatef(proffsouter->fleft, proffsouter->ftop, 0.0f);
		glScalef(fscalea, fscalea, 0.0f);
		glTranslatef((proffsinner->fleft + pelema->fx),
			(proffsinner->ftop + pelema->fy), 0.0f);
		if(pelema->rotation != 0) {
			glRotatef(pelema->rotation, 0.0f, 0.0f, 1.0f);
			glTranslatef((0.0f - pelema->frotorgx),
				(0.0f - pelema->frotorgy), 0.0f);
		}
	}
	glBegin(GL_LINE_STRIP);
	glNormal3f(0.0f, 0.0f, -1.0f);
	if(bfocuseda == XJSE_B_TRUE)
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	else
		glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
	glVertex3f(fl, ft,  0.0f);
	glVertex3f(fr, ft,  0.0f);
	glVertex3f(fr, fb,  0.0f);
	glVertex3f(fl, fb,  0.0f);
	glVertex3f(fl, ft,  0.0f);
	glEnd();
	glPopMatrix();
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	// rotation, scaling
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
	{
		ScePspFVector3	v0 = {	0.0f, 0.0f, 0.0f,	};
		v0.x = proffsouter->fleft;
		v0.y = proffsouter->ftop;
		sceGumTranslate(&v0);
		v0.x = fscalea;
		v0.y = fscalea;
		sceGumScale(&v0);
		v0.x = (proffsinner->fleft + pelema->fx);
		v0.y = (proffsinner->ftop + pelema->fy);
		sceGumTranslate(&v0);
		if(pelema->rotation != 0) {
			sceGumRotateZ((xjse_float_t)(pelema->rotation * M_PI) / 180.0f);
			v0.x = (0.0f - pelema->frotorgx);
			v0.y = (0.0f - pelema->frotorgy);
			sceGumTranslate(&v0);
		}
	}
	xjse_color_t	xc0 = 0;
	if(bfocuseda == XJSE_B_TRUE)
		xc0 = 0x664444FF;
	else
		xc0 = 0x10FFFFFF;

	sceGuDisable(GU_TEXTURE_2D);
	xtk_vertex_c_t* pverts =
		(xtk_vertex_c_t*)sceGuGetMemory(4 * sizeof(xtk_vertex_c_t));
	XTK_SET_VERT_C(pverts[0], xc0, fl, ft, 0.0f);
	XTK_SET_VERT_C(pverts[1], xc0, fr, ft, 0.0f);
	XTK_SET_VERT_C(pverts[2], xc0, fl, fb, 0.0f);
	XTK_SET_VERT_C(pverts[3], xc0, fr, fb, 0.0f);
	sceGumDrawArray(GU_TRIANGLE_STRIP, XTK_GU_DRAWMODE_C, 4, 0, pverts);
	sceGuEnable(GU_TEXTURE_2D);
#endif	//XTPF_PSP

	return	XJSE_SUCCESS;
}

/**
 */
xjse_result_t xtk_pixbuf_draw(void* ptarget, XJSEXADGELEM* pelema, xtk_pixbuf_t* ppixbufa, xjse_rectf_t* proffsouter, xjse_rectf_t* proffsinner, xjse_rectf_t* pruv, xjse_float_t fscalea, xjse_opacity_t opacitya)
{
	xtk_pixbuf_t	*ppb1 = ppixbufa;
	if(ppb1 == 0)
		goto	cleanup;

	xjse_float_t
		fscalex = (pelema->fcx / (xjse_float_t)ppixbufa->nimgwidth),
		fscaley = (pelema->fcy / (xjse_float_t)ppixbufa->nimgheight);

	xjse_rectf_t	ruv1;
	if(pruv != 0) {
		memcpy(&ruv1, pruv, sizeof(xjse_rectf_t));
	} else {
		ruv1.fleft = 0.0f;	ruv1.ftop = 0.0f;
		ruv1.fright = 1.0f;	ruv1.fbottom = 1.0f;
	}

	// texture must be square, but image boundary is not.
	xjse_float_t	furate = ((xjse_float_t)(ppb1->nimgwidth) /
		(xjse_float_t)(ppb1->ntexwidth));
	xjse_float_t	fvrate = ((xjse_float_t)(ppb1->nimgheight) /
		(xjse_float_t)(ppb1->ntexheight));
	// image boundary rect
	xjse_float_t	fl = ruv1.fleft * fscalex, ft = ruv1.ftop * fscaley,
		fr = (xjse_float_t)(ppb1->nimgwidth) * ruv1.fright * fscalex,
		fb = (xjse_float_t)(ppb1->nimgheight) * ruv1.fbottom * fscaley;
	// uv rect
	xjse_float_t	ful0 = furate * ruv1.fleft, fvt0 = fvrate * ruv1.ftop,
		fur0 = furate * ruv1.fright, fvb0 = fvrate * ruv1.fbottom;

	// apply parents' opacity
	xjse_float_t	fopac = (
		((xjse_float_t)(pelema->opacity) * (xjse_float_t)opacitya)
			/ (xjse_float_t)XTKC_OPACITY_MAX);
	//XJSE_TRACE("(X) drawing opacity: %d, %d -> %f, %f", pelema->opacity,
	//	opacitya, (xjse_float_t)(pelema->opacity), (xjse_float_t)opacitya);

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	{
		glTranslatef(proffsouter->fleft, proffsouter->ftop, 0.0f);
		glScalef(fscalea, fscalea, 0.0f);
		glTranslatef((proffsinner->fleft + pelema->fx),
			(proffsinner->ftop + pelema->fy), 0.0f);
		if(pelema->rotation != 0) {
			glRotatef(pelema->rotation, 0.0f, 0.0f, 1.0f);
			glTranslatef((0.0f - pelema->frotorgx),
				(0.0f - pelema->frotorgy), 0.0f);
		}
	}
#if	1	// show image boundary (debugging)
	glBegin(GL_LINE_STRIP);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
	glVertex3f(fl, ft,  0.0f);
	glVertex3f(fr, ft,  0.0f);
	glVertex3f(fr, fb,  0.0f);
	glVertex3f(fl, ft,  0.0f);
	glVertex3f(fl, fb,  0.0f);
	glVertex3f(fr, fb,  0.0f);
	glEnd();
#endif	//1
	glBindTexture(GL_TEXTURE_2D, ppb1->ntex1);
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, -1.0f);
	XTK_SET_VERT_TC(ful0, fvt0,	1.0f, 1.0f, 1.0f, fopac, fl, ft,  0.0f);
	XTK_SET_VERT_TC(fur0, fvt0,	1.0f, 1.0f, 1.0f, fopac, fr, ft,  0.0f);
	XTK_SET_VERT_TC(fur0, fvb0,	1.0f, 1.0f, 1.0f, fopac, fr, fb,  0.0f);
	XTK_SET_VERT_TC(ful0, fvb0,	1.0f, 1.0f, 1.0f, fopac, fl, fb,  0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
#else	//ENABLEGLUT
#define	TESTROTATE
#ifdef	TESTROTATE
	{
	GdkPixbuf*	ppb0 = ppb1->pgdkpixbuf;
	// apply each position's config
	xjse_int_t	nleft = (pelema->x + prectoffset->fleft);
	xjse_int_t	ntop = (pelema->y + prectoffset->ftop);
#if	1	// testing
	// testing: rotate keeping center
	// possible maximum size after rotation
	gint	nsize = XJSE_GREATER(
		XJSE_GREATER(pelema->rotorgx, (ppb1->ntexwidth - pelema->rotorgx)),
		XJSE_GREATER(pelema->rotorgy, (ppb1->ntexheight - pelema->rotorgy))
		) * 2;
	// create square pixbufs in maximum size (source, dest)
	GdkPixbuf*	ppbtsrc = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(ppb0),
		gdk_pixbuf_get_has_alpha(ppb0), gdk_pixbuf_get_bits_per_sample(ppb0),
		nsize, nsize);
	GdkPixbuf*	ppbtdst = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(ppb0),
		gdk_pixbuf_get_has_alpha(ppb0), gdk_pixbuf_get_bits_per_sample(ppb0),
		nsize, nsize);
	// put image into src pixbuf, taking rotorg to pixbuf's center
	gdk_pixbuf_copy_area(ppb0, 0, 0, ppb1->nimgwidth, ppb1->nimgheight,
		ppbtsrc, nsize / 2 - pelema->rotorgx, nsize / 2 - pelema->rotorgy);
	// make rotated image in dest pixbuf
	xtk_pixbuf_rotate(ppbtsrc, ppbtdst, pelema->rotation, fscalex, fscaley);
	// blit it to the target
	gdk_draw_pixbuf((GdkDrawable*)ptarget, 0, ppbtdst, 0, 0,
		nleft - (nsize / 2 - pelema->rotorgy),
		ntop - (nsize / 2 - pelema->rotorgx),
		gdk_pixbuf_get_width(ppbtdst), gdk_pixbuf_get_height(ppbtdst),
		GDK_RGB_DITHER_NONE, 0, 0);
	// clean up temporary things
	XJSE_SAFENNP(ppbtsrc, gdk_pixbuf_unref);
	XJSE_SAFENNP(ppbtdst, gdk_pixbuf_unref);
#else	// testing
	gint		nsize = (ppb1->ntexwidth > ppb1->ntexheight ?
		ppb1->ntexwidth : ppb1->ntexheight);
	GdkPixbuf*	ppbtemp0 = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(ppb0),
		gdk_pixbuf_get_has_alpha(ppb0), gdk_pixbuf_get_bits_per_sample(ppb0),
		nsize, nsize);
	xtk_pixbuf_rotate(ppb0, ppbtemp0, pelema->rotation, fscalex, fscaley);
	gdk_draw_pixbuf((GdkDrawable*)ptarget, 0, ppbtemp0, 0, 0,
		nleft, ntop,
		gdk_pixbuf_get_width(ppbtemp0), gdk_pixbuf_get_height(ppbtemp0),
		GDK_RGB_DITHER_NONE, 0, 0);
	XJSE_SAFENNP(ppbtemp0, gdk_pixbuf_unref);
#endif	// testing
	}
#else	//TESTROTATE
	gdk_draw_pixbuf((GdkDrawable*)ptarget, 0, ppb1->pgdkpixbuf,
		0, 0, nleft, ntop, pelema->cx, pelema->cy, GDK_RGB_DITHER_NONE, 0, 0);
#endif	//TESTROTATE
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	xtk_vertex_tc_t*	pverts =
		(xtk_vertex_tc_t*)sceGuGetMemory(4 * sizeof(xtk_vertex_tc_t));

	// setting the texture
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);	//GU_TFX_REPLACE
	sceGuTexMode(GU_PSM_8888, 0 ,0 ,0);
	sceGuTexImage(0, ppb1->ntexwidth, ppb1->ntexheight,
		ppb1->ntexwidth, (void*)ppb1->pdata);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	//sceGuTexScale(1.0f, 1.0f);
	//sceGuTexOffset(0.0f, 0.0f);

	// rotation, scaling
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
	{
		ScePspFVector3	v0 = {	0.0f, 0.0f, 0.0f,	};
		v0.x = proffsouter->fleft;
		v0.y = proffsouter->ftop;
		sceGumTranslate(&v0);
		v0.x = fscalea;
		v0.y = fscalea;
		sceGumScale(&v0);
		v0.x = (proffsinner->fleft + pelema->fx);
		v0.y = (proffsinner->ftop + pelema->fy);
		sceGumTranslate(&v0);
		if(pelema->rotation != 0) {
			sceGumRotateZ((xjse_float_t)(pelema->rotation * M_PI) / 180.0f);
			v0.x = (0.0f - pelema->frotorgx);
			v0.y = (0.0f - pelema->frotorgy);
			sceGumTranslate(&v0);
		}
	}

	// draw
//xjse_color_t	c0 = (0x00FFFF00);
	xjse_color_t	c0 = (0x00FFFFFF |
		((xjse_color_t)(fopac) << (8 * 3)));
	//	((xjse_color_t)(((xjse_float_t)XTKC_OPACITY_MAX) * fopac) << (8 * 3)));
//XJSE_TRACE("XXXX opac: %5.1f, %d, %x", fopac, ((xjse_color_t)(fopac)), c0);
	//	((xjse_color_t)pelema->opacity << (8 * 3)));
	XTK_SET_VERT_TC(pverts[0], ful0, fvt0, c0, fl, ft, 0.0f);
	XTK_SET_VERT_TC(pverts[1], fur0, fvt0, c0, fr, ft, 0.0f);
	XTK_SET_VERT_TC(pverts[2], ful0, fvb0, c0, fl, fb, 0.0f);
	XTK_SET_VERT_TC(pverts[3], fur0, fvb0, c0, fr, fb, 0.0f);
	sceGumDrawArray(GU_TRIANGLE_STRIP, XTK_GU_DRAWMODE_TC, 4, 0, pverts);
#endif	//XTPF_PSP

cleanup:
	return	XJSE_SUCCESS;
}

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
#else	//ENABLEGLUT
/* rotate pixbuf
 * original from: http://www.cozmixng.org/repos/csd/Examples/gdk-pixbuf/rotate.c
 * @param r rotate in degrees
 * @param mx magnificent rate x
 * @param my magnificent rate y
 */
xjse_result_t xtk_pixbuf_rotate(GdkPixbuf *src, GdkPixbuf *dest, gint r, gfloat mx, gfloat my)
{
	gint	w1, h1, w2, h2;
	gint	x1, y1, x2, y2;
	gint	dx1, dy1, dx2, dy2;
	gint	px1, py1, px2, py2;
	gint	i, j;
	guint32	*ptr1, *ptr2;
	guchar	*ptr, *buf;
	gint	buf_size;

	w1 = gdk_pixbuf_get_width(src);
	h1 = gdk_pixbuf_get_height(src);
	w2 = gdk_pixbuf_get_width(dest);
	h2 = gdk_pixbuf_get_height(dest);
	px1 = w1 / 2;
	py1 = h1 / 2;
	px2 = w2 / 2;
	py2 = h2 / 2;
	buf = gdk_pixbuf_get_pixels(src);
	buf_size = gdk_pixbuf_get_rowstride(src);

#define	ROTCOS	cos(M_PI * 2 / 360 * r)
#define	ROTSIN	sin(M_PI * 2 / 360 * r)
	x1 = (((0 - px2) * ROTCOS + (0 - py2) * ROTSIN)
		* (1 / mx) + px1) * 0x10000;
	y1 = (((0 - py2) * ROTCOS - (0 - px2) * ROTSIN)
		* (1 / my) + py1) * 0x10000;
	dx1 = (((1 - px2) * ROTCOS + (0 - py2) * ROTSIN)
		* (1 / mx) + px1) * 0x10000;
	dy1 = (((0 - py2) * ROTCOS - (1 - px2) * ROTSIN)
		* (1 / my) + py1) * 0x10000;
	dx2 = (((0 - px2) * ROTCOS + (1 - py2) * ROTSIN)
		* (1 / mx) + px1) * 0x10000;
	dy2 = (((1 - py2) * ROTCOS - (0 - px2) * ROTSIN)
		* (1 / my) + py1) * 0x10000;
#undef	ROTCOS
#undef	ROTSIN
	ptr2 = (guint32*)gdk_pixbuf_get_pixels(dest);
	dx1 -= x1;
	dy1 -= y1;
	dx2 -= x1;
	dy2 -= y1;
	w1 <<= 16;
	h1 <<= 16;
	for(j=0; j<h2; j++) {
		x2 = x1;
		y2 = y1;
		for(i=0; i<w2; i++) {
			if ((x2 >= 0) && (x2 < w1) && (y2 >= 0) && (y2 < h1)) {
				ptr = buf +  buf_size * (y2 >> 16) + ((x2 >> 16) * 4);
				ptr1 = (guint32 *)ptr;
				*ptr2 = *ptr1;
			} else {
				*ptr2 = 0;
			}
			ptr2++;
			x2 += dx1;
			y2 += dy1;
		}
		x1 += dx2;
		y1 += dy2;
	}

	return	XJSE_SUCCESS;
}
#endif	//ENABLEGLUT
#endif	//XTPF_Linux

