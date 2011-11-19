// xtk_ui: userinterface. (display & controls)

#include <sys/types.h>	// opendir, readdir
#include <dirent.h>		// opendir, readdir
#include <sys/stat.h>	// stat
#ifdef	XTPF_Linux
#include <math.h>	//sinf
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#include <floatonly.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <psputility_netparam.h>	//netData
#endif	//XTPF_PSP

#include <xjse.h>
#include "xtk.h"

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
// things needed because glut callbacks does not take args
static XTKCTX*		g_ptkctx = 0;
static XJSECTX**	g_ppjsectx = 0;
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
static unsigned int __attribute__((aligned(16)))	g_anlist[262144];
#endif	//XTPF_PSP

#ifdef	XTPF_PSP
const unsigned short	XTKTEXTALIGN_TO_INTRAFONTALGIN[] = {
	INTRAFONT_ALIGN_LEFT, INTRAFONT_ALIGN_CENTER, INTRAFONT_ALIGN_RIGHT,	};
#endif	//XTPF_PSP

#define	XTKC_NMENULEVEL_DEFAULT	(1)
XTK_MENUITEM	g_amenuitems[XTKC_NMAX_MENUITEMS] = {
	{	XTK_MICMD_CLICK,		0, "click",	},
	{	XTK_MICMD_ENUMPROFS,	0, "select prof",	},
	{	XTK_MICMD_ENUMWLANAPS,	0, "select wlan ap",	},
	{	XTK_MICMD_TEST01,		0, "(test)",	},
	{	XTK_MICMD_EXIT,			0, "exit",	},
	{	XTK_MICMD_NONE,			0, 0,	},
};

/**
 */
xjse_result_t xtk_string_draw(XTKCTX* ptkctxa, const char* psza, xjse_color_t* pcolora, xjse_color_t* pcolorshadowa, xjse_float_t flefta, xjse_float_t ftopa, xjse_float_t fsizea, xjse_uint8_t ualigna, xjse_float_t fscalea)
{
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
		glColor4d(pcolora->fr, pcolora->fg, pcolora->fb, pcolora->fa);
		glRasterPos3d(flefta, ftopa + XTK_BASEFONTPIXS * 0.5f, 0.0f);
		glPushAttrib(GL_CURRENT_BIT);
		xjse_int_t	n0 = 0;
		while(psza[n0])
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, psza[n0++]);
		glPopAttrib();
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
		xjse_float_t	ffontsize = (fsizea * fscalea);
		intraFontSetStyle(ptkctxa->pfont1, ffontsize, *pcolora, *pcolorshadowa,
			XTKTEXTALIGN_TO_INTRAFONTALGIN[ualigna]);
		intraFontPrint(ptkctxa->pfont1,
			flefta, ftopa + XTK_BASEFONTPIXS * ffontsize, psza);
#endif	//XTPF_PSP

	return	XJSE_SUCCESS;
}

/**
 * @param bfreedefault specify if you need to free the default one
 */
xjse_result_t xtk_menulist_free(XTKCTX* ptkctx, xjse_uint8_t umenulevel, xjse_bool_t bfreedefault)
{
	XTK_MENUITEM*	pmi = ptkctx->apmenuitems[umenulevel];
	if(pmi == 0)
		goto	cleanup;
	if(umenulevel == XTKC_NMENULEVEL_DEFAULT && bfreedefault != XJSE_B_TRUE)
		goto	cleanup;

	XJSE_SAFENNP(pmi, free);
	ptkctx->apmenuitems[umenulevel] = 0;

cleanup:
	return	XJSE_SUCCESS;
}

/**
 */
xjse_result_t xtk_menulist_set(XTKCTX* ptkctx, xjse_uint8_t umenulevel, XTK_MENUITEM* pmia)
{
	xtk_menulist_free(ptkctx, umenulevel, XJSE_B_FALSE);

	ptkctx->apmenuitems[umenulevel] = pmia;

//cleanup:
	return	XJSE_SUCCESS;
}

/** make menuitem list from profiles/ file list.
 * caller must free every entry's pszcaption and the list.
 */
