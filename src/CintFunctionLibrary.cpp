
#include "CintFunctionLibrary.h"

#define _USE_MATH_DEFINES
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

double PhaseSpaceMassless( double Mll, double M0 ){
	return pow(1 - (Mll*Mll)/(M0*M0), 3);
}

double PhaseSpaceMassless( double *x, double *par ){
	return PhaseSpaceMassless( x[0], par[0] );
}

double PhaseSpaceMassive( double Mll, double M0, double Mn ){
	if ( Mll > M0 )
        return 0;
    if ( Mn < 0.001 ) // just make sure we use the massless form, was getting some trouble with 0.0 != 0.0 and not canceling
    	return PhaseSpaceMassless( Mll, M0 );
    
    double a = pow( 1 + ( Mll*Mll ) / ( M0*M0 - Mn*Mn ), 2.0);
    double b = ( 4.0 * M0*M0 * Mll*Mll ) / pow( M0*M0 - Mn*Mn, 2.0);
    
    // sqrt of negative numbers is a good thing, but not today
    if ( a - b < 0 )
    	return 0;

    return pow( a - b, 3.0/2.0 );
}

double PhaseSpaceMassive( double *x, double *par ){
	return PhaseSpaceMassive( x[0], par[0], par[1] );
}

double FormFactor2( double Mll, double G0, double iL2 ){

	double FF = fabs(1.0 / ( pow(1.0 - Mll*Mll * iL2, 2.0 ) + G0 * iL2 ));
	return FF;
}
double FormFactor2( double *x, double *par ){
	return FormFactor2( x[0], par[0], par[1] );
}


double QED( double Mll, double ml, double Nd ){
	if ( Mll < ml * 2.0 )
		return 0.0;

	const double alpha = 1.0/137.036;
	const double Pi    = 3.14159265;
	const double a = (alpha * 2.0 / 3.0 ) / ( Pi * Mll );
	
	double a1 = (1.0+2.0*ml*ml/(Mll*Mll));
    double a2 = sqrt( 1.0 - 4 * ml*ml / ( Mll*Mll )  );

	return a1 * a2 * (alpha * Nd/3.0) / ( Pi * Mll );
}
double QED( double *x, double *par ){
	return QED( x[0], par[0], par[1] );
}


double KrollWada( double Mll, double M0, double Mn, double ml, double G0, double iL2, double Nd ){
	double v = QED( Mll, ml, Nd ) * PhaseSpaceMassive( Mll, M0, Mn ) * FormFactor2( Mll, G0, iL2 );
	if ( v < 0.0 )
		return 0.0;
	return v;
}
double KrollWada( double *x, double *par ){
	return KrollWada( x[0], par[0], par[1], par[2], par[3], par[4], par[5] );
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

	double A = pow( n/fabs(alpha), n) * exp(-alpha*alpha/2.0);
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

double CrystalBall2( double x, double N, double mu, double sig, double n, double alpha, double m, double beta ){

	double A = pow(n/fabs(alpha), n) * exp(-alpha*alpha/2.0);
	double B = n/fabs(alpha) - fabs(alpha);

	double C = pow(m/fabs(beta), m) * exp(-beta*beta/2.0);
	double D = m/fabs(beta) - fabs(beta);

	double norm = (x - mu) / sig;

	if(norm < -alpha) {
		return N * A * pow(B - norm, -n);
	} else if(norm < beta) {
		return N * exp(-0.5*norm*norm);
	}
	
	return N * C * pow(D+norm, -m);
}

double CrystalBall2( double *x, double *par ){
	return CrystalBall2( x[0], par[0], par[1], par[2], par[3], par[4], par[5], par[6] );
}