#ifndef __HDR_H
#define __HDR_H

#include "x.h"

int cmppt (PTERM **a, PTERM **b);
int cmpv (BIT *x, BIT *y, int n);
int cmpptx (PTERM **a, PTERM **b);
int cmppth (PTERM **a, PTERM **b);
PTERM *find_pterm (PTERM *pt, PTERM *pts[], int npts);
long pthash (BIT *inv, int n);
int ptindex (BIT *inv, int n);

#endif