xjse_result_t xtk_menu_profiles_milist(XTKCTX* pctx, XTK_MENUITEM** ppmilist)
{
	DIR*			pdir = 0;
	struct dirent*	pde = 0;
	struct stat		stat1;
	char			sztemp[XJSE_NMAX_PATH + 1];

	pdir = opendir(XJSE_PATH_PROFILES);
	if(pdir == 0) {
		XJSE_TRACE("(E) opendir() failed!");
		goto	failed;
	}

	xjse_int_t		nents = 0;
	XTK_MENUITEM*	pmi1 = (XTK_MENUITEM*)malloc(
		sizeof(XTK_MENUITEM) * XTKC_NMAX_MENUITEMS);
	memset(pmi1, 0, sizeof(XTK_MENUITEM) * XTKC_NMAX_MENUITEMS);

	memset(&stat1, 0, sizeof(struct stat));
	while((pde = readdir(pdir)) != 0) {
		xjse_snprintf(0, sztemp, XJSE_NMAX_PATH,
			"%s/%s", XJSE_PATH_PROFILES, pde->d_name);
		stat(sztemp, &stat1);
		if(!(S_ISDIR(stat1.st_mode))) {
		//if(pde->d_type == DT_REG) {	//DT_DIR (only Linux has this)
			XJSE_TRACE("(X) prof file found: [%s]", pde->d_name);
			pmi1[nents].cmd = XTK_MICMD_SELECTPROF;
			pmi1[nents].pszcaption = (char*)malloc(strlen(pde->d_name) + 1);
			strcpy(pmi1[nents].pszcaption, pde->d_name);

			if(++nents == XTKC_NMAX_MENUITEMS)
				break;
		}
	}

	*ppmilist = pmi1;

cleanup:
	XJSE_SAFENNP(pdir, closedir);

	return	XJSE_SUCCESS;

failed:
	goto	cleanup;
}

/** make menuitem list of wlan aps.
 * caller must free every entry's pszcaption and the list.
 */
xjse_result_t xtk_menu_wlanaps_milist(XTKCTX* pctx, XTK_MENUITEM** ppmilist, xjse_int_t napcurrent)
{
	XTK_MENUITEM*	pmi1 = (XTK_MENUITEM*)malloc(
		sizeof(XTK_MENUITEM) * XTKC_NMAX_MENUITEMS);
	memset(pmi1, 0, sizeof(XTK_MENUITEM) * XTKC_NMAX_MENUITEMS);

    xjse_int_t	i = 0, nents = 0;
	pmi1[nents].cmd = XTK_MICMD_SELECTWLANAP;
	pmi1[nents].narg = 0;
	pmi1[nents].pszcaption = (char*)malloc(16);
	snprintf(pmi1[nents].pszcaption, XTKC_NMAXLEN_MENUITEMCAPT,
		"%soffline", (i == napcurrent ? "> " : ""));
	nents++;

#define	XPNETC_NMAXWLANAPS	(8)
    //for(i = 1; i < XTKC_NMAX_MENUITEMS; i++) {
    for(i = 1; i < XPNETC_NMAXWLANAPS; i++) {
#ifdef	XTPF_PSP
        netData nd0;
        if(sceUtilityCheckNetParam(i) != 0)
            break;
        sceUtilityGetNetParam(i, 0, &nd0);
        XJSE_TRACE("(X) net param: [%d, %s]", i, (char*)&nd0);
		pmi1[nents].cmd = XTK_MICMD_SELECTWLANAP;
		pmi1[nents].narg = i;
		pmi1[nents].pszcaption = (char*)malloc(strlen((char*)&nd0) + 1);
		snprintf(pmi1[nents].pszcaption, XTKC_NMAXLEN_MENUITEMCAPT,
			"%s%s", (i == napcurrent ? "> " : ""), (char*)&nd0);
#endif	//XTPF_PSP
#ifdef	XTPF_Linux
		pmi1[nents].cmd = XTK_MICMD_SELECTWLANAP;
		pmi1[nents].narg = i;
		pmi1[nents].pszcaption = (char*)malloc(16);
		snprintf(pmi1[nents].pszcaption, XTKC_NMAXLEN_MENUITEMCAPT,
			"%sap %d", (i == napcurrent ? "> " : ""), nents);
#endif	//XTPF_Linux
		nents++;
    }
#undef	XPNETC_NMAXWLANAPS

	*ppmilist = pmi1;

cleanup:
	return	XJSE_SUCCESS;

//failed:
	goto	cleanup;
}

/**
 */
