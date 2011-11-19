
#ifndef	_INCLUDE_XGG_H_
#define	_INCLUDE_XGG_H_


// meanings of flag on top of the XGG JS private data
// has XJSEXADGELEM in private data.
#define	XGG_PRIVFLAG_HAS_XADGELEM	(1 << 1)
#define	XGG_HAS_XADGELEM(x)	(((x)->flag & XGG_PRIVFLAG_HAS_XADGELEM) != 0)

// max # of level of elements
#define	XGG_NMAXELEMLEVEL	(8)
#define	XGG_NMAXELEMS		(64)

/** element types
 */
enum {
	XGG_ETYP_VIEW = 1, XGG_ETYP_BASICELEMENT, XGG_ETYP_BUTTON,
	XGG_ETYP_CHECKBOX, XGG_ETYP_OBJECT, XGG_ETYP_CONTENTAREA, XGG_ETYP_DIV,
	XGG_ETYP_EDIT, XGG_ETYP_IMG, XGG_ETYP_LABEL, XGG_ETYP_A,
	XGG_ETYP_PROGRESSBAR, XGG_ETYP_SCROLLBAR, XGG_ETYP_LISTBOX,
	XGG_EXTYP_LISTITEM,
};

/**
 */
typedef	struct __tag_xjse_treeelement	XJSE_TREEELEM;
struct	__tag_xjse_treeelement {
	char*				pszname;		// share memory with name property
	JSObject*			poself;
	XJSE_TREEELEM*		pparent;
	XJSE_TREEELEM*		pfirstchild;
	XJSE_TREEELEM*		pnextsibling;
	xjse_uint_t			uflags;
};
typedef struct __tag_xjse_elementpriv	XJSE_ELEMENTPRIV;
struct __tag_xjse_elementpriv {
	XJSE_TREEELEM		te;
};
typedef struct __tag_xdompriv {
	XJSECTX*			pjsectx;
	JSObject*			pothis;

	JSObject*			ponodelist;		// Array holds all elements
	xjse_int_t			nelems;			// # of all elements
	XJSE_TREEELEM*		apelems[XGG_NMAXELEMS];
	xjse_int_t			nelemlevel;		// element child level, root is 0
	XJSE_TREEELEM*		apelemstack[XGG_NMAXELEMLEVEL];// current stack trace
}	XJSE_XDOMPRIV;

#define	XJSEC_TREEELEMFLAG_HASMOUSEEVENT	(1 << 1)

// all xgg elements must have this on top of their private data
struct	__tag_xggprivhdr {
	XJSE_TREEELEM	te;			// must be top of this struct

	xjse_uint_t		flag;
	xjse_uint_t		type;		// XGG_ETYP_
	XJSEXADGELEM*	pxadgelem;	// ptr to XJSEXADGELEM (0 if it doesnt have)
	void*			pespec;		// element specific data (0 if it doesnt have)

	xjse_cb_xadget_render	pfrender;
};

// access macro
#define	TREEELEM_TO_XADGELEM(pelem)	 (((XGGPRIVHDR*)(pelem))->pxadgelem)
#define	TREEELEM_TO_XGGPRIVHDR(pelem)	 ((XGGPRIVHDR*)(pelem))
#define	XGGPRIVHDR_TO_XADGELEM(pgghdr)	 ((pgghdr)->pxadgelem)
#define	XGGPRIVHDR_TO_TREEELEM(pgghdr)	 (&((pgghdr)->te))

struct	__tag_xggctx {
	XJSECTX*		pjsectx;
	JSObject*		pview;			// object of root element

	xjse_int_t		nelems;
	XJSE_TREEELEM*	apelems[XGG_NMAXELEMS];
	// ctx for constructing as parsing xml
	xjse_int_t		nelemlevel;		// element child level, root is 0
	XJSE_TREEELEM*	apelemstack[XGG_NMAXELEMLEVEL];	// current stack trace
	XJSE_TREEELEM*	pdirectsibling;	// used by insertElement()

	XJSE_TREEELEM*	pfocusedelem;
};

// extra attributes: view
typedef	struct	__tag_xggepec_view {
	xjse_int_t		nwidth;
	xjse_int_t		nheight;
}	XGGESPEC_VIEW;


JSObject*	xjse_xgg_new_system(XJSECTX* pctxa, const char* pszobjname) ;
JSObject*	xjse_xgg_new_options(XJSECTX* pctxa, const char* pszobjname) ;
JSObject*	xjse_xgg_new_basicelement(XJSECTX* pctxa, const char* pszobjname) ;
JSObject*	xjse_xgg_new_button(XJSECTX* pctxa, const char* pszobjname) ;
JSObject*	xjse_xgg_new_view(XJSECTX* pctxa, const char* pszobjname) ;
JSObject*	xjse_xgg_new_div(XJSECTX* pctxa, const char* pszobjname) ;
JSObject*	xjse_xgg_new_img(XJSECTX* pctxa, const char* pszobjname) ;
JSObject*	xjse_xgg_new_label(XJSECTX* pctxa, const char* pszobjname) ;
JSObject*	xjse_xgg_new_progressbar(XJSECTX* pctxa, const char* pszobjname) ;

