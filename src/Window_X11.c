#include "Core.h"
#if CC_WIN_BACKEND == CC_WIN_BACKEND_X11
/*
   The Open Toolkit Library License
  
   Copyright (c) 2006 - 2009 the Open Toolkit library.
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
   the Software, and to permit persons to whom the Software is furnished to do
   so, subject to the following conditions:
  
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
  
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.
*/

#include "_WindowBase.h"
#include "String.h"
#include "Funcs.h"
#include "Bitmap.h"
#include "Options.h"
#include "Errors.h"
#include "Utils.h"
/*
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/XF86keysym.h>
#ifdef CC_BUILD_XINPUT2
#include <X11/extensions/XInput2.h>
#endif
*/
#include "../misc/x11/min-xlib.h"
#include "../misc/x11/min-keysymdef.h"
#include "../misc/x11/min-xutil.h"
#include "../misc/x11/min-xkblib.h"
#include "../misc/x11/min-xinput2.h"
#include "../misc/x11/min-XF86keysym.h"
#include <stdio.h>

#ifdef X_HAVE_UTF8_STRING
#define CC_BUILD_XIM
/* XIM support based off details described in */
/* https://tedyin.com/posts/a-brief-intro-to-linux-input-method-framework/ */
#endif

#if defined CC_BUILD_HPUX || defined CC_BUILD_IRIX
#undef CC_BUILD_XIM
#endif

#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD    1
#define _NET_WM_STATE_TOGGLE 2

static Display* win_display;
static Window win_rootWin;
static XVisualInfo win_visual;
#ifdef CC_BUILD_XIM
static XIM win_xim;
static XIC win_xic;
#endif
 
static Atom wm_destroy, net_wm_state, net_wm_ping;
static Atom net_wm_state_minimized;
static Atom net_wm_state_fullscreen;

static Atom xa_clipboard, xa_targets, xa_utf8_string, xa_data_sel;
static Atom xa_atom = 4;
static cc_bool grabCursor;
static long win_eventMask = StructureNotifyMask | /* SubstructureNotifyMask | */ 
	ExposureMask      | KeyReleaseMask  | KeyPressMask    | KeymapStateMask   | 
	PointerMotionMask | FocusChangeMask | ButtonPressMask | ButtonReleaseMask | 
	EnterWindowMask   | LeaveWindowMask | PropertyChangeMask;

static int MapNativeKey(KeySym key, unsigned int state) {
	if (key >= XK_0 && key <= XK_9) { return '0' + (key - XK_0); }
	if (key >= XK_A && key <= XK_Z) { return 'A' + (key - XK_A); }
	if (key >= XK_a && key <= XK_z) { return 'A' + (key - XK_a); }

	if (key >= XK_F1 && key <= XK_F24)    { return CCKEY_F1  + (key - XK_F1); }
	if (key >= XK_KP_0 && key <= XK_KP_9) { return CCKEY_KP0 + (key - XK_KP_0); }

	/* Same Num Lock behaviour as Windows and text editors */
	if (key >= XK_KP_Home && key <= XK_KP_Delete && !(state & Mod2Mask)) {
		if (key == XK_KP_Home) return CCKEY_HOME;
		if (key == XK_KP_Up)   return CCKEY_UP;
		if (key == XK_KP_Page_Up) return CCKEY_PAGEUP;

		if (key == XK_KP_Left)   return CCKEY_LEFT;
		if (key == XK_KP_Insert) return CCKEY_INSERT;
		if (key == XK_KP_Right)  return CCKEY_RIGHT;

		if (key == XK_KP_End)  return CCKEY_END;
		if (key == XK_KP_Down) return CCKEY_DOWN;
		if (key == XK_KP_Page_Down) return CCKEY_PAGEDOWN;
	}
	
	switch (key) {
		case XF86XK_AudioLowerVolume: return CCKEY_VOLUME_DOWN;
		case XF86XK_AudioMute:        return CCKEY_VOLUME_MUTE;
		case XF86XK_AudioRaiseVolume: return CCKEY_VOLUME_UP;
		
		case XF86XK_AudioPlay: return CCKEY_MEDIA_PLAY;
		case XF86XK_AudioStop: return CCKEY_MEDIA_STOP;
		case XF86XK_AudioPrev: return CCKEY_MEDIA_PREV;
		case XF86XK_AudioNext: return CCKEY_MEDIA_NEXT;
		
		case XF86XK_HomePage:   return CCKEY_BROWSER_HOME;
		case XF86XK_Mail:       return CCKEY_LAUNCH_MAIL;
		case XF86XK_Search:     return CCKEY_BROWSER_SEARCH;
		case XF86XK_Calculator: return CCKEY_LAUNCH_CALC;
		
		case XF86XK_Back:       return CCKEY_BROWSER_PREV;
		case XF86XK_Forward:    return CCKEY_BROWSER_NEXT;
		case XF86XK_Stop:       return CCKEY_BROWSER_STOP;
		case XF86XK_Refresh:    return CCKEY_BROWSER_REFRESH;
		case XF86XK_Sleep:      return CCKEY_SLEEP;
		case XF86XK_Favorites:  return CCKEY_BROWSER_FAVORITES;
		case XF86XK_AudioMedia: return CCKEY_LAUNCH_MEDIA;
		case XF86XK_MyComputer: return CCKEY_LAUNCH_APP1;
	}

	/* A chromebook user reported issues with pressing some keys: */
	/*  tilde - "Unknown key press: (8000060, 800007E) */
	/*  quote - "Unknown key press: (8000027, 8000022) */
	/* Note if 8000 is stripped, you get '0060' (XK_grave) and 0027 (XK_apostrophe) */
	/*   ChromeOS seems to also mask to 0xFFFF, so I also do so here */
	/* https://chromium.googlesource.com/chromium/src/+/lkgr/ui/events/keycodes/keyboard_code_conversion_x.cc */
	key &= 0xFFFF;

	switch (key) {
		case XK_Escape: return CCKEY_ESCAPE;
		case XK_Return: return CCKEY_ENTER;
		case XK_space: return CCKEY_SPACE;
		case XK_BackSpace: return CCKEY_BACKSPACE;

		case XK_Shift_L: return CCKEY_LSHIFT;
		case XK_Shift_R: return CCKEY_RSHIFT;
		case XK_Alt_L: return CCKEY_LALT;
		case XK_Alt_R: return CCKEY_RALT;
		case XK_Control_L: return CCKEY_LCTRL;
		case XK_Control_R: return CCKEY_RCTRL;
		case XK_Super_L: return CCKEY_LWIN;
		case XK_Super_R: return CCKEY_RWIN;
		case XK_Meta_L: return CCKEY_LWIN;
		case XK_Meta_R: return CCKEY_RWIN;

		case XK_Menu:  return CCKEY_MENU;
		case XK_Tab:   return CCKEY_TAB;
		case XK_minus: return CCKEY_MINUS;
		case XK_plus:  return CCKEY_EQUALS;
		case XK_equal: return CCKEY_EQUALS;

		case XK_Caps_Lock: return CCKEY_CAPSLOCK;
		case XK_Num_Lock:  return CCKEY_NUMLOCK;

		case XK_Pause: return CCKEY_PAUSE;
		case XK_Break: return CCKEY_PAUSE;
		case XK_Scroll_Lock: return CCKEY_SCROLLLOCK;
		case XK_Insert:  return CCKEY_INSERT;
		case XK_Print:   return CCKEY_PRINTSCREEN;
		case XK_Sys_Req: return CCKEY_PRINTSCREEN;

		case XK_backslash: return CCKEY_BACKSLASH;
		case XK_bar:       return CCKEY_BACKSLASH;
		case XK_braceleft:    return CCKEY_LBRACKET;
		case XK_bracketleft:  return CCKEY_LBRACKET;
		case XK_braceright:   return CCKEY_RBRACKET;
		case XK_bracketright: return CCKEY_RBRACKET;
		case XK_colon:      return CCKEY_SEMICOLON;
		case XK_semicolon:  return CCKEY_SEMICOLON;
		case XK_quoteright: return CCKEY_QUOTE;
		case XK_quotedbl:   return CCKEY_QUOTE;
		case XK_quoteleft:  return CCKEY_TILDE;
		case XK_asciitilde: return CCKEY_TILDE;

		case XK_comma: return CCKEY_COMMA;
		case XK_less:  return CCKEY_COMMA;
		case XK_period:  return CCKEY_PERIOD;
		case XK_greater: return CCKEY_PERIOD;
		case XK_slash:    return CCKEY_SLASH;
		case XK_question: return CCKEY_SLASH;

		case XK_Left:  return CCKEY_LEFT;
		case XK_Down:  return CCKEY_DOWN;
		case XK_Right: return CCKEY_RIGHT;
		case XK_Up:    return CCKEY_UP;

		case XK_Delete: return CCKEY_DELETE;
		case XK_Home:   return CCKEY_HOME;
		case XK_End:    return CCKEY_END;
		case XK_Page_Up:   return CCKEY_PAGEUP;
		case XK_Page_Down: return CCKEY_PAGEDOWN;

		case XK_KP_Add: return CCKEY_KP_PLUS;
		case XK_KP_Subtract: return CCKEY_KP_MINUS;
		case XK_KP_Multiply: return CCKEY_KP_MULTIPLY;
		case XK_KP_Divide:  return CCKEY_KP_DIVIDE;
		case XK_KP_Decimal: return CCKEY_KP_DECIMAL;
		case XK_KP_Insert: return CCKEY_KP0;
		case XK_KP_End:  return CCKEY_KP1;
		case XK_KP_Down: return CCKEY_KP2;
		case XK_KP_Page_Down: return CCKEY_KP3;
		case XK_KP_Left:  return CCKEY_KP4;
		case XK_KP_Begin: return CCKEY_KP5;
		case XK_KP_Right: return CCKEY_KP6;
		case XK_KP_Home: return CCKEY_KP7;
		case XK_KP_Up:   return CCKEY_KP8;
		case XK_KP_Page_Up: return CCKEY_KP9;
		case XK_KP_Delete:  return CCKEY_KP_DECIMAL;
		case XK_KP_Enter:   return CCKEY_KP_ENTER;
		
		case XK_ISO_Level3_Shift: return CCKEY_RALT; /* AltGr mode switch on some European keyboard layouts */
	}
	return INPUT_NONE;
}