xjse_result_t xtk_menu_draw(XTKCTX* pctx, xjse_time_t tnow)
{
	xjse_float_t	f0 = 1.0f, fl, ft = 0.0f,
		fr = XTK_SCREENWIDTH, fb = XTK_SCREENHEIGHT;

	// opening animation
	if(pctx->tmenufrom != 0) {
		if(tnow < pctx->tmenufrom + pctx->tmenudura) {
			f0 = sinf(((xjse_float_t)(tnow - pctx->tmenufrom)
				/ (xjse_float_t)(pctx->tmenudura)) * M_PI / 2.0f);
		} else {
			pctx->tmenufrom = 0;
			pctx->tmenudura = 0;
		}
	}
	if(XTK_MENU_IS_OPENED(pctx)) {
		fl = (XTK_SCREENWIDTH - XTK_MENUWIDTH * f0);
	} else {
		fl = (XTK_SCREENWIDTH - XTK_MENUWIDTH + XTK_MENUWIDTH * f0);
	}

	if(fl < XTK_SCREENWIDTH) {
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f, 0.0f, -1.0f);
		XTK_SET_VERT_C(0.0f, 0.0f, 0.0f, 0.8f, fl, ft,  0.0f);
		XTK_SET_VERT_C(0.0f, 0.0f, 0.0f, 0.6f, fr, ft,  0.0f);
		XTK_SET_VERT_C(0.0f, 0.0f, 0.0f, 0.8f, fl, fb,  0.0f);
		XTK_SET_VERT_C(0.0f, 0.0f, 0.0f, 0.6f, fr, fb,  0.0f);
		glEnd();
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		sceGuDisable(GU_TEXTURE_2D);
		xtk_vertex_c_t*	pverts =
			(xtk_vertex_c_t*)sceGuGetMemory(4 * sizeof(xtk_vertex_c_t));
		XTK_SET_VERT_C(pverts[0], 0xFFFFFFFF, fl, ft, 0.0f);
		XTK_SET_VERT_C(pverts[1], 0x33FFFFFF, fr, ft, 0.0f);
		XTK_SET_VERT_C(pverts[2], 0xFFFFFFFF, fl, fb, 0.0f);
		XTK_SET_VERT_C(pverts[3], 0x33FFFFFF, fr, fb, 0.0f);
		sceGumDrawArray(GU_TRIANGLE_STRIP, XTK_GU_DRAWMODE_C, 4, 0, pverts);
		sceGuEnable(GU_TEXTURE_2D);
#endif	//XTPF_PSP

		xjse_int_t		i;
		XTK_MENUITEM*	pmenuitems = pctx->apmenuitems[pctx->umenulevel];
		if(pmenuitems == 0)
			goto	cleanup;

		for(i = 0; i < XTKC_NMAX_MENUITEMS; i++) {
			if(pmenuitems[i].cmd == 0)
				break;

			xjse_color_t	color0, cshadow;
			if(pctx->aumenuitem[pctx->umenulevel] == i) {
				XJSE_COLOR_SET(&color0, 0xFFFFFFFF);
			} else {
				XJSE_COLOR_SET(&color0, 0x88FFFFFF);
			}
			XJSE_COLOR_SET(&cshadow, 0xFF000000);
			xtk_string_draw(pctx, pmenuitems[i].pszcaption,
				&color0, &cshadow,
				fl + 10.0f, 20.0f * (xjse_float_t)i + 100.0f,
				12.0f / XTK_BASEFONTPIXS, XTK_TEXTALIGN_LEFT, 1.0f);
		}
	}

cleanup:
	return	XJSE_SUCCESS;
}

/** rendering
 */
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
void xtk_cb_render(void)
#else	//ENABLEGLUT
void xtk_cb_render(XTKCTX* pctx)
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
void xtk_cb_render(XTKCTX* pctx, xjse_int_t nfps)
#endif	//XTPF_PSP
{
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	XTKCTX*	pctx = g_ptkctx;
#endif	//ENABLEGLUT
#endif	//XTPF_Linux

	// pre-rendering process
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	//glEnable(GL_DEPTH_TEST);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// move O to left-top, reverse cood system
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 480, 0, 272);
	glScalef(1.0f, -1.0f, 1.0f);
	glTranslatef(0.0f, -272.0f, 0.0f);
