
#ifndef	_INCLUDE_XJSE_TYPES_H_
#define	_INCLUDE_XJSE_TYPES_H_

	// ----8< ---- debug macros, conventionals

#define XJSE_TRACE(vargs...)	{	fprintf(stderr, "[%s:%d](X) ", __FILE__, __LINE__); fprintf(stderr, ## vargs);	fputs("\n", stderr);	}

#define	XJSE_SAFENNP(target, f) {	if((target) != 0) {	f(target);	(target) = 0;	}	}
//#define	XJSE_ASSERT(x) {	assert(x);	}
#define	XJSE_ASSERT(x) {	;	}

#define	XJSE_GREATER(x, y)	((x) > (y) ? (x) : (y))


	// ----8< ---- types

typedef	struct	__tag_xjsectx		XJSECTX;
typedef	struct	__xtkctx			XTKCTX;
typedef	struct	__tag_xggprivhdr	XGGPRIVHDR;
typedef	struct	__tag_xjse_color4f	xjse_color4f_t;

typedef	unsigned int		xjse_result_t;
typedef	unsigned char		xjse_uint8_t;
typedef	int					xjse_int_t;
typedef	unsigned int		xjse_uint_t;
typedef	long				xjse_long_t;
typedef	unsigned long		xjse_ulong_t;
typedef	unsigned long long	xjse_uint64_t;
typedef	size_t				xjse_size_t;
typedef	unsigned char		xjse_bool_t;
typedef	xjse_uint64_t		xjse_time_t;
typedef	xjse_uint64_t		xjse_timeout_t;
typedef	float				xjse_float_t;
#ifdef	XTPF_Linux
typedef	xjse_color4f_t		xjse_color_t;
typedef	FILE*				xjse_fd_t;
typedef struct __xtk_pixbuf	xtk_pixbuf_t;
#endif	//XTPF_Linux
#ifdef	XP_PSP
typedef	unsigned long		xjse_color_t;
typedef	int					xjse_fd_t;
typedef struct __xtk_pixbuf	xtk_pixbuf_t;
#endif	//XP_PSP

// callbacks
typedef	xjse_result_t(*xjse_cb_xadget_render)(XTKCTX*, XJSECTX*, XGGPRIVHDR*, void*);
// xpnet::HTTPGetExCB
typedef	xjse_result_t(*xjse_cb_xpnet_data)(char* pszdata, xjse_int_t nbytes, xjse_int_t ntotalfinalsize, void* puserdata) ;


#define	XJSE_TIMEOUT_TO_SEC(x)	(xjse_int_t)((x) / 1000)
#define	XJSE_SEC_TO_TIMEOUT(x)	(xjse_timeout_t)((x) * 1000)

#ifdef	XTPF_Linux
#define	XJSE_COLOR_SETA(pcolor)	(pcolor)->fa = 1.0f;
#define	XJSE_COLOR_SET(pcolor, cx)	{	\
	(pcolor)->fa = (((xjse_float_t)((cx & 0xFF000000) >> 24)) / 255.0f);	\
	(pcolor)->fr = (((xjse_float_t)((cx & 0x00FF0000) >> 16)) / 255.0f);	\
	(pcolor)->fg = (((xjse_float_t)((cx & 0x0000FF00) >>  8)) / 255.0f);	\
	(pcolor)->fb = (((xjse_float_t)((cx & 0x000000FF) >>  0)) / 255.0f);	\
	}
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#define	XJSE_COLOR_SETA(pcolor)	*(pcolor) |= 0xFF000000;
#define	XJSE_COLOR_SET(pcolor, cx)	*(pcolor) = cx;
#endif	//XTPF_PSP

#define	XJSE_NMAXXADGETS	(4)
#define	XJSE_NMAX_PATH		(255)
#define	XJSE_NMAXTIMERS		(8)

#define	XJSE_PATH_XADGETS	"xadgets"

// not be auto-cleared (recurring)
#define	XJSE_TIMERFLAGS_RECURR	(1 << 0)

// timer object
typedef	struct	__tag_xjsetimer {
	xjse_time_t		nexttime;	// next time being executed
	xjse_timeout_t	timeout;	// interval time
	//char*			pszexec;	// script being executed
	JSFunction*		pfexec;		// function being executed
	xjse_uint_t		flags;
}	XJSETIMER;

