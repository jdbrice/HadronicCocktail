
#include "CintFunctionLibrary.h"

#define _USE_MATH_DEFINES
#include <cmath>

// ROOT
#include "TF1.h"




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

double Gamma( double Mll, double ml, double gamma0, double Mreso ){
	if ( Mll - 2 * ml < 0 )
		return 0;
	if ( Mreso - 2 * ml < 0 )
		return 0;

	return gamma0 * ( Mreso / Mll ) * sqrt( (Mll*Mll - 4 * ml * ml) / (Mreso*Mreso - 4 * ml * ml) );
}

// TODO: convert this to a combination of BW * PS
// double RhoMassDistribution( double *x, double *par ){
// 	// massRho = 0.77549
//  //    massRho2 = massRho**2
    
//  //    GammaEm = GammaE(m)
//  //    return m * massRho * GammaE(m) / ( ( massRho2 - m*m )**2 + massRho2*( GammaE(m)+GammaE(m)*Gamma2)) * PS( pT,T,m ) 

//     double Mll = x[0];
    
//     double Mreso = 0.77549; // rho mass in [GeV/c^2]
//     double gamma0 = 0.1491; // intrinsic width of rho for ee channel [GeV/c^2]
//     double gamma2 = 4.72e-5;
//     double Ml = par[0];
//     double pT = par[1];
//     double T  = par[2];
//     double g  = PlcGamma( Mll, Mreso, gamma0, Ml, 0 );

//     return Mll * Mreso * g / ( pow( Mreso*Mreso - Mll*Mll, 2.0 ) + Mreso*Mreso * pow( g+g*gamma2, 2.0 )) * PhaseSpace( x, &par[1] );


// }

double PhaseSpaceVacuumRho( double Mll, double pT, double T ){
	double E = sqrt( Mll*Mll + pT*pT );
	return (Mll / E ) * exp( - E / T );
}


double PhaseSpaceVacuumRho( double E, double T ){
	return (1.0 / E) * exp( -E/T );
}

double PhaseSpaceVacuumRho( double *x, double *p ){
	return PhaseSpaceVacuumRho( x[0], p[0] );
}



double GammaRho(double mass){
	const double Gamma0 = 0.1491;
	const double Massrho		= 0.77549;
const double Masspi0 		= 0.1349766;
	if(mass>2.*Masspi0) 
		return Gamma0*Massrho/mass*TMath::Power((mass*mass-4.*Masspi0*Masspi0)/(Massrho*Massrho-4.*Masspi0*Masspi0),1.5);
	else
		return 0;
}

//-------------------------------------------------------
double GammaE(double mass, double MassLepton)
{
	const double Gamma0 = 0.1491;
	const double Massrho		= 0.77549;
	const double Masspi0 		= 0.1349766;
	if(mass>2.*MassLepton) 
		return Gamma0*Massrho/mass*TMath::Power((mass*mass-4.*MassLepton*MassLepton)/(Massrho*Massrho-4.*MassLepton*MassLepton),0.5);
	else
		return 0;
}

double MassVacuumRho( double Mll, double pT, double ml, double gamma0, double gamma2, double T ){

	const double Massrho   = 0.77549;
	double m = Mll;
	double Gamma2 = 4.55e-5;
	return m*Massrho*GammaE(m, ml)/(TMath::Power((Massrho*Massrho-m*m),2.)+Massrho*Massrho*(GammaRho(m)+GammaE(m, ml)*Gamma2)*(GammaRho(m)+GammaE(m, ml)*Gamma2))*PhaseSpaceVacuumRho(m,pT,T)*1e2;


	const double mass_vacuum_rho   = 0.77549; 		// GeV/c^2
	const double m2 = pow( mass_vacuum_rho, 2.0 );
	const double mass_pion0 = 0.1349766;	// GeV/c^2

	double gammall   = Gamma( Mll, ml, gamma0, mass_vacuum_rho );
	double gammapipi = Gamma( Mll, mass_pion0, gamma0, mass_vacuum_rho );

	double a = Mll * mass_vacuum_rho * gammall;
	double b = pow( m2 - Mll*Mll, 2.0 );
	double c = m2 * pow( gammapipi + gammall * gamma2, 2.0 );

	double PS = PhaseSpaceVacuumRho( Mll, pT, T );
	
	return (a / ( b + c ) ) * PS * 1e2;
}

