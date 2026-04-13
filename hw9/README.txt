* Implementation

In this coding task, you will implement cubic Bezier curve evaluation and
subdivision.  You should use De Casteljau's algorithm to subdivide and evaluate
the curve.

IMPORTANT NOTE: You should *not* modify any files other than "curves.cpp".  We
will be compiling your "curves.cpp" file against our code base. If you modify
other files for your solution, it may fail to compile or work properly in our
code base.

* Compilation

Run scons to compile the project:
%> scons

* Running and Automatic Grading

In this project, there is only one executable which serves as the grading script
as well.  You can run it as:
%> ./curves

NOTE: The program will write out the follwing files which may be helfpul for debugging:
- curve.eps: illustrate the curve that your Evaluate function produces.
- subdivision.eps: illustrates the two curves that your subdivision in Evaluate produces.
- symmetry.eps: If you code fails the symmetry test, this file will show the curve as seen in the reverse direction.