#ifdef	XTPF_Linux
#ifdef	ENABLEGLUT
#define XTK_SET_VERT_TC(pu, pv, cr, cg, cb, ca, px, py, pz)	{	\
	glTexCoord2f(pu, pv);	\
	glColor4f(cr, cg, cb, ca);	\
	glVertex3f(px, py,  pz);	\
}
#define XTK_SET_VERT_C(cr, cg, cb, ca, px, py, pz)	{	\
	glColor4f(cr, cg, cb, ca);	\
	glVertex3f(px, py,  pz);	\
}
#endif	//ENABLEGLUT
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
#define	XTK_GU_DRAWMODE_TC	(GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_COLOR_8888 | GU_TRANSFORM_3D)
typedef struct	__xtk_vertex_tc {
	xjse_float_t	u, v;
	xjse_color_t	color;
	xjse_float_t	x, y, z;
}	xtk_vertex_tc_t;
#define XTK_SET_VERT_TC(vert, pu, pv, pcolor, px, py, pz)	{	\
	(vert).u = pu;	(vert).v = pv;	\
	(vert).color = pcolor;	\
	(vert).x = px;	(vert).y = py;	(vert).z = pz;	\
}

#define	XTK_GU_DRAWMODE_C	(GU_VERTEX_32BITF | GU_COLOR_8888 | GU_TRANSFORM_3D)
typedef struct	__xtk_vertex_c {
	xjse_color_t	color;
	xjse_float_t	x, y, z;
}	xtk_vertex_c_t;
#define XTK_SET_VERT_C(vert, pcolor, px, py, pz)	{	\
	(vert).color = pcolor;	\
	(vert).x = px;	(vert).y = py;	(vert).z = pz;	\
}
#endif	//XTPF_PSP

typedef struct	__tag_xjsepoint {
	xjse_int_t		x;
	xjse_int_t		y;
}	xjse_point_t;

typedef struct	__tag_xjserectf {
	xjse_float_t	fleft;		//
	xjse_float_t	ftop;		//
	xjse_float_t	fright;		//
	xjse_float_t	fbottom;	//
}	xjse_rectf_t;

struct	__tag_xjse_color4f {
	xjse_float_t	fa;		//
	xjse_float_t	fr;		//
	xjse_float_t	fg;		//
	xjse_float_t	fb;		//
};

/** basic config data of an xadget.
 */
typedef struct	__tag_xjsexadgetconfig {
	xjse_rectf_t	rectinsc;	// boundary in screen
}	XJSEXADGCONF;

/** js runtime context
 */
struct	__tag_xjsectx {
	char*			pszname;	// name of xadget (used as part of path)

	JSContext*		pctx;
	JSObject*		poglobal;

	void*			pggctx;		// xgg context
	XJSETIMER		timers[XJSE_NMAXTIMERS];

	XJSEXADGCONF	xadgconf;
	XJSEXADGCONF*	pxadgconfflipto;	// flip destination
};

/** basic data of an element of a w/gadget. (ex. view, img)
 */
typedef struct	__tag_xjsexadgetelem	XJSEXADGELEM;
struct	__tag_xjsexadgetelem {
	xjse_float_t			fx;			// left end cood
	xjse_float_t			fy;			// top end cood
	xjse_float_t			fcx;		// width
	xjse_float_t			fcy;		// height
	xjse_uint8_t			uopacity;	//
	xjse_float_t			frotorgx;	// x center of rotation
	xjse_float_t			frotorgy;	// y center of rotation
	xjse_int_t				rotation;	// rotation degree
	xjse_bool_t				bvisible;	//

	//struct __tag_xjsexadget*	pparent;	// moved to XGGPRIVHDR#pparent
};

#define	XJSE_TIMER_IS_VALID(ptimerx)	((ptimerx)->timeout > 0)
#define	XJSE_TIMER_IS_TIMEDOUT(ptimerx, tnow)	((ptimerx)->nexttime < tnow)


	// ----8< ---- constants

#define	XJSE_B_TRUE			(1)
#define	XJSE_B_FALSE		(0)
#define	XJSE_B_TOGGLE(b)	b = (b == XJSE_B_TRUE ? XJSE_B_FALSE : XJSE_B_TRUE)

