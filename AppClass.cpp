// AppClass.cpp
//
// Here is where we define the members of AppClass

#include <windows.h>
#include <cmath>
#include <stdio.h>
#include "AppClass.h"

#define PI 3.14159265358979

/***********************
 *                     *
 *   A P P C L A S S   *
 *                     *
 ***********************/

AppClass::AppClass()
{
   static const int axiom[2] = {1, 'F'};
   static const int rule[9] = {8, 'F', '-', 'F', '+', '+', 'F', '-', 'F'};
   int *axiom_mem, *rule_mem;

   axiom_mem = (int *) malloc((axiom[1]+1)*sizeof(int));
    rule_mem = (int *) malloc(( rule[1]+1)*sizeof(int));

   memcpy(axiom_mem, axiom, (axiom[1]+1)*sizeof(int));
   memcpy( rule_mem,  rule, ( rule[1]+1)*sizeof(int));

   grammar.setAxiom(axiom_mem);
   grammar.addRule('F', rule_mem);
   commandMode = false;

   max_depth = 7;

   return;
}

AppClass::~AppClass()
{
   return;
}

void AppClass::Init(HWND hwnd)
{
   char buffer[1024];
   int i, j, m, n;
   char *temp;

   // initialization code
   // store the handle to our application's window
   this->hwnd = hwnd;

   // initialize simulation data
   double angle = 0.0;

   // Create the pens we will use for drawing
   // these pens are created here and are later destroyed in method Done()
   white_pen = CreatePen(PS_SOLID, 1, 0xFFFFFF);
   black_pen = CreatePen(PS_SOLID, 1, 0x000000);
   black_brush = CreateSolidBrush(0x000000);

   nPresets = 0;
   curPreset = 0;
   doPaintTitle = false;

   // Read in stuff from the configuration file, if any
   FILE *file = fopen("Turtle.cfg", "rt");
   if (file == NULL)
      return;
   m = 0;
   n = 0;
   preset_title = (char **) malloc(0);
   preset_cmd = (char ***) malloc(0);
   while(1)
   {
      // Read a line from the config file
      if (fgets(buffer, 1024, file) == NULL)
         break;
      // Strip off whitespace (if any)
      for(i=0,j=0;i<1024;i++)
      {
         if (buffer[i] > ' ')
            buffer[j++] = buffer[i];
         else if (buffer[i] == 0)
            break;
      }
      buffer[j] = 0;
      // If empty then nothing
      if (j == 0)
         continue;
      // Is it a title?
      if (strstr(buffer, "title=") == buffer)
      {
         // Add it to the title
         temp = (char *) malloc((j+1)*sizeof(char));
         strcpy(temp, buffer+6);
         if (m > 0)
         {
            preset_cmd[m-1] = (char **) realloc(preset_cmd[m-1], (n+1)*sizeof(char *));
            preset_cmd[m-1][n] = NULL;
         }
         preset_title = (char **) realloc(preset_title, (m+1)*sizeof(char *));
         preset_title[m] = temp;
         preset_cmd = (char ***) realloc(preset_cmd, (m+1)*sizeof(char *));
         preset_cmd[m] = (char **) malloc(0);
         m++;
         n = 0;
      }
      else
      {
         if (m == 0)
            continue;
         temp = (char *) malloc((j+1)*sizeof(char));
         strcpy(temp, buffer);
         preset_cmd[m-1] = (char **) realloc(preset_cmd[m-1], (n+1)*sizeof(char *));
         preset_cmd[m-1][n] = temp;
         n++;
      }
   }
   if (m > 0)
   {
      preset_cmd[m-1] = (char **) realloc(preset_cmd[m-1], (n+1)*sizeof(char *));
      preset_cmd[m-1][n] = NULL;
   }

   fclose(file);

   nPresets = m;
   loadPreset();

   return;
}

void AppClass::Done()
{
    // this method cleans everything up nicely
    
    DeleteObject(white_pen);
    DeleteObject(black_pen);
    DeleteObject(black_brush);
}

// other member functions would be listed here as well

void AppClass::onPaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   HDC hdc;
   PAINTSTRUCT ps;
   RECT rect;
   HPEN old_pen;
   double sx, sy, tx, ty;

   GetClientRect(hwnd, &rect);
   sx = (rect.right -rect.left)*0.75;
   sy = (rect.bottom-rect.top )*0.75;
   tx = rect.left+(rect.right -rect.left)*0.125;
   ty = rect.left+(rect.bottom-rect.top )*0.125;

   hdc = BeginPaint(hwnd, &ps);
   if (doShowHelp)
      showHelp(hdc);
   else
   {
      paintCommand(hdc);
      paintTitle(hdc);
      old_pen = (HPEN) SelectObject(hdc, white_pen);
      gdiTurtle.setDC(hdc);
      gdiTurtle.setAutoZoom(true);
      gdiTurtle.render(&grammar);
      gdiTurtle.setAutoZoom(false);
      gdiTurtle.scale(sx, sy);
      gdiTurtle.translate(tx, ty);
      gdiTurtle.fixTransform();
      gdiTurtle.render(&grammar);
      SelectObject(hdc, old_pen);
   }
   EndPaint(hwnd, &ps);

   return;
}

