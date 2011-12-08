#ifndef _particle_h_
#define _particle_h_
#include <iostream>
#include <cassert>
#include <cmath>
#include "generatormt.h"
#include "vect.h"
#include "pdg.h"
#include "jednostki.h"


using namespace PDG; 

/// elementary particle on a mass shell is represented as a lorentz fourvector
/// and a pdg number. the position r-is included for tracting it in the nucleous
/// ks and orgi are introduced by Jarek for compatibility with the HEP-event structure
class particle : public vect
{

  double _mass;    ///< on shell mass
public:
  vect r;	   ///< position and time relative to the centre of the Nucleus event start time
  int pdg;         ///< pdg code of the particle  
  char ks;         ///< from HEP event
  char orgin;      ///< from HEP event
  double travelled;///< distance from creation // (or norm of initial neutrino)
  int id;          ///< index in the vector 'all'
  int mother;      ///< index of mother in the vector 'all'
  int endproc;     ///< id of process that destroyed the particle
  double fz;
  double pt;  
  int mother_pdg;
  int mother_proc;
  vec mother_momentum;
  double mother_ek;
  int his_nqel;
  int his_nspp;
  int his_ndpp;
  int his_pqel;
  int his_pcex;
  int his_pspp;
  int his_pdpp;
  int his_ptpp;
  
public:
inline particle(){travelled=x=y=z=t=_mass=pdg=mother=0;id=-1;}
inline particle (int code,double mass);      ///< create particle at rest 
inline particle (double mass);               ///< create particle at rest 
inline particle (vect fourmomentum);         ///< create moving particle
inline bool operator==(particle& p2);        ///< check if self->pdg==pdg2
inline bool operator!=(particle& p2);        ///< check if self->pdg!=pdg2
inline double E ();                          ///< particle energy 
inline double energy ();                     ///< particle energy 
inline double Ek();                          ///< particle kinetic energy 
inline double mass ();                       ///< particle mass    
inline double m ();                          ///< particle mass    
inline int charge();                         ///< particle charge
inline double mass2 ();                      ///< mass squared
inline void set_mass (double mass);          ///< set particle mass and adjust energy
inline void set_pdg_and_mass (int pdg, double mass); ///< set particle pdg and mass
inline void set_proton (){set_pdg_and_mass(pdg_proton,mass_proton);}    ///< set particle pdg and mass
inline void set_neutron (){set_pdg_and_mass(pdg_neutron,mass_neutron);} ///< set particle pdg and mass
inline void set_pi (){set_pdg_and_mass(pdg_pi,mass_pi);}                ///< set particle pdg and mass
inline void set_piP (){set_pdg_and_mass(pdg_piP,mass_piP);}             ///< set particle pdg and mass
inline void set_piM (){set_pdg_and_mass(-pdg_piP,mass_piP);}            ///< set particle pdg and mass

inline void set_momentum (vec p);            ///< set particle momentum and adjust energy
inline void set_energy (double E);           ///< set particle energy and adjust momentum
inline vec p();                              ///< the momentum as a 3-vector
inline vect& p4();                           ///< the four-momentum 
inline vec v();                              ///< the velocity as a 3-vector
inline double v2();                          ///< the velocity squared
inline void krok_czasowy(double dt);         ///< move the particle by v*dt                  
inline void krok(double dl);                 ///< move the particle by dl                   
inline double momentum ();                   ///< value of the momentum 
inline double momentum2 ();                  ///< momenutm squared
inline bool is_valid();                             ///< checkes for nan in energy an momentum
inline bool decay (particle & p1, particle & p2);   ///< cause particle to decay into p1 and p2
                                             ///< (preserves the masses of p1 and p2)   
                                     
inline bool lepton ();						///true if lepton   
inline bool pion   ();						///true if pion
inline bool nucleon();						///true if nucleon

inline particle& mom()                      ///mother particle !!! ONLY for elements of the vector all !!!
{  return this[mother-id];
}

inline int proc()                          /// how was the particle created !!! ONLY for elements of the vector all !!!
{  return this[mother-id].endproc;
}


inline int gen()                           /// which generation!!! ONLY for elements of the vector all !!!
{   
	int ile=0;
	particle* p=this;
	while(p->id)
	{
	  p=&this[p->mother-id];
	  ile++;
	}
	return ile;  
}

inline double mcos(){return p().dir()*mom().p().dir();}

inline void wfz(double val);
inline void wpt(double val);
inline void set_mother (particle &p1);
inline void set_new ();
inline double Ek_in_frame(vec v);	///<
inline double fozo();

friend ostream & operator<<(ostream & out,particle p)
{
   return out<<"["<<p.pdg<<"/"<<p._mass<<"]"<<vect(p)<<"@"<<p.r/fermi;
}
};

bool decay(vect v, particle & p1, particle & p2);  /// the total momentum is enough
bool decay2 (vect k,vect p4, particle & p1, particle & p2,double &coef);


bool particle::decay (particle & p1, particle & p2)
{
 return ::decay(p4(),p1,p2);
}



double relative_speed(particle & a,particle &b);	///<

double Ek_in_frame(particle & a,vec v);	///<


/// get cos takes value x with probablity 
/// proportional to (A*x+B)*x*x*x+1
double get_cos(double A, double B, double C, double D, double E, double F, double G, double H);    
/////////////////////////////////////////////////////////////////////////////////////
//                I M P L E M E N T A C J A
/////////////////////////////////////////////////////////////////////////////////////

