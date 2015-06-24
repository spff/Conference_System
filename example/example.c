#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XInput2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Error 1

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
  * generate a new mouse pointer
  * @param  name  given a mouse pointer name
  * @return On success, generated mouse pointer's device id
  *         On failure, return -1
  */
int  GeneratePointer(char *name){

    XIAddMasterInfo c;
    XIDeviceInfo *info;
    int ndevices, deviceid, i;
    const int MaxNameLen = 1024;
    char devicename[MaxNameLen];
    

    deviceid = -1;
    c.type = XIAddMaster;
    c.name = name;
    c.send_core = 1;
    c.enable = 1;

    Status s = XIChangeHierarchy(dpy, (XIAnyHierarchyChangeInfo*)&c, 1);

    if (!s)
    {
      // get device id
      info = XIQueryDevice(dpy, XIAllDevices, &ndevices);
      strncpy(devicename, name, MaxNameLen);
      strncat(devicename, " pointer", MaxNameLen);
      for(i = 0; i < ndevices; i++)
      {
	  // printf("device: %s\t id: %d\n", info[i].name, info[i].deviceid);
	  if (!strncmp(info[i].name, devicename, MaxNameLen))
	  {
	      deviceid = info[i].deviceid;
	      break;
	  }
      }
    }

    return deviceid;
}

/**
  * Remove a master device.
  * By default, all attached devices are set to Floating, unless parameters are
  * given.
  *
  * @param deviceid given a device id
  * @return On success, zero
  *         On failure, non-zero
  */
int RemovePointer(int deviceid){
  XIRemoveMasterInfo r;
  int ret;
  r.type = XIRemoveMaster;
  r.deviceid = deviceid;
  r.return_mode = XIFloating;
  ret = XIChangeHierarchy(dpy, (XIAnyHierarchyChangeInfo*)&r, 1);
  XFlush(dpy);
  return ret;
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

/* New */
void MoveMousePointer(int deviceid, int x, int y)
{
  XIWarpPointer(dpy, deviceid, None, window, 0, 0, 0, 0, x, y);
  XFlush(dpy);
}


int main(int argc, char *argv[])
{
	int x=0, y=0, deviceid;
	const int MaxNameLen = 1024;
	char name[MaxNameLen];
	
	strncpy(name, "pointer1", MaxNameLen);
	
	init();
	
	// generate pointer 
	deviceid = GeneratePointer(name);
	if (deviceid != -1)
	{
	  printf("Generate a new pointer %s, id is %d\n", name, deviceid);
	}
	
	while(True)
	{
		usleep(1000);
		
		// move pointer
		MoveMousePointer(deviceid, x+=1, y+=1);
		
		if (y > 600)
		{
		  if (RemovePointer(deviceid) == 0)
		  {
		    printf("remove pointer success!\n");
		  }
		  else
		  {
		    printf("remove pointer failure!\n");
		  }
		  break;
		}
	}
	return 0;
}