void AppClass::onHelp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   RECT rect;

   doShowHelp = true;
   // Resize the window so the help message will fit

   GetWindowRect(hwnd, &rect);
   rect.right -= rect.left;
   rect.bottom -= rect.top;
   if (rect.right < 600)
      rect.right = 600;
   if (rect.bottom < 650)
      rect.bottom = 650;

   SetWindowPos(hwnd, NULL, 0, 0, rect.right, rect.bottom,
      SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOREPOSITION);
   InvalidateRect(hwnd, NULL, TRUE);
   return;
}

void AppClass::onChar(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   HDC hdc;
   int n;

   if (doShowHelp)
   {
      doShowHelp = false;
      InvalidateRect(hwnd, NULL, TRUE);
      return;
   }

   if (commandMode)
   {
      n = strlen(cmd_buffer);
      if (wParam == VK_ESCAPE)
         commandMode = false;
      else if (wParam == VK_RETURN)
      {
         doCommand();
         commandMode = false;
      }
      else if (wParam == VK_BACK)
      {
         if (cmd_buffer[0] != 0)
            cmd_buffer[n-1] = 0;
      }
      else if ((wParam >= ' ') && (wParam <= '~'))
      {
         cmd_buffer[n] = wParam;
         cmd_buffer[n+1] = 0;
      }

      hdc = GetDC(hwnd);
      paintCommand(hdc);
      ReleaseDC(hwnd, hdc);
      return;
   }

   if (wParam == VK_ESCAPE)
   {
      PostQuitMessage(0);
      return;
   }
   else if (wParam == VK_RETURN)
   {
      commandMode = true;
      cmd_buffer[0] = 0;
   }
   else if (wParam == '-')
   {
      n = gdiTurtle.getDepth();
      if (n > 0)
         gdiTurtle.setDepth(n-1);
      InvalidateRect(hwnd, NULL, TRUE);
   }
   else if (wParam == '+')
   {
      n = gdiTurtle.getDepth();
      if (n < max_depth)
         gdiTurtle.setDepth(n+1);
      InvalidateRect(hwnd, NULL, TRUE);
   }
   else if (wParam == '[')
   {
      curPreset--;
      if (curPreset < 0)
         curPreset += nPresets;
      loadPreset();
   }
   else if (wParam == ']')
   {
      curPreset++;
      if (curPreset >= nPresets)
         curPreset -= nPresets;
      loadPreset();
   }

   return;
}

void AppClass::paintCommand(HDC hdc)
{
   HBRUSH old_brush;
   HPEN old_pen;
   SetTextColor(hdc, 0xFFFFFF);
   SetBkMode(hdc, OPAQUE);
   SetBkColor(hdc, 0x000000);
   old_brush = (HBRUSH) SelectObject(hdc, black_brush);
   old_pen   = (HPEN  ) SelectObject(hdc, black_pen  );
   Rectangle(hdc, 0, 0, 10000, 20);
   if (commandMode)
      TextOut(hdc, 0, 0, cmd_buffer, strlen(cmd_buffer));
   SelectObject(hdc, old_brush);
   SelectObject(hdc, old_pen  );
   return;
}

void AppClass::paintTitle(HDC hdc)
{
   HBRUSH old_brush;
   HPEN old_pen;
   SetTextColor(hdc, 0xFFFFFF);
   SetBkMode(hdc, OPAQUE);
   SetBkColor(hdc, 0x000000);
   old_brush = (HBRUSH) SelectObject(hdc, black_brush);
   old_pen   = (HPEN  ) SelectObject(hdc, black_pen  );
   Rectangle(hdc, 0, 20, 10000, 40);
   if (doPaintTitle)
      TextOut(hdc, 0, 20, preset_title[curPreset], strlen(preset_title[curPreset]));
   Rectangle(hdc, 0, 40, 10000, 60);
   static const char help_msg[] = "Press F1 for help...";
   TextOut(hdc, 0, 40, help_msg, strlen(help_msg));
   
   SelectObject(hdc, old_brush);
   SelectObject(hdc, old_pen  );
   return;
}