#define	XJSE_SUCCESS		(0)
#define	XJSE_E_BASE			(2000)
#define	XJSE_E_UNKNOWN		(XJSE_E_BASE | 0)
#define	XJSE_E_TIMERTOOMANY	(XJSE_E_BASE | 101)
#define	XJSE_E_INVALIDPARAM	(XJSE_E_BASE | 102)
#define	XJSE_E_MALLOC		(XJSE_E_BASE | 103)
#define	XJSE_IS_SUCCESS(x)	(x == XJSE_SUCCESS)


	// ----8< ---- xjse functions

xjse_result_t	xjse_init(JSRuntime**) ;
xjse_result_t	xjse_cleanup(JSRuntime*) ;
xjse_result_t	xjse_ctx_new(JSRuntime*, XJSECTX**) ;
xjse_result_t	xjse_ctx_delete(XJSECTX*) ;

xjse_result_t	xjse_time_now(xjse_time_t* ptimea) ;

xjse_result_t	xjse_timer_add(XJSECTX* pctxa, xjse_timeout_t timeouta, JSFunction* pfexeca, xjse_uint_t uflags, xjse_int_t* pnid) ;
xjse_result_t xjse_timer_reset(XJSETIMER* ptimera) ;
xjse_result_t	xjse_timer_destroy(XJSECTX* pctxa, xjse_int_t indexa) ;
xjse_result_t	xjse_timer_find_timedout(XJSECTX* pctxa, XJSETIMER** pptimera) ;

void	xjse_argb_to_color(xjse_color_t* pcolor, const char* pszargb) ;


JSBool	xjse_debugprint(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval) ;
void	printJSError(JSContext *cx, const char *message, JSErrorReport *report) ;
void	xjse_expat_error_report(XML_Parser parser, const XML_Char *filename) ;
char*	xjse_expat_get_attribute_linear(XML_Char** ppattrs, const char* pszattrname) ;

xjse_result_t xjse_strres_load(XJSECTX* pjsectxa, char* pszxadgetname, char* pszlang) ;


xjse_result_t	xjse_xhr_define(JSContext *pcxa, JSObject *poglobal) ;
xjse_result_t	xjse_xgg_define(XJSECTX* pctxa) ;

// javascript objects: "not implemented"
JSBool	xjse_x_propop_notimpl(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp) ;
JSBool	xjse_x_notimpl(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval) ;


	// ----8< ---- utility functions for portability

typedef	struct	tag_xjse_memctx {
}	XJSE_MEMCTX;

void*	xjse_malloc(XJSE_MEMCTX* pvctxa, xjse_size_t sizea);
void	xjse_free(XJSE_MEMCTX* pvctxa, void* pva1);
void	xjse_memset(XJSE_MEMCTX* pvctxa, void* pva1, xjse_int_t na1, xjse_size_t sizea);
xjse_size_t	xjse_strlen(XJSE_MEMCTX* pvctxa, const char* psza);
char*	xjse_strcpy(XJSE_MEMCTX* pvctxa, char* pszdst, const char* pszsrc);
xjse_int_t	xjse_strcmp(XJSE_MEMCTX* pvctxa, const char* psz1, const char* psz2);
xjse_int_t	xjse_atoi(XJSE_MEMCTX* pvctxa, const char* psz1);
#define	xjse_snprintf(pvctxa, ...)	snprintf(__VA_ARGS__)

xjse_fd_t	xjse_fopen(const char* pszpath, xjse_bool_t bwritea);
xjse_size_t	xjse_fread(xjse_fd_t fda, char* bufa, xjse_size_t sizea);
xjse_size_t	xjse_fwrite(xjse_fd_t fda, const char* bufa, xjse_size_t sizea);
void	xjse_fclose(xjse_fd_t fda);


#define	XJSE_XPNET_IS_SUCCESS(x)	(x == 0)
#ifdef __cplusplus
extern "C" {
#endif
xjse_result_t	xjse_xpnet_httprequest(char* pszurl, xjse_cb_xpnet_data pfcb, void* puserdata) ;
#ifdef __cplusplus
}
#endif


#endif	//_INCLUDE_XJSE_TYPES_H_
