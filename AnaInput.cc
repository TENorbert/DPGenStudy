#include "AnaInput.h"

AnaInput* AnaInput::m_Instance = NULL ;

//AnaInput::AnaInput( string datacardInput ) {
AnaInput::AnaInput() {

  datacardfile = "DataCard.txt" ;

}

AnaInput::~AnaInput(){

   cout<<" close input "<<endl ;

}

AnaInput* AnaInput::Instance() {

    if (! m_Instance ) {
       m_Instance = new AnaInput( ) ;
    } 
       
    return m_Instance ;

}

void AnaInput::SetDatacard( string datacardInput ) {
 
     datacardfile = datacardInput ;
}

//vector<TTree*> forestData ;
//vector<TTree*> forestMC ;
void AnaInput::LinkForests( TString treeName ){

  cout<<" Linking all trees !!!"<<endl;

  vector<string> fNamesData ;
  GetParameters( "TheData", &fNamesData );
  for ( size_t i =0 ; i< fNamesData.size(); i++ ) {
      TTree* tr = GetTree( fNamesData[i], treeName ) ;
      forestData.push_back( tr );
  }

  vector<string> fNamesMC ;
  GetParameters( "TheMC", &fNamesMC );
  for ( size_t i =0 ; i< fNamesMC.size(); i++ ) {
      TTree* tr = GetTree( fNamesMC[i], treeName ) ;
      forestMC.push_back( tr );
  }

}

// GetForest , run first
void AnaInput::GetForest( string DataSet, TString treeName ) {

    cout<<"  =>>> getting a forest of "<< treeName <<endl ;
    vector<string> fileList;
    GetParameters( DataSet , &fileList );

    vector<TTree*> forest ;
    for ( size_t i =0 ; i< fileList.size(); i++ ) {
        TTree* tr = GetTree( fileList[i], treeName ) ;
        forestData.push_back( tr );
    }
}

TTree* AnaInput::TreeMap( string fileName ) {

    vector<string> f0Names ;
    GetParameters( "TheData", &f0Names );
    vector<string> f1Names ;
    GetParameters( "TheMC", &f1Names );

    TTree* theTr = 0;
    for ( size_t i=0; i< f0Names.size(); i++ ) {
        if ( f0Names[i] == fileName ) theTr = forestData[i] ;
    }
    for ( size_t i=0; i< f1Names.size(); i++ ) {
        if ( f1Names[i] == fileName ) theTr = forestMC[i] ;
    }


    return theTr ;
}

// get the tree from a specific file(s)
TTree* AnaInput::GetTree( string fName, TString treeName, TFile* file  ) {
  
  TTree* tr = 0;

  string filePath ;
  GetParameters( "RootFiles", &filePath );

  TString theFileName ;
  TChain* theChain = new TChain( treeName ) ;

  if ( fName[ fName.size()-1 ] == '+'  ) {
     string ChainName = fName.substr( 0, fName.size()-1 ) + "Chain"  ;
     vector<string> chainlist;
     GetParameters( ChainName, &chainlist );
     cout<<" * fileName+ = "<< ChainName <<endl;
     for ( size_t j=0; j< chainlist.size(); j++) {
         theFileName = filePath + chainlist[j]+".root" ;
         //cout<<" ** fileName = "<< theFileName <<endl;
         theChain->Add( theFileName );
     }
  } else {
    theFileName = filePath + fName+".root" ;
    cout<<" * fileName = "<< theFileName <<endl;
    theChain->Add( theFileName );
    //if ( file == NULL ) file = TFile::Open( theFileName );
    //tr = (TTree*) file->Get( treeName );

  }
  tr = theChain ;

  return tr ;
}


