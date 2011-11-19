
	----8< ----

	// js sample: define a special class
static JSBool testobjA_ctor(JSContext *pcxa, JSObject *pobja, uintN argc, jsval *pva1, jsval *rval)
{
//	JSString	*pstr1;
//	pstr1 = JS_ValueToString(pcxa, *pva1);
//	XJSE_TRACE("test! (%d, %s)", argc, JS_GetStringBytes(pstr1));
	XJSE_TRACE("testobjA_ctor!");

	return	JS_TRUE;
}

		// Define custom class/ object
		JSClass	ctestA = {
			"TestObjA", JSCLASS_HAS_PRIVATE,
			JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
			JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
			JSCLASS_NO_OPTIONAL_MEMBERS
		};
		JSFunctionSpec	afuncs_testA[] = {
			{	"debug", xjse_debugprint, (uint8)1, (uint8)0, 0,	},
			{	0, 0, 0, 0, 0,	},	// JS_FS_END,
		}; 
		JSObject		*pobj_proto1 = JS_InitClass(pcx1,
			poglobal, NULL, &ctestA, testobjA_ctor, 1,
			NULL, afuncs_testA, NULL, afuncs_testA);
		if(pobj_proto1 == 0)
			goto	failed;


	----8< ----

JSBool xgg_options_resolve(JSContext* pcxa, JSObject *pobja, jsval id, uintN flags, JSObject** ppobja)
{
    XJSE_TRACE("xgg_options_resolve. (%s)",
        JS_GetStringBytes(JS_ValueToString(pcxa, id)));
    if((flags & JSRESOLVE_ASSIGNING) != 0) {
        JSBool  bresolved;
        if(!JS_ResolveStandardClass(pcxa, pobja, id, &bresolved))
            return  JS_FALSE;
        if(bresolved)
            *ppobja = pobja;
    }
    return  JS_TRUE;
}

static JSClass class_xgg_options = {
    "options", JSCLASS_HAS_PRIVATE | JSCLASS_NEW_RESOLVE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    //JS_EnumerateStub, JS_ResolveStub,
    JS_EnumerateStandardClasses, xgg_options_resolve,
    JS_ConvertStub, xgg_options_dtor,
    JSCLASS_NO_OPTIONAL_MEMBERS
};


	----8< ----

		b1 = JS_LookupProperty(pcxa, pobja, psz1, &vr1);
        if(b1 != JS_TRUE) {
            XJSE_TRACE("JS_LookupProerty() failed! (%s)", psz1);
            goto    failed;
        }
        XJSE_TRACE("JS_LookupProerty(): (%s, %s)", psz1,
            JS_GetStringBytes(JS_ValueToString(pcxa, vr1)));


	----8< ----

	// js sample:
	char		*script = "var xhr = XMLHttpRequest(); xhr; ";
	b1 = JS_EvaluateScript(pcx1, poglobal, script, strlen(script),
		"testjs01", (uint32)0, &rval);

	// using JS_DefineFunction() instead
	//JSFunction* pf0 = JS_NewFunction(pctxa->pctx, xgg_options, 2, 0, 0, "");


	----8< ----

	// js sample: ExecuteScript
    jsval       rval;
    JSString    *str;
    char        buf[1024];
    char        *pszjsfilename = (char*)buf;
    JSScript    *pscript1;

    xjse_snprintf(0, buf, 1024, "./samples/%s.js", pszname);
    pscript1 = JS_CompileFile(pcx1, poglobal, pszjsfilename);
    if(pscript1 == 0)
        goto    failed;
    b1 = JS_ExecuteScript(pcx1, poglobal, pscript1, &rval);
    if(!b1)
        goto    failed;
    JS_DestroyScript(pcx1, pscript1);

    str = JS_ValueToString(pcx1, rval);
    XJSE_TRACE("script result: (%d) %s",
        (b1 == JS_TRUE), JS_GetStringBytes(str));


	----8< ----

