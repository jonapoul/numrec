#ifndef ROOTFINDER_H
#define ROOTFINDER_H

class RootFinder {
private:
   double f   (double x);
   double dfdx(double x);
   double min (double a, double b) { return (a < b) ? a : b; }
   double max (double a, double b) { return (a < b) ? b : a; }
   int    sign(double x)           { return (x > 0.0) ? 1 : -1; }

public:
   double epsilon;
   double a;
   double b;
   double c;
   double d;
   RootFinder();

   void   test_methods ();
   double bisection    (double x1, double x2, int* n);
   double newtonraphson(double x,             int* n);
   double secant       (double x1, double x2, int* n);
   double hybrid       (double x1, double x2, int* n);
};

#endif