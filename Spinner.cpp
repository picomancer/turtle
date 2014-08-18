// Spinner.cpp
//
// General framework for a simulation

// DEFINES
#define WIN32_LEAN_AND_MEAN

// INCLUDES
#include <windows.h>
#include "AppClass.h"           // include the application class

// THE ONLY GLOBAL VARIABLE
AppClass MyApp;

// * declare, but not define, Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/* Here's where we start */
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int iCmdShow)

{
    static char szClassName[ ] = "Typical Window";
    static char szAppTitle[ ] = "Ben Johnson / PHYS 219";

    HWND hwnd;               /* This is the handle for our window */
    MSG msg;                 /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;        /* This is the function is called by windows */
    wincl.style = CS_HREDRAW | CS_VREDRAW ;     /* send WM_PAINT if resized at all*/
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                  /* No menu */
    wincl.cbClsExtra = 0;                       /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                       /* structure or the window instance */
    /* Use black brush to paint the background window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the window*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           szAppTitle,          /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           0,                   /* horizontal window position */
           0,                   /* vertical window position */
           400,                 /* The initial window width -- client + nonclient */
           400,                 /* The initial window height -- client + nonclient */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    // Now initialize our application
    MyApp.Init(hwnd);
    
    /* Make the window visible on the screen */
    ShowWindow (hwnd, iCmdShow);
    /* force validation of client area */
    UpdateWindow(hwnd);
    
    /* Here we use PeekMessage to see if there are any messages */
    /* if not, then we want to do one step in our simulation */
    
    /* infinite loop until we call break command */
    
    /*
    for( ; ; )
    {
        // look for a meassge
        if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
        {
            // there is a message
            // check that we are not quitting
            if(msg.message == WM_QUIT)
                break;
            // Ok, we're not quitting, so process message

            // Translate virtual-key messages into character messages
            TranslateMessage(&msg);
            // Send message to WindowProcedure
            DispatchMessage(&msg);
        }
        
        // There was no message, so let's do one step in our simulation
        MyApp.Iterate();
    }
    */

   while(GetMessage(&msg, NULL, 0, 0))
   {
      // Translate virtual-key messages into character messages
      TranslateMessage(&msg);
      // Send message to WindowProcedure
      DispatchMessage(&msg);
   }

   MyApp.Done();        // do clean-up like freeing memory, etc.

   // grr...someone needs to set their tab stops to 3 not 4

   /* The program return-value is 0 - The value that PostQuitMessage() gave */
   return msg.wParam;
}


/*  This function is called indirectly by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:                /* initialize the data for our model */
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);        /* send a WM_QUIT to the message queue */
            return 0;

        case WM_CHAR:               /* allow us to quit by pressing escape */
            MyApp.onChar(hwnd, message, wParam, lParam);
            return 0;

        case WM_PAINT:
           MyApp.onPaint(hwnd, message, wParam, lParam);
           return 0;

        case WM_HELP:
           MyApp.onHelp(hwnd, message, wParam, lParam);
           return 0;
            
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