vector<double> AnaInput::NormalizeComponents( string cfgFile ){

  if ( cfgFile == "" ) cfgFile = datacardfile ;

  double lumi ;
  GetParameters("Lumi", &lumi, cfgFile );

  vector<double> nEvents ;
  GetParameters( "NEvents" , &nEvents, cfgFile );
  vector<double> xsec;
  GetParameters("XSection", &xsec, cfgFile );
  vector<double> Eff;
  GetParameters("Eff", &Eff, cfgFile )  ;

  vector<double> nSkim;
  GetParameters("NSkim", &nSkim, cfgFile )  ;

  vector<double> normV ;
  for ( size_t i=0; i < xsec.size(); i++ ) {

     double skimEff = ( nSkim.size() > 0 ) ? nSkim[i]/nEvents[i] : 1. ;
     double nBase = xsec[i]*Eff[i]*skimEff ;
     double Scal = (nBase*lumi) / nEvents[i] ;
     //printf(" N: %.1f , X: %.1f , Eff: %.1f , NSkim: %.1f = Scal : %.4f \n", nEvents[i], xsec[i], Eff[i], nSkim[i], Scal ) ;
     printf(" N: %.1f , X: %.1f , Eff: %.1f  = Scale : %.4f \n", nEvents[i], xsec[i], Eff[i],  Scal ) ;
     //cout<<" Normalization =  "<< Scal << endl;
     normV.push_back( Scal ) ;
  } 

  if ( normV.size() < 1) normV.push_back( 1. ) ;

  return normV ;
}

// Methods to read DataCard.txt
void AnaInput::GetParameters(string paraName, int* thePara, string cfgFile ){

     if ( cfgFile == "" ) cfgFile = datacardfile ;

     fstream paraFile( cfgFile.c_str() );
     if ( !paraFile.is_open() )  cout<<" file opened error => check file path and the folder "<<endl;
     string  line;
     string  getName;
     string  getValue;
     size_t  pos ;
     size_t  vpos ;

     bool gotIt = false ;
     while ( getline(paraFile, line) ) {
           if ( line[0] == '#' ) continue ;

           pos = line.find( paraName );
           vpos = pos + paraName.size() + 2;
           if ( pos < line.npos ) {
              string str_end = line.substr(vpos-1, 1) ;
              if ( str_end == ' ' || str_end == '=') {
                 getName  = line.substr( pos, paraName.size() );
                 getValue = line.substr( vpos );
                 *thePara = atoi( getValue.c_str() );
                 //cout<< paraName <<" = "<< *thePara << endl;
                 gotIt = true;
              }
           }
           if ( gotIt ) break ;
     }
     paraFile.close();
}

void AnaInput::GetParameters(string paraName, double* thePara, string cfgFile ){

     if ( cfgFile == "" ) cfgFile = datacardfile ;

     fstream paraFile( cfgFile.c_str() );
     if ( !paraFile.is_open() )  cout<<" file opened error => check file path and the folder "<<endl;
     string  line;
     string  getName;
     string  getValue;
     size_t  pos ;
     size_t  vpos ;

     bool gotIt = false ;
     while ( getline(paraFile, line) ) {
           if ( line[0] == '#' ) continue ;

           pos = line.find( paraName );
           vpos = pos + paraName.size() + 2;
           if ( pos < line.npos ) {
              string str_end = line.substr(vpos-1, 1) ;
              if ( str_end == ' ' || str_end == '=') {
                 getName  = line.substr( pos, paraName.size() );
                 getValue = line.substr( vpos );
                 *thePara = atof( getValue.c_str() );
                 //cout<< paraName <<" = "<< *thePara << endl;
                 gotIt = true ;
              }
           }
           if ( gotIt ) break ;
     }
     paraFile.close();
}