/* NOTE: This may not be entirely accurate, because user can configure keycode mappings */
static const cc_uint8 keycodeMap[136] = {
/* 00 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 08 */ 0, CCKEY_ESCAPE, '1', '2', '3', '4', '5', '6',
/* 10 */ '7', '8', '9', '0', CCKEY_MINUS, CCKEY_EQUALS, CCKEY_BACKSPACE, CCKEY_TAB, 
/* 18 */ 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
/* 20 */ 'O',  'P', CCKEY_LBRACKET, CCKEY_RBRACKET, CCKEY_ENTER, CCKEY_LCTRL, 'A', 'S', 
/* 28 */ 'D', 'F', 'G', 'H',  'J', 'K', 'L', CCKEY_SEMICOLON,
/* 30 */ CCKEY_QUOTE, CCKEY_TILDE, CCKEY_LSHIFT, CCKEY_BACKSLASH, 'Z', 'X', 'C', 'V', 
/* 38 */ 'B', 'N', 'M', CCKEY_PERIOD, CCKEY_COMMA, CCKEY_SLASH, CCKEY_RSHIFT, CCKEY_KP_MULTIPLY,
/* 40 */ CCKEY_LALT, CCKEY_SPACE, CCKEY_CAPSLOCK, CCKEY_F1, CCKEY_F2, CCKEY_F3, CCKEY_F4, CCKEY_F5, 
/* 48 */ CCKEY_F6, CCKEY_F7, CCKEY_F8, CCKEY_F9, CCKEY_F10, CCKEY_NUMLOCK, CCKEY_SCROLLLOCK, CCKEY_KP7,
/* 50 */ CCKEY_KP8, CCKEY_KP9, CCKEY_KP_MINUS, CCKEY_KP4, CCKEY_KP5, CCKEY_KP6, CCKEY_KP_PLUS, CCKEY_KP1, 
/* 58 */ CCKEY_KP2, CCKEY_KP3, CCKEY_KP0, CCKEY_KP_DECIMAL, 0, 0, 0, CCKEY_F11,
/* 60 */ CCKEY_F12, 0, 0, 0, 0, 0, 0, 0, 
/* 68 */ 0, 0, 0, 0, CCKEY_RALT, CCKEY_RCTRL, CCKEY_HOME, CCKEY_UP, 
/* 70 */ CCKEY_PAGEUP, CCKEY_LEFT, CCKEY_RIGHT, CCKEY_END, CCKEY_DOWN, CCKEY_PAGEDOWN, CCKEY_INSERT, CCKEY_DELETE, 
/* 78 */ 0, 0, 0, 0, 0, 0, 0, CCKEY_PAUSE, 
/* 80 */ 0, 0, 0, 0, 0, CCKEY_LWIN, 0, CCKEY_RWIN
};

static int MapNativeKeycode(unsigned int keycode) {
	return keycode < Array_Elems(keycodeMap) ? keycodeMap[keycode] : 0;
}

static void RegisterAtoms(void) {
	Display* display = win_display;
	wm_destroy = XInternAtom(display, "WM_DELETE_WINDOW", true);
	net_wm_state = XInternAtom(display, "_NET_WM_STATE", false);
	net_wm_ping  = XInternAtom(display, "_NET_WM_PING",  false);
	net_wm_state_minimized  = XInternAtom(display, "_NET_WM_STATE_MINIMIZED",  false);
	net_wm_state_fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", false);

	xa_clipboard   = XInternAtom(display, "CLIPBOARD",   false);
	xa_targets     = XInternAtom(display, "TARGETS",     false);
	xa_utf8_string = XInternAtom(display, "UTF8_STRING", false);
	xa_data_sel    = XInternAtom(display, "CC_SEL_DATA", false);
}

static void RefreshWindowBounds(int width, int height) {
	if (width != Window_Main.Width || height != Window_Main.Height) {
		Window_Main.Width  = width;
		Window_Main.Height = height;
		Event_RaiseVoid(&WindowEvents.Resized);
	}
}

typedef int (*X11_ErrorHandler)(Display* dpy, XErrorEvent* ev);
typedef int (*X11_IOErrorHandler)(Display* dpy);
static X11_ErrorHandler   realXErrorHandler;
static X11_IOErrorHandler realXIOErrorHandler;

static void LogXErrorCore(const char* msg) {
	char traceBuffer[2048];
	cc_string trace;
	Platform_LogConst(msg);

	String_InitArray(trace, traceBuffer);
	Logger_Backtrace(&trace, NULL);
	Platform_Log(traceBuffer, trace.length);
}

static int LogXError(Display* dpy, XErrorEvent* ev) {
	LogXErrorCore("== unhandled X11 error ==");
	return realXErrorHandler(dpy, ev);
}

static int LogXIOError(Display* dpy) {
	LogXErrorCore("== unhandled XIO error ==");
	return realXIOErrorHandler(dpy);
}

static void HookXErrors(void) {
	realXErrorHandler   = XSetErrorHandler(LogXError);
	realXIOErrorHandler = XSetIOErrorHandler(LogXIOError);
}


/*########################################################################################################################*
*--------------------------------------------------Public implementation--------------------------------------------------*
*#########################################################################################################################*/
static XVisualInfo GLContext_SelectVisual(void);

void Window_PreInit(void) { 
	DisplayInfo.CursorVisible = true;
}

void Window_Init(void) {
	Display* display = XOpenDisplay(NULL);
	int screen;

	if (!display) Process_Abort("Failed to open the X11 display. No X server running?");
	screen = DefaultScreen(display);
	HookXErrors();

	win_display = display;
	win_rootWin = RootWindow(display, screen);
	Input.Sources = INPUT_SOURCE_NORMAL;

	/* TODO: Use Xinerama and XRandR for querying these */
	DisplayInfo.Width  = DisplayWidth(display,  screen);
	DisplayInfo.Height = DisplayHeight(display, screen);
	DisplayInfo.Depth  = DefaultDepth(display,  screen);
	DisplayInfo.ScaleX = 1;
	DisplayInfo.ScaleY = 1;
}

