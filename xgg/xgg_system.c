// Google Gadget: options

#include <xjse.h>
#include "xtk.h"
#include "xgg.h"
#ifdef	XTPF_PSP
#include <psppower.h>
#endif	//XTPF_PSP


	// ----8< ---- properties
	// id: JPropertySpec.id, jsval: vargs

#if	0
static JSBool xgg_system_propop_bios_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, vp[0]));
	XJSE_TRACE("xgg_system_propop_bios_get. (%s)", psz1);

	return  JS_TRUE;
}
#endif

static JSBool xgg_system_power_propop_charging_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
#ifdef	XTPF_Linux
	*vp = BOOLEAN_TO_JSVAL(0);
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	*vp = BOOLEAN_TO_JSVAL(scePowerIsBatteryCharging());
#endif	//XTPF_PSP

	return  JS_TRUE;
}

static JSBool xgg_system_power_propop_percentremaining_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
#ifdef	XTPF_Linux
	*vp = INT_TO_JSVAL(33);
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	*vp = INT_TO_JSVAL(scePowerGetBatteryLifePercent());
#endif	//XTPF_PSP

	return  JS_TRUE;
}

static JSBool xgg_system_power_propop_pluggedin_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
#ifdef	XTPF_Linux
	*vp = BOOLEAN_TO_JSVAL(0);
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	*vp = BOOLEAN_TO_JSVAL(scePowerIsBatteryCharging());
#endif	//XTPF_PSP

	return  JS_TRUE;
}

static JSBool xgg_system_power_propop_timeremaining_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
#ifdef	XTPF_Linux
	*vp = INT_TO_JSVAL(30 * 60);	// seconds
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	*vp = BOOLEAN_TO_JSVAL(scePowerGetBatteryLifeTime());
#endif	//XTPF_PSP

	return  JS_TRUE;
}

static JSBool xgg_system_power_propop_timetotal_get(JSContext *pcxa, JSObject *pobja, jsval id, jsval *vp)
{
#ifdef	XTPF_Linux
	*vp = INT_TO_JSVAL(60 * 60);	// seconds
#endif	//XTPF_Linux
#ifdef	XTPF_PSP
	// reverse calc.
	xjse_int_t	npercent = scePowerGetBatteryLifePercent();
	*vp = BOOLEAN_TO_JSVAL(scePowerGetBatteryLifeTime() * npercent / 100);
#endif	//XTPF_PSP

	return  JS_TRUE;
}

	// ----8< ---- functions

static JSBool xgg_system_getfileicon(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
	char	*psz1 = JS_GetStringBytes(JS_ValueToString(pcxa, pva1[0]));
	XJSE_TRACE("xgg_system_getfileicon (%s)", psz1);

	return  JS_TRUE;
}

static void xgg_system_dtor(JSContext* pcxa, JSObject *pobja)
{
}

	// ----8< ---- class definition

static JSClass class_xgg_system = {
	"__xgg_class_system", 0,	//JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, xgg_system_dtor,
	JSCLASS_NO_OPTIONAL_MEMBERS
};
JSPropertySpec	props_xgg_system[] = {
//	{	"count",		61, (JSPROP_ENUMERATE | JSPROP_READONLY),
//		xgg_system_propop_count_get, NULL,	},
//		xjse_x_propop_notimpl, xjse_x_propop_notimpl,	},
	{	0,	},
};
JSFunctionSpec	funcs_xgg_system[] = {
	{	"getFileIcon",				xgg_system_getfileicon,	1, 0, 0,	},
	{	"languageCode",				xjse_x_notimpl,	0, 0, 0,	},
	{	"localtimeToUniversalTime",	xjse_x_notimpl,	1, 0, 0,	},
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

static JSClass class_xgg_system_power = {
	"__xgg_class_system_power", 0,	//JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};
JSPropertySpec	props_xgg_system_power[] = {
	{	"charging",			101, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xgg_system_power_propop_charging_get, NULL,	},
	{	"percentRemaining",	102, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xgg_system_power_propop_percentremaining_get, NULL,	},
	{	"pluggedIn",		103, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xgg_system_power_propop_pluggedin_get, NULL,	},
	{	"timeRemaining",	104, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xgg_system_power_propop_timeremaining_get, NULL,	},
	{	"timeTotal",		105, (JSPROP_ENUMERATE | JSPROP_READONLY),
		xgg_system_power_propop_timetotal_get, NULL,	},
	{	0,	},
};
JSFunctionSpec	funcs_xgg_system_power[] = {
	{	0, 0, 0, 0, 0,	},	// JS_FS_END,
};

	// ----8< ---- 

xjse_result_t	xjse_xgg_define_system(XJSECTX* pctxa)
{
	JSBool		b1;
	jsval		v1;

	JSObject	*posystem = JS_NewObject(
		pctxa->pctx, &class_xgg_system, NULL, NULL);
	if(posystem == 0)
		goto	failed;
	v1 = OBJECT_TO_JSVAL(posystem);
	b1 = JS_SetProperty(pctxa->pctx, pctxa->poglobal, "system", &v1);
	if(b1 != JS_TRUE)
		goto	failed;

	JSObject	*posystem_power = JS_NewObject(
		pctxa->pctx, &class_xgg_system_power, NULL, NULL);
	if(posystem_power == 0)
		goto	failed;
	b1 = JS_DefineProperties(
		pctxa->pctx, posystem_power, props_xgg_system_power);
	if(b1 != JS_TRUE)
		goto	failed;
	v1 = OBJECT_TO_JSVAL(posystem_power);
	b1 = JS_SetProperty(pctxa->pctx, posystem, "power", &v1);
	if(b1 != JS_TRUE)
		goto	failed;

	return	XJSE_SUCCESS;

failed:
	return	XJSE_E_UNKNOWN;
}

