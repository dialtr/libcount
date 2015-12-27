#include <inttypes.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  double x = pow(2.0, 3.0);
  x = 1.0 / x;
  printf("%lf\n", x);

  double y = pow(2.0, -3.0);
  printf("%lf\n", y);

  return 0;
}