void AppClass::showHelp(HDC hdc)
{
   HBRUSH old_brush;
   HPEN old_pen;
   const static char *help_text[] =
   {"Ben Johnson / PHYS 219 / January-February 2006",
    "",
    "Turtle:  A general purpose L-system interpreter / fractal viewer for Windows 95+",
    "",
    "Valid keys:",
    "  [  ]           Change to the previous/next predefined fractal (from TURTLE.CFG)",
    "  +  -           Change the level of recursion",
    "  F1             Display this help",
    "  Esc            Exit the program",
    "  Enter          Enter command mode",
    "",
    "Command mode commands:",
    "  L=stuff        Add rule L=stuff to the grammar, replacing the old rule for L",
    "  0=stuff        Set the axiom (path at recursion level 0)",
    "  r=number       Set the recursion level to number (ignores the recursion limit)",
    "  a=number       Set the turning angle to number degrees",
    "  d=number       Set the delta parameter (for | command)",
    "",
    "Turtle commands:",
    "  F              Move forward 1 unit",
    "  G              Go forward 1 unit without drawing",
    "  +              Turn to the right",
    "  -              Turn to the left",
    "  [              Save position on the position stack",
    "  ]              Restore position from the position stack",
    "  |              Move forward delta^depth units, where depth is the current recursion depth",
    "  number letter  Execute the command \"letter\" number times",
    "",
    "TURTLE.CFG only commands:",
    "  title=string   Define a new fractal with name string",
    NULL};
   int i;

   SetTextColor(hdc, 0xFFFFFF);
   SetBkMode(hdc, OPAQUE);
   SetBkColor(hdc, 0x000000);
   old_brush = (HBRUSH) SelectObject(hdc, black_brush);
   old_pen   = (HPEN  ) SelectObject(hdc, black_pen  );
   for(i=0;help_text[i]!=NULL;i++)
      TextOut(hdc, 0, i*20, help_text[i], strlen(help_text[i]));
   SelectObject(hdc, old_brush);
   SelectObject(hdc, old_pen  );
   return;
}

void AppClass::doCommand()
{
   int *buffer, i, n;

   // Execute a command entered by the user
   if ((cmd_buffer[0] != 0) && (cmd_buffer[1] == '='))
   {
      // What are we setting?
      // Upper case letter -> production rule
      // 0 -> Axiom
      // a -> Angle
      // r -> Depth (ignores max depth)
      // d -> Delta (base for | command)

      if ((cmd_buffer[0] == '0') ||
          ((cmd_buffer[0] >= 'A') &&
           (cmd_buffer[0] <= 'Z'))
         )
      {
         n = strlen(cmd_buffer)-2;
         buffer = (int *) malloc((n+1)*sizeof(int));
         buffer[0] = n;
         for(i=0;i<n;i++)
            buffer[i+1] = cmd_buffer[i+2];
         if (cmd_buffer[0] == '0')
            grammar.setAxiom(buffer);
         else
            grammar.addRule(cmd_buffer[0], buffer);
      }
      else if (cmd_buffer[0] == 'a')
         // Translate the buffer to an angle
         gdiTurtle.setDTheta(PI*atof(cmd_buffer+2)/180.0);
      else if (cmd_buffer[0] == 'r')
         // Set the depth regardless of the max_depth level
         gdiTurtle.setDepth(atoi(cmd_buffer+2));
      else if (cmd_buffer[0] == 'd')
         gdiTurtle.setDelta(atof(cmd_buffer+2));
   }
   InvalidateRect(hwnd, NULL, TRUE);
   doPaintTitle = false;
   return;
}

void AppClass::loadPreset()
{
   int i;

   for(i=0;preset_cmd[curPreset][i] != NULL;i++)
   {
      strcpy(cmd_buffer, preset_cmd[curPreset][i]);
      doCommand();
   }

   doPaintTitle = true;
   return;
}

/*************************
 *                       *
 *   G D I T U R T L E   *
 *                       *
 *************************/

GDITurtle::GDITurtle()
{
   return;
}

GDITurtle::~GDITurtle()
{
   return;
}

void GDITurtle::setDC(HDC hdc)
{
   this->hdc = hdc;
   return;
}

void GDITurtle::lineTo(double x, double y)
{
   LineTo(hdc, (int) (transform[0]*x + transform[1]*y + transform[2]),
               (int) (transform[3]*x + transform[4]*y + transform[5]));
   return;
}

void GDITurtle::moveTo(double x, double y)
{
   MoveToEx(hdc, (int) (transform[0]*x + transform[1]*y + transform[2]),
                 (int) (transform[3]*x + transform[4]*y + transform[5]),
                 NULL);
   return;
}

