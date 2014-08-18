// AppClass.h
//
// In this header file we only DECLARE the AppClass class, we don't DEFINE it.
// The definition of the class takes place in the file AppClass.cpp


// preprocessor directives to make sure that we haven't included this stuff already
#ifndef APPCLASS_H
#define APPCLASS_H

#include "TURTLE.H"

class GDITurtle : public Turtle
{
public:
   GDITurtle();
   ~GDITurtle();

   void setDC(HDC hdc);
   void lineTo(double x, double y);
   void moveTo(double x, double y);

protected:
   HDC hdc;
};

class AppClass
{
public:
   AppClass();           // constructor
   ~AppClass();          // destructor might be needed too

   void Init(HWND hwnd); // method for setting up the simulation -- need window handle
   void Done();          // method to perform cleanup when quitting
   void onPaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
                        // method to handle WM_PAINT messages
   void onChar(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
                        // method to handle WM_CHAR messages
   void onHelp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
                        // method to handle WM_HELP messages
   void doCommand();
   void paintCommand(HDC hdc);
   void paintTitle(HDC hdc);
   void loadPreset();
   void showHelp(HDC hdc);

protected:
   // declare private data and methods here
    
   HWND hwnd;          // this will hold the handle to our application window
   HPEN white_pen, black_pen;
   HBRUSH black_brush;

   Dcfg grammar;
   GDITurtle gdiTurtle;

   int data;
   char cmd_buffer[1024];
   bool commandMode;

   int max_depth;

   char **preset_title;
   char ***preset_cmd;
   int nPresets;
   int curPreset;
   bool doPaintTitle;

   bool doShowHelp;
};

// that's it!

#endif