double MassVacuumRho( double *x, double *par ){
	return MassVacuumRho( x[0], par[0], par[1], par[2], par[3], par[4] );
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

TF1 *fTsallisBlastWave_Integrand_r   = nullptr;
TF1 *fTsallisBlastWave_Integrand_phi = nullptr;
TF1 *fTsallisBlastWave_Integrand_y   = nullptr;

double TsallisBlastWave_Integrand_y( const double *x, double* p ) {
	double y = x[0];
	fTsallisBlastWave_Integrand_r->SetParameter( 6, y );
	double integral = fTsallisBlastWave_Integrand_phi->Integral( -TMath::Pi(), TMath::Pi() );
	return TMath::CosH(y) * integral;
}

double TsallisBlastWave_Integrand_phi(const double *x, double* p){
	double phi = x[0];
	fTsallisBlastWave_Integrand_r->SetParameter(7, phi);
	double integral = fTsallisBlastWave_Integrand_r->Integral(0., 1.);
	return integral;
}

double TsallisBlastWave_Integrand_r(const double *x, const double *p){
	/* 
	 x[0] -> r (radius)
	 p[0] -> mT (transverse mass)
	 p[1] -> pT (transverse momentum)
	 p[2] -> beta_max (surface velocity)
	 p[3] -> T (freezout temperature)
	 p[4] -> n (velocity profile)
	 p[5] -> q
	 p[6] -> y (rapidity)
	 p[7] -> phi (azimuthal angle)
	*/

	double r        = x[0];
	double mt       = p[0];
	double pt       = p[1];
	double beta_max = p[2];
	double temp_1   = 1. / p[3];
	double n        = p[4];
	double q        = p[5];
	double y        = p[6];
	double phi      = p[7];

	if (q <= 1.) return r;

	double beta = beta_max * TMath::Power(r, n);
	double rho = TMath::ATanH(beta);

	double part1 = mt * TMath::CosH(y) * TMath::CosH(rho);
	double part2 = pt * TMath::SinH(rho) * TMath::Cos(phi);
	double part3 = part1 - part2;
	double part4 = 1 + (q - 1.) * temp_1 * part3;
	double expo  = -1. / (q - 1.);
	//  printf("part1=%f, part2=%f, part3=%f, part4=%f, expo=%f\n", part1, part2, part3, part4, expo);
	double part5 = TMath::Power(part4, expo);

	return r * part5;
}

double TsallisBlastWave_Func(const double *x, const double *p){
  /* dN/dpt */
  
  double pt       = x[0];
  double mass     = p[0];
  double mt       = TMath::Sqrt(pt * pt + mass * mass);
  double beta_max = p[1];
  double temp     = p[2];
  double n        = p[3];
  double q        = p[4];
  double norm     = p[5];
  double ymin     = p[6];
  double ymax     = p[7];

  if (nullptr == fTsallisBlastWave_Integrand_r)
    fTsallisBlastWave_Integrand_r = new TF1("fTsallisBlastWave_Integrand_r", TsallisBlastWave_Integrand_r, 0., 1., 8);
  if (nullptr == fTsallisBlastWave_Integrand_phi)
    fTsallisBlastWave_Integrand_phi = new TF1("fTsallisBlastWave_Integrand_phi", TsallisBlastWave_Integrand_phi, -TMath::Pi(), TMath::Pi(), 0);
  if (nullptr == fTsallisBlastWave_Integrand_y)
    fTsallisBlastWave_Integrand_y = new TF1("fTsallisBlastWave_Integrand_y", TsallisBlastWave_Integrand_y, -0.5, 0.5, 0);

  fTsallisBlastWave_Integrand_r->SetParameters(mt, pt, beta_max, temp, n, q, 0., 0.);
  double integral = fTsallisBlastWave_Integrand_y->Integral(ymin, ymax);
  return norm * mt * integral;
}

TF1 *TsallisBlastWave(const char *name, double mass, double beta_max, double temp, double n, double q, double norm, double ymin, double ymax){
  
  TF1 *fTsallisBlastWave = new TF1(name, TsallisBlastWave_Func, 0., 10., 8);
  fTsallisBlastWave->SetParameters(mass, beta_max, temp, n, q, norm, ymin, ymax);
  fTsallisBlastWave->SetParNames("mass", "beta_max", "T", "n", "q", "norm", "ymin", "ymax");
  fTsallisBlastWave->FixParameter(0, mass);
  
  fTsallisBlastWave->FixParameter(6, ymin);
  fTsallisBlastWave->FixParameter(7, ymax);

  fTsallisBlastWave->SetParLimits(1, 0.01, 0.99);
  fTsallisBlastWave->SetParLimits(2, 0.01, 1.);
  fTsallisBlastWave->SetParLimits(3, 0.1, 10.);
  fTsallisBlastWave->SetParLimits(4, 1., 10.);

  return fTsallisBlastWave;
}