void Window_Free(void) { }

/* See misc/x11/x11_icon_gen.cs for how to generate this file */
#include "../misc/x11/CCIcon_X11.h"

static void ApplyIcon(Window win) {
	Atom net_wm_icon = XInternAtom(win_display, "_NET_WM_ICON", false);
	Atom xa_cardinal = XInternAtom(win_display, "CARDINAL", false);
	
	XChangeProperty(win_display, win, net_wm_icon, xa_cardinal, 32, PropModeReplace, 
					(unsigned char*)CCIcon_Data, CCIcon_Size);
}

static XVisualInfo Select2DVisual(void) {
	XVisualInfo info = { 0 };
	int screen  = DefaultScreen(win_display);
	info.depth  = DefaultDepth(win_display, screen);
	info.visual = DefaultVisual(win_display, screen);
	return info;
}

static void DoCreateWindow(int width, int height, int _2d) {
	XSetWindowAttributes attributes = { 0 };
	XSizeHints hints = { 0 };
	Atom protocols[2];
	int supported, x, y;
	Window focus, win;
	int visualID;
	int focusRevert;

	x = Display_CentreX(width);
	y = Display_CentreY(height);
	RegisterAtoms();

#if CC_GFX_BACKEND_IS_GL()
	win_visual = _2d ? Select2DVisual() : GLContext_SelectVisual();
#else
	win_visual = Select2DVisual();
#endif
	visualID = win_visual.visual ? win_visual.visual->visualid : 0;

	Platform_Log2("Creating window (depth: %i, visual: %h)", &win_visual.depth, &visualID);
	attributes.colormap   = XCreateColormap(win_display, win_rootWin, win_visual.visual, AllocNone);
	attributes.event_mask = win_eventMask;

	win = XCreateWindow(win_display, win_rootWin, x, y, width, height,
		0, win_visual.depth, InputOutput, win_visual.visual,
#ifdef CC_BUILD_IRIX
		CWColormap | CWEventMask | CWBackPixel | CWBorderPixel, &attributes);
#else
		/* Omitting black/border pixels produces nicer looking resizing on some WMs */
		CWColormap | CWEventMask, &attributes);
#endif

	if (!win) Process_Abort("XCreateWindow failed");

#ifdef CC_BUILD_XIM
	win_xim = XOpenIM(win_display, NULL, NULL, NULL);
	win_xic = XCreateIC(win_xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
						XNClientWindow, win, NULL);
#endif

	/* Set hints to try to force WM to create window at requested x,y */
	/* Without this, some WMs will instead place the window whereever */
	hints.base_width  = width;
	hints.base_height = height;
	hints.flags = PSize | PPosition;
	XSetWMNormalHints(win_display, win, &hints);

	/* Register for window destroy notification */
	protocols[0] = wm_destroy;
	protocols[1] = net_wm_ping;
	XSetWMProtocols(win_display, win, protocols, 2);

	/* Request that auto-repeat is only set on devices that support it physically.
	   This typically means that it's turned off for keyboards (which is what we want).
	   We prefer this method over XAutoRepeatOff/On, because the latter needs to
	   be reset before the program exits. */
	XkbSetDetectableAutoRepeat(win_display, true, &supported);

	RefreshWindowBounds(width, height);
	Window_Main.Exists     = true;
	Window_Main.Handle.val = win;
	Window_Main.UIScaleX   = DEFAULT_UI_SCALE_X;
	Window_Main.UIScaleY   = DEFAULT_UI_SCALE_Y;
	grabCursor = Options_GetBool(OPT_GRAB_CURSOR, false);
	
	/* So right name appears in e.g. Ubuntu Unity launchbar */
	XClassHint hint = { 0 };
	#ifdef CC_BUILD_FLATPAK
		hint.res_name   = (char*)"net.classicube.flatpak.client";
		hint.res_class  = (char*)"net.classicube.flatpak.client";
	#else
		hint.res_name   = (char*)GAME_APP_TITLE;
		hint.res_class  = (char*)GAME_APP_TITLE;
	#endif
	XSetClassHint(win_display, win, &hint);
	ApplyIcon(win);

	/* Check for focus initially, in case WM doesn't send a FocusIn event */
	XGetInputFocus(win_display, &focus, &focusRevert);
	if (focus == win) Window_Main.Focused = true;
}

void Window_Create2D(int width, int height) { 
	DoCreateWindow(width, height, true); 
}

void Window_Create3D(int width, int height) { 
	DoCreateWindow(width, height, false); 
}

void Window_Destroy(void) {
	Window win = Window_Main.Handle.val;
	/* sync and discard all events queued */
	XSync(win_display, true);
	XDestroyWindow(win_display, win);
	Window_Main.Exists = false;
}

void Window_SetTitle(const cc_string* title) {
	Window win = Window_Main.Handle.val;
	char str[NATIVE_STR_LEN];
	String_EncodeUtf8(str, title);
	XStoreName(win_display, win, str);
}

static char clipboard_copy_buffer[256];
static char clipboard_paste_buffer[256];
static cc_string clipboard_copy_text  = String_FromArray(clipboard_copy_buffer);
static cc_string clipboard_paste_text = String_FromArray(clipboard_paste_buffer);
static cc_bool clipboard_paste_received;

void Clipboard_GetText(cc_string* value) {
	Window win   = Window_Main.Handle.val;
	Window owner = XGetSelectionOwner(win_display, xa_clipboard);
	int i;
	if (!owner) return; /* no window owner */

	XConvertSelection(win_display, xa_clipboard, xa_utf8_string, xa_data_sel, win, 0);
	clipboard_paste_received    = false;
	clipboard_paste_text.length = 0;

	/* wait up to 1 second for SelectionNotify event to arrive */
	for (i = 0; i < 100; i++) {
		Window_ProcessEvents(0.0);
		if (clipboard_paste_received) {
			String_AppendString(value, &clipboard_paste_text);
			return;
		} else {
			Thread_Sleep(10);
		}
	}
}

void Clipboard_SetText(const cc_string* value) {
	Window win = Window_Main.Handle.val;
	String_Copy(&clipboard_copy_text, value);
	XSetSelectionOwner(win_display, xa_clipboard, win, 0);
}

int Window_GetWindowState(void) {
	cc_bool fullscreen = false, minimised = false;
	Window win = Window_Main.Handle.val;
	Atom prop_type;
	unsigned long items, after;
	unsigned char* data = NULL;
	int i, prop_format;
	Atom* list;

	XGetWindowProperty(win_display, win,
		net_wm_state, 0, 256, false, xa_atom, &prop_type,
		&prop_format, &items, &after, &data);

	if (!data) return WINDOW_STATE_NORMAL;
	list = (Atom*)data;
		
	for (i = 0; i < items; i++) {
		Atom atom = list[i];

		if (atom == net_wm_state_minimized) {
			minimised  = true;
		} else if (atom == net_wm_state_fullscreen) {
			fullscreen = true;
		}
	}
	XFree(data);

	if (fullscreen) return WINDOW_STATE_FULLSCREEN;
	if (minimised)  return WINDOW_STATE_MINIMISED;
	return WINDOW_STATE_NORMAL;
}

static void ToggleFullscreen(long op) {
	Window win = Window_Main.Handle.val;
	XEvent ev  = { 0 };
	ev.xclient.type   = ClientMessage;
	ev.xclient.window = win;
	ev.xclient.message_type = net_wm_state;
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = op;
	ev.xclient.data.l[1] = net_wm_state_fullscreen;

	XSendEvent(win_display, win_rootWin, false,
		SubstructureRedirectMask | SubstructureNotifyMask, &ev);
	XSync(win_display, false);
	XRaiseWindow(win_display, win);
	Window_ProcessEvents(0.0);
}

