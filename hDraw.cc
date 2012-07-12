#include "hDraw.h"
//#include "MathFunctions.h"
hDraw::hDraw( string datacardfile ){

  Input  = new AnaInput( datacardfile );

  Input->GetParameters("PlotType",      &plotType ) ;
  Input->GetParameters("Path",          &hfolder ) ;

  gSystem->mkdir( hfolder.c_str() );


  c1  = new TCanvas("c1","", 800, 600);
  c2  = new TCanvas("c2","", 800, 600);
  c3  = new TCanvas("c3","", 800, 600);

  func1 = NULL ;
}

hDraw::~hDraw(){

  delete c1 ;
  delete c2 ;
  delete c3 ;
  delete Input ;
  cout<<" Draw ! "<<endl ;

}

void hDraw::Draw( TH1D* h1, string plotName, string xTitle, string yTitle, string logY, float statY, int color, TLegend* leg ) {

      c1->Clear();
      c1->SetFillColor(10);
      c1->SetFillColor(10);
      c1->SetLogy(0);

      gStyle->SetOptStat("eoumi");
      if ( strncasecmp( "logY", logY.c_str(), logY.size() ) ==0 && logY.size() > 0 ) c1->SetLogy() ;

      gStyle->SetStatY( statY  );
      gStyle->SetStatTextColor( color );


      h1->GetXaxis()->SetTitle( xTitle.c_str() );
      h1->GetYaxis()->SetTitle( yTitle.c_str() );
      h1->SetLineColor( color ) ;

      c1->cd();
      h1->Draw() ;
      c1->Update();

      if ( leg != NULL ) leg->Draw("sames") ;

      if ( plotName != "" || plotName != " " ) {
         TString plotname1 = hfolder + plotName + "."+plotType ;
         c1->Print( plotname1 );
         //if ( func1 != NULL ) func1 = NULL ;
      }

}

void hDraw::DrawAppend( TH1D* h1, string plotName, float statY, int color, TLegend* leg ) {

      gStyle->SetStatY( statY  );
      gStyle->SetStatTextColor( color );

      h1->SetLineColor( color ) ;

      c1->cd();
      h1->DrawCopy("sames") ;
      c1->Update();

      if ( leg != NULL ) leg->Draw("sames") ;

      if ( plotName != "" || plotName != " " ) {
         TString plotname1 = hfolder + plotName + "."+plotType ;
         c1->Print( plotname1 );
         //if ( func1 != NULL ) func1 = NULL ;
      }
}

void hDraw::CreateNxM( string plotName , int N, int M ) {

      c2->Clear();
      c2->SetFillColor(10);
      c2->SetFillColor(10);
      c2->Divide(N, M) ;

      gStyle->SetOptStat("eroumi");

      plotname2 = hfolder + plotName + "."+plotType ;
}

void hDraw::DrawNxM( int id, TH1D* h1, string xTitle, string yTitle, string logY, int color, bool close  ) {

      c2->cd( id );

      gPad->SetLogy(0) ;
      if ( strncasecmp( "logY", logY.c_str(), logY.size() ) ==0  && logY.size() > 0 ) gPad->SetLogy(1) ;

      gStyle->SetStatTextColor( color );
      h1->SetLineColor( color ) ;

      h1->GetXaxis()->SetTitle( xTitle.c_str() );
      h1->GetYaxis()->SetTitle( yTitle.c_str() );

      h1->Draw() ;
      c2->Update();

      if ( close ) {
         c2->Print( plotname2 );
      }
}


void hDraw::FitNDraw( TH1D* h1, string plotName, string xTitle, string yTitle, string logY, float statY, int color, TLegend* leg ) {

      // Plot the histogram
      c1->Clear();
      c1->SetFillColor(10);
      c1->SetFillColor(10);
      c1->SetLogy(0);

      gStyle->SetOptStat("emio");
      if ( strncasecmp( "logY", logY.c_str(), logY.size() ) ==0 && logY.size() > 0 ) c1->SetLogy() ;

      gStyle->SetStatY( statY  );
      gStyle->SetStatTextColor( color );

      h1->GetXaxis()->SetTitle( xTitle.c_str() );
      h1->GetYaxis()->SetTitle( yTitle.c_str() );
      h1->SetLineColor( color ) ;

      c1->cd();
      h1->Draw() ;
      c1->Update();

      // Define the fitting function and fit
      h1->Fit( func1, "R" );
      func1->Draw("same") ;
      c1->Update();

      if ( leg != NULL ) leg->Draw("sames") ;

      if ( plotName != "" || plotName != " " ) {
         TString plotname1 = hfolder + plotName + "."+plotType ;
         c1->Print( plotname1 );
      }
}