particle::particle (int code, double mass):vect(mass,0,0,0),_mass (mass),pdg(code)
  {
    id=-1;mother=0;endproc=-1;travelled=0;
  }


particle::particle (double mass):vect(mass,0,0,0),_mass (mass)
  {
    id=-1;mother=0;endproc=-1;travelled=0;
  }
particle::particle (vect fourmomentum):vect (fourmomentum)
  { 
    _mass = sqrt (fourmomentum * fourmomentum);
    id=-1;mother=0;endproc=-1;travelled=0;
  }
inline bool particle::operator==(particle& p2)
{
  return pdg==p2.pdg;
}
inline bool particle::operator!=(particle& p2)
{
  return pdg!=p2.pdg;
}

//    particle(int code):_code(code){}

double particle::energy ()
  {
    return t;
  }

double particle::E()
  {
    return t;
  }

double particle::Ek()
  {
    return t-_mass;
  }

double particle::mass ()
  {
    return _mass;
  }

double particle::m ()
  {
    return _mass;
  }

int particle::charge ()
  {
    return PDG::charge(pdg);
  }

double particle::mass2 ()
  {
    return _mass*_mass;
  }

void particle::set_mass (double mass)
  {
    _mass = mass;
    t=sqrt(x*x+y*y+z*z+_mass*_mass);
  }

void particle::set_pdg_and_mass (int pdg, double mass)
  { this->pdg=pdg;
    _mass = mass;
    t=sqrt(x*x+y*y+z*z+_mass*_mass);
  }


void particle::set_momentum (vec p)
  {
    x=p.x;
    y=p.y;
    z=p.z;
    t = sqrt (p * p + _mass * _mass);
  }
 
  // adjust momentum so that to obtain the given energy
  // on the mass shell
void particle::set_energy (double E)
  {
    t=E; 
    double k=sqrt((E*E-_mass*_mass)
                 /(x*x+y*y+z*z)
		 );
    x*=k;
    y*=k;
    z*=k;
  }

vec particle::p ()
  {
    return vec(x,y,z);
  }

vect& particle::p4 ()
  {
    return *this;
  }

vec particle::v ()
  {
    return vec(x/t,y/t,z/t);
  }
double particle::v2 ()
  {
    return (x*x+y*y+z*z)/t/t;
  }

double particle::momentum ()
  {
    return vec(x,y,z).length();
  }

double particle::momentum2 ()
  {
    return x*x+y*y+z*z;
  }

bool particle::is_valid () ///< no nan errors in mass and 4-momentum
{ return _mass==_mass && x==x && y==y && z==z && t==t;
}

void particle::krok(double dl)
{ 
  double V=v().length(); 
  r+=vect(dl*v()/V, dl/V);
  travelled+=dl;
}

void particle::krok_czasowy(double dt)
{  
  r+=vect(dt*v(),dt);
  travelled+=v().length()*dt;
}

bool particle::lepton()
{
	int x=pdg>0?pdg:-pdg;
	return x >= 11 && x <= 16;
}

bool particle::pion()
{
	 return pdg==111 || pdg==211 || pdg==-211;
}

bool particle::nucleon()
{   
	return pdg==pdg_proton || pdg== pdg_neutron;
}


void particle::wfz(double val)
{
	fz = val/fermi;
}

void particle::wpt(double val)
{
	pt = val;
}

void particle::set_mother (particle &p1)
{
	mother_pdg = p1.pdg;
	mother_proc = p1.endproc;
	mother_momentum = p1.p();
	mother_ek = p1.Ek();
	
	his_nqel = p1.his_nqel;
	his_nspp = p1.his_nspp;
	his_ndpp = p1.his_ndpp;
	his_pqel = p1.his_pqel;
	his_pcex = p1.his_pcex;
	his_pspp = p1.his_pspp;
	his_pdpp = p1.his_pdpp;
	his_ptpp = p1.his_ptpp;
	
	switch(mother_proc)
	{
		case 10: his_nqel++; break;
		case 12: his_nspp++; break;
		case 13: his_ndpp++; break;
		case 20: his_pqel++; break;
		case 21: his_pcex++; break;
		case 22: his_pspp++; break;
		case 23: his_pdpp++; break;
		case 24: his_ptpp++; break;
		default: break;
	}
}

void particle::set_new ()
{
	mother_pdg = 0;
	mother_proc = 0;
	mother_momentum = vec(0,0,0);
	mother_ek = 0;
	
	his_nqel = 0;
	his_nspp = 0;
	his_ndpp = 0;
	his_pqel = 0;
	his_pcex = 0;
	his_pspp = 0;
	his_pdpp = 0;
	his_ptpp = 0;
}

double particle::fozo()
{
	return fz;
}

double particle::Ek_in_frame (vec v)
{
  vect plab = p4 ();
  return plab.boost (-v).t-mass();
}

////////////////////////////////////////////////////////////
///  M2 = squared mass of decaying particle 
///  ma2, mb2 = squared masses of decay products
/// result = squared momentum of decay products (in cms) if decay possible
///          negative number if deacy is impossible
inline double cms_momentum2(double M2,double ma2,double mb2)
{ if(M2<ma2+mb2) return -1; 
  double X=M2+ma2-mb2;
  return X*X/4/M2-ma2;
}

inline double cms_momentum(double M,double ma,double mb)
{ //if(M<ma+mb) return -1;
  double X=(M+ma+mb)*(M-ma-mb)*(M+ma-mb)*(M-ma+mb);
  
  return sqrt(X)/(2*M);
}

#endif