#ifndef _sfevent_h_
#define _sfevent_h_
#include "params.h"
#include "event1.h"
#include "nucleus.h"

bool has_sf(nucleus &t, int method);

double sfevent(params &p,event &e,nucleus &t);
double sfevent2cc(params &p,event &e,nucleus &t);
double sfevent2nc(params &p,event &e,nucleus &t);

#endif