void hDraw::FitNDrawAppend( TH1D* h1, string plotName, float statY, int color, TLegend* leg ) {

      gStyle->SetStatY( statY  );
      gStyle->SetStatTextColor( color );

      h1->SetLineColor( color ) ;

      c1->cd();
      h1->DrawCopy("sames") ;
      c1->Update();

      h1->Fit( func1, "R" );
      func1->Draw("same") ;
      c1->Update();

      if ( leg != NULL ) leg->Draw("sames") ;

      if ( plotName != "" || plotName != " " ) {
         TString plotname1 = hfolder + plotName + "."+plotType ;
         c1->Print( plotname1 );
         if ( func1 != NULL ) func1 = NULL ;
      }
}

void hDraw::SetFitParameter( string fitFunc_, double fitMin_, double fitMax_ , int nFitPara_,  double* initVals, int color ) {

   fitMin = fitMin_ ;
   fitMax = fitMax_ ;
   nFitPara = nFitPara_ ;

   gStyle->SetOptFit(111);

   if ( strncasecmp( "Gaus", fitFunc_.c_str(), fitFunc_.size() ) == 0  )  {
      func1 = new TF1("func1", &hDraw::fitGS, fitMin, fitMax, nFitPara );  
      func1->SetParameter( 0, initVals[0] );
      func1->SetParameter( 1, initVals[1] );
      func1->SetParameter( 2, initVals[2] );
      func1->SetLineColor( color );
   }
   if ( strncasecmp( "Exp", fitFunc_.c_str(), fitFunc_.size() ) == 0  )  {
      func1 = new TF1("func1", &hDraw::fExp , fitMin, fitMax, nFitPara );
      func1->SetParameter( 0, initVals[0] );
      func1->SetParameter( 1, initVals[1] );
      func1->SetLineColor( color );
   }

}

void hDraw::SetFitParameter( string fitFunc_, TH1D* h1, double fitMin_, double fitMax_ , int nFitPara_, int color ) {

   if ( fitMin_ != fitMax_ ) fitMin = fitMin_ ;
   if ( fitMin_ != fitMax_ ) fitMax = fitMax_ ;
   nFitPara = nFitPara_ ;

   gStyle->SetOptFit(111);

   if ( strncasecmp( "Gaus", fitFunc_.c_str(), fitFunc_.size() ) == 0  )  {
      double maxv = h1->GetMaximum() ;
      double mean = h1->GetMean() ;
      double rms  = h1->GetRMS() ;
      if ( fitMin_ == fitMax_ ) fitMin = mean - (2.*rms) ;
      if ( fitMin_ == fitMax_ ) fitMax = mean + (2.*rms) ;

      func1 = new TF1("func1", &hDraw::fitGS, fitMin, fitMax, nFitPara );  
      func1->SetParameter( 0, maxv );
      func1->SetParameter( 1, mean );
      func1->SetParameter( 2, rms  );
      func1->SetLineColor( color );
   }

}

