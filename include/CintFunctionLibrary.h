#ifndef CINT_FUNCTION_LIBRARY_H
#define CINT_FUNCTION_LIBRARY_H

double TF_BreitWigner( double *x, double *par ){
	double width = par[0];
	double mass  = par[1];
	double x0    = x[0];
	
	return 2.0 * width / (   pow( x0 - mass, 2.0 ) + pow( width / 2.0, 2.0 )   );
}

#endif