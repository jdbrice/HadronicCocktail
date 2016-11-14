#include "FunctionLibrary.h"

int FunctionLibrary::tf1_instance_count = 0;

double TFormula_BreitWigner( double *x, double *par ){
	double width = par[0];
	double mass  = par[1];
	double x0    = x[0];
	
	return 2.0 * width / (   pow( x0 - mass, 2.0 ) + pow( width / 2.0, 2.0 )   );
}


double TFormula_MasslesPS( double *x, double *par ){
	double mass = par[0];
	double x0 = x[0];

	return pow( 1 - x0*x0 / ( mass*mass ), 3 );
}