// Drawing Efficiency Plot from two histograms    
void hDraw::EffPlot( TH1D* hCut, TH1D* hAll, double minBinContent, int beginBin, int endBin, string graphName ) {

   endBin = ( endBin == -1 ) ? hAll->GetNbinsX() : endBin ;

   double ba = 0 ;
   double bc = 0 ;
   double x  = 0 ;
   double rbin  = 0 ;
   double rbin_ = 0 ;
   vector<double> baV ;
   vector<double> bcV ;
   vector<double> xV ;
   vector<double> xW ;
   vector<double> errL;
   vector<double> errH;
   double bW = hCut->GetBinWidth(1) ;
   cout<<" bin width = " << bW <<endl ;
   // Accumulate bin contain
   for ( int i= beginBin ; i<= endBin; i++ ) {
       double bc_ = hCut->GetBinContent(i) ;
       double ba_ = hAll->GetBinContent(i) ;
       double x_  = hAll->GetBinCenter(i) ;
       // rebin the histogram in order to have consistence statistic for each bin
       if ( ba < minBinContent || ba < bc || rbin < rbin_ ) {
          ba += ba_ ;
          bc += bc_ ;
          x  +=  x_ ;
          rbin++ ;
       }
       //if ( ba >= minBinContent || i == endBin ) {
       else {
          bcV.push_back( bc ) ;
          baV.push_back( ba ) ;
          xV.push_back( x / rbin )  ;
          xW.push_back( rbin * bW / 2.) ;
          // sc's method to calculate error
          pair<double,double> errs = EffError( ba, bc ) ;
          errH.push_back( errs.first ) ;
          errL.push_back( errs.second ) ;
          rbin_ = rbin ;
          //cout<<" x: "<< x/rbin <<" rb: "<< rbin <<" bW:"<< (rbin*bW) / 2. << "  bc: "<< bc <<"  ba: "<< ba ;
          //cout<<" eff:"<< bc/ba <<" + "<< errs.first <<" - "<< errs.second << endl ;
          bc   = 0 ;
          ba   = 0 ;
          x    = 0 ;
          rbin = 0 ;
       }
   }

   cout<<" nbins = "<< xV.size() <<endl ;

   // calculate proper error
   int nbins = (int) xV.size() ;
   TH1D* hTop      = new TH1D("hTop", " ",  nbins,  0, nbins );
   TH1D* hBot      = new TH1D("hBot", " ",  nbins,  0, nbins );
   for ( int i=0; i< nbins ; i++ ) {
       hTop->SetBinContent( i+1 , bcV[i] ) ;
       hBot->SetBinContent( i+1 , baV[i] ) ;
   }
   TGraphAsymmErrors* gRatio = new TGraphAsymmErrors();
   gRatio->BayesDivide( hTop, hBot );

   const int sz = (int) xV.size() ;
   Double_t eYL[sz], eYH[sz], fX[sz], eX[sz];
   Double_t* fY = gRatio->GetY() ;
   Double_t yA[sz], eY_L[sz], eY_H[sz] ;  // for SC's method
   for ( Int_t i=0 ; i< sz ; i++) {
       fX[i]  = xV[i] ;
       eX[i]  = xW[i] ;
       eYL[i] = gRatio->GetErrorYlow(i) ;
       eYH[i] = gRatio->GetErrorYhigh(i) ;
       // for SC's method
       yA[i]   = bcV[i] / baV[i] ;
       eY_L[i] = errL[i] ;
       eY_H[i] = errH[i] ;
   }

   TGraphAsymmErrors* gr  = new TGraphAsymmErrors( sz, fX, fY, eX, eX, eYL, eYH );
   TGraphAsymmErrors* gr1 = new TGraphAsymmErrors( sz, fX, yA, eX, eX, eY_L, eY_H ); // SC's result !

   TCanvas* c0 = new TCanvas("c0","", 800, 700);
   c0->SetFillColor(10);
   c0->SetFillColor(10);
   gPad->SetGridx();
   gPad->SetGridy();
   c0->SetLeftMargin(0.15);
   c0->SetRightMargin(0.12);
   c0->SetTopMargin(0.1);
   c0->SetBottomMargin(0.12);

   c0->cd();
   gStyle->SetOptStat(kTRUE);
   gStyle->SetOptFit(111);

   gr->SetMaximum( 1.1 );
   gr->SetMinimum( 0.0 );
   gr->SetMarkerStyle(20);
   gr->SetMarkerSize(1);
   gr->SetLineWidth(2);
   gStyle->SetTitleFontSize(0.04) ;
   gr->SetTitle(" Efficiency in time bins") ;
   gr->GetXaxis()->SetTitleOffset(1.34);
   gr->GetYaxis()->SetTitleOffset(1.39);
   gr->GetXaxis()->SetTitleFont(42);
   gr->GetYaxis()->SetTitleFont(42);
   gr->GetXaxis()->SetTitleSize(0.04);
   gr->GetYaxis()->SetTitleSize(0.04);
   gr->GetXaxis()->SetTitle(" ECAL Time (ns)") ;
   gr->GetYaxis()->SetTitle(" Efficiency ") ;
   gr->Draw("AP");

   c0->Update();
   TString plotname = hfolder + graphName + "Asym."+plotType ;
   c0->Print( plotname );
   
   c0->cd();
   gr1->SetTitle(" Efficiency in time bins") ;
   gr1->SetMaximum( 1.1 );
   gr1->SetMinimum( 0.0 );
   gr1->SetMarkerColor(4);
   gr1->SetMarkerStyle(22);
   gr1->SetMarkerSize(1);
   gr1->SetLineWidth(2);
   gr1->GetXaxis()->SetTitle(" ECAL Time (ns)") ;
   gr1->GetYaxis()->SetTitle(" Efficiency ") ;
   gr1->Draw("AP");

   c0->Update();
   plotname = hfolder + graphName + "Asym1."+plotType ;
   c0->Print( plotname );
   
}