void AnaInput::GetParameters(string paraName, string* thePara, string cfgFile ){

     if ( cfgFile == "" ) cfgFile = datacardfile ;

     fstream paraFile( cfgFile.c_str() );
     if ( !paraFile.is_open() )  cout<<" file opened error => check file path and the folder "<<endl;
     string  line;
     string  getName;
     size_t  pos ;
     size_t  vpos ;

     bool gotIt = false ;
     while ( getline(paraFile, line) ) {
           if ( line[0] == '#' ) continue ;

           pos = line.find( paraName );
           vpos = pos + paraName.size() + 2;

           if ( pos < line.npos ) {
              string str_end = line.substr(vpos-1, 1) ;
              if ( str_end == ' ' || str_end == '=') {
                 //cout<<" pos = "<< pos <<endl;
                 getName  = line.substr( pos, paraName.size() );
                 //*thePara = line.substr( vpos );
                 //cout<< paraName <<" = "<< *thePara << endl;
                 string strTmp = line.substr( vpos );
                 for (string::iterator it = strTmp.begin(); it< strTmp.end(); it++) {
                     if ( (*it) != ',' && (*it) != ' ' && (*it) != '(' && (*it) != ')' && (*it) != '=') thePara->push_back( *it );
                 }
                 gotIt = true ;
              }
           }
           if ( gotIt ) break;
     }
     paraFile.close();
}

void AnaInput::GetParameters(string paraName, vector<double>* thePara, string cfgFile ){

     if ( cfgFile == "" ) cfgFile = datacardfile ;

     fstream paraFile( cfgFile.c_str() );
     if ( !paraFile.is_open() )  cout<<" file opened error => check file path and the folder "<<endl;
     string  line;
     string  getName;
     string  getValue;
     size_t  pos ;
     size_t  vpos ;
     vector<double>  vvec;

     while ( getline(paraFile, line) ) {
           if ( line[0] == '#' ) continue ;

           pos = line.find( paraName );
           vpos = pos + paraName.size() + 1;
           if ( pos < line.npos ) {
              getName  = line.substr( pos, paraName.size() );
              string arrVal = line.substr( vpos );
              if ( arrVal[0] != '=' && arrVal[0] != ' ' ) continue;
	      int vidx = 0;
	      string vtemp ;
	      //cout<< paraName <<" = ( " ;
              for (string::iterator it = arrVal.begin(); it< arrVal.end(); it++) {
                  if ( (*it) != ',' && (*it) != ' ' && (*it) != '(' && (*it) != ')' && (*it) != '=') vtemp.push_back( *it );
                  if ( (*it) == ',' || (*it) == ')' ) { 
                     if ( vtemp.size() > 0 ) vvec.push_back( atof( vtemp.c_str() ) ) ;
		     //cout<< vtemp << *it;
		     vidx++ ;
		     vtemp.clear() ;
                  }
              }
              *thePara = vvec ;
           }
     }
     paraFile.close();

} 

void AnaInput::GetParameters(string paraName, vector<string>* thePara, string cfgFile ){

     if ( cfgFile == "" ) cfgFile = datacardfile ;

     fstream paraFile( cfgFile.c_str() );

     if ( !paraFile.is_open() )  cout<<" file opened error => check file path and the folder "<<endl;
     string  line;
     string  getName;
     string  getValue;
     size_t  pos ;
     size_t  vpos ;
     vector<string>  vvec;

     while ( getline(paraFile, line) ) {
           if ( line[0] == '#' ) continue ;

           pos = line.find( paraName );
           vpos = pos + paraName.size() ;
           if ( pos < line.npos ) {
              getName  = line.substr( pos, paraName.size() );
              string arrVal = line.substr( vpos );
              if ( arrVal[0] != '=' && arrVal[0] != ' ' ) continue;
	      int vidx = 0;
	      string vtemp ;
	      //cout<< paraName <<" = ( " ;
              for (string::iterator it = arrVal.begin(); it< arrVal.end(); it++) {
                  if ( (*it) != ',' && (*it) != ' ' && (*it) != '(' && (*it) != ')' && (*it) != '=') vtemp.push_back( *it );
                  if ( (*it) == ',' || (*it) == ')' ) { 
                     if ( vtemp.size() > 0 ) vvec.push_back( vtemp ) ;
		     //cout<< vtemp << *it;
		     vidx++ ;
		     vtemp.clear() ;
                  }
              }
              *thePara = vvec ;
           }
     }
     paraFile.close();

}
 
