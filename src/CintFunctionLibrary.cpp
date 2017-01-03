
#include "CintFunctionLibrary.h"


#include <cmath>

double BreitWigner( double Mll, double Mreso, double gamma ){
	return 2.0 * gamma / (   pow( Mll - Mreso, 2.0 ) + pow( gamma / 2.0, 2.0 )   );
}

double BreitWigner( double *x, double *par ){
	double width = par[0];
	double mass  = par[1];
	double x0    = x[0];
	
	return 2.0 * width / (   pow( x0 - mass, 2.0 ) + pow( width / 2.0, 2.0 )   );
}

double PhaseSpace( double *x, double *par ){
	double m  = x[0];
	double pT = par[0];
	double T  = par[1];

	if ( m <= 0 || pT <= 0 )
		return 0;

	double E = sqrt( m*m + pT*pT );
	return m / E * exp( -E / T );
}

double PhasSpaceMassive( double Mll, double M0, double Mn ){
	if ( Mll > M0 )
        return 0;
    
    double a = pow( 1 - ( Mll*Mll ) / ( M0*M0 - Mn*Mn ), 2.0);
    double b = ( 4.0 * M0*M0 * Mll*Mll ) / pow( M0*M0 - Mn*Mn, 2.0);
    
    // sqrt of negative numbers is a good thing, but not today
    if ( a - b < 0 )
    	return 0;

    return pow( a - b, 3.0/2.0 );
}

double PhaseSpaceMassive( double *x, double *par ){
	return PhasSpaceMassive( x[0], par[0], par[1] );
}

double FormFactor2( double Mll, double G0, double iL2 ){
	double FF = fabs(1.0 / ( pow(1.0 - Mll*Mll * iL2, 2.0 ) + G0*G0 * iL2 ));
	return pow( FF, 2.0 );
}
double FormFactor2( double *x, double *par ){
	return FormFactor2( x[0], par[0], par[1] );
}


double QED( double Mll, double ml ){
	if ( Mll < ml * 2.0 )
		return 0;
	return sqrt( 1.0 - (4.0*ml*ml) / ( Mll*Mll ) ) * ( 1.0 + (2.0*ml*ml)/(Mll*Mll) ) * (1.0/Mll);
}
double QED( double *x, double *par ){
	return QED( x[0], par[0] );
}


double KrollWada( double Mll, double M0, double Mn, double ml, double G0, double iL2 ){
	double v = QED( Mll, ml ) * PhasSpaceMassive( Mll, M0, Mn ) * FormFactor2( Mll, G0, iL2 );
	if ( v < 0.0 )
		return 0.0;
	return v;
}
double KrollWada( double *x, double *par ){
	return KrollWada( x[0], par[0], par[1], par[2], par[3], par[4] );
}







double PlcGamma( double Mll, double Mreso, double gamma0, double Ml, double l ){
	double sp = (2 * l + 1) / 2.0;
	double a = Mll*Mll - 4.0 * Ml * Ml;
	double b = Mreso*Mreso - 4.0 * Ml * Ml;
	return gamma0 * ( Mreso / Mll ) * pow( a/b, sp );
}

// TODO: convert this to a combination of BW * PS
double RhoMassDistribution( double *x, double *par ){
	// massRho = 0.77549
 //    massRho2 = massRho**2
    
 //    GammaEm = GammaE(m)
 //    return m * massRho * GammaE(m) / ( ( massRho2 - m*m )**2 + massRho2*( GammaE(m)+GammaE(m)*Gamma2)) * PS( pT,T,m ) 

    double Mll = x[0];
    
    double Mreso = 0.77549; // rho mass in [GeV/c^2]
    double gamma0 = 0.1491; // intrinsic width of rho for ee channel [GeV/c^2]
    double gamma2 = 4.72e-5;
    double Ml = par[0];
    double pT = par[1];
    double T  = par[2];
    double g  = PlcGamma( Mll, Mreso, gamma0, Ml, 0 );

    return Mll * Mreso * g / ( pow( Mreso*Mreso - Mll*Mll, 2.0 ) + Mreso*Mreso * ( g+g*gamma2)) * PhaseSpace( x, &par[1] );


}

double CrystalBall( double x, double N, double mu, double sig, double n, double alpha ){

	double A = pow( n/fabs(alpha), n) * exp(-alpha*alpha/2.);
	double B = n/fabs(alpha) - fabs(alpha);
	double norm = (x-mu)/sig;

	if(norm > -alpha) {
		return N * exp(-0.5*norm*norm);
	} else {
		return N * A * pow(B-norm, -n);
	}
}
double CrystalBall( double *x, double *par ){
	return CrystalBall( x[0], par[0], par[1], par[2], par[3], par[4] );
}