cc_result Window_EnterFullscreen(void) {
	ToggleFullscreen(_NET_WM_STATE_ADD); return 0;
}
cc_result Window_ExitFullscreen(void) {
	ToggleFullscreen(_NET_WM_STATE_REMOVE); return 0;
}

int Window_IsObscured(void) { return 0; }

void Window_Show(void) {
	Window win = Window_Main.Handle.val;
	XMapWindow(win_display, win); 
}

void Window_SetSize(int width, int height) {
	Window win = Window_Main.Handle.val;
	XResizeWindow(win_display, win, width, height);
	Window_ProcessEvents(0.0);
}

void Window_RequestClose(void) {
	Event_RaiseVoid(&WindowEvents.Closing);
}


/*########################################################################################################################*
*---------------------------------------------------Mouse event handling--------------------------------------------------*
*#########################################################################################################################*/
static int MapNativeMouse(int button) {
	if (button == 1) return CCMOUSE_L;
	if (button == 2) return CCMOUSE_M;
	if (button == 3) return CCMOUSE_R;

	if (button ==  8) return CCMOUSE_X1;
	if (button ==  9) return CCMOUSE_X2;
	if (button == 10) return CCMOUSE_X3;
	if (button == 11) return CCMOUSE_X4;
	if (button == 12) return CCMOUSE_X5;
	if (button == 13) return CCMOUSE_X6;

	/* Mouse horizontal and vertical scroll */
	if (button >= 4 && button <= 7) return 0;
	Platform_Log1("Unknown mouse button: %i", &button);
	return 0;
}

static void HandleButtonPress(XEvent* e) {
	int btn = MapNativeMouse(e->xbutton.button);
	
	if (btn) Input_SetPressed(btn);
	else if (e->xbutton.button == 4) Mouse_ScrollVWheel( +1);
	else if (e->xbutton.button == 5) Mouse_ScrollVWheel( -1);
	else if (e->xbutton.button == 6) Mouse_ScrollHWheel(+1);
	else if (e->xbutton.button == 7) Mouse_ScrollHWheel(-1);
}

static void HandleButtonRelease(XEvent* e) {
	int btn = MapNativeMouse(e->xbutton.button);
	if (btn) Input_SetReleased(btn);
}


/*########################################################################################################################*
*-------------------------------------------------Keyboard event handling-------------------------------------------------*
*#########################################################################################################################*/
static int TryGetKey(XKeyEvent* ev) {
	KeySym keysym1 = XLookupKeysym(ev, 0);
	KeySym keysym2 = XLookupKeysym(ev, 1);

	int key = MapNativeKey(keysym1, ev->state);
	if (!key) key = MapNativeKey(keysym2, ev->state);
	if (key) return key;

	Platform_Log3("Unknown key %i (%x, %x)", &ev->keycode, &keysym1, &keysym2);
	/* The user may be using a keyboard layout such as cryllic - */
	/*   fallback to trying to conver the raw scancodes instead */
	return MapNativeKeycode(ev->keycode);
}

static void HandleKeyPress(XEvent* e) {
	char data[64];
	int i, key, status;
	cc_codepoint cp;
	char* chars = data;
	Status status_type;
	
	key = TryGetKey(&e->xkey);
	if (key) Input_SetPressed(key);
	
#ifdef CC_BUILD_XIM
	if (win_xic) {
		status = Xutf8LookupString(win_xic, &e->xkey, data, Array_Elems(data), NULL, &status_type);
		while (status > 0) 
		{
			i = Convert_Utf8ToCodepoint(&cp, (cc_uint8*)chars, status);
			if (!i) break;

			Event_RaiseInt(&InputEvents.Press, cp);
			status -= i; chars += i;
		}
		return;
	}
#endif

	/* Treat the 8 bit characters as first 256 unicode codepoints */
	/* This only really works for latin keys (e.g. so some finnish keys still work) */
	status = XLookupString(&e->xkey, data, Array_Elems(data), NULL, NULL);
	for (i = 0; i < status; i++) 
	{
		Event_RaiseInt(&InputEvents.Press, (cc_uint8)data[i]);
	}
}

static void HandleKeyRelease(XEvent* e) {
	int key = TryGetKey(&e->xkey);
	if (key) Input_SetReleased(key);
}

static void HandleMappingNotify(XEvent* e) {
	if (e->xmapping.request == MappingModifier || e->xmapping.request == MappingKeyboard) {
		Platform_LogConst("keyboard mapping refreshed");
		XRefreshKeyboardMapping(&e->xmapping);
	}
}


/*########################################################################################################################*
*-------------------------------------------------Selection event handling------------------------------------------------*
*#########################################################################################################################*/
static Atom GetSelectionProperty(XEvent* e) {
	Atom prop = e->xselectionrequest.property;
	if (prop) return prop;

	/* For obsolete clients. See ICCCM spec, selections chapter for reasoning. */
	return e->xselectionrequest.target;
}

static void HandleSelectionNotify(XEvent* e) {
	Atom prop_type;
	int prop_format;
	unsigned long items, after;
	cc_uint8* data = NULL;
	Window win = Window_Main.Handle.val;
		
	if (e->xselection.selection != xa_clipboard) return;
	if (e->xselection.target != xa_utf8_string)  return; 
	if (e->xselection.property != xa_data_sel)   return;
	
	XGetWindowProperty(win_display, win, xa_data_sel, 0, 1024, false, 0,
		&prop_type, &prop_format, &items, &after, &data);
	XDeleteProperty(win_display, win, xa_data_sel);

	if (data && items && prop_type == xa_utf8_string) {
		clipboard_paste_received    = true;
		clipboard_paste_text.length = 0;
		String_AppendUtf8(&clipboard_paste_text, data, items);
	}
	if (data) XFree(data);
}

static void HandleSelectionRequest(XEvent* e) {
	XEvent reply = { 0 };
	reply.xselection.type       = SelectionNotify;
	reply.xselection.send_event = true;
	reply.xselection.display    = win_display;
	reply.xselection.requestor  = e->xselectionrequest.requestor;
	reply.xselection.selection  = e->xselectionrequest.selection;
	reply.xselection.target     = e->xselectionrequest.target;
	reply.xselection.property   = 0;
	reply.xselection.time       = e->xselectionrequest.time;

	if (e->xselectionrequest.selection == xa_clipboard && e->xselectionrequest.target == xa_utf8_string && clipboard_copy_text.length) {
		reply.xselection.property = GetSelectionProperty(e);
		char str[800];
		int len = String_EncodeUtf8(str, &clipboard_copy_text);

		XChangeProperty(win_display, reply.xselection.requestor, reply.xselection.property, xa_utf8_string, 8,
			PropModeReplace, (unsigned char*)str, len);
	} else if (e->xselectionrequest.selection == xa_clipboard && e->xselectionrequest.target == xa_targets) {
		reply.xselection.property = GetSelectionProperty(e);

		Atom data[2] = { xa_utf8_string, xa_targets };
		XChangeProperty(win_display, reply.xselection.requestor, reply.xselection.property, xa_atom, 32,
			PropModeReplace, (unsigned char*)data, 2);
	}
	XSendEvent(win_display, e->xselectionrequest.requestor, true, 0, &reply);
}


/*########################################################################################################################*
*------------------------------------------------------Event handling-----------------------------------------------------*
*#########################################################################################################################*/
static void HandleWMDestroy(void) {
	Platform_LogConst("Exit message received.");
	Event_RaiseVoid(&WindowEvents.Closing);
	Window_Main.Exists = false;
}

static void HandleWMPing(XEvent* e) {
	e->xany.window = win_rootWin;
	XSendEvent(win_display, win_rootWin, false,
		SubstructureRedirectMask | SubstructureNotifyMask, e);
}
static void HandleGenericEvent(XEvent* e);

static void HandleClientMessage(XEvent* e) {
	if (e->xclient.data.l[0] == wm_destroy) {
		HandleWMDestroy();
	} else if (e->xclient.data.l[0] == net_wm_ping) {
		HandleWMPing(e);
	}
}