// return asymmetry errors <H,L>
pair<double, double> hDraw::EffError( double N_all, double N_pass ) {

    double eff0 = N_pass / N_all ;
    pair<double,double> theErr ;
    if ( eff0 > 1 ) {
       theErr = make_pair( 0 , 0 );
       return theErr ;
    }
    cout<<" N_All: "<<N_all <<"  N_pass: "<< N_pass << endl;
    int nStep = 1000 ;
    double step = 1. / nStep ;
    //cout<<" step = "<< step <<endl;
    Double_t par[3] = { 1, N_all, N_pass } ;
    Double_t xL[1] = { eff0 } ;
    Double_t xR[1] = { eff0 } ;
    double IntEff = (N_all+ 1) * step * BinomialErr( xR, par ) ;
    //cout<<" Eff 0th : "<< BinomialErr( xR, par ) << endl ;  

    bool skipR = false ;
    bool skipL = false ;
    Double_t pR = 0 ;
    Double_t pL = 0 ;
    while ( IntEff < 0.683)  {
        if ( !skipR && xR[0] < 1 ) {
           xR[0] +=  step ;
           pR = BinomialErr( xR, par ) ;
           IntEff += (pR*step*(N_all+1) ) ;
           //cout<<" ("<< xR[0] <<") --> R : "<< IntEff <<"  pR = "<< pR <<endl ;
        }
        if ( !skipL && xL[0] > 0 ) {
           xL[0] -=  step ;
           pL = BinomialErr( xL, par ) ;
           IntEff += (pL*step*(N_all+1) ) ;
           //cout<<" ("<< xL[0] <<") <-- L : "<< IntEff <<"  pL = "<< pL <<endl;
        }
        //cout<<" ------ "<<endl; 
        skipR = ( pL > pR ) ? true : false ;
        skipL = ( pL < pR ) ? true : false ;
        if ( pL == pR ) {
           skipR = false ;
           skipL = false ;
        }
    }
    //cout<<"  ["<< N_pass/N_all <<"] Prob = "<< IntEff <<endl ; 
    //cout<<"                 - "<< (N_pass/N_all) - xL[0] <<endl ;
    //cout<<"                 + "<< xR[0] - (N_pass/N_all) <<endl ;
    theErr      = make_pair( xR[0] - eff0 , eff0 - xL[0] );

    return theErr ;
}

Double_t hDraw::BinomialErr( Double_t* x, Double_t* par ) {

  Double_t N_all  = par[1] ;
  Double_t N_pass = par[2] ;

  //Double_t Bxy = TMath::Beta( ( N_pass + 1 ), ( N_all - N_pass + 1 ) ) ;
  //cout<< " Beta(x,y) = "<< Bxy <<endl ;
  //Double_t Cnk = pow(x[0], N_pass ) * pow( (1-x[0]) , (N_all - N_pass) ) ;
  //Double_t prob = par[0]*Cnk / ( Bxy * (N_all + 1. ) );
  
  double betaPDF = TMath::BetaDist( x[0],  (N_pass + 1) , (N_all - N_pass + 1) ) ;
  Double_t prob = (par[0] / (N_all + 1.) ) * betaPDF ;
  //cout<<" x = "<< x[0] <<" betaPDF:"<<  betaPDF << " p = "<< prob <<endl ;

  if ( x[0] < 0 || x[0] > 1 ) prob = 0 ;

  return prob ;

}


Double_t hDraw::fExp(Double_t *v, Double_t *par) {
      Double_t arg = v[0] /par[1];

      Double_t fitval = par[0]*TMath::Exp( -1*arg);
      return fitval;
}

Double_t hDraw::fitGS(Double_t *x, Double_t *par) {

     Double_t gs_Value = TMath::Gaus(x[0],par[1],par[2]) ;
     Double_t fitV = par[0]*gs_Value ;
     return fitV;
}

void hDraw::EffProbPlot( double N_all, string graphName ){

  cout<<" N_All = "<< N_all <<endl ;
  TF1* fn1 = new TF1("fn1", hDraw::BinomialErr, 0., 1., 3);

  fn1->SetParameter( 0, 1. ) ;

   TCanvas* c0 = new TCanvas("c0","", 800, 600);
   c0->SetFillColor(10);
   c0->SetFillColor(10);
   gPad->SetGridx();

   for ( int i = 0 ; i <= N_all; i++ ) {

      if ( N_all >= 50 && i%5 != 0 ) continue ;
      c0->cd();
      fn1->SetParameter( 1, N_all ) ;
      fn1->SetParameter( 2, i ) ;
      if ( i == 0 ) fn1->Draw() ;
      fn1->DrawCopy("sames") ;
      c0->Update() ;
      //double intP = fn1->Integral( 0, 1 ) ; 
      //cout<<" Int = "<< intP <<endl ; 
   }
   TString plotname = hfolder + graphName + "."+plotType ;
   c0->Print( plotname );

   delete fn1 ;
   delete c0 ;
}
