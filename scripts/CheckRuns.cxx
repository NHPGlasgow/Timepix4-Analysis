#include "TimepixTree.C"
//g. penman 29.05.25
//as far as i can remember this was a hacky real time script to try and
//check runs as they happened in the counting house
//its pretty messy as a result
//i would probably just use the slighly cleaner BeamTestAnalysis.C
//nevertheless something useful might be in this script so I wont delete for now

void CheckRuns(){
  
  //gStyle->SetOptStat(1);

  TString rootfile_dir = "/scratch1/tpx4_config/data/trees/";
  rootfile_dir = "data/";
  //tuesday
  TString file1 = "N200-250204-171615.root";
  //file1 = "N200-250204-111826.root";
  //file1 = "N200-250204-120930.root";
  //file1 = "N200-250204-121018.root"; //seg fault unkown branches
  //file1 = "N200-250204-124314.root";
  //file1 = "N200-250204-124625.root";
  //file1 = "N200-250204-125733.root";
  //file1 = "N200-250204-150442.root";
  //file1 = "N200-250204-152541.root"; //no data
  //file1 = "N200-250204-153130.root"; //data but no hits?
  //file1 = "N200-250204-153611.root"; //double peak with tdiff reversal
  //file1 = "N200-250204-171615.root"; //double peak with tdiff reversal 
  //file1 = "N200-250204-170740.root"; //stacatto, no tdiff peak
  
  //file1 = "N200-250204-181555.root"; //stacatto and double tdiff peak with reversal. This file produces good coincidence between n200 and n55 as well though. TOA for digi and nodigi also shows weird sporadic/periodic structure!?

  //wednesday
  //file1 = "N200-250205-092738.root"; //first ~10M TOA screwed up - bottlenecked at ~1value
  //file1 = "N200-250205-103529.root"; //stacatto and double tdiff peak with reversal
  //file1 = "N200-250205-112018.root"; ////stacatto and double tdiff peak with reversal
  //file1 = "N200-250205-131201.root";
  //file1 = "N200-250205-151101.root"; // no events
  //file1 = "N200-250205-151359.root"; // no events
  //file1 = "N200-250205-151740.root"; //10Mhz pulser test in hall
  //file1 = "N200-250205-160807.root"; //good
  //file1 = "N200-250205-160944.root"; //good
  //file1 = "N200-250205-161908.root"; //good
  //file1 = "N200-250205-170323.root"; // noisy
  //file1 = "N200-250205-170524.root";
  //file1 = "N200-250205-170754.root";
  //file1 = "N200-250205-171003.root";

  //thursday
  //first 2 runs are good
  //file1 = "N200-250206-092742.root"; //first run, no rotation
  //file1 = "N55-250206-092753.root";
  //file1 = "N200-250206-093157.root";
  //file1 = "N55-250206-093156.root";
  //9am runs before 6deg tilt, playing with tagger channel numbers
  //file1 = "N200-250206-093747.root"; //dummy
  //file1 = "N55-250206-093753.root"; 
  //file1 = "N200-250206-093831.root"; //bad, weirdly large wide tdiff background
  //file1 = "N55-250206-093834.root";
  //back to good tagger channel
  //file1 = "N200-250206-094352.root";
  //file1 = "N55-250206-094401.root";

  //label needed
  //file1 = "N200-250206-110739.root";
  //file1 = "

  //12deg tilt, "final" vthresh values
  file1 = "N55-250206-123249.root";
  file1 = "N200-250206-132139.root";
  
  TString filenoroot = file1;
  filenoroot.ReplaceAll(".root","");
  int run = std::stoi(filenoroot(filenoroot.Length()-6,6)); 

  file1 = rootfile_dir+file1;
  //TString file2 = "";
  //file1 = rootfile_dir+file2;
  
  TFile *f1 = new TFile(file1,"OPEN");
  //TFile *f2 = new TFile(file2,"OPEN");
  
  TTree* T1 = (TTree*) f1->Get("clusterTree");
  //TTree* T2 = (TTree*) f2->Get("clusterTree");
  TimepixTree *Tpx1 = new TimepixTree(T1);
  //TimepixTree *Tpx2 = new TimepixTree(T2);

    
  int nev1 = T1->GetEntries();
  //int nev2 = T2->GetEntries();
  
  cout << Form("%i events in file 1",nev1) << endl;
  //cout << Form("%i events in file 2",nev2) << endl;
  
  int nev=nev1;
  int nev_want = pow(10,6);
  //nev_want=-1;
    if(nev_want==-1) nev=nev;
    else if(nev>nev_want) nev = nev_want;
  
  //time of arrival
  TH1D *htoa_raw = new TH1D("htoa_raw","Raw Time of Arrival; toa [25.0 / 128 ns]",250,0,200*pow(10,9));
  TH1D *htoa = new TH1D("htoa","Time of Arrival; toa [s]",250,0,180*pow(10,3));
  TH1D *htoa_zoomed = new TH1D("htoa_zoomed","Time of Arrival Zoomed In; toa [s]",180,0,180);
  TH1D *htoa_sensor = new TH1D("htoa_sensor","Time of Arrival Sensor Pixels; toa [25.0 / 128.0 ns]",250,-1,-1);
  TH1D *htoa_digi = new TH1D("htoa_digi","Time of Arrival Digital Pixels; toa [25.0 / 128.0 ns]",250,-1,-1);
  
  //time over threshold
  TH1D *htot_raw_0 = new TH1D("htot_raw_0","Time Over Threshold; tot [25 ns]",250,-1,20);
  TH1D *htot_raw_1 = new TH1D("htot_raw_1","Time Over Threshold; tot [25 ns]",250,-1,20);
  TH1D *htot = new TH1D("htot","Time Over Threshold for Pixels with Tagger Time Coinc; tot [25 ns]",250,0,10);
  TH1D *htot_coinc = new TH1D("htot_coinc","Time Over Threshold for Pixels with Tagger Time Coinc; tot [25 ns]",250,0,30);
  
  //positions
  TH1D *hx_raw = new TH1D("hx_raw","x pos; x [m]",250,0,30);
  TH1D *hx = new TH1D("hx","x pos; x [m]",250,0,30);
  
  TH1D *hy_raw = new TH1D("hy_raw","y pos; y [m]",250,0,30);
  TH1D *hy = new TH1D("hy","y pos; y [m]",250,0,30);
  
  TH2D *h2d_xy_raw = new TH2D("h2d_xy_raw","2D xy pos raw; col; row",512,0,512,512,0,512);
  TH2D *h2d_xy_coinc = new TH2D("h2d_xy_coinc","2D xy pos of hits in coinc with tagger; col; row",512,0,512,512,0,512);

  
  double tdiff_min = -1.5*pow(10,3);
  double tdiff_max = -200;
  //difference between event and previous event
  //likely hits in same "real" clustering being counted as seperate events?
  TH1D *htdiff_events_0 = new TH1D("htdiff_events_0","Time difference between consecutive events; #Delta t [25.0 / 128 ns]",250,tdiff_min,tdiff_max);
  TH1D *htdiff_events_1 = new TH1D("htdiff_events_1","Time difference between consecutive events; #Delta t [25.0 / 128 ns]",250,tdiff_min,tdiff_max);
 TH1D *hxdiff_events = new TH1D("hxdiff_events","#Delta x pos between consecutive events; #Delta x [mm]",100,-30,-30);
  TH1D *hydiff_events = new TH1D("hydiff_events","#Delta y pos between consecutive events; #Delta y [mm]",100,-30,-30);
  TH2D *hxydiff_events = new TH2D("hxydiff_events","Correlation between x and y pos diff for consecutive events; #Delta x [mm]; #Delta y [mm]",100,0,5,100,0,5);

  TH2D *h2d_xtdiff_coinc = new TH2D("h2d_xtdiff_coinc","Correlation between tdiff and xpos (column)",512,0,512,250,tdiff_min,tdiff_max);
  TH2D *h2d_ytdiff_coinc = new TH2D("h2d_ytdiff_coinc","Correlation between tdiff and ypos (row)",512,0,512,250,tdiff_min,tdiff_max);

  TH2D *h2d_ytdiff_coinc_signal = new TH2D("h2d_ytdiff_coinc_signal","Correlation between tdiff and ypos (row), Signal Leads",512,0,512,250,tdiff_min,tdiff_max);
  TH2D *h2d_ytdiff_coinc_digi = new TH2D("h2d_ytdiff_coinc_digi","Correlation between tdiff and ypos (row), Digi hit Leads",512,0,512,250,tdiff_min,tdiff_max);
    
  
  //positions after event diff time cut
  TH1D *hxdiff_events_tcut = new TH1D("hxdiff_events_tcut","#Delta x pos between consecutive events_tcut; #Delta x [mm]",100,-30,-30);
  TH1D *hydiff_events_tcut = new TH1D("hydiff_events_tcut","#Delta y pos between consecutive events_tcut; #Delta y [mm]",100,-30,-30);
  TH2D *hxydiff_events_tcut = new TH2D("hxydiff_events_tcut","Correlation between x and y pos diff for consecutive events_tcut; #Delta x [mm]; #Delta y [mm]",100,0,5,100,0,5);
  TH1D *htoa_zoomed_tcut = new TH1D("htoa_zoomed_tcut","Time of Arrival Zoomed In; toa [s]",180,0,180);
  
  
  //hits relative to heartbeat
  TH1D *hHB_raw = new TH1D("hHB_raw","Raw Heartbeat Signal",250,0,pow(10,13));
  TH1D *htoa_hb = new TH1D("htoa_hb","ToA Ref Subtract Heartbeat",250,-1,-1);

  //coincidences between the two TPX layers
  //TH1D *htcoin = new TH1D("htcoin","Coincidence between tpx1 and tpx2 ??",250,-1,-1);
  //TH1D *hdx = new TH1D("hdx","#Delta x pos between tpx1 and tpx2 ??",250,-50,50);
  //TH1D *hdy = new TH1D("hdy","#Delta y pos between tpx1 and tpx2 ??",250,-50,50);
  
  
  //exact names in tree for reference
  //int nhits,col[nhits],row[nhits];
  //long toa[nhits];
  //double tot[nhits];
  //int mincol,maxcol,minrow,maxrow,diginput;
  //bool pileup[nhits];
  //double xpos,ypos,cltot;
  //int ctoa[nhits],ctot[nhits],ftoaRise[nhits],ftoaFall[nhits],uftoaStart[nhits],uftoaStop[nhits];
  //long heartbeat[nhits;
  //ulons packet[nhits];
  //int ccltot;
  //double threshold;
  
  //some new variables for analysis
  long toa1_l;
  long time_prev1,toa1;
  double tot1, tot_prev1;
  int x_prev1,x1;
  int y_prev1,y1;

  int ev_stat,last_ev_stat;

  //wed fits
  double tpeak = -625.0;
  double tcut = 3*39.0;

  //thur fits
  tpeak = -1001;
  tcut = 3*21;
  //tpeak=0;
  //tcut=pow(10,5);

  int ev2_min=0;
  int ev2_max = nev;
  double tmin = -250;
  double tmax = 250;
  
  for(int ev=0; ev<nev; ev++){
    //int to_ev = SO[ev].first;
    if(ev%100000==0) cout << ev << " / " << nev << endl;
      //cout << "i: " << ev << " Sorted Event num: " << SO[ev].first << " Sorted Event toa: " << SO[ev].second <<  endl;
    int to_ev = ev;
    Tpx1->GetEntry(to_ev);
    
    
    //if first event no previous time
    //else set previous event vars
    if(ev==0){
      time_prev1=0;
      x_prev1=0;
      y_prev1=0;
      last_ev_stat=0;
      tot_prev1=0.0;
    }else{
      time_prev1=toa1;
      x_prev1=x1;
      y_prev1=y1;
      last_ev_stat=ev_stat;
      tot_prev1 = tot1;
    }
    
    //then update toa1 to this event toa
    //toa1 = (double) Tpx1->toa[0] * (25.0/128.) / pow(10.0,9.0);
    toa1 =  Tpx1->toa[0];
    tot1 = Tpx1->tot[0];
    ev_stat = Tpx1->digitalInput;
    //x1 = (double) Tpx1->xpos;
    //y1 = (double) Tpx1->ypos;
    x1 = Tpx1->col[0];
    y1 = Tpx1->row[0];
    
    long tdiff1 = toa1 - time_prev1;
    //tdiff1 = tdiff1 * pow(10,-9);
    //cout << tdiff1 << endl;
    //double toa2 = (double) Tpx2->toa[0] * (25.0/128.0) / pow(10.0,9.0);
    //double tcoin = toa1 - toa2;
    
    //double xdiff1 = x1 - x_prev1;
    //double x2 = (double) Tpx2->xpos;
    //double dx = x1-x2;
    
    //double ydiff1 = y1 - y_prev1;
    //double y2 = (double) Tpx2->ypos;
    //double dy = y1-y2;

    htoa_raw->Fill(Tpx1->toa[0]);
    htoa->Fill(toa1);
    htoa_zoomed->Fill(toa1);
    htot->Fill(tot1);
    
    hx_raw->Fill(x1);
    hy_raw->Fill(y1);
    
       
    hHB_raw->Fill(Tpx1->heartbeat[0]);
    double toa_hb = 25.0 * pow(10,-9) * (Tpx1->toa[0]/128. - Tpx1->heartbeat[0]);
    htoa_hb->Fill(toa_hb);
    
    htoa_zoomed_tcut->Fill(toa1);
    
    if(ev==0) continue;

    if(Tpx1->digitalInput==0){
      htoa_digi->Fill(Tpx1->toa[0]);
      htot_raw_0->Fill(Tpx1->tot[0]);
    } else {
      h2d_xy_raw->Fill(x1,y1);
      htoa_sensor->Fill(Tpx1->toa[0]);
      htot_raw_1->Fill(Tpx1->tot[0]);
    }    
    //if( tot_prev1 < 3.5) continue;
    
    if(ev_stat==0 && last_ev_stat!=0){
      tot1 = tot_prev1;
      x1 = x_prev1;
      y1 = y_prev1;
      tdiff1 = -tdiff1;
      htdiff_events_1->Fill(tdiff1);
      h2d_ytdiff_coinc_digi->Fill(y1,tdiff1);
    }else if (ev_stat!=0 && last_ev_stat==0){
      htdiff_events_0->Fill(tdiff1);
      h2d_ytdiff_coinc_signal->Fill(y1,tdiff1);
    }else if(ev_stat==0 && last_ev_stat==0){
      ;
    }else if(ev_stat!=0 && last_ev_stat!=0){
      ;
    }
    else continue;
    
    if((ev_stat!=0 && last_ev_stat==0) || (ev_stat==0 && last_ev_stat!=0)){
      h2d_xtdiff_coinc->Fill(x1,tdiff1);
      h2d_ytdiff_coinc->Fill(y1,tdiff1);
      if(fabs(tdiff1-tpeak)>tcut) continue;
      htot_coinc->Fill(tot1);
      h2d_xy_coinc->Fill(x1,y1);
    }
    
    
  }
  
  //gPad->SetLogy();
  //htoa_raw->Draw();
  //gPad->Print("htoa_raw.png");
  
  //htoa->Draw();
  //gPad->Print("htoa.png");
  
  //htoa_zoomed->Draw();
  //gPad->Print("htoa_zoomed.png");

  //htot_raw_0->SetLineColor(kBlack);
  //htot_raw_0->Draw();
  //htot_raw_1->SetLineColor(kRed);
  //htot_raw_1->Draw("same");
  //gPad->Print("htot_raw.png");
  
  //htot->Draw();
  //gPad->Print("htot.png");
  
  //hx_raw->Draw();
  //gPad->Print("hdx_raw.png");
  //hy_raw->Draw();
  //gPad->Print("hdy_raw.png");
  //gPad->SetLogy(0);
  //h2d_xy_raw->Draw("colz");
  //gPad->Print("h2d_xy_raw.png");

  //gPad->Close();
  
  TCanvas *c00 = new TCanvas("c00","",1000,1000);
  c00->Divide(2,3);
  c00->cd(1)->SetLogy(0);
  TH1D *htdiff_events = (TH1D*) htdiff_events_0->Clone("htdiff_events");
  htdiff_events->Add(htdiff_events_1);
  htdiff_events->SetLineColor(kBlack);
  htdiff_events->SetMinimum(0);
  htdiff_events->Draw();
  TLine l;
  l.SetLineColor(kRed);
  l.DrawLine(tpeak-tcut,0,tpeak-tcut,htdiff_events->GetMaximum());
  l.DrawLine(tpeak+tcut,0,tpeak+tcut,htdiff_events->GetMaximum());
  htdiff_events_0->SetLineColor(kBlue);
  htdiff_events_0->Draw("same");
  htdiff_events_1->SetLineColor(kRed);
  htdiff_events_1->Draw("same");
  int maxbin = htdiff_events->GetMaximumBin();
  double xmax = htdiff_events->GetBinCenter(maxbin);
  TF1 *f = new TF1("f","gaus",xmax-100,xmax+100);
  htdiff_events->Fit(f,"","",xmax-100,xmax+100);
  gPad->Print("htdiff_events.png");

  c00->cd(2);
  htot_coinc->Draw();

  c00->cd(3);
  h2d_xy_raw->Draw("colz");
  c00->cd(4);
  h2d_xy_coinc->Draw("colz");
  c00->Print(Form("%i.png",run));

  c00->cd(5);
  h2d_xtdiff_coinc->Draw("colz");
  c00->cd(6);
  h2d_ytdiff_coinc->Draw("colz");

  TCanvas *c01 = new TCanvas("c01","",1000,500);
  c01->Divide(2,1);
  c01->cd(1);
  h2d_ytdiff_coinc_signal->Draw("colz");
  c01->cd(2);
  h2d_ytdiff_coinc_digi->Draw("colz");
  
  new TCanvas();
  htoa_sensor->SetLineColor(kRed);
  htoa_sensor->Draw();
  new TCanvas();
  htoa_digi->Draw("same");

  
  
  /*
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
  
  htoa_zoomed_tcut->Draw();
  gPad->Print("htoa_zoomed_tcut.png");
  
  hHB_raw->Draw();
  gPad->Print("hHB_raw.png");
  
  htoa_hb->Draw();
  gPad->Print("htoa_hb.png");

  
  htxdiff->Draw("colz");
  htcoin->Draw();
  hdx->Draw();
  hdy->Draw();
  */
}