#else	//ENABLEGLUT
	if(pctx->pwindow != 0) {
		gdk_draw_rectangle(pctx->ppmbackbuff,
			pctx->pwindow->style->bg_gc[GTK_STATE_ACTIVE],
			TRUE, 0, 0, -1, -1);
	}
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	sceGuStart(GU_DIRECT, g_anlist);
	sceGuClearColor(0xFFAAAAAA);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
	sceGuAmbient(0xFFFFFFFF);
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumOrtho(0, XTK_SCREENWIDTH, XTK_SCREENHEIGHT, 0, -1, 1);
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
#endif	//XTPF_PSP

	// rendering things
	{
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
		// show xadgconf frames (for debug)
		xjse_int_t	i;
		for(i = 0; i < XJSE_NMAXXADGETS; i++) {
			XJSEXADGCONF*	pconf =
				&(pctx->ppxadgconfs[pctx->ncurrxadgconf][i]);
			xjse_float_t
				fl = pconf->rectinsc.fleft, fr = pconf->rectinsc.fright,
				ft = pconf->rectinsc.ftop, fb = pconf->rectinsc.fbottom;
			glBegin(GL_LINE_STRIP);
			glNormal3f(0.0f, 0.0f, -1.0f);
			XTK_SET_VERT_C(0.2f, 0.2f, 0.4f, 0.8f, fl, ft,  0.0f);
			XTK_SET_VERT_C(0.2f, 0.2f, 0.4f, 0.8f, fr, ft,  0.0f);
			XTK_SET_VERT_C(0.2f, 0.2f, 0.4f, 0.8f, fr, fb,  0.0f);
			XTK_SET_VERT_C(0.2f, 0.2f, 0.4f, 0.8f, fl, fb,  0.0f);
			XTK_SET_VERT_C(0.2f, 0.2f, 0.4f, 0.8f, fl, ft,  0.0f);
			XTK_SET_VERT_C(0.2f, 0.2f, 0.4f, 0.8f, fr, fb,  0.0f);
			glEnd();
		}
#endif	//ENABLEGLUT
#endif	//XTPF_Linux

		// exec system-set callback
		if(pctx->pfonceonloop != 0) {
			pctx->pfonceonloop(pctx, pctx->pfonceonlooparg);
		}

		xjse_time_t		tnow;
		xjse_time_now(&tnow);

		//
		xtk_menu_draw(pctx,tnow);

		//
		if(XTK_MENU_IS_OPENED(pctx)) {
			xjse_color_t	color0, cshadow;
			XJSE_COLOR_SET(&color0, 0x88FFFFFF);
			XJSE_COLOR_SET(&cshadow, 0x88000000);
			xtk_string_draw(pctx, g_pszvers, &color0, &cshadow, 10.0f, 255.0f,
				12.0f / XTK_BASEFONTPIXS, XTK_TEXTALIGN_LEFT, 1.0f);
		}
	}

	// post-rendering process
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
#else	//ENABLEGLUT
	//
	if(pctx->pwindow !=0 ) {
		xjse_int_t		cx0, cy0;
		gdk_drawable_get_size(pctx->pwindow->window, &cx0, &cy0);
		GdkRectangle	rect = {	0, 0, cx0, cy0,	};
		gdk_window_invalidate_rect(pctx->pwindow->window, &rect, TRUE);;
	}
#endif	//ENABLEGLUT
	// wait a while (save battery consumption)
	xjse_sleep(200);
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
{
	char	szfps[200];
	sprintf(szfps, "%d fps", nfps);	// - Oiel Vert.
	intraFontSetStyle(pctx->pfont1,
		0.6f, 0xFFFFFFFF, 0xFF000000, INTRAFONT_ALIGN_RIGHT);
	intraFontPrint(pctx->pfont1,
		XTK_SCREENWIDTH - 10.0f, XTK_SCREENHEIGHT - 10.0f, szfps);
}
	//
	sceGuFinish();
	sceGuSync(0, 0);
	//
//	pspDebugScreenSetOffset((xjse_int_t)pctx->pfb0);
//	pspDebugScreenSetXY(0, 0);
//	pspDebugScreenPrintf("%d fps. (3)", nfps);
	//
	sceDisplayWaitVblankStart();
	pctx->pfb0 = sceGuSwapBuffers();
	// wait a while (save battery consumption)
	//scePowerSetCpuClockFrequency(1);
	//scePowerSetCpuClockFrequency(222);
#endif	//XTPF_PSP
}

/** flip xadgets' position (like Alt+Tab)
 */
static xjse_result_t xtk_xadgets_flip(XTKCTX* ptkctx, XJSECTX** ppjsectx, xjse_int_t noffset)
{
	xjse_int_t		i, nxadgets = 0;
	xjse_time_t		tnow;
	xjse_time_now(&tnow);

	//if(tnow < (ptkctx->tflipfrom + XTKC_FLIPREIN))
	if(tnow < (ptkctx->tflipfrom + ptkctx->tflipdura))
		goto	cleanup;	// too early to flip again

	// count loaded xadgets
	for(i = 0; i < XJSE_NMAXXADGETS; i++) {
		XJSECTX*	px1 = ppjsectx[i];
		if(px1 == 0)
			break;
	}
	nxadgets = i;

	ptkctx->nflipoffset += noffset;
	if(ptkctx->nflipoffset < 0)
		ptkctx->nflipoffset = (nxadgets - 1);
	if(ptkctx->nflipoffset >= nxadgets)
		ptkctx->nflipoffset %= nxadgets;

	// flip configs
	for(i = 0; i < XJSE_NMAXXADGETS; i++) {
		XJSECTX*	px1 = ppjsectx[i];
		if(px1 == 0)
			break;
		// clockwise, anti-clockwise (NOTE: this order affects focusing order)
		//xjse_int_t	n0 = ((i + ptkctx->nflipoffset) % nxadgets);
		xjse_int_t	n0 = ((i + nxadgets - ptkctx->nflipoffset) % nxadgets);
		px1->pxadgconfflipto
			= &(ptkctx->ppxadgconfs[ptkctx->ncurrxadgconf][n0]);
		//XJSE_TRACE("(X) flip: %d, %p", i, px1->pxadgconfflipto);
	}
	ptkctx->tflipfrom = tnow; 	ptkctx->tflipdura = XTKC_FLIPDURA;

cleanup:
	return	XJSE_SUCCESS;
}