static Bool FilterEvent(Display* d, XEvent* e, XPointer w) { 
	return
		e->xany.window == (Window)w ||
		!e->xany.window || /* KeymapNotify events don't have a window */
		e->type == GenericEvent; /* For XInput events */
}

void Window_ProcessEvents(float delta) {
	Window win = Window_Main.Handle.val;
	XEvent e;
	Window focus;
	int focusRevert;

	while (Window_Main.Exists) {
		if (!XCheckIfEvent(win_display, &e, FilterEvent, (XPointer)win)) break;
		if (XFilterEvent(&e, None) == True) continue;

		switch (e.type) {
		case GenericEvent:
			HandleGenericEvent(&e); break;
		case ClientMessage:
			HandleClientMessage(&e); break;

		case DestroyNotify:
			Platform_LogConst("Window destroyed");
			Window_Main.Exists = false;
			break;

		case ConfigureNotify:
			RefreshWindowBounds(e.xconfigure.width, e.xconfigure.height);
			break;

		case Expose:
			if (e.xexpose.count == 0) Event_RaiseVoid(&WindowEvents.RedrawNeeded);
			break;

		case LeaveNotify:
			XGetInputFocus(win_display, &focus, &focusRevert);
			if (focus == PointerRoot) {
				Window_Main.Focused = false; Event_RaiseVoid(&WindowEvents.FocusChanged);
			}
			break;

		case EnterNotify:
			XGetInputFocus(win_display, &focus, &focusRevert);
			if (focus == PointerRoot) {
				Window_Main.Focused = true; Event_RaiseVoid(&WindowEvents.FocusChanged);
			}
			break;

		case KeyPress:
			HandleKeyPress(&e); break;
		case KeyRelease:
			HandleKeyRelease(&e); break;
		case ButtonPress:
			HandleButtonPress(&e); break;
		case ButtonRelease:
			HandleButtonRelease(&e); break;

		case MotionNotify:
			Pointer_SetPosition(0, e.xmotion.x, e.xmotion.y);
			break;

		case FocusIn:
		case FocusOut:
			/* Don't lose focus when another app grabs key or mouse */
			if (e.xfocus.mode == NotifyGrab || e.xfocus.mode == NotifyUngrab) break;

			Window_Main.Focused = e.type == FocusIn;
			Event_RaiseVoid(&WindowEvents.FocusChanged);
			/* TODO: Keep track of keyboard when focus is lost */
			if (!Window_Main.Focused) Input_Clear();
			break;

		case MappingNotify:
			HandleMappingNotify(&e); break;

		case PropertyNotify:
			if (e.xproperty.atom == net_wm_state) {
				Event_RaiseVoid(&WindowEvents.StateChanged);
			}
			break;

		case SelectionNotify:
			HandleSelectionNotify(&e); break;
		case SelectionRequest:
			HandleSelectionRequest(&e); break;
		}
	}
}

void Gamepads_Init(void) {

}

void Gamepads_Process(float delta) { }

static void Cursor_GetRawPos(int* x, int* y) {
	Window rootW, childW;
	int childX, childY;
	unsigned int mask;
	XQueryPointer(win_display, win_rootWin, &rootW, &childW, x, y, &childX, &childY, &mask);
}

void Cursor_SetPosition(int x, int y) {
	Window win = Window_Main.Handle.val;
	XWarpPointer(win_display, None, win, 0, 0, 0, 0, x, y);
	XFlush(win_display); /* TODO: not sure if XFlush call is necessary */
}

static Cursor blankCursor;
static void Cursor_DoSetVisible(cc_bool visible) {
	Window win = Window_Main.Handle.val;
	if (visible) {
		XUndefineCursor(win_display, win);
	} else {
		if (!blankCursor) {
			char data  = 0;
			XColor col = { 0 };
			Pixmap pixmap = XCreateBitmapFromData(win_display, win, &data, 1, 1);
			blankCursor   = XCreatePixmapCursor(win_display, pixmap, pixmap, &col, &col, 0, 0);
			XFreePixmap(win_display, pixmap);
		}
		XDefineCursor(win_display, win, blankCursor);
	}
}


/*########################################################################################################################*
*-----------------------------------------------------X11 message box-----------------------------------------------------*
*#########################################################################################################################*/
struct X11MessageBox {
	Window win;
	Display* dpy;
	GC gc;
	unsigned long white, black, background;
	unsigned long btnBorder, highlight, shadow;
};

static unsigned long X11_Col(struct X11MessageBox* m, cc_uint8 r, cc_uint8 g, cc_uint8 b) {
	Colormap cmap = XDefaultColormap(m->dpy, DefaultScreen(m->dpy));
	XColor col = { 0 };
	col.red   = r << 8;
	col.green = g << 8;
	col.blue  = b << 8;
	col.flags = DoRed | DoGreen | DoBlue;

	XAllocColor(m->dpy, cmap, &col);
	return col.pixel;
}

static void X11MessageBox_Init(struct X11MessageBox* m) {
	m->black = BlackPixel(m->dpy, DefaultScreen(m->dpy));
	m->white = WhitePixel(m->dpy, DefaultScreen(m->dpy));
	m->background = X11_Col(m, 206, 206, 206);

	m->btnBorder = X11_Col(m, 60,  60,  60);
	m->highlight = X11_Col(m, 144, 144, 144);
	m->shadow    = X11_Col(m, 49,  49,  49);

	m->win = XCreateSimpleWindow(m->dpy, DefaultRootWindow(m->dpy), 0, 0, 100, 100,
								0, m->black, m->background);
	XSelectInput(m->dpy, m->win, ExposureMask   | StructureNotifyMask |
							   KeyReleaseMask   | PointerMotionMask |
								ButtonPressMask | ButtonReleaseMask );

	m->gc = XCreateGC(m->dpy, m->win, 0, NULL);
	XSetForeground(m->dpy, m->gc, m->black);
	XSetBackground(m->dpy, m->gc, m->background);
}

static void X11MessageBox_Free(struct X11MessageBox* m) {
	XFreeGC(m->dpy, m->gc);
	XDestroyWindow(m->dpy, m->win);
}

struct X11Textbox {
	int x, y, width, height;
	int lineHeight, descent;
	const char* text;
};

static void X11Textbox_Measure(struct X11Textbox* t, XFontStruct* font) {
	cc_string str = String_FromReadonly(t->text), line;
	XCharStruct overall;
	int direction, ascent, descent, lines = 0;

	for (; str.length; lines++) {
		String_UNSAFE_SplitBy(&str, '\n', &line);
		XTextExtents(font, line.buffer, line.length, &direction, &ascent, &descent, &overall);
		t->width = max(overall.width, t->width);
	}

	t->lineHeight = ascent + descent;
	t->descent    = descent;
	t->height     = t->lineHeight * lines;
}

static void X11Textbox_Draw(struct X11Textbox* t, struct X11MessageBox* m) {
	cc_string str = String_FromReadonly(t->text), line;
	int y = t->y + t->lineHeight - t->descent; /* TODO: is -descent even right? */

	for (; str.length; y += t->lineHeight) {
		String_UNSAFE_SplitBy(&str, '\n', &line);
		XDrawString(m->dpy, m->win, m->gc, t->x, y, line.buffer, line.length);		
	}
}

struct X11Button {
	int x, y, width, height;
	cc_bool clicked;
	struct X11Textbox text;
};