/*	expat sample: reading xml stream 
	strcpy(buf, "<xc></x");
	if(!XML_Parse(parser1, buf, strlen(buf), 0)) {
		reportExpatError(parser1, "filenamexxxx");
	}
	strcpy(buf, "c>");
	if(!XML_Parse(parser1, buf, strlen(buf), 1)) {
		reportExpatError(parser1, "filenamexxxx");
	}
*/


	----8< ----

	// creating gdk widgets
    GtkWidget   *peb1 = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(pctx->pwindow), peb1);
    gtk_widget_show(peb1);
    gtk_widget_set_usize(peb1, 300, 20);
    //
    GtkWidget   *plabel1 = gtk_label_new("testing gtk widgets...");
    gtk_container_add(GTK_CONTAINER(peb1), plabel1);
    gtk_widget_show(plabel1);
    gtk_widget_set_usize(plabel1, 300, 20);

	----8< ----

	// loading module (20080105 way to dlsym() is not resolved)
    SceUID      uidmod = 0;
    uidmod = sceKernelLoadModule("testjs01km.prx", 0, 0);   // restricted
    //SceModule *pmod = sceKernelFindModuleByUID(uidmod);   // kernel code
    //SceUID        uidf = sceIoOpen("testjs01km.prx", PSP_O_RDONLY, 0777);
    //uidmod = sceKernelLoadModuleByID(uidf, 0, 0); // device restricted
    //uidmod = sceKernelLoadModuleMs("ms0:/testjs01km.prx", 0, 0);  // illegal perm call
    XJSE_TRACE("sceKernelLoadModule returned (%d)", uidmod);
    if(uidmod < 0) {
        XJSE_TRACE("sceKernelLoadModule (probably) failed! (%x)", uidmod);
        uidmod = 0;
    } else {
        // from psplink/lib.c
        xjse_uint8_t    digest[20];
        xjse_int_t      nid, n0;
        char*           pszfname = "xjse_pspkm_timer_start"+
        if(sceKernelUtilsSha1Digest(
            (xjse_uint8_t*)pszfname, strlen(pszfname), digest) >= 0) {
            nid = digest[0] | (digest[1] << 8) | (digest[2] << 16)
                | (digest[3] << 24);
            struct SceLibraryEntryTable *pentry;
            SceModule* pmod = sceKernelFindModuleByUID(uidmod);	// kernel code
            if(pmod != 0) {
                while(n0 < pmod->ent_size) {
                    pentry = (struct SceLibraryEntryTable*)(pmod->ent_top + n0);
                    if((pentry->libname) && (strcmp(pentry->libname, pszfname) == 0)) {
                        break;
                    } else if(!pentry->libname && !pszfname) {
                        break;
                    }
                    n0 += (pentry->len * 4);
                }
            }
            if(pentry != 0) {
                for(n0 = 0; n0 < pentry->stubcount; n0++) {
                    if(entry->entrytable[n0] == nid) {
                        // found!
                    }
                }
            }
        }
    }

    if(uidmod != 0) {
        sceKernelStopModule(uidmod, 0, 0, 0, 0);
        sceKernelUnloadModule(uidmod);
    }
    //sceIoClose(uidf);

	----8< ----
	// more recent LoadModule example (20081223)

    {
        xjse_int_t          nflags = 0;
//      SceUID              uid0 = 2;   // 1: kernel, 2: user partition
//      SceKernelLMOption   opt0;
//      memset(&opt0, 0, sizeof(SceKernelLMOption));
//      opt0.size = sizeof(SceKernelLMOption);
//      opt0.mpidtext = uid0;
//      opt0.mpiddata = uid0;
//      opt0.position = 0;
//      opt0.access = 1;
        SceUID  uidprx = sceKernelLoadModule("libjs.prx", nflags,
            0); // &opt0 to load in the kernel part.
        XJSE_TRACE("(X) prx [%p]", uidprx);
#if 0
        SceModule   *pmod = sceKernelFindModuleByUID(uidprx);//kernel code?
        XJSE_TRACE("(X) mod %p\n", pmod);
    {
        printf("Attr %04X, Version %x.%x\n",
            pmod->attribute, pmod->version[0], pmod->version[1]);
        printf("Name %s\n", pmod->modname);
        printf("Text %08X, Size %08X, Data Size %08X\n",
            pmod->text_addr, pmod->text_size, pmod->data_size);
        printf("Entry Addr %08X\n", pmod->entry_addr);
        printf("Stub %p %08X, Ent %p %08X\n",
            pmod->stub_top, pmod->stub_size, pmod->ent_top, pmod->ent_size);
        for(i = 0; i < pmod->nsegment; i++) {
            printf("Segment[%d] %08X %08X\n",
                i, pmod->segmentaddr[i], pmod->segmentsize[i]);
        }
        xjse_int_t  fd0;
        xjse_int_t  nret = sceKernelStartModule(uidprx, 0, NULL, &fd0, NULL);
    }

        // Let's test the module's exports
        XJSE_TRACE("(X) Module Info [%p]", (void*)getModuleInfo());
    }

	----8< ----

{
    int             n1 = 0;
    struct timeval  tv1;
    n1 = gettimeofday(&tv1, 0);
    XJSE_TRACE("(X) XXXX gettimeofday (%d)[%d, %d]", n1, (int)tv1.tv_sec, (int)tv1.tv_usec);
}

	----8< ---- kernel mode module

#ifdef  XTPF_PSP
SceUID xmain_loadprx(const char *pszpath, xjse_int_t nflags, xjse_int_t ntype)
{
    SceKernelLMOption   option;
    SceUID              uid1;

    // If the type is 0, then load the module in the kernel partition,
    // otherwise load it in the user partition.
    uid1 = (ntype ? 2 : 1);

    // Initialize structure
    memset(&option, 0, sizeof(option));
    option.size = sizeof(option);
    option.mpidtext = uid1;
    option.mpiddata = uid1;
    option.position = 0;
    option.access = 1;

    return sceKernelLoadModule(pszpath, nflags, (ntype ? &option : NULL));
}
#endif  //XTPF_PSP

#if 0
    // load kernel mode prx
    SceUID  uidprx = xmain_loadprx("host0:/xjse_km.prx", 0, 0);
    if(uidprx < 0) {
        XJSE_TRACE("xmain_loadprx() failed! [%d]", uidprx);
        goto    failed;
    }
    sceKernelStartModule(uidprx, 0, 0, NULL, NULL);
#endif

