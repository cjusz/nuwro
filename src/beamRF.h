#ifndef _BEAMRF_H_
#define _BEAMRF_H_

#include "rootf.h"
#include "beam.h"
#include "particle.h"
#include "params.h"


class BeamRFCallback
{
public:
	virtual void Done() { /* this method notifies you about end of last file, so shoot goes to next loop */ }
};



class BeamRF : public beam
{
	BeamRFCallback				*	_cb;
	RootFolder<  RootFReader< ND5Event >  >	_folder;
	RootFReader< ND5Event >	* _file;
	int	_nextFile;
	int _nextElem;
//	vector <ND5Event> events;
	ND5Event *events[100000];
	int N;
	double * acum;
	double * acum2;
	int curevent;
	int file_first;
	int file_limit;
	


	/// checks if current element is the last one
	bool LastElem()
	{
		if( _file == 0 )
			return true;	
		return _file->Count() == _nextElem;		
	}
	
	/// checks if current file is the last one
	bool LastFile()
	{
		return _nextFile == _folder.Count()  || _nextFile-file_first+1==file_limit;
	}
	
	/// returns true if any element was found and stored in ,,event''
	/// otherwise false
	bool NextElement( ND5Event & event )
	{
		if( _file != 0 )
		{
			if( LastElem() == false )
			{
				event = *_file->GetEntry( _nextElem );
				_nextElem += 1;
				return true;	
			}
			else if( LastFile() == false )
			{
				_nextElem = 0;
				_file = _folder.File( _nextFile );
				_nextFile += 1;
				if( LastElem() == false )
				{
					event = *_file->GetEntry( _nextElem );
					_nextElem += 1;
					return true;
				}	
			}
		}
		return false;
	}
	
	/// this method prepares object to start reading root files
	/// from the begining
	bool NextLoop()
	{
		if( _folder.Count() > 0 )
		{  file_first=min(max(1,file_first),_folder.Count());
			_file = _folder.File( file_first -1);
			_nextFile = file_first;
			_nextElem = 0;
			return true;
		}
		else
		{
			return false;
		}
	}
///////////////////////////////////////////////////////////////////	
public:
	
	BeamRF(params &p, string treename=string("h3002"), BeamRFCallback * cb = 0 )  
	: _cb( cb ), 
	_folder( p.beam_folder, treename ),
	_file( 0 ),curevent(0),file_first(p.beam_file_first),file_limit(p.beam_file_limit)
	{ 
		if( NextLoop() == false )
		{
			cerr << "BeamRF: wrong input folder or files type\n";
			throw 0;
		}
		//int n=60000000;
		//n/=10;
		//events.reserve(n);
		N=0;
		while(read());
		cout<<endl;
		acum=new double[N];
		acum2=new double[N];
		double prev=0;
		for(int i=0;i<N;i++)
		  acum[i]=prev+=events[i/100000][i%100000].norm;
		prev=0;
		for(int i=0;i<N;i++)
		  {	ND5Event& ev=events[i/100000][i%100000];
		    acum2[i]=prev+=ev.norm*ev.Enu;
		  }
	}
	
	~BeamRF()
	{  
	   for(int i=0;i<(N+100000-1)/100000;i++)
	     delete []events[i];
	   delete []acum;
	   delete []acum2;
	}
	
////////////////////////////////////////////////////////////////////////	
	virtual particle shoot2()
	{ 
		ND5Event e;
		
		/// ask for next element
		if( NextElement( e )  == false )
		{
			/// if was not possible to get next element then back to first file
			NextLoop();
			/// get element
			NextElement( e );
		}
		
		/// if this step ends up with last element then notify client
		if( _cb )
			if( LastElem()  &&  LastFile() )
				_cb->Done();

		/// translate root file event to nuwro particle
		particle p( e.gipart, 0.0 );
		double E=e.Enu*1000;
        	p.r.x = e.xnu*10;
        	p.r.y = e.ynu*10;
        	p.r.z = 0;
     		p.r.t = 0;
        	p.t=E;
        	p.x=e.nnu[0]*E;
        	p.y=e.nnu[1]*E;
        	p.z=e.nnu[2]*E;
        	p.travelled=e.norm;
        	
        		
		return p;
	}	
////////////////////////////////////////////////////////////////////////
	virtual particle shoot(bool dis)
	{  int n=N;//events.size();
		ND5Event e;
		bool weighted=false;
		double *acc=(dis?acum2:acum);
/*		if(weighted)
		{
		e=events[curevent++];
		if(curevent==n)
		  curevent=0;
	    }
	    else */
	    { double x=frandom()*acc[n-1];
	      int i=0,j=n-1;
	      while(i<j)
	      {
			  int s=(i+j)/2;
			  if(x<acc[s])
			    j=s;
			  else
			    i=s+1;  
		  }
		  e=events[i/100000][i%100000];
		}
		particle p( e.gipart, 0.0 );
		double E=e.Enu*1000;
        	p.r.x = e.xnu*10;
        	p.r.y = e.ynu*10;
        	p.r.z = 0;
     		p.r.t = 0;
        	p.t=E;
        	p.x=e.nnu[0]*E;
        	p.y=e.nnu[1]*E;
        	p.z=e.nnu[2]*E;
        	p.travelled=(weighted?e.norm:1);
        	
        		
		return p;
	}	
////////////////////////////////////////////////////////////////////////
	bool read()
	{ 
		ND5Event e;
		
		/// ask for next element
		bool res= NextElement( e );
//        events.push_back(e);
        if(N%100000==0) 
          {
           cerr<< "File:"<<_nextFile<<" "<< N+1<<" beam events read...\r"<<flush;
           events[N/100000]=new ND5Event[100000];
	       }
	     events[N/100000][N%100000]=e;
	     N++;  
        return res;
	}	


};



#endif // _BEAMRF_H_