#ifndef CINT_FUNCTION_LIBRARY_H
#define CINT_FUNCTION_LIBRARY_H

double BreitWigner( double Mll, double M0, double G0 );
double BreitWigner( double *x, double *par );

double PhaseSpace( double *x, double *par );
double PlcGamma( double m, double mReso, double gamma0, double ml, double l );


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
double PhasSpaceMassive( double Mll, double M0, double Mn );
double PhaseSpaceMassive( double *x, double *par );
double FormFactor2( double Mll, double G0, double iL2 );
double FormFactor2( double *x, double *par );

double QED( double Mll, double ml );
double QED( double *x, double *par );

double KrollWada( double Mll, double M0, double Mn, double ml, double G0, double iL2 );
double KrollWada( double *x, double *par );




#endif