/** toggle menu.
 */
xjse_result_t xtk_menu_toggle(XTKCTX* ptkctx, xjse_int_t noffset)
{
	xjse_time_t	tnow;
	xjse_time_now(&tnow);
	if(ptkctx->tmenufrom != 0 && tnow < (ptkctx->tmenufrom + ptkctx->tmenudura))
		goto	cleanup;

	// it's a bit complicated because umenulevel is unsigned
	xjse_uint8_t	utargetlevel = 0;
	if(noffset > 0 && ptkctx->umenulevel + noffset > XTKC_NMAX_MENULEVEL)
		utargetlevel = XTKC_NMAX_MENULEVEL;
	else
	if(noffset < 0 && XTKC_NMAX_MENULEVEL - noffset < 0)
		utargetlevel = 0;
	else
		utargetlevel = (ptkctx->umenulevel + noffset);

	// menu items of that level is not ready
	if(noffset > 0 &&
		ptkctx->apmenuitems[utargetlevel] == 0) {
		goto	cleanup;
	}
	// free current menulist when going back
	if(noffset < 0) {
		xjse_int_t	i;
		for(i = 0; i > noffset; i--) {
			xtk_menulist_free(ptkctx, ptkctx->umenulevel + i, XJSE_B_FALSE);
		}
	}

	ptkctx->umenulevel = utargetlevel;
	ptkctx->tmenufrom = tnow;
	ptkctx->tmenudura = XTKC_MENUDURA;
//XJSE_TRACE("(X) targetlevel? 2 [%d]", ptkctx->umenulevel);

cleanup:
	return	XJSE_SUCCESS;
}
static void xtk_menuitem_offset(XTKCTX* ptkctx, xjse_time_t tnow, xjse_int_t noffset)
{
	if(tnow < ptkctx->tdpadlast + XTKC_DPADREIN)
		return;
	if(!XTK_MENU_IS_OPENED(ptkctx))
		return;

	xjse_uint_t	ucurr = ptkctx->aumenuitem[ptkctx->umenulevel];

	if((noffset > 0 && ucurr < XTKC_NMAX_MENUITEMS - noffset)
		|| (noffset < 0 && ucurr > 0)) {
		xjse_uint_t	u0 = ucurr + noffset;
		XTK_MENUITEM*	pi0 = &((ptkctx->apmenuitems[ptkctx->umenulevel])[u0]);
		if(pi0->cmd != XTK_MICMD_NONE) {
			ptkctx->aumenuitem[ptkctx->umenulevel] = u0;
			ptkctx->tdpadlast = tnow;
		}
	}
}

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
void xtk_onkeyproc(xjse_uint8_t ckey, xjse_int_t nx, xjse_int_t ny)
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
void xtk_onkeyproc(XTKCTX* ptkctx, XJSECTX** ppjsectx)
#endif	//XTPF_PSP
{
	xjse_time_t	tnow;
	xjse_time_now(&tnow);

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	XTKCTX*		ptkctx = g_ptkctx;
	XJSECTX**	ppjsectx = g_ppjsectx;

	if(ckey == 'l') {
		xtk_xadgets_flip(ptkctx, ppjsectx, -1);
	} else if(ckey == 'r') {
		xtk_xadgets_flip(ptkctx, ppjsectx, +1);
	} else if(ckey == 't') {
		xtk_menu_toggle(ptkctx, (XTK_MENU_IS_OPENED(ptkctx) ? -1 : +1));
	} else if(ckey == 'q') {	// cancel
		if(XTK_MENU_IS_OPENED(ptkctx)) {
			xtk_menu_toggle(ptkctx, -1);
		}
	} else if(ckey == 'o' || ckey == 'x') {
		if(XTK_MENU_IS_OPENED(ptkctx)) {
			if(ptkctx->pfonmenuitem != 0) {
				ptkctx->pfonmenuitem(ptkctx, ptkctx->pfonmenuitemarg);
			}
		} else {
			// mouse click
			xjse_xadget_click(ppjsectx, ptkctx->nflipoffset);
		}
	} else if(ckey == 'a' && tnow > ptkctx->tdpadlast + XTKC_DPADREIN) {//(left)
		xjse_xadget_focus_next(ppjsectx, ptkctx->nflipoffset, 0);
		ptkctx->tdpadlast = tnow;
	} else if(ckey == 'w') {	//(up)
		xtk_menuitem_offset(ptkctx, tnow, -1);
	} else if(ckey == 'z') {	//(down)
		xtk_menuitem_offset(ptkctx, tnow, +1);
	} else {
		XJSE_TRACE("(X) unconfigured key pressed: (%x)", ckey);
	}
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	SceCtrlData ctrldata;
	sceCtrlReadBufferPositive(&ctrldata, 1);
	if(ctrldata.Buttons & PSP_CTRL_LTRIGGER) {
		xtk_xadgets_flip(ptkctx, ppjsectx, -1);
	}
	if(ctrldata.Buttons & PSP_CTRL_RTRIGGER) {
		xtk_xadgets_flip(ptkctx, ppjsectx, +1);
	}
	if(ctrldata.Buttons & PSP_CTRL_TRIANGLE) {
		xtk_menu_toggle(ptkctx, (XTK_MENU_IS_OPENED(ptkctx) ? -1 : +1));
	}
	if(ctrldata.Buttons & PSP_CTRL_SQUARE) {
		if(XTK_MENU_IS_OPENED(ptkctx)) {
			xtk_menu_toggle(ptkctx, -1);
		}
	}
	if(ctrldata.Buttons & PSP_CTRL_CIRCLE ||
		ctrldata.Buttons & PSP_CTRL_CROSS) {
		if(XTK_MENU_IS_OPENED(ptkctx)) {
			if(ptkctx->pfonmenuitem != 0) {
				ptkctx->pfonmenuitem(ptkctx, ptkctx->pfonmenuitemarg);
			}
		} else {
			// mouse click
			xjse_xadget_click(ppjsectx, ptkctx->nflipoffset);
		}
	}
	if(ctrldata.Buttons & PSP_CTRL_LEFT
		&& tnow > ptkctx->tdpadlast + XTKC_DPADREIN) {
		xjse_xadget_focus_next(ppjsectx, ptkctx->nflipoffset, 0);
		ptkctx->tdpadlast = tnow;
	}
#if	0
	if(ctrldata.Buttons & PSP_CTRL_UP
		&& tnow > ptkctx->tdpadlast + XTKC_DPADREIN) {
		if(XTK_MENU_IS_OPENED(ptkctx)) {
			if(ptkctx->aumenuitem[ptkctx->umenulevel] > 0)
				ptkctx->aumenuitem[ptkctx->umenulevel]--;
			ptkctx->tdpadlast = tnow;
		}
#else
	if(ctrldata.Buttons & PSP_CTRL_UP) {
		xtk_menuitem_offset(ptkctx, tnow, -1);
#endif
	}
#if	0
	if(ctrldata.Buttons & PSP_CTRL_DOWN
		&& tnow > ptkctx->tdpadlast + XTKC_DPADREIN) {
		if(XTK_MENU_IS_OPENED(ptkctx)) {
			if(ptkctx->aumenuitem[ptkctx->umenulevel] < XTKC_NMAX_MENUITEMS - 1)
				ptkctx->aumenuitem[ptkctx->umenulevel]++;
			ptkctx->tdpadlast = tnow;
		}
#else
	if(ctrldata.Buttons & PSP_CTRL_DOWN) {
		xtk_menuitem_offset(ptkctx, tnow, +1);
#endif
	}
#endif	//XTPF_PSP
}

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
#else	//ENABLEGLUT
void xtk_on_window_destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();

	((XTKCTX*)data)->pwindow = 0;
}

static gboolean xtk_on_drawingarea_configure(GtkWidget* widget, GdkEventConfigure* pevent, gpointer pdata)
{
	XJSE_TRACE("drawingarea_configure: (%p, %p)", pevent, pdata);

	GdkPixmap*	ppm1 = ((XTKCTX*)pdata)->ppmbackbuff;
	if(ppm1 != 0)
		gdk_pixmap_unref(ppm1);

	// allocate pixmaps and fill (for double buffering)
	ppm1 = gdk_pixmap_new(widget->window,
		widget->allocation.width, widget->allocation.height, -1);

	((XTKCTX*)pdata)->ppmbackbuff = ppm1;
XJSE_TRACE("backbuff: (%p -> %p)", pdata, ((XTKCTX*)pdata)->ppmbackbuff);

	return	TRUE;
}

static gboolean xtk_on_drawingarea_expose(GtkWidget *widget, GdkEventExpose* pevent, gpointer pdata)
{
	GdkPixmap*	ppm1 = ((XTKCTX*)pdata)->ppmbackbuff;
	//XJSE_TRACE("drawingarea_expose: (%p -> %p)", pdata, ppm1);
	gdk_draw_pixmap(widget->window, widget->style->black_gc, ppm1,
		pevent->area.x, pevent->area.y, pevent->area.x, pevent->area.y,
		pevent->area.width, pevent->area.height);

	return	TRUE;
}
#endif	//ENABLEGLUT
#endif	//XTPF_Linux

/**
 */
xjse_result_t xtk_init(XTKCTX** ppctx, int argc, char** argv)
{
	XTKCTX*	pctx = (XTKCTX*)malloc(sizeof(XTKCTX));
	memset(pctx, 0, sizeof(XTKCTX));
	XJSE_TRACE("xtk_init() starting...");

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(480, 272);
	glutCreateWindow("testjs01");
	glutDisplayFunc(xtk_cb_render);
	glutKeyboardFunc(xtk_onkeyproc);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
#else	//ENABLEGLUT
	gboolean	b0 = gtk_init_check(&argc, &argv);
	if(b0 != TRUE) {
		XJSE_TRACE("gtk_init_check() failed!");
		goto	failed;
	}

	pctx->pwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(pctx->pwindow), 0);
	g_signal_connect(GTK_OBJECT(pctx->pwindow), "destroy",
		GTK_SIGNAL_FUNC(xtk_on_window_destroy), pctx);

	//
	GtkWidget	*pda1 = gtk_drawing_area_new();
	g_signal_connect(GTK_OBJECT(pda1), "expose_event",
		GTK_SIGNAL_FUNC(xtk_on_drawingarea_expose), pctx);
	g_signal_connect(GTK_OBJECT(pda1), "configure_event",
		GTK_SIGNAL_FUNC(xtk_on_drawingarea_configure), pctx);
	gtk_container_add(GTK_CONTAINER(pctx->pwindow), pda1);
	gtk_widget_show(pda1);
	gtk_widget_set_usize(pda1, XTK_SCREENWIDTH, XTK_SCREENHEIGHT);

	gtk_widget_show(pctx->pwindow);
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#define	BUF_WIDTH	(512)
	intraFontInit();
	pctx->pfont1 = intraFontLoad(
		"flash0:/font/ltn0.pgf", INTRAFONT_CACHE_ASCII);

	pctx->pfb0 = (void*)((BUF_WIDTH * 272 * 4) * 0);	//GU_PSM_8888
	pctx->pfb1 = (void*)((BUF_WIDTH * 272 * 4) * 1);	//GU_PSM_8888
	pctx->pzb0 = (void*)((BUF_WIDTH * 272 * 4) * 2);	//GU_PSM_4444

	sceGuInit();
	sceGuStart(GU_DIRECT, g_anlist);
	sceGuDrawBuffer(GU_PSM_8888, pctx->pfb0, BUF_WIDTH);
	sceGuDispBuffer(480, 272, pctx->pfb1, BUF_WIDTH);
	sceGuDepthBuffer(pctx->pzb0, BUF_WIDTH);
	sceGuOffset(2048 - (480 / 2), 2048 - (272 / 2));
	// create a viewport centered at 2048,2048 width 480 and height 272
	sceGuViewport(2048, 2048, 480, 272);
	sceGuDepthRange(65535, 0);

	sceGuScissor(0, 0, 480, 272);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

	sceGuFinish();
	sceGuSync(0, 0);		// wait untill the list has finished
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);	// turn on the display

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
#endif	//XTPF_PSP

	memset(pctx->apmenuitems, 0, sizeof(XTK_MENUITEM*) * XTKC_NMAX_MENULEVEL);
	pctx->apmenuitems[XTKC_NMENULEVEL_DEFAULT]
		= (XTK_MENUITEM*)malloc(sizeof(g_amenuitems));
	memcpy(pctx->apmenuitems[XTKC_NMENULEVEL_DEFAULT],
		g_amenuitems, sizeof(g_amenuitems));

	*ppctx = pctx;
	XJSE_TRACE("xtk_init() done. (%p)", pctx);
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	g_ptkctx = pctx;
#endif	//ENABLEGLUT
#endif	//XTPF_Linux

	return	XJSE_SUCCESS;

