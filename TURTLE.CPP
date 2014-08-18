
#include "TURTLE.H"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979

using namespace std;

Dcfg::Dcfg()
{
   cur_depth = -1;
   max_depth = -1;
   axiom = NULL;
   return;
}

Dcfg::~Dcfg()
{
   return;
}

void Dcfg::addRule(int lhs, LArray rhs)
{
   map<int, LArray, ltint>::iterator iter;
   if (lhs < 0)
   {
      printf("Error:  negative symbols are reserved\n");
      exit(1);
   }
   iter = m.find(lhs);
   if (iter != m.end())
      free((*iter).second);
   m[lhs] = rhs;
   return;
}

LArray Dcfg::apply(LArray array)
{
   map<int, LArray, ltint>::iterator iter;
   int i;
   LArray source, target;
   int tsize, block_size;

   tsize = array[0];
   target = (LArray) malloc((tsize+1)*sizeof(int));
   target[0] = 0;

   for(i=1;i<=array[0];i++)
   {
      iter = m.find(array[i]);
      if (iter == m.end())
      {
         source = array+i;
         block_size = 1;
      }
      else
      {
         source = ((*iter).second)+1;
         block_size = ((*iter).second)[0];
      }
      while (target[0] + block_size >= tsize)
      {
         tsize *= 2;
         target = (LArray) realloc(target, (tsize+1)*sizeof(int));
      }
      memcpy(target+target[0]+1, source, block_size*sizeof(int));
      target[0] += block_size;
   }

   target = (LArray) realloc(target, (target[0]+1)*sizeof(int));
   return target;
}

void Dcfg::setAxiom(LArray axiom)
{
   if (this->axiom != NULL)
      free(this->axiom);
   this->axiom = axiom;
   return;
}

void Dcfg::restart(int depth)
{
   cur_depth = 0;
   max_depth = depth;
   array_stack.clear();
   location_stack.clear();
   array_stack.push_back(axiom);
   location_stack.push_back(1);
   return;
}

int Dcfg::getNext()
{
   LArray a = array_stack.back();
   int loc = location_stack.back();
   map<int, LArray, ltint>::iterator iter;

   if (axiom == NULL)
      return -1;

   while (1)
   {
      while (loc > a[0])
      {
         cur_depth--;
         array_stack.pop_back();
         location_stack.pop_back();
         a = array_stack.back();
         loc = location_stack.back();
         if (cur_depth >= 0)
            continue;
         else
            return -1;
      }

      if (cur_depth >= max_depth)
         break;
      iter = m.find(a[loc]);
      if (iter != m.end())
      {
         (location_stack.back())++;
         a = (*iter).second;
         loc = 1;
         array_stack.push_back(a);
         location_stack.push_back(loc);
         cur_depth++;
      }
      else
         break;
   }
   
   (location_stack.back())++;
   
   return a[loc];
}

int Dcfg::getCurDepth()
{
   return cur_depth;
}

Turtle::Turtle()
{
   x = 0.0;
   y = 0.0;
   theta = 0.0;
   dtheta = PI/3;
   depth = 3;
   delta = 0.5;
   return;
}

Turtle::~Turtle()
{
   return;
}

void Turtle::render(Dcfg *grammar)
{
   int s, d;
   grammar->restart(depth);
   data_stack.clear();   
   x = 0.0;
   y = 0.0;
   theta = 0.0;
   hasNumber = false;
   if (autoZoom)
   {
      min_x = 0.0;
      min_y = 0.0;
      max_x = 0.0;
      max_y = 0.0;
   }
   else
      moveTo(0.0, 0.0);

   while(1)
   {
      s = grammar->getNext();
      if (s == -1)
         break;
      d = grammar->getCurDepth();
      exec(s, d);
   }

   if (autoZoom)
   {
      if (max_x == min_x)
         max_x++;
      if (max_y == min_y)
         max_y++;
      transform[0] = 1.0/(max_x-min_x);
      transform[1] = 0.0;
      transform[2] = -min_x*transform[0];
      transform[3] = 0.0;
      transform[4] = 1.0/(max_y-min_y);
      transform[5] = -min_y*transform[4];
   }
   return;
}

void Turtle::exec(int command, int depth)
{
   int i, n;
   double d;

   if (command <= ' ')    // Ignore whitespace
      return;

   if ((command >= '0') && (command <= '9'))
   {
      if (!hasNumber)
         number = 0;
      number = number*10+(command-'0');
      hasNumber = true;
      return;
   }

   if (!hasNumber)
      number = 1;

   for(i=0;i<number;i++)
   {
      switch(command)
      {
      case 'F':
         if (autoZoom)
            addPoint(x, y);
         x += cos(theta);
         y += sin(theta);
         if (autoZoom)
            addPoint(x, y);
         else
            lineTo(x, y);
         break;
      case 'G':
         x += cos(theta);
         y += sin(theta);
         moveTo(x, y);
         break;
      case '+':
         theta += dtheta;
         break;
      case '-':
         theta -= dtheta;
         break;
      case '[':
         data_stack.push_back(x);
         data_stack.push_back(y);
         data_stack.push_back(theta);
         break;
      case ']':
         n = data_stack.size();
         if (n > 0)
         {
            x = data_stack[n-3];
            y = data_stack[n-2];
            theta = data_stack[n-1];
            data_stack.resize(n-3);
            moveTo(x, y);
         }
         break;
      case '|':
         d = pow(this->delta, depth-1);
         x += d*cos(theta);
         y += d*sin(theta);
         if (autoZoom)
            addPoint(x, y);
         else
            lineTo(x, y);
         break;
      }
   }
   hasNumber = false;
   return;
}

void Turtle::addPoint(double x, double y)
{
   if (x < min_x)
      min_x = x;
   if (y < min_y)
      min_y = y;
   if (x > max_x)
      max_x = x;
   if (y > max_y)
      max_y = y;

   return;
}

void Turtle::setDepth(int depth)
{
   if (depth < 0)
      this->depth = 0;
   else
      this->depth = depth;
   return;
}

int Turtle::getDepth()
{
   return this->depth;
}

void Turtle::setDTheta(double dtheta)
{
   this->dtheta = dtheta;
   return;
}

void Turtle::setDelta(double delta)
{
   if (delta < 0)
      this->delta = 0.0;
   else if (delta > 1.0)
      this->delta = 1.0;
   else
      this->delta = delta;
   return;
}

void Turtle::setAutoZoom(bool autoZoom)
{
   this->autoZoom = autoZoom;
   return;
}

void Turtle::scale(double sx, double sy)
{
   transform[0] *= sx;
   transform[1] *= sx;
   transform[2] *= sx;
   transform[3] *= sy;
   transform[4] *= sy;
   transform[5] *= sy;
   return;
}

void Turtle::translate(double tx, double ty)
{
   transform[2] += tx;
   transform[5] += ty;
   return;
}

void Turtle::fixTransform()
{
   if (transform[0] < transform[4])
      transform[4] = transform[0];
   else
      transform[0] = transform[4];
   return;
}
