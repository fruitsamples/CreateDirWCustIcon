/*	File:		CreateDirWithCustomIcon.c		Description:This snippet demonstrates how to create a directory and how to steal a custom				icon from another directory by CatMoving the "Icon\r" file from the other				directory.	Author:		VMC	Copyright: 	Copyright: � 1996-1999 by Apple Computer, Inc.				all rights reserved.		Disclaimer:	You may incorporate this sample code into your applications without				restriction, though the sample code has been provided "AS IS" and the				responsibility for its operation is 100% yours.  However, what you are				not permitted to do is to redistribute the source as "DSC Sample Code"				after having made changes. If you're going to re-distribute the source,				we require that you make it clear in the source that the code was				descended from Apple Sample Code, but that you've made changes.		Change History (most recent first):				6/25/99	Updated for Metrowerks Codewarror Pro 2.1(KG)*/#include <StandardFile.h>#include <Script.h>#include "CreateDirWCustIcon.h"/*****  globals *****/Boolean					gQuitTheApp = false;		/* true = quit program *//*****	main() *****/void main(){	short			sleepTime  =  60;	EventRecord		myEvent;		initApp();								/* call initialization routine */	setUpMenus();	do	{		if (WaitNextEvent(everyEvent, &myEvent, sleepTime, NIL))			switch (myEvent.what)			{				case mouseDown:					doMousedown(&myEvent);					break;									case keyDown:				case autoKey:					doKey(&myEvent);					break;				case kHighLevelEvent:					doHighLevel(&myEvent);					break;				case activateEvt:					break;								case updateEvt:					doUpdate(&myEvent);					break; 								case diskEvt:					break;							case osEvt:				/*  I don't really care yet. */					break;			}	}  while (!gQuitTheApp);			} 				/* end of main *//*****	initApp() *****/void initApp(){		/* Memory specific initializations. */		MaxApplZone();				/* grow the heap to its maximum size */	MoreMasters();		   		/* create more master pointers		 */	MoreMasters();	MoreMasters();	MoreMasters();	MoreMasters();	/* Initializing the ROM Managers */		InitGraf((Ptr) &qd.thePort);	InitFonts();	InitWindows();	InitMenus();	FlushEvents(everyEvent,0);	TEInit();	InitDialogs(NIL);	InitCursor();		if (!checkGestaltFeatures())		ExitToShell();		/* Install the required Apple Event Handlers */	if (!installAEHandlers())		ExitToShell();}				/* end of initApp() *//*****  checkGestaltFeatures() *****/Boolean checkGestaltFeatures(){	long		gestaltFeature;	OSErr		myErr;	myErr = Gestalt(gestaltSystemVersion, &gestaltFeature); 	/* which SysVersion present? */	if (myErr == noErr)	{		gestaltFeature = (gestaltFeature >> 8) & 0xf; 										/* shift result over & mask out major version number */		if (gestaltFeature < 7) 		/* This is a System 7+ shell.  We quit otherwise. */		{			StopAlert(BADSYSTEMID, nil);			return(false);		}	}		if (myErr == noErr)	{		myErr = Gestalt(gestaltQuickdrawVersion, &gestaltFeature);													/* we want color QD cuz we're spoiled */		if (myErr == noErr)		{			if(gestaltFeature < gestalt32BitQD)			{				StopAlert(BADQUICKDRAWID, nil);				return(false);			}		}	}				if (myErr == noErr)	{		myErr = Gestalt(gestaltAppleEventsAttr, &gestaltFeature);		if (myErr == noErr)		{			if (!(gestaltFeature & 0xf))			{				StopAlert(NOAPPLEEVENTS, nil);				return(false);			}		}	}	if (!myErr)				return(true);			/* if there was an error we cannot continue */	else	{		return(false);			/* we made it through without a hitch */	}}/*****	installAEHandlers *****/Boolean installAEHandlers(){	OSErr		myErr;		myErr = AEInstallEventHandler ( kCoreEventClass, 					kAEOpenApplication, NewAEEventHandlerProc(DoOpenAppAE), 0L, false );	if (myErr == noErr)		myErr = AEInstallEventHandler ( kCoreEventClass,					kAEOpenDocuments, NewAEEventHandlerProc(DoOpenDocAE), 0L, false );	if (myErr == noErr)		myErr = AEInstallEventHandler ( kCoreEventClass,					kAEPrintDocuments, NewAEEventHandlerProc(DoPrintDocAE), 0L, false );	if (myErr == noErr)		myErr = AEInstallEventHandler ( kCoreEventClass,					kAEQuitApplication, NewAEEventHandlerProc(DoQuitAppAE), 0L, false );	if (myErr)		return(false);	else		return(true);}/*****	setUpMenus() 			This will put up our menus.  Be sure to read the comments by the			AppendMenu() and SetItem() calls to see how different options			behave.		*****/void setUpMenus(){	short			n;	MenuHandle		theMenu;		for (n = 0; n < MAXMENUS; n++)	{		theMenu = GetMenu(FIRSTMENUID + n);		if (theMenu)  							InsertMenu(theMenu, 0);	}	theMenu = GetMenuHandle(APPLEMENU);	if (theMenu)		AppendResMenu(theMenu, 'DRVR');		DrawMenuBar();	return;}					/* end of setUpMenus()  *//*****	setUpWindow() *****/void setUpWindow( void ){	WindowPtr	myWind;	myWind = GetNewCWindow(WINDOWID, nil, PUTINFRONT);	if (myWind != nil)	{		ShowWindow(myWind);	}}/*****	doMousedown()			We figure out where the user has clicked the mouse.  If the user			clicks anywhere but the menu bar, we beep.  Otherwise, we figure			out which menu they have clicked and dispatch.*****/void doMousedown(EventRecord *eventRec){	short		windPart;	WindowPtr	myWind;	long		menuResult;		windPart = FindWindow(eventRec->where, &myWind);		switch(windPart)	{		case inContent:			SelectWindow(myWind);			break;		case inDrag:			DragWindow( myWind, eventRec->where, &qd.screenBits.bounds );			break;		case inMenuBar:			menuResult = MenuSelect(eventRec->where);			dispatch(menuResult);			break;		case inSysWindow:			SystemClick( eventRec, myWind );			break;					case inGoAway:			gQuitTheApp = true;			break;					default:			break;	}	return;} 				/* end of doMousedown *//***** 	doKey()			We ignore keys pressed unless they are accompanied by a 			command key.  Then we dispatch.*****/void doKey(EventRecord *eventRec){	char	keyPressed;	long	menuResult;		keyPressed = (char) (eventRec->message & charCodeMask);		if((eventRec->modifiers & cmdKey) != 0)	{		menuResult = MenuKey(keyPressed);		dispatch(menuResult);		}	return;} 				/* end of do_key *//*****	doHighLevel() *****/void doHighLevel(EventRecord *eventRec){	OSErr myErr;	myErr = AEProcessAppleEvent(eventRec);}pascal OSErr	DoOpenAppAE(AppleEvent theAppleEvent, AppleEvent reply, long refCon){	#pragma unused(theAppleEvent, reply, refCon)		setUpWindow();	return(noErr);}pascal OSErr	DoOpenDocAE(AppleEvent theAppleEvent, AppleEvent reply, long refCon){	#pragma unused(theAppleEvent, reply, refCon)		return(noErr);}pascal OSErr	DoPrintDocAE(AppleEvent theAppleEvent, AppleEvent reply, long refCon){	#pragma unused(theAppleEvent, reply, refCon)		return(noErr);}pascal OSErr	DoQuitAppAE(AppleEvent theAppleEvent, AppleEvent reply, long refCon){	#pragma unused(theAppleEvent, reply, refCon)		gQuitTheApp = true;	return(noErr);}/***** doUpdate() *****/void	doUpdate(EventRecord *eventRec){	GrafPtr		savedPort;	WindowPtr	theWindow;	theWindow = (WindowPtr) eventRec->message;		GetPort(&savedPort);	SetPort((GrafPtr) theWindow);	BeginUpdate(theWindow);	EndUpdate(theWindow);	SetPort(savedPort);}/***** 	dispatch()			We determine which menu the user has chosen (either with mouse			or with command keys) and jump to the routine that handles			that menu's commands.*****/void dispatch(long menuResult){	short		theMenu;			/* menu selected */	short		theItem;			/* item selected */		theMenu = HiWord (menuResult);		/* menuID selected */	theItem = LoWord (menuResult);		/* item# selected */		switch (theMenu)	{		case APPLEMENU:			doAppleCmds(theItem);			break;		case FILEMENU:			doFileCmds(theItem);			break;					case EDITMENU:			break;		case TESTMENU:			doTestCmds(theItem);			break;	}	HiliteMenu(0);}				/* end of dispatch *//***** 	doAppleCmds() 			When the user chooses the "About MyMenuText" item, we display the			About box.  If the user chooses a DA, we open the DA.*****/void doAppleCmds(short theItem){		MenuHandle		myMenu;	Str255			name;	short			dummy;		if(theItem == appleABOUT)	{		Alert(ABOUTID, (ModalFilterUPP) NIL);	}	else	{		myMenu = GetMenuHandle (APPLEMENU);		if (myMenu)		{			GetMenuItemText(myMenu, theItem, name);			dummy = OpenDeskAcc(name);		}	}	return;} 				/* end of doAppleCmds *//*****	doFileCmds() 			When the user chooses Quit on the File menu, we quit.*****/void doFileCmds (short theItem){	switch (theItem)	{					case fileQUIT:			gQuitTheApp = true;			break;	}	return;} /* end of doFileCmds *//*****	doTestCmds()			When the user chooses the item in the Test menu, they			will get to choose a folder with a custom icon*****/void doTestCmds (short theItem){		StandardFileReply	reply;	FSSpec				spec;	OSErr				err;		switch (theItem)	{		case testDOTHETEST:			//open a folder with a custom icon and choose the "Icon\r" file			StandardGetFile(nil, -1, nil, &reply);						if (reply.sfGood)			{				//make an FSSpec to the "Icon\r" file				err = FSMakeFSSpec(reply.sfFile.vRefNum, reply.sfFile.parID, 									reply.sfFile.name, &spec);				if (!err)					makeCustomIconFolder(&spec);			}			break;				}		return;}				/* end of doTestCmds */#define		HAS_CUST_ICON	0x0400#define 	HAS_BEEN_INIT 	0x0100void makeCustomIconFolder(FSSpec *iconFileSpec){	OSErr		err;	FSSpec		foldSpec;	long		dirID;	CInfoPBRec	pb;	/*			Make an FSSpec for the directory we're creating		For demo purposes, we'll put this in the root directory		of the boot volume.	*/	err = FSMakeFSSpec(-1, fsRtDirID, "\pNew Folder", &foldSpec);		/* Since the folder doesn't exist, we should get fnfErr */	if (err == fnfErr)	{		//create the the directory		err = FSpDirCreate(&foldSpec, smSystemScript, &dirID);					if (!err)		{			pb.dirInfo.ioCompletion = nil;			pb.dirInfo.ioNamePtr = "\pNew Folder"; // we care about file names			pb.dirInfo.ioVRefNum = foldSpec.vRefNum;			pb.dirInfo.ioFDirIndex = 0;	// use ioNamePtr			pb.dirInfo.ioDrDirID = foldSpec.parID;						err = PBGetCatInfoSync(&pb);						//set the custom icon flag bit in the DInfo record for the new folder			if (!err)			{				if ( (pb.dirInfo.ioDrUsrWds.frFlags & HAS_CUST_ICON) == 0)			    {			        pb.dirInfo.ioDrUsrWds.frFlags |= HAS_CUST_ICON;  			        	/* turn this bit on  */			        pb.dirInfo.ioDrUsrWds.frFlags &= ~HAS_BEEN_INIT; 			        	/* turn this bit off */			    }				/*					PBGetCatInfo messes up the dirID field in parameter block					so we have to reset it.				*/				pb.dirInfo.ioDrDirID = foldSpec.parID;			    err = PBSetCatInfoSync(&pb);	// set the hasCustomIcon flag			    			    //Move the icon file into the new folder			    if (!err)			    {					/*						Move our invisible Icon file ("\pIcon\r")						into our new folder.					*/					err = FSpCatMove(iconFileSpec, &foldSpec);										if (!err)					{						/* 							Do a Mod Date Bump to get Finder to update cache							Note:  Because the Finder caches information about							open windows, you might need to use the BumpDate							routine from MoreFiles						*/					}				}			}		}	}		return;}