static void X11Button_Draw(struct X11Button* b, struct X11MessageBox* m) {
	struct X11Textbox* t;
	int begX, endX, begY, endY;

	XSetForeground(m->dpy, m->gc,  m->btnBorder);
	XDrawRectangle(m->dpy, m->win, m->gc, b->x, b->y,
					b->width, b->height);

	t = &b->text;
	begX = b->x + 1; endX = b->x + b->width  - 1;
	begY = b->y + 1; endY = b->y + b->height - 1;

	if (b->clicked) {
		XSetForeground(m->dpy, m->gc,  m->highlight);
		XDrawRectangle(m->dpy, m->win, m->gc, begX, begY,
						endX - begX, endY - begY);
	} else {
		XSetForeground(m->dpy, m->gc, m->white);
		XDrawLine(m->dpy, m->win, m->gc, begX, begY,
					endX - 1, begY);
		XDrawLine(m->dpy, m->win, m->gc, begX, begY,
					begX, endY - 1);

		XSetForeground(m->dpy, m->gc, m->highlight);
		XDrawLine(m->dpy, m->win, m->gc, begX + 1, endY - 1,
					endX - 1, endY - 1);
		XDrawLine(m->dpy, m->win, m->gc, endX - 1, begY + 1,
					endX - 1, endY - 1);

		XSetForeground(m->dpy, m->gc, m->shadow);
		XDrawLine(m->dpy, m->win, m->gc, begX, endY, endX, endY);
		XDrawLine(m->dpy, m->win, m->gc, endX, begY, endX, endY);
	}

	XSetForeground(m->dpy, m->gc, m->black);
	t->x = b->x + b->clicked + (b->width  - t->width)  / 2;
	t->y = b->y + b->clicked + (b->height - t->height) / 2;
	X11Textbox_Draw(t, m);
}

static int X11Button_Contains(struct X11Button* b, int x, int y) {
	return x >= b->x && x < (b->x + b->width) &&
		   y >= b->y && y < (b->y + b->height);
}

static Bool X11_FilterEvent(Display* d, XEvent* e, XPointer w) { return e->xany.window == (Window)w; }
static void X11_MessageBox(const char* title, const char* text, struct X11MessageBox* m) {
	struct X11Button ok    = { 0 };
	struct X11Textbox body = { 0 };
	Window win = Window_Main.Handle.val;

	Atom protocols[2];
	XFontStruct* font;
	int x, y, width, height;
	XSizeHints hints = { 0 };
	int mouseX = -1, mouseY = -1, over;
	XEvent e;

	X11MessageBox_Init(m);
	XMapWindow(m->dpy, m->win);
	XStoreName(m->dpy, m->win, title);

	protocols[0] = XInternAtom(m->dpy, "WM_DELETE_WINDOW", false);
	protocols[1] = XInternAtom(m->dpy, "_NET_WM_PING",     false);
	XSetWMProtocols(m->dpy, m->win, protocols, 2);

	font = XQueryFont(m->dpy, XGContextFromGC(m->gc));
	if (!font) return;

	/* Compute size of widgets */
	body.text = text;
	X11Textbox_Measure(&body, font);
	ok.text.text = "OK";
	X11Textbox_Measure(&ok.text, font);
	ok.width  = ok.text.width  + 70;
	ok.height = ok.text.height + 10;

	/* Compute size and position of window */
	width  = body.width                   + 20;
	height = body.height + 20 + ok.height + 20;
	x = DisplayWidth (m->dpy, DefaultScreen(m->dpy))/2 -  width/2;
	y = DisplayHeight(m->dpy, DefaultScreen(m->dpy))/2 - height/2;
	XMoveResizeWindow(m->dpy, m->win, x, y, width, height);

	/* Adjust bounds of widgets */
	body.x = 10; body.y = 10;
	ok.x = width/2 - ok.width/2;
	ok.y = height  - ok.height - 10;

	/* This marks the window as popup window of the main window */
	/* http://tronche.com/gui/x/icccm/sec-4.html#WM_TRANSIENT_FOR */
	/* Depending on WM, removes minimise and doesn't show in taskbar */
	if (win) XSetTransientForHint(m->dpy, m->win, win);

	XFreeFontInfo(NULL, font, 1);
	XUnmapWindow(m->dpy, m->win); /* Make window non resizeable */

	hints.flags      = PSize | PMinSize | PMaxSize;
	hints.min_width  = hints.max_width  = hints.base_width  = width;
	hints.min_height = hints.max_height = hints.base_height = height;

	XSetWMNormalHints(m->dpy, m->win, &hints);
	XMapRaised(m->dpy, m->win);
	XFlush(m->dpy);

    for (;;) {
		/* The naive solution is to use XNextEvent(m->dpy, &e) here. */
		/* However this causes issues as that removes events that */
		/* should have been delivered to the main game window. */
		/* (e.g. breaks initial window resize with i3 WM) */
		XIfEvent(m->dpy, &e, X11_FilterEvent, (XPointer)m->win);

		switch (e.type)
		{
		case ButtonPress:
		case ButtonRelease:
			if (e.xbutton.button != Button1) break;
			over = X11Button_Contains(&ok, mouseX, mouseY);

			if (ok.clicked && e.type == ButtonRelease) {
				if (over) return;
			}
			ok.clicked = e.type == ButtonPress && over;
			/* fallthrough to redraw window */

		case Expose:
		case MapNotify:
			XClearWindow(m->dpy, m->win);
			X11Textbox_Draw(&body, m);
			X11Button_Draw(&ok, m);
			XFlush(m->dpy);
			break;

		case KeyRelease:
			if (XLookupKeysym(&e.xkey, 0) == XK_Escape) return;
			break;

		case ClientMessage:
			/* { WM_DELETE_WINDOW, _NET_WM_PING } */
			if (e.xclient.data.l[0] == protocols[0]) return;
			if (e.xclient.data.l[0] == protocols[1]) HandleWMPing(&e);
			break;

		case MotionNotify:
			mouseX = e.xmotion.x; mouseY = e.xmotion.y;
			break;
		}
	}
}

static void ShowDialogCore(const char* title, const char* msg) {
	struct X11MessageBox m = { 0 };
	m.dpy = win_display;
	
	/* Failing to create a display means can't display a message box. */
	/* However the user might have launched the game through terminal, */
	/* so fallback to console instead of just dying from a segfault */
	if (!m.dpy) {
		Platform_LogConst("### MESSAGE ###");
		Platform_LogConst(title);
		Platform_LogConst(msg);
		return;
	}
	
	X11_MessageBox(title, msg, &m);
	X11MessageBox_Free(&m);
	XFlush(m.dpy); /* flush so window disappears immediately */
}

static cc_result OpenSaveFileDialog(const char* args, FileDialogCallback callback, const char* defaultExt) {
	cc_string path; char pathBuffer[1024];
	char result[4096] = { 0 };
	int len;
	/* TODO this doesn't detect when Zenity doesn't exist */
	FILE* fp = popen(args, "r");
	if (!fp) return 0;

	/* result from zenity is normally just one string */
	while (fgets(result, sizeof(result), fp)) { }
	pclose(fp);

	len = String_Length(result);
	if (!len) return 0;

	String_InitArray(path, pathBuffer);
	String_AppendUtf8(&path, result, len);

	/* Add default file extension if necessary */
	if (defaultExt) {
		cc_string file = path;
		Utils_UNSAFE_GetFilename(&file);
		if (String_IndexOf(&file, '.') == -1) String_AppendConst(&path, defaultExt);
	}
	callback(&path);
	return 0;
}

cc_result Window_OpenFileDialog(const struct OpenFileDialogArgs* args) {
	const char* const* filters = args->filters;
	cc_string path; char pathBuffer[1024];
	int i;

	String_InitArray_NT(path, pathBuffer);
	String_Format1(&path, "zenity --file-selection --file-filter='%c (", args->description);

	for (i = 0; filters[i]; i++)
	{
		if (i) String_Append(&path, ',');
		String_Format1(&path, "*%c", filters[i]);
	}
	String_AppendConst(&path, ") |");

	for (i = 0; filters[i]; i++)
	{
		String_Format1(&path, " *%c", filters[i]);
	}
	String_AppendConst(&path, "'");

	path.buffer[path.length] = '\0';
	return OpenSaveFileDialog(path.buffer, args->Callback, NULL);
}