void AnaInput::GetParameters(string paraName, vector<int>* thePara, string cfgFile ){

     if ( cfgFile == "" ) cfgFile = datacardfile ;

     fstream paraFile( cfgFile.c_str() );
     if ( !paraFile.is_open() )  cout<<" file opened error => check file path and the folder "<<endl;
     string  line;
     string  getName;
     string  getValue;
     size_t  pos ;
     size_t  vpos ;
     vector<int>  vvec;

     while ( getline(paraFile, line) ) {
           if ( line[0] == '#' ) continue ;

           pos = line.find( paraName );
           vpos = pos + paraName.size() ;
           if ( pos < line.npos ) {
              getName  = line.substr( pos, paraName.size() );
              string arrVal = line.substr( vpos );
              if ( arrVal[0] != '=' && arrVal[0] != ' ' ) continue;
	      //int vidx = 0;
	      string vtemp ;
	      //cout<< paraName <<" = ( " ;
              for (string::iterator it = arrVal.begin(); it< arrVal.end(); it++) {
                  if ( (*it) != ',' && (*it) != ' ' && (*it) != '(' && (*it) != ')' && (*it) != '=') vtemp.push_back( *it );
                  if ( (*it) == ',' || (*it) == ')' ) { 
                     if ( vtemp.size() > 0 ) vvec.push_back( atoi( vtemp.c_str() ) ) ;
		     //cout<< vtemp << *it;
		     //vidx++ ;
		     vtemp.clear() ;
                  }
              }
              *thePara = vvec ;
           }
     }
     paraFile.close();

}

 
// Weighting for GEN-SIM format of GMSB Signal MC , pt is neutralino's pt, ct is its lifetime in the rest frame.
// pt bin is 25 GeV (from 0 ~ 500 GeV), ct bin is 200 mm ( 0 ~ 4000 mm)
double AnaInput::RecoWeight( double pT, double ct ) {

     int x = ( pT/25)  ;
     if ( x > 14 ) x = 14 ;
     int y = ( ct/200)  ;
     if ( y > 19 ) y = 19 ;
     int k = (y*15) + x ;

     double eff180[300] = {
 0.00883, 0.00202, 0.00007, 0.00006, 0.00016, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.07921, 0.03803, 0.01314, 0.00590, 0.00175, 0.00158, 0.00160, 0.00000, 0.00076, 0.00042, 0.00023, 0.00134, 0.00000, 0.00000, 0.00000,
 0.06286, 0.07152, 0.04148, 0.02171, 0.01337, 0.00527, 0.00371, 0.00405, 0.00239, 0.00176, 0.00000, 0.00186, 0.00169, 0.00000, 0.00000,
 0.08642, 0.11777, 0.08223, 0.05737, 0.04098, 0.02742, 0.01214, 0.00746, 0.00863, 0.00791, 0.00524, 0.00557, 0.00180, 0.00000, 0.00128,
 0.15094, 0.12662, 0.11846, 0.10376, 0.06917, 0.06004, 0.02487, 0.01679, 0.01541, 0.01248, 0.00621, 0.00681, 0.00276, 0.00000, 0.00000,
 0.20513, 0.13992, 0.16803, 0.15035, 0.13655, 0.09185, 0.07235, 0.02441, 0.01782, 0.00349, 0.00275, 0.00181, 0.00000, 0.00000, 0.00000,
 0.17857, 0.17857, 0.16817, 0.19282, 0.14246, 0.11058, 0.08717, 0.06250, 0.02199, 0.00717, 0.00789, 0.00000, 0.00000, 0.00000, 0.00000,
 0.20000, 0.14400, 0.15884, 0.16745, 0.18861, 0.14891, 0.12462, 0.09236, 0.05870, 0.01333, 0.01010, 0.00000, 0.00000, 0.00000, 0.00000,
 0.16667, 0.15455, 0.10599, 0.13580, 0.17279, 0.15584, 0.12842, 0.06301, 0.06867, 0.01754, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.10526, 0.19355, 0.13125, 0.14610, 0.20117, 0.16566, 0.11816, 0.07538, 0.04000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.23077, 0.15556, 0.21212, 0.15909, 0.17407, 0.17176, 0.16749, 0.07792, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.06667, 0.24390, 0.19835, 0.14368, 0.11538, 0.11834, 0.08271, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.20930, 0.13043, 0.12057, 0.09655, 0.07576, 0.09375, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.21875, 0.21687, 0.16541, 0.12687, 0.07317, 0.50000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.30769, 0.12903, 0.12963, 0.06173, 0.05882, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.22222, 0.10417, 0.08696, 0.12698, 0.28571, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.07143, 0.12500, 0.12727, 0.12500, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.06250, 0.20513, 0.08108, 0.14286, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.25000, 0.33333, 0.16216, 0.02778, 0.20000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.10000, 0.11034, 0.13158, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000
     } ;

// Efficiency for 140
     double eff140[300] = {
 0.00188, 0.00010, 0.00008, 0.00000, 0.00000, 0.00000, 0.00000, 0.00009, 0.00011, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.02296, 0.00880, 0.00124, 0.00060, 0.00018, 0.00018, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.06087, 0.01923, 0.00686, 0.00409, 0.00174, 0.00087, 0.00063, 0.00000, 0.00040, 0.00000, 0.00057, 0.00000, 0.00000, 0.00000, 0.00000,
 0.03139, 0.03173, 0.02456, 0.00916, 0.00421, 0.00367, 0.00200, 0.00044, 0.00000, 0.00134, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.03817, 0.05654, 0.04899, 0.02965, 0.01613, 0.00546, 0.00489, 0.00256, 0.00080, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.06329, 0.05580, 0.07518, 0.05872, 0.03016, 0.01835, 0.00461, 0.00192, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.05634, 0.07778, 0.08703, 0.07957, 0.04671, 0.02857, 0.01034, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.10000, 0.07895, 0.07595, 0.08222, 0.08653, 0.04981, 0.01402, 0.00298, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.10256, 0.08017, 0.10337, 0.08756, 0.06835, 0.05903, 0.00567, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.09302, 0.05584, 0.08088, 0.09234, 0.08008, 0.06052, 0.02941, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.08333, 0.08844, 0.07074, 0.08633, 0.07500, 0.00995, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.05932, 0.08594, 0.09496, 0.07661, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.06250, 0.09434, 0.06726, 0.10121, 0.03871, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.05263, 0.06604, 0.05093, 0.05882, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.07692, 0.07595, 0.06870, 0.09420, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.00000, 0.02400, 0.02222, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.07547, 0.04808, 0.10526, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.03774, 0.04124, 0.05128, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.05128, 0.03509, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.01859, 0.05435, 0.11111, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000
     } ;

     double eff120[300] = {
 0.00046, 0.00020, 0.00005, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00534, 0.00266, 0.00017, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00580, 0.00443, 0.00311, 0.00097, 0.00069, 0.00024, 0.00000, 0.00031, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.02640, 0.00633, 0.00498, 0.00364, 0.00201, 0.00034, 0.00037, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.01951, 0.01831, 0.01857, 0.00551, 0.00422, 0.00225, 0.00112, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.02000, 0.04192, 0.02682, 0.02106, 0.00618, 0.00131, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.06818, 0.04198, 0.04457, 0.04170, 0.01589, 0.00492, 0.00256, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.01010, 0.02989, 0.04393, 0.05983, 0.02378, 0.01025, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.01235, 0.05863, 0.05830, 0.04657, 0.04937, 0.00952, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.03333, 0.02712, 0.04404, 0.04368, 0.03523, 0.00781, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.02222, 0.04348, 0.03421, 0.02575, 0.01736, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.02326, 0.02283, 0.05135, 0.07042, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.02439, 0.03627, 0.05208, 0.04461, 0.03704, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.02500, 0.06719, 0.04217, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.04167, 0.02837, 0.04926, 0.02381, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.08108, 0.05600, 0.03911, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.01176, 0.04167, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.02381, 0.03000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.04000, 0.04878, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00840, 0.02494, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000  
     } ;

// Efficiency for 100
     double eff100[300] = {
 0.00018, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00220, 0.00185, 0.00038, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.00091, 0.00116, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00459, 0.00269, 0.00249, 0.00067, 0.00063, 0.00149, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.00499, 0.00413, 0.00551, 0.00185, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00901, 0.01288, 0.01553, 0.01202, 0.00404, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.01111, 0.00969, 0.02164, 0.01840, 0.00172, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.01316, 0.01278, 0.01923, 0.02985, 0.00615, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.01042, 0.03299, 0.02350, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.01154, 0.02094, 0.03413, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.01575, 0.01597, 0.02286, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.02222, 0.00000, 0.03347, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.00000, 0.01015, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.00606, 0.03846, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.02256, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.01770, 0.03125, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.01351, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00930, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000
     } ;

     double eff_syst7[300] = {
 0.01117, 0.00213, 0.00029, 0.00012, 0.00016, 0.00005, 0.00005, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.06931, 0.04040, 0.01617, 0.00897, 0.00330, 0.00333, 0.00178, 0.00071, 0.00095, 0.00083, 0.00047, 0.00107, 0.00032, 0.00000, 0.00046,
 0.06857, 0.07030, 0.05468, 0.03057, 0.01735, 0.00823, 0.00711, 0.00499, 0.00307, 0.00282, 0.00041, 0.00186, 0.00169, 0.00131, 0.00000,
 0.08642, 0.12190, 0.08426, 0.06232, 0.04952, 0.03589, 0.02088, 0.00653, 0.01015, 0.00949, 0.00640, 0.00627, 0.00270, 0.00000, 0.00256,
 0.15094, 0.13312, 0.11846, 0.09969, 0.06583, 0.07505, 0.03571, 0.02453, 0.02545, 0.01092, 0.00532, 0.00681, 0.00414, 0.00000, 0.00000,
 0.20513, 0.13580, 0.16189, 0.14610, 0.13445, 0.11059, 0.07838, 0.03872, 0.01980, 0.01048, 0.00275, 0.00725, 0.00287, 0.00000, 0.00000,
 0.17857, 0.17857, 0.16817, 0.18526, 0.14106, 0.11058, 0.09322, 0.07781, 0.03079, 0.01792, 0.01316, 0.00000, 0.00000, 0.00000, 0.00000,
 0.20000, 0.14400, 0.15523, 0.16274, 0.17438, 0.13431, 0.11692, 0.09414, 0.06087, 0.02333, 0.02020, 0.00000, 0.00000, 0.00000, 0.00000,
 0.16667, 0.15455, 0.10599, 0.13580, 0.16631, 0.14069, 0.11368, 0.06301, 0.05150, 0.01754, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.10526, 0.19355, 0.13125, 0.14610, 0.20117, 0.16265, 0.11239, 0.05528, 0.02000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.23077, 0.15556, 0.21212, 0.15909, 0.17407, 0.17176, 0.15764, 0.07792, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.06667, 0.24390, 0.19835, 0.14368, 0.11538, 0.11834, 0.08271, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.20930, 0.13043, 0.12057, 0.09655, 0.07576, 0.09375, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.21875, 0.21687, 0.16541, 0.12687, 0.07317, 0.50000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.30769, 0.12903, 0.12963, 0.06173, 0.05882, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.22222, 0.10417, 0.08696, 0.12698, 0.28571, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.07143, 0.12500, 0.12727, 0.12500, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.06250, 0.20513, 0.08108, 0.14286, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.25000, 0.33333, 0.16216, 0.02778, 0.20000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.10000, 0.11034, 0.13158, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000
     };

     double eff_syst8[300] = {
 0.00857, 0.00171, 0.00007, 0.00006, 0.00016, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.07673, 0.03862, 0.01246, 0.00567, 0.00194, 0.00140, 0.00142, 0.00036, 0.00057, 0.00021, 0.00023, 0.00107, 0.00000, 0.00000, 0.00000,
 0.06286, 0.06182, 0.04337, 0.01949, 0.01012, 0.00494, 0.00371, 0.00343, 0.00205, 0.00106, 0.00000, 0.00186, 0.00169, 0.00000, 0.00000,
 0.08642, 0.11570, 0.08122, 0.05524, 0.03756, 0.02592, 0.01068, 0.00559, 0.00710, 0.00527, 0.00524, 0.00557, 0.00180, 0.00000, 0.00256,
 0.15094, 0.12662, 0.12923, 0.10376, 0.06500, 0.05718, 0.01658, 0.01679, 0.01273, 0.00702, 0.00532, 0.00568, 0.00276, 0.00000, 0.00000,
 0.20513, 0.13992, 0.17828, 0.16028, 0.13025, 0.09278, 0.05771, 0.02104, 0.01188, 0.00466, 0.00275, 0.00181, 0.00000, 0.00000, 0.00000,
 0.17857, 0.17857, 0.16817, 0.19849, 0.15084, 0.11772, 0.08475, 0.04847, 0.01613, 0.00358, 0.00263, 0.00000, 0.00000, 0.00000, 0.00000,
 0.20000, 0.14400, 0.15884, 0.16745, 0.19217, 0.15182, 0.12308, 0.06572, 0.03478, 0.00667, 0.02020, 0.00000, 0.00000, 0.00000, 0.00000,
 0.16667, 0.15455, 0.10599, 0.13580, 0.17279, 0.15801, 0.12842, 0.07123, 0.06867, 0.01754, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.10526, 0.19355, 0.13125, 0.14610, 0.20117, 0.16867, 0.12680, 0.07538, 0.04000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.23077, 0.15556, 0.21212, 0.15909, 0.17407, 0.17176, 0.16749, 0.07792, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.06667, 0.24390, 0.19835, 0.14368, 0.11538, 0.11834, 0.08271, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.20930, 0.13043, 0.12057, 0.09655, 0.07576, 0.09375, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.21875, 0.21687, 0.16541, 0.12687, 0.07317, 0.50000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.30769, 0.12903, 0.12963, 0.06173, 0.05882, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.22222, 0.10417, 0.08696, 0.12698, 0.28571, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.07143, 0.12500, 0.12727, 0.12500, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.06250, 0.20513, 0.08108, 0.14286, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.25000, 0.33333, 0.16216, 0.02778, 0.20000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
 0.00000, 0.10000, 0.11034, 0.13158, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000
     };

     int systType = 0 ;
     GetParameters("SystType", &systType );
     int susy_scale= 180 ;
     GetParameters("SUSYScale", &susy_scale );

     double mismatch_gen_reco = 1. / 0.974 ;
     double weight = eff180[k]*mismatch_gen_reco ;
     if ( susy_scale == 140 )  weight = eff140[k]*mismatch_gen_reco ;
     if ( susy_scale == 120 )  weight = eff120[k]*mismatch_gen_reco ;
     if ( susy_scale == 100 )  weight = eff100[k]*mismatch_gen_reco ;

     if ( systType == 7 ) weight = eff_syst7[k]*mismatch_gen_reco ;
     if ( systType == 8 ) weight = eff_syst8[k]*mismatch_gen_reco ;

     if ( weight < 0.  ) weight = 0. ;

     return weight ;
}

/*
// uncertainty for 140
 model  250   +1.31  -1.28
 model 1000   +5.28  -4.71 
 model 2000   +9.54  -8.74 
 model 3000  +11.76 -10.74 
 model 4000  +12.37 -11.28 
 model 6000  +12.17 -10.99

// uncertainty for 120
 model 250  +0.40 -0.40 
 model 500  +1.48 -1.31 
 model 1000  +3.89 -3.47 
 model 2000  +7.13 -6.28 
 model 3000  +8.33 -7.28 
 model 4000  +8.13 -7.07 
 model 6000  +7.90 -6.74 

// uncertainty for 100
 model  250  +0.57 -0.21 
 model  500  +1.69 -1.07 
 model 2000  +5.77 -4.56 
 model 4000  +5.81 -4.41 
 model 6000  +5.09 -3.76 
*/