//failed:
//	return	XJSE_E_UNKNOWN;
}

xjse_result_t xtk_cleanup(XTKCTX* pctx)
{
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
	XJSE_SAFENNP(pctx->ptexbackbuff, free);
#else	//ENABLEGLUT
	if(pctx->ppmbackbuff != 0) {
		gdk_pixmap_unref(pctx->ppmbackbuff);
	}
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	XJSE_SAFENNP(pctx->pfont1, intraFontUnload);
	intraFontShutdown();
#endif	//XTPF_PSP

	xjse_int_t	i;
	for(i = 0; i < pctx->nxadgconfs; i++)
		XJSE_SAFENNP(pctx->ppxadgconfs[i], free);
	XJSE_SAFENNP(pctx->ppxadgconfs, free);

	for(i = 0; i < XTKC_NMAX_MENULEVEL; i++)	// (0th does not exists)
		xtk_menulist_free(pctx, i, XJSE_B_TRUE);	//

	XJSE_SAFENNP(pctx, free);

	return	XJSE_SUCCESS;
}

xjse_result_t xtk_eventcb_set(XTKCTX* ptkctx, xjse_uint8_t ucbtype, xtk_cb_on_event_t pfa, void* pparama)
{
	switch(ucbtype) {
	case XTKC_CBTYPE_ONCEONLOOP:
		ptkctx->pfonceonloop = pfa;
		ptkctx->pfonceonlooparg = pparama;
		break;
	case XTKC_CBTYPE_MENUITEM:
		ptkctx->pfonmenuitem = pfa;
		ptkctx->pfonmenuitemarg = pparama;
	}

	return	XJSE_SUCCESS;
}

