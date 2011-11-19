#include <pspmoduleexport.h>
#define NULL ((void *) 0)

extern void module_start;
extern void module_info;
static const unsigned int __syslib_exports[4] __attribute__((section(".rodata.sceResident"))) = {
	0xD632ACDB,
	0xF01D73A7,
	(unsigned int) &module_start,
	(unsigned int) &module_info,
};

extern void getModuleInfo;
extern void JS_CallFunction;
extern void JS_CallFunctionValue;
extern void JS_CompileScript;
extern void JS_ConvertStub;
extern void JS_DefineFunctions;
extern void JS_DefineProperties;
extern void JS_DefineObject;
extern void JS_DestroyContext;
extern void JS_EnumerateStub;
extern void JS_EvaluateScript;
extern void JS_FinalizeStub;
extern void JS_Finish;
extern void JS_GetArrayLength;
extern void JS_GetContextPrivate;
extern void JS_GetElement;
extern void JS_GetParent;
extern void JS_GetPrivate;
extern void JS_GetProperty;
extern void JS_GetRuntime;
extern void JS_GetStringBytes;
extern void JS_Init;
extern void JS_InitClass;
extern void JS_InitStandardClasses;
extern void JS_IsArrayObject;
extern void JS_NewContext;
extern void JS_NewObject;
extern void JS_NewStringCopyZ;
extern void JS_PropertyStub;
extern void JS_ResolveStub;
extern void JS_SetContextPrivate;
extern void JS_SetErrorReporter;
extern void JS_SetPrivate;
extern void JS_SetProperty;
extern void JS_TypeOfValue;
extern void JS_ValueToFunction;
extern void JS_ValueToString;
extern void JS_ValueToInt32;
extern void JS_SetElement;
extern void JS_ValueToObject;
extern void JS_NewArrayObject;
extern void JS_NewFunction;
extern void JS_GetFunctionObject;
extern void JS_SetParent;
extern void JS_NewString;
extern void JS_ExecuteScript;
extern void JS_DestroyScript;
extern void JS_GetGlobalObject;
extern void JS_malloc;
extern void JS_SetCallReturnValue2;
extern void JS_HasProperty;
extern void JS_NewNumberValue;
static const unsigned int __libjs_exports[104] __attribute__((section(".rodata.sceResident"))) = {
	0x563FF2B2,
	0x1E5F352F,
	0xC4B2969D,
	0x7DDD8BFD,
	0xB5AA670E,
	0x1A5E080A,
	0xFE9C14A7,
	0xB7C56828,
	0xF62151C6,
	0x7510AC2D,
	0x74E2B820,
	0xD028CB58,
	0x821A8328,
	0x4E70F3DB,
	0xA7AB8D5B,
	0xEA38DB5A,
	0xEF2C7A07,
	0x461B81D2,
	0xC7FE4A8F,
	0x25424146,
	0x49EFE8BA,
	0x0D01D88B,
	0x631841BA,
	0x8D8F5E96,
	0x2183C20E,
	0x897B0340,
	0x2F43E7EB,
	0x1787286D,
	0x214749D3,
	0x0A44FBF2,
	0x13326907,
	0xFC5107B1,
	0x3A5D05E5,
	0x4BA87D5A,
	0x3FF642DD,
	0x98B7E67A,
	0xA768FD0F,
	0x4D976DCB,
	0x21BA1D07,
	0x050D2D5F,
	0x2705BC53,
	0x9999D716,
	0x709B73F2,
	0x2778A34F,
	0x6FF2BF18,
	0xB95F576F,
	0x3CD26882,
	0xF57A83EF,
	0x71D1250B,
	0x99711B94,
	0x009C4906,
	0x9D034CDB,
	(unsigned int) &getModuleInfo,
	(unsigned int) &JS_CallFunction,
	(unsigned int) &JS_CallFunctionValue,
	(unsigned int) &JS_CompileScript,
	(unsigned int) &JS_ConvertStub,
	(unsigned int) &JS_DefineFunctions,
	(unsigned int) &JS_DefineProperties,
	(unsigned int) &JS_DefineObject,
	(unsigned int) &JS_DestroyContext,
	(unsigned int) &JS_EnumerateStub,
	(unsigned int) &JS_EvaluateScript,
	(unsigned int) &JS_FinalizeStub,
	(unsigned int) &JS_Finish,
	(unsigned int) &JS_GetArrayLength,
	(unsigned int) &JS_GetContextPrivate,
	(unsigned int) &JS_GetElement,
	(unsigned int) &JS_GetParent,
	(unsigned int) &JS_GetPrivate,
	(unsigned int) &JS_GetProperty,
	(unsigned int) &JS_GetRuntime,
	(unsigned int) &JS_GetStringBytes,
	(unsigned int) &JS_Init,
	(unsigned int) &JS_InitClass,
	(unsigned int) &JS_InitStandardClasses,
	(unsigned int) &JS_IsArrayObject,
	(unsigned int) &JS_NewContext,
	(unsigned int) &JS_NewObject,
	(unsigned int) &JS_NewStringCopyZ,
	(unsigned int) &JS_PropertyStub,
	(unsigned int) &JS_ResolveStub,
	(unsigned int) &JS_SetContextPrivate,
	(unsigned int) &JS_SetErrorReporter,
	(unsigned int) &JS_SetPrivate,
	(unsigned int) &JS_SetProperty,
	(unsigned int) &JS_TypeOfValue,
	(unsigned int) &JS_ValueToFunction,
	(unsigned int) &JS_ValueToString,
	(unsigned int) &JS_ValueToInt32,
	(unsigned int) &JS_SetElement,
	(unsigned int) &JS_ValueToObject,
	(unsigned int) &JS_NewArrayObject,
	(unsigned int) &JS_NewFunction,
	(unsigned int) &JS_GetFunctionObject,
	(unsigned int) &JS_SetParent,
	(unsigned int) &JS_NewString,
	(unsigned int) &JS_ExecuteScript,
	(unsigned int) &JS_DestroyScript,
	(unsigned int) &JS_GetGlobalObject,
	(unsigned int) &JS_malloc,
	(unsigned int) &JS_SetCallReturnValue2,
	(unsigned int) &JS_HasProperty,
	(unsigned int) &JS_NewNumberValue,
};

const struct _PspLibraryEntry __library_exports[2] __attribute__((section(".lib.ent"), used)) = {
	{ NULL, 0x0000, 0x8000, 4, 1, 1, &__syslib_exports },
	{ "libjs", 0x0000, 0x0001, 4, 0, 52, &__libjs_exports },
};