cc_result Window_SaveFileDialog(const struct SaveFileDialogArgs* args) {
	const char* const* titles   = args->titles;
	const char* const* fileExts = args->filters;
	cc_string path; char pathBuffer[1024];
	int i;

	String_InitArray_NT(path, pathBuffer);
	String_AppendConst(&path, "zenity --file-selection");
	for (i = 0; fileExts[i]; i++)
	{
		String_Format3(&path, " --file-filter='%c (*%c) | *%c'", titles[i], fileExts[i], fileExts[i]);
	}
	String_AppendConst(&path, " --save --confirm-overwrite");

	/* TODO: Utf8 encode filename */
	if (args->defaultName.length) {
		String_Format1(&path, " --filename='%s'", &args->defaultName);
	}

	path.buffer[path.length] = '\0';
	return OpenSaveFileDialog(path.buffer, args->Callback, fileExts[0]);
}

static GC fb_gc;
static XImage* fb_image;
static void* fb_data;
static int fb_fast, fb_depth;

void Window_AllocFramebuffer(struct Bitmap* bmp, int width, int height) {
	Window win = Window_Main.Handle.val;
	XWindowAttributes attribs = { 0 };

	if (!fb_gc) fb_gc = XCreateGC(win_display, win, 0, NULL);
	XGetWindowAttributes(win_display, win, &attribs);
	fb_depth = attribs.depth;

	bmp->scan0  = (BitmapCol*)Mem_Alloc(width * height, BITMAPCOLOR_SIZE, "window pixels");
	bmp->width  = width;
	bmp->height = height;

	/* X11 requires that the image to draw has same depth as window */
	/* Easy for 24/32 bit case, but much trickier with other depths */
	/*  (have to do a manual and slow second blit for other depths) */
	fb_fast = attribs.depth == 24 || attribs.depth == 32;
	fb_data = fb_fast ? bmp->scan0 : Mem_Alloc(width * height, BITMAPCOLOR_SIZE, "window blit");

	fb_image = XCreateImage(win_display, attribs.visual,
		attribs.depth, ZPixmap, 0, (char*)fb_data,
		width, height, 32, 0);
}

static void BlitFramebuffer(int x1, int y1, int width, int height, struct Bitmap* bmp) {
	unsigned char* dst;
	BitmapCol* row;
	BitmapCol src;
	cc_uint32 pixel;
	int R, G, B;
	int x, y, depth = fb_depth;

	for (y = y1; y < y1 + height; y++) {
		row = Bitmap_GetRow(bmp, y);
		dst = ((unsigned char*)fb_image->data) + y * fb_image->bytes_per_line;

		for (x = x1; x < x1 + width; x++) {
			src = row[x];
			R = BitmapCol_R(src);
			G = BitmapCol_G(src);
			B = BitmapCol_B(src);

			switch (depth)
			{
			case 30: /* R10 G10 B10 A2 */
				pixel = (R << 2) | ((G << 2) << 10) | ((B << 2) << 20) | (0x03 << 30);
				((cc_uint32*)dst)[x] = pixel;
				break;
			case 16: /* B5 G6 R5 */
				pixel = (B >> 3) | ((G >> 2) <<  5) | ((R >> 3) << 11);
				((cc_uint16*)dst)[x] = pixel;
				break;
			case 15: /* B5 G5 R5 */
				pixel = (B >> 3) | ((G >> 3) <<  5) | ((R >> 3) << 10);
				((cc_uint16*)dst)[x] = pixel;
				break;
			case 8:  /* B2 G3 R3 */
				pixel = (B >> 6) | ((G >> 5) <<  2) | ((R >> 5) <<  5);
				((cc_uint8*) dst)[x] = pixel;
				break;
			}
		}
	}
}

void Window_DrawFramebuffer(Rect2D r, struct Bitmap* bmp) {
	Window win = Window_Main.Handle.val;
	/* Convert 32 bit depth to window depth when required */
	if (!fb_fast) BlitFramebuffer(r.x, r.y, r.width, r.height, bmp);

	XPutImage(win_display, win, fb_gc, fb_image,
		r.x, r.y, r.x, r.y, r.width, r.height);
}

void Window_FreeFramebuffer(struct Bitmap* bmp) {
	XFree(fb_image);
	Mem_Free(bmp->scan0);
	if (bmp->scan0 != fb_data) Mem_Free(fb_data);
}

void OnscreenKeyboard_Open(struct OpenKeyboardArgs* args) { }
void OnscreenKeyboard_SetText(const cc_string* text) { }
void OnscreenKeyboard_Close(void) { }

static cc_bool rawMouseInited, rawMouseSupported;
static int xiOpcode;

#ifdef CC_BUILD_XINPUT2
static void CheckMovementDelta(double dx, double dy) {
	/* Despite the assumption that XI_RawMotion is relative,     */
	/*  unfortunately there's a few buggy corner cases out there */
	/*  where absolute coordinates are provided instead.         */
	/* The ugly code belows tries to detect these corner cases,  */
	/*  and disables XInput2 when that happens                   */
	static int valid, fails;

	if (valid) return;
	/* The default window resolution is 854 x 480, so if there's */
	/*  a delta less than half of that, then it's almost certain */
	/*  that the provided coordinates are relative.*/
	if (dx < 300 || dy < 200) { valid = true; return; }

	if (fails++ < 20) return;
	/* Checked over 20 times now, but no relative coordinates,   */
	/*  so give up trying to use XInput2 anymore.                */
	Platform_LogConst("Buggy XInput2 detected, disabling it.."); 
	rawMouseSupported = false;
}

static void HandleGenericEvent(XEvent* e) {
	const double* values;
	XIRawEvent* ev;
	double dx, dy;

	if (!rawMouseSupported || e->xcookie.extension != xiOpcode) return;
	if (!XGetEventData(win_display, &e->xcookie)) return;

	if (e->xcookie.evtype == XI_RawMotion && Input.RawMode) {
		ev     = (XIRawEvent*)e->xcookie.data;
		values = ev->raw_values;

		/* Raw motion events may not always have values for both axes */
		dx = XIMaskIsSet(ev->valuators.mask, 0) ? *values++ : 0;
		dy = XIMaskIsSet(ev->valuators.mask, 1) ? *values++ : 0;

		CheckMovementDelta(dx, dy);
		/* Using 0.5f here makes the sensitivity about same as normal cursor movement */
		Event_RaiseRawMove(&PointerEvents.RawMoved, dx * 0.5f, dy * 0.5f);
	}
	XFreeEventData(win_display, &e->xcookie);
}

static void InitRawMouse(void) {
	XIEventMask evmask;
	unsigned char masks[XIMaskLen(XI_LASTEVENT)] = { 0 };
	int ev, err, major, minor;

	if (!Options_GetBool(OPT_RAW_INPUT, true)) {
		Platform_LogConst("XInput disabled");
		return;
	}

	if (!XQueryExtension(win_display, "XInputExtension", &xiOpcode, &ev, &err)) {
		Platform_LogConst("XInput unsupported");
		return;
	}

	/* Only XInput 2.0 is actually required. However, 2.0 has the annoying */
	/* behaviour where raw input is NOT delivered while pointer is grabbed. */
	/* (i.e. if you press mouse button, no more raw mouse movement events) */
	/* http://wine.1045685.n8.nabble.com/PATCH-0-9-Implement-DInput8-mouse-using-RawInput-and-XInput2-RawEvents-only-td6016923.html */
	/* Thankfully XInput >= 2.1 corrects this behaviour */
	/* http://who-t.blogspot.com/2011/09/whats-new-in-xi-21-raw-events.html */
	major = 2; minor = 2;
	if (XIQueryVersion(win_display, &major, &minor) != Success) {
		Platform_Log2("Only XInput %i.%i supported", &major, &minor);
		return;
	}

	/* Sometimes XIQueryVersion will report Success, even though the */
	/* supported version is only 2.0! So make sure to handle that. */
	if (major < 2 || minor < 2) {
		Platform_Log2("Only XInput %i.%i supported", &major, &minor);
		return;
	}

	XISetMask(masks, XI_RawMotion);
	evmask.deviceid = XIAllMasterDevices;
	evmask.mask_len = sizeof(masks);
	evmask.mask     = masks;

	XISelectEvents(win_display, win_rootWin, &evmask, 1);
	rawMouseSupported = true;
}
#else
static void HandleGenericEvent(XEvent* e) { }
static void InitRawMouse(void) { }
#endif