/** load xtk xadget config (frame position, etc.)
 * temporary version - be loaded from file in the future
 */
#if	0
xjse_result_t xtk_xadgconfs_load(XTKCTX* ptkctx)
{
	xjse_int_t	i;

	ptkctx->nxadgconfs = 1;
	ptkctx->ncurrxadgconf = 0;
	XJSEXADGCONF**	ppconfs = (XJSEXADGCONF**)xjse_malloc(0,
		sizeof(XJSEXADGCONF*) * ptkctx->nxadgconfs);
	for(i = 0; i < ptkctx->nxadgconfs; i++)
		ppconfs[i] = (XJSEXADGCONF*)xjse_malloc(
			0, sizeof(XJSEXADGCONF) * XJSE_NMAXXADGETS);
	ptkctx->ppxadgconfs = ppconfs;

#define	XJSEFILLXADGCONF(nxconf, nxidx, fxleft, fxtop, fxright, fxbottom) {	\
	XJSEXADGCONF*	pconf = &(ppconfs[nxconf][nxidx]);	\
	if(pconf != 0) {	\
		pconf->rectinsc.fleft = fxleft;	\
		pconf->rectinsc.ftop = fxtop;	\
		pconf->rectinsc.fright = fxright;	\
		pconf->rectinsc.fbottom = fxbottom;	\
	}	\
}
	XJSEFILLXADGCONF(0, 0,  15.0f,  10.0f, 280.0f, 272.0f);
	XJSEFILLXADGCONF(0, 1, 300.0f,  10.0f, 470.0f, 100.0f);
	XJSEFILLXADGCONF(0, 2, 300.0f, 100.0f, 470.0f, 190.0f);
	XJSEFILLXADGCONF(0, 3, 300.0f, 190.0f, 470.0f, 260.0f);
