#include "FunctionLibrary.h"

// int FunctionLibrary::tf1_instance_count = 0;

/* BreitWigner 
 * @x 		x[0] single variable : mass [GeV/c^2]
 * @par 	par[0] : width of the resonance
 * 			par[1] : pdg mass of the resonance
 * @return 	The evaluated BReitWigner value
 */
double TF_BreitWigner( double *x, double *par ){
	double width = par[0];
	double mass  = par[1];
	double x0    = x[0];
	
	return 2.0 * width / (   pow( x0 - mass, 2.0 ) + pow( width / 2.0, 2.0 )   );
}


double TF_MasslesPS( double *x, double *par ){
	double mass = par[0];
	double x0 = x[0];

	return pow( 1 - x0*x0 / ( mass*mass ), 3 );
}

/* Form Factor
 *
 */
double FFSquare(double *x, double *par){
	double m = x[0];
	double LambdaInvSquare = par[0];
	return pow(1.-m*m*LambdaInvSquare,-2);
}