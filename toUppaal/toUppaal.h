#ifndef __TOUPPAAL_H
#define __TOUPPAAL_H
#include "../system/system.h"

void toUppaal(struct system * syst);
/* beware, it does not work correctly when there is some 
label that is not either never asap or always asap in 
the product automaton of the system */ 

#endif