xjse_result_t	xjse_xgg_define_debug(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_system(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_options(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_basicelement(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_button(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_view(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_div(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_img(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_label(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_progressbar(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_plugin(XJSECTX* pctxa) ;
xjse_result_t	xjse_xgg_define_pluginhelper(XJSECTX* pctxa) ;
//xjse_result_t	xjse_xgg_define(XJSECTX* pctxa) ;	// -> xjse.h

xjse_result_t	xgg_load_one(XJSECTX* pctxa) ;
xjse_result_t	xgg_cleanup_one(XJSECTX* pjsectxa) ;

void	xgg_xadgelem_setdefaultvalues(JSContext* pcxa, JSObject* pobja, XJSEXADGELEM*) ;
xjse_result_t xgg_xadget_apply_trans(XTKCTX* ptkctxa, XJSECTX* pjsectxa, XGGPRIVHDR* phdra, xjse_rectf_t* proffsoutera, xjse_rectf_t* proffsinnera, xjse_float_t* pfscale, xjse_opacity_t* popacity) ;
xjse_result_t xgg_xadget_reload_img(XJSECTX* pjsectx, xtk_pixbuf_t** pppixbuf, const char* pszname, xjse_uint8_t* pdata, xjse_size_t datasize) ;

JSObject*	xgg_new_element(XJSECTX* pctxa, JSObject* poparent) ;
JSObject*	xgg_new_nodelist(XJSECTX* pctxa, JSObject* poparent) ;
xjse_result_t xgg_treeelem_family_set(XJSE_TREEELEM* pelema, XJSE_TREEELEM* apelemsa[], XJSE_TREEELEM* apelemstacka[], XJSE_TREEELEM* pdirectsiblinga, xjse_int_t nelemsa, xjse_int_t nelemlevela) ;


// *NOTE: user of basicelement must have XJSEXADGELEM on the 2nd top of 
// its JS Private data. (the next of top-most flag member)

JSBool xgg_global_strings(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval) ;
JSBool xgg_global_settimeout(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval) ;
JSBool xgg_global_setinterval(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval) ;
JSBool xgg_global_clearinterval(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval) ;
//JSBool xgg_global_appendelement(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval) ;
JSBool xgg_global_insertelement(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval) ;
JSBool xgg_global_removeelement(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval) ;
JSBool xgg_global_beginanimation(JSContext *pcxa, JSObject *pobja, uintN argc, jsval* pva1, jsval *rval) ;


// basic element: property setters, getters
// id: JPropertySpec.id, jsval: vargs

JSBool xgg_be_propop_cursor_get(JSContext*, JSObject*, jsval, jsval*) ;
JSBool xgg_be_propop_cursor_set(JSContext*, JSObject*, jsval, jsval*) ;
JSBool xgg_be_propop_x_get(JSContext*, JSObject*, jsval, jsval*) ;
//JSBool xgg_be_propop_x_set(JSContext*, JSObject*, jsval, jsval*) ;
//JSBool xgg_be_propop_pinx_set(JSContext*, JSObject*, jsval, jsval*) ;
//JSBool xgg_be_propop_piny_set(JSContext*, JSObject*, jsval, jsval*) ;
//JSBool xgg_be_propop_width_set(JSContext*, JSObject*, jsval, jsval*) ;
//JSBool xgg_be_propop_height_set(JSContext*, JSObject*, jsval, jsval*) ;
//JSBool xgg_be_propop_rotation_set(JSContext*, JSObject*, jsval, jsval*) ;
JSBool xgg_be_propop_xn_set(JSContext*, JSObject*, jsval, jsval*) ;

// basic element: function implementations

//JSBool xgg_be_appendelement(JSContext*, JSObject*, uintN, jsval*, jsval*) ;
//JSBool xgg_be_children(JSContext*, JSObject*, uintN, jsval*, jsval*) ;
JSBool xgg_global_propop_children_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp) ;

// basic element: JSPropertySpec, JSFunctionSpec (partial)

enum {	XGG_BE_PID_CHILDREN = 21, XGG_BE_PID_CURSOR, XGG_BE_PID_DROPTARGET,
	XGG_BE_PID_ENABLED, XGG_BE_PID_HEIGHT, XGG_BE_PID_HITTEST, XGG_BE_PID_MASK,
	XGG_BE_PID_NAME, XGG_BE_PID_OFFSETHEIGHT, XGG_BE_PID_OFFSETWIDTH,
	XGG_BE_PID_OFFSETX, XGG_BE_PID_OFFSETY, XGG_BE_PID_OPACITY,
	XGG_BE_PID_PARENTELEMENT, XGG_BE_PID_PINX, XGG_BE_PID_PINY,
	XGG_BE_PID_ROTATION, XGG_BE_PID_TAGNAME, XGG_BE_PID_TOOLTIP,
	XGG_BE_PID_WIDTH, XGG_BE_PID_VISIBLE, XGG_BE_PID_X, XGG_BE_PID_Y,	} ;

#define	XGG_BE_JSPROPSPEC_PARTIAL	\
	{	"children",		XGG_BE_PID_CHILDREN,	\
		(JSPROP_ENUMERATE | JSPROP_READONLY),	\
		xgg_global_propop_children_get, 0,	},	\
		/*xjse_x_propop_notimpl, 0,	},	*/\
	{	"cursor",		XGG_BE_PID_CURSOR, (JSPROP_ENUMERATE),	\
		xgg_be_propop_cursor_get, xgg_be_propop_cursor_set,	},	\
	{	"dropTarget",	XGG_BE_PID_DROPTARGET, (0),	\
		JS_PropertyStub, JS_PropertyStub,	},	\
	{	"enabled",		XGG_BE_PID_ENABLED, (0),	\
		JS_PropertyStub, xgg_be_propop_xn_set,	},	\
	{	"height",		XGG_BE_PID_HEIGHT, (JSPROP_ENUMERATE),	\
		JS_PropertyStub, xgg_be_propop_xn_set,	},	\
	{	"hitTest",		XGG_BE_PID_HITTEST, (0),	\
		JS_PropertyStub, JS_PropertyStub,	},	\
	{	"mask",			XGG_BE_PID_MASK, (0),	\
		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},	\
	{	"name",			XGG_BE_PID_NAME, (JSPROP_ENUMERATE),	\
		JS_PropertyStub, JS_PropertyStub, },	\
	{	"offsetHeight",	XGG_BE_PID_OFFSETHEIGHT,	\
		(JSPROP_ENUMERATE | JSPROP_READONLY),	JS_PropertyStub, 0,	},	\
	{	"offsetWidth",	XGG_BE_PID_OFFSETWIDTH,	\
		(JSPROP_ENUMERATE | JSPROP_READONLY),	JS_PropertyStub, 0,	},	\
	{	"offsetX",		XGG_BE_PID_OFFSETX,	\
		(JSPROP_ENUMERATE | JSPROP_READONLY),	JS_PropertyStub, 0,	},	\
	{	"offsetY",		XGG_BE_PID_OFFSETY,	\
		(JSPROP_ENUMERATE | JSPROP_READONLY),	JS_PropertyStub, 0,	},	\
	{	"opacity",		XGG_BE_PID_OPACITY, (0),	\
		JS_PropertyStub, xgg_be_propop_xn_set,	},	\
	{	"parentElement",XGG_BE_PID_PARENTELEMENT,	\
		(JSPROP_ENUMERATE | JSPROP_READONLY),	JS_PropertyStub, 0,	},	\
	{	"pinX",			XGG_BE_PID_PINX, (JSPROP_ENUMERATE),	\
		JS_PropertyStub, xgg_be_propop_xn_set,	},	\
	{	"pinY",			XGG_BE_PID_PINY, (JSPROP_ENUMERATE),	\
		JS_PropertyStub, xgg_be_propop_xn_set,	},	\
	{	"rotation",		XGG_BE_PID_ROTATION, (JSPROP_ENUMERATE),	\
		xjse_x_propop_notimpl, xgg_be_propop_xn_set,	},	\
	{	"tagName",		XGG_BE_PID_TAGNAME,	\
		(JSPROP_ENUMERATE | JSPROP_READONLY),	JS_PropertyStub, 0,	},	\
	{	"tooltip",		XGG_BE_PID_TOOLTIP, (0),	\
		JS_PropertyStub, xjse_x_propop_notimpl,	},	\
	{	"width",		XGG_BE_PID_WIDTH, (JSPROP_ENUMERATE),	\
		JS_PropertyStub, xgg_be_propop_xn_set,	},	\
	{	"visible",		XGG_BE_PID_VISIBLE, (0),	\
		JS_PropertyStub, xgg_be_propop_xn_set,	},	\
	{	"x",			XGG_BE_PID_X, (JSPROP_ENUMERATE),	\
		JS_PropertyStub, xgg_be_propop_xn_set,	},	\
	{	"y",			XGG_BE_PID_Y, (JSPROP_ENUMERATE),	\
		JS_PropertyStub, xgg_be_propop_xn_set,	},

#define	XGG_BE_JSFUNCSPEC_PARTIAL	\
	{	"appendElement",		xgg_global_insertelement,	1, 0, 0,	},	\
	/*{	"children",				xgg_be_children,	1, 0, 0,	},	*/\
	{	"focus",				xjse_x_notimpl,	0, 0, 0,	},	\
	{	"insertElement",		xgg_global_insertelement,	2, 0, 0,	},	\
	{	"killFocus",			xjse_x_notimpl,	0, 0, 0,	},	\
	{	"removeElement",		xgg_global_removeelement,	1, 0, 0,	},	\
	{	"removeAllElements",	xjse_x_notimpl,	0, 0, 0,	},


#endif	//_INCLUDE_XGG_H_

