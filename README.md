
This is the last large project I worked on using the Windows API.  This was an assignment
for a course about dynamical systems taught by the physics department.  This assignment
was to draw the Koch snowflake, and being an overachiever at the time, I wrote a program
that could draw the Koch snowflake -- and all the other L-system based fractals from the
textbook!  This is called "turtle graphics."

I apologize for the inconsistent filename capitalization and lack of Makefile.

Some of the Windows API code is a framework provided by the professor, hence the extensive
commenting.

One of my innovations was a command line interface; when you press the Enter key it sets
`command_mode` flag to true.  Then it allows you to enter a command, or use Backspace
to edit out mistakes.

Looking at the code now I see it has no internationalization support, and the command
input buffer is allowed to overflow.  I would declare internationalization to be
out-of-scope, and handle buffer overflow by using the C++ `string` class instead of
a fixed-length C-style string.

