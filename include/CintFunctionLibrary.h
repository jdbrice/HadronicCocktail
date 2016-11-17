#ifndef CINT_FUNCTION_LIBRARY_H
#define CINT_FUNCTION_LIBRARY_H


double BreitWigner( double *x, double *par );
// {
// 	double width = par[0];
// 	double mass  = par[1];
// 	double x0    = x[0];
	
// 	return 2.0 * width / (   pow( x0 - mass, 2.0 ) + pow( width / 2.0, 2.0 )   );
// }

double PhaseSpace( double *x, double *par );
// {
// 	double m  = x[0];
// 	double pT = par[0];
// 	double T  = par[1];

// 	if ( m <= 0 || pT <= 0 )
// 		return 0;

// 	double E = sqrt( m*m + pT*pT );
// 	return m / E * exp( -E / T );
// }

#endif