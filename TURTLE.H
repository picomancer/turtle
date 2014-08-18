

/*

  Turtle geometry classes by Ben Johnson 
  PHYS 219
  Part of the Koch snowflake assignment

  Instead of hard-coding the Koch snowflake, I thought I'd
  take a different approach.  I have developed these classes
  for studying L-systems in general.  The classes here are
  as follows:

  LArray : A lightweight array-storage type.  It's simply
           an array of integers whose first member specifies
           the length.

  Dcfg : Deterministic context-free grammar class.  (General
         CFG's are defined in the book; a deterministic CFG is
         one where each symbol appears on the left hand side
         of at most one rule.)  Add the rules you want with
         the addRule method.  To apply the rules on one pass
         through a string, use the apply method.
         
         Since using apply a lot of times will take a lot
         of memory, I also provide the setAxiom, restart,
         and getNext methods.  The setAxiom method sets
         the axiom (initial string) to use, while the
         restart method allows you to change the depth.

         See the Dcfg_test sample program for more information.

         Warning:  For efficiency (and my own laziness),
         this class uses zero-copy semantics, meaning
         that the caller is responsible for making sure
         that the LArray's for the rules and axiom are
         not changed or deallocated while the Dcfg object
         is still alive!  For convenience I have the
         Dcfg object free the axiom / rules when they're
         replaced...should make a flag to control this
         behavior in the future

         Why do I use int's instead of char's?  In case
         you ever need more than 256 symbols, it will make
         life easier :)

   Turtle : A purely geometric class that follows the
            instructions contained in a Dcfg.  The
            symbols F+-[] are interpreted according to
            the book.  A number n before a symbol will
            repeat that symbol n times.  The
            moveTo and lineTo commands are declared
            as virtual functions.  That way you can
            put whatever system-specific graphics
            code into a subclass, making it easier
            to port this program to other systems
            (such as DOS or Linux), output the graphics
            to a file, or use your own custom rendering
            routines

   ltint : A class we must define as the comparator for
           the STL map class (don't think too hard about
           what I'm trying to do, I just grabbed it from
           the exapmle code at
           http://www.sgi.com/tech/stl/Map.html and
           modified it appropriately)
*/

#include <vector>
#include <map>

using namespace std;

struct ltint
{
   bool operator()(int i, int j) const
   {
      return i < j;
   }
};

typedef int *LArray;

class Dcfg
{
public:
   Dcfg();
   ~Dcfg();
   
   // Methods for basic usage
   void addRule(int lhs, LArray rhs);
      // Add a rule to our list of rules
   LArray apply(LArray array);
      // Apply the rules to the symbols in an array
      // (if there is no rule for a symbol, leave it alone)

   // Methods for advanced usage
   void setAxiom(LArray axiom);
      // Set the first rule
   void restart(int depth);
      // Set the depth and start at the beginning
   int getNext();
      // Get the next symbol in the string resulting
      // from running apply on axiom depth times
   int getCurDepth();
      // Get the current depth
   
protected:
   map<int, LArray, ltint> m;
   
   LArray axiom;
   int cur_depth, max_depth;
   vector<LArray> array_stack;
   vector<int> location_stack;
};

class Turtle
{
public:
   Turtle();
   ~Turtle();

   void render(Dcfg *grammar);
      // Render the formation contained in the grammar
   virtual void exec(int command, int depth);
      // Execute a single turtle command (forward, turn, etc)
   void setDepth(int depth);
      // Set the recursion depth (fairly obvious)
   int getDepth();
      // Get the current recursion depth
   void setDTheta(double dtheta);
      // Set the turning angle
   void setDelta(double delta);
      // Set the delta for the | command

   virtual void lineTo(double x, double y) = 0;
   virtual void moveTo(double x, double y) = 0;
      // lineTo and moveTo will be implemented by a subclass
      // which will handle the system-dependent graphics
      // stuff like HDC's, HBRUSHes, double buffers,
      // Bresenham's algorithm, etc.

   // Allow us to do an auto-zoom pass
   void setAutoZoom(bool autoZoom);
   void addPoint(double x, double y);
   void scale(double sx, double sy);
   void translate(double tx, double ty);
   void fixTransform();

protected:
   double x, y, theta;
   vector<double> data_stack;

   int depth;
   double dtheta;
   double delta;

   bool hasNumber;
   int number;

   // Variables for the autozoom feature
   bool autoZoom;
   double min_x, min_y, max_x, max_y;
   double transform[6];
};
