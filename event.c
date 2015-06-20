#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Display *dpy;
Window window;

Bool init()
{
  // connect to X server
  // arg: NULL, it defaults to the value of the DISPLAY environment variable.
  if ((dpy=XOpenDisplay(NULL)) == NULL){  
    fprintf(stderr, "fail to open display!\n");
    return False;
  }
  
  //returns the root window for the default screen.
  window = DefaultRootWindow(dpy);  

  return True;
}



/**
 * @param which: which button 
 * 	1 - "left",
 * 	2 - "middle",
 * 	3 - "right",
 * 	4 - "fourth",
 * 	5 - "fifth"
 * @param what: what event is
 * 	ButtonPress
 * 	ButtonRelease
 */
Bool GenerateMouseEvent(int which, int what)
{
  return XTestFakeButtonEvent(dpy, which, what==ButtonPress, CurrentTime);
}

/*****************************************************************************
Bool GenerateMouseEvent(int which, int what)
{
  XEvent event;
  memset(&event, 0x00, sizeof(event));
  
  event.type 			= what;
  event.xbutton.button 		= which;
  // the event window is on the same screen as the root window
  event.xbutton.same_screen 	= True;
  
  //get pointer coordinates - assign to event
  XQueryPointer(dpy, 
		RootWindow(dpy, DefaultScreen(dpy)),
		&event.xbutton.root,
		&event.xbutton.window,
		&event.xbutton.x_root,
		&event.xbutton.y_root,
		&event.xbutton.x,
		&event.xbutton.y,
		&event.xbutton.state	// state: button or key mask
 	      );
  event.xbutton.subwindow = event.xbutton.window; // child window
  
  while (event.xbutton.subwindow)
  {
    event.xbutton.window = event.xbutton.subwindow;
    XQueryPointer(dpy,
		  event.xbutton.window,
		  &event.xbutton.root,
		  &event.xbutton.subwindow,
		  &event.xbutton.x_root,
		  &event.xbutton.y_root,
		  &event.xbutton.x,
		  &event.xbutton.y,
		  &event.xbutton.state
    );
  }

  int rc = XSendEvent(dpy,  
		      PointerWindow, // he destination window is the window that contains the pointer.
		      True, 	     // propagate
		      0xfff,	     // event mask
		      &event
 		    );
  
  XFlush(dpy);
  
  return rc;
}
******************************************************************************/


void MoveMousePointer(int x, int y)
{
  XWarpPointer(dpy, None, window, 0, 0, 0, 0, x, y);
  XFlush(dpy);
}

/**
 * @param which: which key 
 * @param what: what event is
 * 	KeyPress
 * 	KeyRelease
 */
Bool GenerateKeyEvent(unsigned int which, int what)
{
  return XTestFakeKeyEvent(dpy, which, what==KeyPress, CurrentTime);
}
