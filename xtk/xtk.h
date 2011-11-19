#ifndef	_INCLUDE_XTK_H_
#define	_INCLUDE_XTK_H_

#if	defined(XTPF_Linux) && defined(ENABLEGLUT)
#include <GL/glut.h>
void FGAPIENTRY glutMainLoopEvent( void ) ;	// not exported function
#endif	//defined(XTPF_Linux) && defined(ENABLEGLUT)
#ifdef  XTPF_PSP
#include "intrafont/intraFont.h"
#endif

// screen size
#define	XTK_SCREENWIDTH		(480.0f)
#define	XTK_SCREENHEIGHT	(272.0f)
#define	XTK_MENUWIDTH		(130.0f)
// font size in pix
#define	XTK_BASEFONTPIXS	(18.0f)
// pixbuf cache size (# of images)
#define	XTKC_NMAX_PBCACHE	(120)
// animation duration, key re-input duration (msec.)
#define	XTKC_FLIPDURA		(300)
#define	XTKC_FLIPREIN		(300)
#define	XTKC_MENUDURA		(300)
#define	XTKC_MENUREIN		(300)
#define	XTKC_DPADREIN		(200)
#define	XTKC_NMAX_MENULEVEL	(8)
#define	XTKC_NMAX_MENUITEMS	(8)
#define	XTKC_NMAXLEN_MENUITEMCAPT	(32)
// opacity max
#define XTKC_OPACITY_MAX	((xjse_opacity_t)255)

#define XTK_TEXTALIGN_LEFT		(0)
#define XTK_TEXTALIGN_CENTER	(1)
#define XTK_TEXTALIGN_RIGHT		(2)

#define XTK_MENU_IS_OPENED(ptkctx)	(ptkctx->umenulevel > 0)


#ifdef	XTPF_PSP
#endif	//XTPF_PSP
// generated from png, or jpeg, etc. for PSP only
struct	__xtk_pixbuf {
	XTKCTX*					ptkctx;
	char*					pszfilename;
	xjse_int_t				ntexwidth;	// real width of data, 2^n with n>=0
	xjse_int_t				ntexheight;	// real height of data, 2^n with n>=0
	xjse_int_t				nimgwidth;
	xjse_int_t				nimgheight;
	xjse_uint_t*			pdata;		// array of ARGB_8888
#if	defined(XTPF_Linux) && defined(ENABLEGLUT)
	GLuint					ntex1;
#endif	//defined(XTPF_Linux) && defined(ENABLEGLUT)

	xjse_uint_t				urefcnt;	// reference count
	xjse_size_t				datasize;	//
};

#if	defined(XTPF_Linux) && defined(ENABLEGLUT)
typedef xjse_int_t			xtk_timeout_t;
typedef void*				xtk_window_t;
typedef void*				xtk_ptr_t;
typedef int(*xtk_cb_on_timer)(void);
#endif	//defined(XTPF_Linux) && defined(ENABLEGLUT)
#ifdef  XTPF_PSP
//typedef SceSysTimerId   	xtk_timerid_t;
typedef int             	xtk_timeout_t;
typedef	void*				xtk_window_t;
typedef void*           	xtk_ptr_t;
typedef int(*xtk_cb_on_timer)(void);
#endif

#define	XTKCTXFLG_EXIT		(1 << 1)

// menu
typedef	xjse_uint_t	xtk_micmd_t;
enum {	XTK_MICMD_NONE = 0, XTK_MICMD_CLICK = 101, XTK_MICMD_SELECTPROF = 102, XTK_MICMD_SELECTWLANAP = 105, XTK_MICMD_ENUMPROFS = 103, XTK_MICMD_ENUMWLANAPS = 104, XTK_MICMD_EXIT = 201, XTK_MICMD_TEST01 = 901,	};
typedef	struct	__tag_xtk_menuitem	{
	xtk_micmd_t		cmd;
	xjse_int_t		narg;
	char*			pszcaption;
}	XTK_MENUITEM;


// calbacks
#define	XTKC_CBTYPE_ONCEONLOOP	(1 << 1)
#define	XTKC_CBTYPE_MENUITEM	(1 << 2)
// called by event (p1: XTKCTX*, p2: userdata)
typedef	xjse_int_t(*xtk_cb_on_event_t)(XTKCTX*, void*);
// called every time in loop
typedef	xtk_cb_on_event_t	xtk_cb_once_on_loop_t;
// called when menu item is selected
typedef	xtk_cb_on_event_t	xtk_cb_on_menuitem_t;