void Window_EnableRawMouse(void) {
	Window win = Window_Main.Handle.val;
	DefaultEnableRawMouse();
	if (!rawMouseInited) InitRawMouse();
	rawMouseInited = true;

	if (!grabCursor) return;
	XGrabPointer(win_display, win, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
		GrabModeAsync, GrabModeAsync, win, blankCursor, CurrentTime);
}

void Window_UpdateRawMouse(void) {
	if (rawMouseSupported) {
		/* Handled by XI_RawMotion generic event */
		CentreMousePosition();
	} else {
		DefaultUpdateRawMouse();
	}
}

void Window_DisableRawMouse(void) {
	DefaultDisableRawMouse();
	if (!grabCursor) return;
	XUngrabPointer(win_display, CurrentTime);
}


/*########################################################################################################################*
*-------------------------------------------------------glX OpenGL--------------------------------------------------------*
*#########################################################################################################################*/
#if CC_GFX_BACKEND_IS_GL() && defined CC_BUILD_EGL
static XVisualInfo GLContext_SelectVisual(void) {
	XVisualInfo info = Select2DVisual();
	ctx_visualID = info.visual ? info.visual->visualid : 0;
	return info;
}
#endif

#if CC_GFX_BACKEND_IS_GL() && !defined CC_BUILD_EGL
/* #include <GL/glx.h> */
#include "../misc/x11/min-glx.h"

static GLXContext ctx_handle;
typedef int  (*FP_SWAPINTERVAL)(int interval);
typedef Bool (*FP_QUERYRENDERER)(int attribute, unsigned int* value);
static FP_SWAPINTERVAL swapIntervalMESA, swapIntervalSGI;
static FP_QUERYRENDERER queryRendererMESA;

void GLContext_Create(void) {
	static const cc_string vsync_mesa = String_FromConst("GLX_MESA_swap_control");
	static const cc_string vsync_sgi  = String_FromConst("GLX_SGI_swap_control");
	static const cc_string info_mesa  = String_FromConst("GLX_MESA_query_renderer");
	Window win = Window_Main.Handle.val;

	const char* raw_exts;
	cc_string exts;
	ctx_handle = glXCreateContext(win_display, &win_visual, NULL, true);

	if (!ctx_handle) {
		Platform_LogConst("Context create failed. Trying indirect...");
		ctx_handle = glXCreateContext(win_display, &win_visual, NULL, false);
	}
	if (!ctx_handle) Process_Abort("Failed to create OpenGL context");

	if (!glXIsDirect(win_display, ctx_handle)) {
		Platform_LogConst("== WARNING: Context is not direct ==");
	}
	if (!glXMakeCurrent(win_display, win, ctx_handle)) {
		Process_Abort("Failed to make OpenGL context current.");
	}

	/* GLX may return non-null function pointers that don't actually work */
	/* So we need to manually check the extensions string for support */
	raw_exts = glXQueryExtensionsString(win_display, DefaultScreen(win_display));
	exts = String_FromReadonly(raw_exts);

	if (String_CaselessContains(&exts, &vsync_mesa)) {
		swapIntervalMESA  = (FP_SWAPINTERVAL)GLContext_GetAddress("glXSwapIntervalMESA");
	}
	if (String_CaselessContains(&exts, &vsync_sgi)) {
		swapIntervalSGI   = (FP_SWAPINTERVAL)GLContext_GetAddress("glXSwapIntervalSGI");
	}
	if (String_CaselessContains(&exts, &info_mesa)) {
		queryRendererMESA = (FP_QUERYRENDERER)GLContext_GetAddress("glXQueryCurrentRendererIntegerMESA");
	}
}

void GLContext_Update(void) { }
cc_bool GLContext_TryRestore(void) { return true; }
void GLContext_Free(void) {
	if (!ctx_handle) return;
	glXMakeCurrent(win_display, None, NULL);
	glXDestroyContext(win_display, ctx_handle);
	ctx_handle = NULL;
}

void* GLContext_GetAddress(const char* function) {
	return (void*)glXGetProcAddress(function);
}

cc_bool GLContext_SwapBuffers(void) {
	Window win = Window_Main.Handle.val;
	glXSwapBuffers(win_display, win);
	return true;
}

void GLContext_SetVSync(cc_bool vsync) {
	int res = 0;
	if (swapIntervalMESA) {
		res = swapIntervalMESA(vsync);
	} else if (swapIntervalSGI) {
		res = swapIntervalSGI(vsync);
	}
	if (res) Platform_Log1("Set VSync failed, error: %i", &res);
}

void GLContext_GetApiInfo(cc_string* info) {
	unsigned int vram, acc;
	if (!queryRendererMESA) return;

	queryRendererMESA(0x8186, &acc);  /* GLX_RENDERER_ACCELERATED_MESA */
	queryRendererMESA(0x8187, &vram); /* GLX_RENDERER_VIDEO_MEMORY_MESA */
	String_Format2(info, "VRAM: %i MB, %c", &vram,
		acc ? "HW accelerated" : "no HW acceleration");
}

static void GetAttribs(struct GraphicsMode* mode, int* attribs, int fbCfg, int depth) {
	int i = 0;
	/* See http://www-01.ibm.com/support/knowledgecenter/ssw_aix_61/com.ibm.aix.opengl/doc/openglrf/glXChooseFBConfig.htm%23glxchoosefbconfig */
	/* See http://www-01.ibm.com/support/knowledgecenter/ssw_aix_71/com.ibm.aix.opengl/doc/openglrf/glXChooseVisual.htm%23b5c84be452rree */
	/* for the attribute declarations. Note that the attributes are different than those used in glxChooseVisual */

	/* TODO always use RGBA? need to test 8bpp displays */
	if (DisplayInfo.Depth >= 15) { attribs[i++] = GLX_RGBA; }
	attribs[i++] = GLX_RED_SIZE;   attribs[i++] = mode->R;
	attribs[i++] = GLX_GREEN_SIZE; attribs[i++] = mode->G;
	attribs[i++] = GLX_BLUE_SIZE;  attribs[i++] = mode->B;
	attribs[i++] = GLX_ALPHA_SIZE; attribs[i++] = mode->A;
	attribs[i++] = GLX_DEPTH_SIZE; attribs[i++] = depth;

	attribs[i++] = GLX_DOUBLEBUFFER;
	attribs[i++] = fbCfg ? True : 0;
	attribs[i++] = 0;
}

static XVisualInfo GLContext_SelectVisual(void) {
	int attribs[20];
	int major, minor;
	XVisualInfo* visual = NULL;

	int fbcount, screen;
	GLXFBConfig* fbconfigs;
	XVisualInfo info;
	struct GraphicsMode mode;

	InitGraphicsMode(&mode);
	GetAttribs(&mode, attribs, true, GLCONTEXT_DEFAULT_DEPTH);
	screen = DefaultScreen(win_display);

	if (!glXQueryVersion(win_display, &major, &minor)) {
		Platform_LogConst("glXQueryVersion failed");
	} else if (major >= 1 && minor >= 3) {
		/* ChooseFBConfig returns an array of GLXFBConfig opaque structures */
		fbconfigs = glXChooseFBConfig(win_display, screen, attribs, &fbcount);
		if (fbconfigs && fbcount) {
			/* Use the first GLXFBConfig from the fbconfigs array (best match) */
			visual = glXGetVisualFromFBConfig(win_display, *fbconfigs);
			XFree(fbconfigs);
		}
	}

	if (!visual) {
		Platform_LogConst("Falling back to glXChooseVisual.");
		GetAttribs(&mode, attribs, false, GLCONTEXT_DEFAULT_DEPTH);
		visual = glXChooseVisual(win_display, screen, attribs);
	}
	/* Some really old devices will only supply 16 bit depths */
	if (!visual) {
		GetAttribs(&mode, attribs, false, 16);
		visual = glXChooseVisual(win_display, screen, attribs);
	}
	if (!visual) Process_Abort("Can't find an OpenGL 3D GLX visual");

	info = *visual;
	XFree(visual);
	return info;
}
#endif
#endif
