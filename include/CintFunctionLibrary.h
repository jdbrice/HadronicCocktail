#ifndef CINT_FUNCTION_LIBRARY_H
#define CINT_FUNCTION_LIBRARY_H

class TF1;
#include "TMath.h"


double BreitWigner( double Mll, double M0, double G0 );
double BreitWigner( double *x, double *par );

double PhaseSpace( double *x, double *par );
double PlcGamma( double m, double mReso, double gamma0, double ml, double l );
double Gamma( double Mll, double ml, double gamma0, double Mreso );

/* Phase space for Dalits to massive neutral particle
 * 
 * For $\omega \rightarrow l^+ l^- \pi^0$
 * PS = \left[ \left( 1 + \frac{M_{ll}^2}{ M_{0}^2 - M_{\pi^0}^2 } \right)^2 - \frac{ 4M_0^2 M_{ll}^2  }{ (M_0^2 - M_{\pi^0}^2)^2  } \right]^{ \frac{3}{2} }
 *
 * @Mll 	mass of dilepton pair
 * @M0 		mass of resonance
 * @Mn 		mass of neutral decay product
 *
 * @return 	PhaseSpace factor
 */ 

double PhaseSapceMassless( double Mll, double M0 );
double PhaseSpaceMassless( double *x, double *par );
double PhaseSpaceMassive( double Mll, double M0, double Mn );
double PhaseSpaceMassive( double *x, double *par );
double FormFactor2( double Mll, double G0, double iL2 );
double FormFactor2( double *x, double *par );

double QED( double Mll, double ml, double Nd );
double QED( double *x, double *par );

double KrollWada( double Mll, double M0, double Mn, double ml, double G0, double iL2, double Nd );
double KrollWada( double *x, double *par );

// RHO
double PhaseSpaceVacuumRho( double Mll, double pT, double T );
double PhaseSpaceVacuumRho( double E, double T );
double PhaseSpaceVacuumRho( double *x, double *p );

double MassVacuumRho( double Mll, double pT, double ml, double gamma0, double gamma2, double T );
double MassVacuumRho( double *x, double *par );


double CrystalBall( double x, double N, double mu, double sig, double n, double alpha );
double CrystalBall( double *x, double *par );

double CrystalBall2( double x, double N, double mu, double sig, double n, double alpha, double m, double beta );
double CrystalBall2( double *x, double *par );

extern TF1 *fTsallisBlastWave_Integrand_r;
extern TF1 *fTsallisBlastWave_Integrand_phi;
extern TF1 *fTsallisBlastWave_Integrand_y;

double TsallisBlastWave_Integrand_y( const double *x, double* p );
double TsallisBlastWave_Integrand_phi(const double *x, double* p);
double TsallisBlastWave_Integrand_r(const double *x, const double *p);
double TsallisBlastWave_Func(const double *x, const double *p);
TF1 *TsallisBlastWave(const char *name, double mass, double beta_max = 0.9, double temp = 0.1, double n = 1., double q = 2., double norm = 1.e6, double ymin=-0.5, double ymax=0.5);


#endif