// xtk context
struct	__xtkctx {
	xjse_uint8_t			uflags;			// XTKCTXFLG_

	xjse_int_t				nxadgconfs;		// number of xadgconf sets
	xjse_int_t				ncurrxadgconf;	// current index of xadgconf
	XJSEXADGCONF**			ppxadgconfs;	// xadgconfs, each has MAXXs elems

	xtk_window_t*			pwindow;		// top window
#if	defined(XTPF_Linux) && defined(ENABLEGLUT)
	GLubyte*				ptexbackbuff;
#endif	//defined(XTPF_Linux) && defined(ENABLEGLUT)
#ifdef	XTPF_PSP
	void*					pfb0;			// frame buffer 0, GU_PSM_8888
	void*					pfb1;			// frame buffer 1, GU_PSM_8888
	void*					pzb0;			// depth buffer 0, GU_PSM_4444

	intraFont*				pfont1;
#endif	//XTPF_PSP

	xtk_pixbuf_t*			appixbufcache[XTKC_NMAX_PBCACHE];

	xtk_cb_on_event_t		pfonceonloop;	//
	void*					pfonceonlooparg;//
	xtk_cb_on_event_t		pfonmenuitem;	//
	void*					pfonmenuitemarg;//

	xjse_int_t				nflipoffset;
	xjse_time_t				tflipfrom;		// time when flip started
	xjse_time_t				tflipdura;		//

	XTK_MENUITEM*			apmenuitems[XTKC_NMAX_MENULEVEL];
	xjse_time_t				tmenufrom;		// time when menu started opening
	xjse_time_t				tmenudura;		//
	xjse_uint8_t			umenulevel;		// level of opened menu
	xjse_uint8_t			aumenuitem[XTKC_NMAX_MENULEVEL];// index of focused menu item

	xjse_time_t				tdpadlast;		// time when d-pad last pressed
};


xjse_result_t xtk_init(XTKCTX** ppctx, int argc, char** argv) ;
xjse_result_t xtk_cleanup(XTKCTX* pctx) ;
xjse_result_t xtk_mainloop(XTKCTX* pctx, XJSECTX** ppjsectx, XPNETCTX* pxpnetctx) ;

xjse_result_t xtk_pixbuf_load(XTKCTX* ptkctx, xtk_pixbuf_t** pppba, const char* pszuri, xjse_uint8_t* pdata, xjse_size_t datasize) ;
//xjse_result_t xtk_onceonloop_set(XTKCTX* ptkctx, xtk_cb_once_on_loop_t pfa, void* pparama) ;
xjse_result_t xtk_xadgconfs_load(XTKCTX* ptkctx) ;

xjse_result_t xtk_wireframe_draw(void* ptarget, XJSEXADGELEM* pelema, xjse_rectf_t* proffsouter, xjse_rectf_t* proffsinner, xjse_float_t fscalea, xjse_bool_t bfocuseda) ;
xjse_result_t xtk_pixbuf_draw(void* ptarget, XJSEXADGELEM* pelema, xtk_pixbuf_t* ppixbufa, xjse_rectf_t* proffsouter, xjse_rectf_t* proffsinner, xjse_rectf_t* pruv, xjse_float_t fscalea, xjse_opacity_t opacitya) ;
void xtk_pixbuf_delete(xtk_pixbuf_t* ppba) ;
xjse_result_t xtk_string_draw(XTKCTX* pctxa, const char* psza, xjse_color_t* pcolora, xjse_color_t* pcolorshadowa, xjse_float_t flefta, xjse_float_t ftopa, xjse_float_t fsizea, xjse_uint8_t ualigna, xjse_float_t fscalea) ;

//

xjse_result_t xtk_eventcb_set(XTKCTX* pctxa, xjse_uint8_t ucbtype, xtk_cb_on_event_t pfa, void* pvuserdata) ;

#if	0	// timer never be used
//
typedef	struct	__xtktimer {
	xtk_timerid_t	id;
	xtk_timeout_t	timeout;
}	XTKTIMER;

xjse_result_t xtk_timer_start(XTKCTX* pctx, xtk_timeout_t timeout, xtk_cb_on_timer pfontimer, XTKTIMER** ppresult) ;
xjse_result_t xtk_timer_destroy(XTKCTX* pctx, XTKTIMER* timer) ;
#endif	//0

#endif	//_INCLUDE_XTK_H_
