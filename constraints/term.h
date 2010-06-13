#include "../enum.h"
#include "../utils/rationals.h"
typedef int var;
struct term
{
  var v;
  rational * coeff;
  enum varStatus status;
}
