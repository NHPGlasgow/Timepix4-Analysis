#include "TimepixTree.C"
//#include "/home/garyp/local/SBS-Analysis/MyStyle.C"

double heartbeat_rollover(double hb){
  //started this function but not sure if it will be needed yet
  double clock = 40 * pow(10,6); //40MHz clock
  int roll = 65536;//a heartbeat is set every roll clock cycles @ 40MHz
  
  return hb;
}


void CheckTimeOrder(){
  
  //SetMyStyle();
  gStyle->SetOptStat(1);
  gROOT->SetBatch(1);
  
  TString rootfile_dir = "/w/work5/eic/Timepix/MainzTests/data.250206/trees/";

  TString file = rootfile_dir+"N55-250206-123249_noclustering.root";
  //file = rootfile_dir+"N55-250206-123249_clustering.root";
  //file = rootfile_dir+"N200-250206-123235.root";
  TFile *f = new TFile(file,"OPEN");
  TTree* T = (TTree*) f->Get("clusterTree");
  TimepixTree *Tpx = new TimepixTree(T);
  int nev = T->GetEntries();
  cout << Form("%i events in file",nev) << endl;
  nev=1000000;
  
  //histograms
  //time of arrival
  TH1D *htoa_raw = new TH1D("htoa_raw","Raw Time of Arrival; toa [ns]",250,-1,-1);
  TH1D *htoa = new TH1D("htoa","Time of Arrival;  toa [ns]",250,-1,-1);
  
  //time over threshold
  TH1D *htot_raw = new TH1D("htot_raw","Time Over Threshold; tot [ns]",250,-1,-1);
  TH1D *htot = new TH1D("htot","Time Over Threshold; tot [ns]",250,0,10);
  
  //positions
  TH1D *hx_raw = new TH1D("hx_raw","x pos; x [m]",250,0,30);
  TH1D *hx = new TH1D("hx","x pos; x [m]",250,0,30);
  
  TH1D *hy_raw = new TH1D("hy_raw","y pos; y [m]",250,0,30);
  TH1D *hy = new TH1D("hy","y pos; y [m]",250,0,30);
  
  TH2D *h2d_xy_raw = new TH2D("h2d_xy_raw","2D xy pos; x [m]; y [m]",250,0,30,250,0,30);
  TH2D *h2d_xy = new TH2D("h2d_xy","2D xy pos; x [m]; y [m]",250,0,30,250,0,30);

  //difference between event and previous event
  //likely hits in same "real" clustering being counted as seperate events?
  TH1D *htdiff_events = new TH1D("htdiff_events","Time difference between consecutive events; #Delta t [ns]",250,-2000,2000);
  TH1D *htdiff_events_totcorr = new TH1D("htdiff_events_totcorr","Time difference between consecutive events with tot corr; #Delta t [ns]",250,-20000,20000);
  TH1D *hxdiff_events = new TH1D("hxdiff_events","#Delta x pos between consecutive events; #Delta x [mm]",100,-30,-30);
  TH1D *hydiff_events = new TH1D("hydiff_events","#Delta y pos between consecutive events; #Delta y [mm]",100,-30,-30);
  TH2D *hxydiff_events = new TH2D("hxydiff_events","Correlation between x and y pos diff for consecutive events; #Delta x [mm]; #Delta y [mm]",100,0,0.5,100,0,0.5);

  TH2D *h2d_tdiff_x_events = new TH2D("h2d_tdiff_x_events","Correlation between time and x pos for consecutive events; #Delta x [mm]; #Delta t [128 ns]",100,-1,-1,100,-1,-1);
  
  
  TH2D *htxdiff_events = new TH2D("htxdiff_events","Correlation between time and x pos diff for consecutive events; #Delta x [mm]; #Delta t [128 ns]",100,-1,-1,100,-1,-1);
  
  
  //positions after event diff time cut
  TH1D *hxdiff_events_tcut = new TH1D("hxdiff_events_tcut","#Delta x pos between consecutive events_tcut; #Delta x [mm]",100,-30,-30);
  TH1D *hydiff_events_tcut = new TH1D("hydiff_events_tcut","#Delta y pos between consecutive events_tcut; #Delta y [mm]",100,-30,-30);
  TH2D *hxydiff_events_tcut = new TH2D("hxydiff_events_tcut","Correlation between x and y pos diff for consecutive events_tcut; #Delta x [mm]; #Delta y [mm]",100,0,0.5,100,0,0.5);  
  
  //hits relative to heartbeat
  TH1D *hHB_raw = new TH1D("hHB_raw","Raw Heartbeat Signal",250,0,pow(10,13));
  TH1D *htoa_hb = new TH1D("htoa_hb","ToA Ref Subtract Heartbeat",250,-1,-1);
  
  //exact names in tree for reference
  //int nhits,col[nhits],row[nhits];
  //long toa[nhits];
  //double tot[nhits];
  //int mincol,maxcol,minrow,maxrow,digitalInput;
  //bool pileup[nhits];
  //double xpos,ypos,cltot;
  //int ctoa[nhits],ctot[nhits],ftoaRise[nhits],ftoaFall[nhits],uftoaStart[nhits],uftoaStop[nhits];
  //long heartbeat[nhits;
  //ulons packet[nhits];
  //int ccltot;
  //double threshold;
  
  //some new variables for analysis
  long toa_l;
  double toa_prev,toa;
  double tot_prev,tot;
  double toa_totcorr_prev, toa_totcorr;
  double x_prev,x;
  double y_prev,y;
  double hb;
  int count=-1;
  for(int ev=0; ev<nev; ev++){
    if(ev%100000==0) cout << ev << " / " << nev << endl;
    
    Tpx->GetEntry(ev);


    if(Tpx->digitalInput != -1) continue;
    //if(Tpx->row[0]<=256) continue;
    //if(Tpx->row[0]>256) continue;
    count++;


    //set previous events values
    toa_prev=toa;
    x_prev=x;
    y_prev=y;
    toa_totcorr_prev = toa_totcorr;
    //then update values to this event
    toa = (double) Tpx->toa[0]*25.0/128.0;
    tot = (double) Tpx->tot[0]*25.0;
    toa_totcorr = toa+tot;
    hb = (double) Tpx->heartbeat[0]*25.0 / pow(10.0,9.0);
    x = (double) Tpx->xpos;
    y = (double) Tpx->ypos;
    double tdiff = toa - toa_prev;
    double tdiff_totcorr = toa_totcorr - toa_totcorr_prev;
    double xdiff = x - x_prev;
    double ydiff = y - y_prev;
    double toa_hb = toa-hb;
    
    if (toa <= toa_prev){
      //cout << tdiff << endl;
      //continue;
    }
    
    if(ev==0 || count==0) continue;
    
    htoa_raw->Fill(Tpx->toa[0]);
    htoa->Fill(toa);
    
    htot_raw->Fill(Tpx->tot[0]);
    htot->Fill(tot);
    
    hx_raw->Fill(x);
    hy_raw->Fill(y);
    h2d_xy_raw->Fill(x,y);
    
    htdiff_events->Fill(tdiff);
    htdiff_events_totcorr->Fill(tdiff_totcorr);
    hxdiff_events->Fill(xdiff);
    hydiff_events->Fill(ydiff);
    hxydiff_events->Fill(xdiff,ydiff);
    htxdiff_events->Fill(xdiff,tdiff);
    
    //if "events" less than  second apart then very likely same cosmic event
    if (fabs(tdiff)>200) continue;
    hxdiff_events_tcut->Fill(xdiff);
    hydiff_events_tcut->Fill(ydiff);
    hxydiff_events_tcut->Fill(xdiff,ydiff);
    
    hHB_raw->Fill(Tpx->heartbeat[0]);
    htoa_hb->Fill(toa_hb);
    
  }
  
  new TCanvas();
  htoa_raw->Draw();
  gPad->Print("htoa_raw.png");
  
  htoa->Draw();
  gPad->Print("htoa.png");
    
  htot_raw->Draw();
  gPad->Print("htot_raw.png");
  
  htot->Draw();
  gPad->Print("htot.png");
  
  hx_raw->Draw();
  gPad->Print("hdx_raw.png");
  hy_raw->Draw();
  gPad->Print("hdy_raw.png");
  h2d_xy_raw->Draw("colz");
  gPad->Print("h2d_xy_raw.png");
  
  gPad->SetLogy();
  htdiff_events->Draw();
  htdiff_events_totcorr->SetLineColor(kRed);
  //htdiff_events_totcorr->Draw("same");
  gPad->Print("htdiff_events.png");
  
  hxdiff_events->Draw();
  gPad->Print("hxdiff_events.png");
  hydiff_events->Draw();
  gPad->Print("hydiff_events.png");
  gPad->SetLogy(0);
  hxydiff_events->Draw("colz");
  gPad->Print("hxydiff_events.png");
  
  gPad->SetLogy();
  hxdiff_events_tcut->Draw();
  gPad->Print("hxdiff_events_tcut.png");
  hydiff_events_tcut->Draw();
  gPad->Print("hydiff_events_tcut.png");
  gPad->SetLogy(0);
  hxydiff_events_tcut->Draw("colz");
  gPad->Print("hxydiff_events_tcut.png");
    
  hHB_raw->Draw();
  gPad->Print("hHB_raw.png");
  
  htoa_hb->Draw();
  gPad->Print("htoa_hb.png");
  
  /*
  htxdiff->Draw("colz");
  htcoin->Draw();
  hdx->Draw();
  hdy->Draw();
  */
}