#undef	XJSEFILLXADGCONF

	return	XJSE_SUCCESS;
}
#endif	//0

xjse_result_t xtk_mainloop(XTKCTX* ptkctx, XJSECTX** ppjsectx, XPNETCTX* pxpnetctx)
{
	xjse_int_t	i, ncnt = 0, nfps = 0;
	xjse_time_t	tnow = 0, tprev = 0;
	xjse_time_now(&tprev);

	while(1) {
		xjse_time_now(&tnow);

		// complete flip animation
		if(ptkctx->tflipfrom != 0 &&
			(tnow > (ptkctx->tflipfrom + ptkctx->tflipdura))) {
			for(i = 0; i < XJSE_NMAXXADGETS; i++) {
				XJSECTX*	px1 = ppjsectx[i];
				if(px1 == 0)
					break;
				memcpy(&(px1->xadgconf), px1->pxadgconfflipto,
					sizeof(XJSEXADGCONF));
			}
			ptkctx->tflipfrom = 0;  ptkctx->tflipdura = 0;
		}

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
		if(g_ppjsectx == 0)
			g_ppjsectx = ppjsectx;
		//glutMainLoop();
		glutMainLoopEvent();
#else	//ENABLEGLUT
		//gtk_main();
		if(ptkctx->pwindow == 0)
			break;
		if(gtk_events_pending() == TRUE) {
			gtk_main_iteration();	// always returns 1
		}
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
		xtk_onkeyproc(ptkctx, ppjsectx);
#endif	//XTPF_PSP

		// calc fps
		ncnt++;
		if(tnow > (tprev + 1000)) {
			tprev = tnow;
			nfps = ncnt;
			ncnt = 0;
		}

		// async xpnet
		xjse_result_t	xr1 = xjse_xpnet_do_queue(pxpnetctx);
		if(!XJSE_IS_SUCCESS(xr1)) {
			XJSE_TRACE("(E) xjse_xpnet_do_queue() failed!");
			break;
		}

		// render
#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
//		xtk_cb_render();
#else	//ENABLEGLUT
		xtk_cb_render(ptkctx);
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
		xtk_cb_render(ptkctx, nfps);
#endif	//XTPF_PSP

		if((ptkctx->uflags && XTKCTXFLG_EXIT) != 0) {
			break;
		}
	}

#ifdef	XTPF_PSP
	sceGuTerm();
#endif	//XTPF_PSP

	return	XJSE_SUCCESS;
}

