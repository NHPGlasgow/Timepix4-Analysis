struct RunFile{
  std::string chip;
  std::string name;
  std::string label;
  std::string file;
  std::string filepath;
  //std::string datadir = "/scratch1/tpx4_config/data/trees/";
  std::string datadir = "/w/work6/home/kl13k/scapa/trees/";

  TH1D *htoa;
  TH1D *htoa_rate;
  TH1D *htot;
  TH2D *hxy;
  
  Color_t color;
  
  RunFile(std::string fchip,
	  std::string ffile,
	  std::string fname,
	  std::string flabel)
    : chip(fchip), file(ffile), name(fname), label(flabel)
  {
    filepath = datadir+ffile;
  }
};

void AnalyseRun(RunFile& f,
		TFile* outfile)
{
  
  ROOT::RDataFrame df_raw("clusterTree",f.filepath.c_str());
  auto df_seconds = df_raw
    .Define("toa_seconds", "toa[0]*25.0/(1e9*128.0)");
    
  auto df = df_seconds.Filter("toa_seconds<590");

  gStyle->SetHistLineColor(f.color);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  //first 600 seconds ~ 200 pulses for beam "stability"
  auto htoa = df.Histo1D({
      ("htoa_"+f.name).c_str(),
      (f.label+";Time [s];Hits / bin").c_str(),
      250,0,600},"toa_seconds");
  
  auto htoa_rate = df.Histo1D({
      ("htoa_rate_"+f.name).c_str(),
      (f.label+";Time [s]; Rate [Hz]").c_str(),
      250,0,600},"toa_seconds");

  auto htot = df.Histo1D({
      ("htot_"+f.name).c_str(),
      (f.label+";ToT [ns]; Hits / bin").c_str(),
      250,0,40},"tot");
  
  auto hxy = df.Histo2D({
      ("hxy_"+f.name).c_str(),
      (f.label+";xpos [mm];ypos [mm]").c_str(),
      100,0,25,
      100,0,30},"xpos","ypos");
	
  auto toa_bin_width = htoa->GetXaxis()->GetBinWidth(1);
  htoa_rate->Scale(1.0 / toa_bin_width);
  
  f.htoa = (TH1D*)htoa->Clone(htoa->GetName());
  f.htoa_rate = (TH1D*)htoa_rate->Clone(htoa_rate->GetName());
  f.htot = (TH1D*)htot->Clone(htot->GetName());
  f.hxy = (TH2D*)hxy->Clone(hxy->GetName());
  
  f.htoa->Write();
  f.htoa_rate->Write();
  f.htot->Write();
  f.hxy->Write();
}

// void DrawRuns(){

// }

void CompareRuns(){

  std::string outfilename = "scapa_ana.root";
  TFile *fout = new TFile(outfilename.c_str(), "RECREATE");
  fout->cd();
  
  RunFile n55_before("N55","N55-260415-142248.root","before","Before Shielding");
  RunFile n55_after("N55","N55-260415-150214.root","after","After Shielding");
  RunFile n55_wire1("N55","N55-260415-153211.root","wire1","Thin Wire");
  RunFile n55_wire2("N55","N55-260415-155916.root","wire2","Thick Wire");

  n55_before.color = kP6Red;
  n55_after.color = kP6Blue;
  n55_wire1.color = kP6Grape;
  n55_wire2.color = kP6Yellow;
  
  std::vector<RunFile> runs55;
  runs55.push_back(n55_before);
  runs55.push_back(n55_after);
  runs55.push_back(n55_wire1);
  runs55.push_back(n55_wire2);

  TCanvas *ctoa = new TCanvas();
  TCanvas *ctoa_rate = new TCanvas();
  TCanvas *ctot = new TCanvas();
  TCanvas *cxy = new TCanvas();
  cxy->Divide(2,2);
  
  int iter=1;
  Option_t* drawopts = "hist";
  for(auto& f : runs55){
    AnalyseRun(f, fout);
    if(iter>1) drawopts = "hist same";
    ctoa->cd();
    f.htoa->DrawCopy(drawopts);
    ctoa_rate->cd();
    f.htoa_rate->DrawCopy(drawopts);
    ctot->cd();
    f.htot->DrawCopy(drawopts);
    cxy->cd(iter);
    f.hxy->Draw("colz");
    iter++;
  }
   
  float legx1 = 0.6; float legx2 = 0.85; float legy1 = 0.6; float legy2 = 0.85;
  // ctoa->cd();
  // gPad->BuildLegend(legx1, legy1, legx2, legy2);
  // ctoa_rate->cd();
  // gPad->BuildLegend(legx1, legy1, legx2, legy2);
  ctot->cd();
  gPad->BuildLegend(legx1, legy1, legx2, legy2);
  
  TCanvas *csummary = new TCanvas("csummary","Summary",2000,1200);
  csummary->Divide(2,2);
  
  csummary->cd(1);
  ctot->DrawClonePad();

  csummary->cd(2);
  cxy->DrawClonePad();
  
  csummary->cd(3);
  ctoa->DrawClonePad();
  
  csummary->cd(4);
  ctoa_rate->DrawClonePad();
  

  ctoa->Close();
  ctoa_rate->Close();
  ctot->Close();
  cxy->Close();
}
