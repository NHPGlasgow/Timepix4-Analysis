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

void AnalyseRun(RunFile& f, TFile* outfile)
{
  
  ROOT::RDataFrame df_raw("clusterTree",f.filepath.c_str());
  auto df_seconds = df_raw.Define("toa_seconds", "toa[0]*25.0/(1e9*128.0)");
    
  auto df = df_seconds.Filter("toa_seconds<590");

  gStyle->SetHistLineColor(f.color);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  //first 600 seconds ~ 200 pulses for beam "stability"
  auto htoa = df.Histo1D({
      ("htoa_"+f.name).c_str(),
      (f.label+";Time [s];Hits / bin").c_str(),
      2500,0,600},"toa_seconds");
  
  auto htoa_rate = df.Histo1D({
      ("htoa_rate_"+f.name).c_str(),
      (f.label+";Time [s]; Rate [Hz]").c_str(),
      2500,0,600},"toa_seconds");

  auto htot = df.Histo1D({
      ("htot_"+f.name).c_str(),
      (f.label+";ToT [ns]; Hits / bin").c_str(),
      2500,0,40},"tot");
  
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

  return df;
}

// void DrawRuns(){

// }

void CompareRuns(){

  std::string outfilename = "scapa_ana.root";
  TFile *fout = new TFile(outfilename.c_str(), "RECREATE");
  fout->cd();
  
  RunFile n55_before("N55","N55-260415-142248.root","N55_before","Before Shielding");
  RunFile n55_after("N55","N55-260415-150214.root","N55_after","After Shielding");
  RunFile n55_wire1("N55","N55-260415-153211.root","N55_wire1","Thin Wire");
  RunFile n55_wire2("N55","N55-260415-155916.root","N55_wire2","Thick Wire");
    
  RunFile n200_before("N200","N200-260415-142234.root","N200_before","Before Shielding");
  RunFile n200_after("N200","N200-260415-150214.root","N200_after","After Shielding");
  RunFile n200_wire1("N200","N200-260415-153209.root","N200_wire1","Thin Wire");
  RunFile n200_wire2("N200","N200-260415-155922.root","N200_wire2","Thick Wire");

  //Time offset from N55 to N200 for each run (s)
  auto toff_before = 13.3;
  auto toff_after  = 0.2;
  auto toff_wire1  = 3.5;
  auto toff_wire2  = -6.5;
  
  n55_before.color = kP6Red;
  n55_after.color = kP6Blue;
  n55_wire1.color = kP6Grape;
  n55_wire2.color = kP6Yellow;
  
  std::vector<RunFile> runs55;
  runs55.push_back(n55_before);
  runs55.push_back(n55_after);
  runs55.push_back(n55_wire1);
  runs55.push_back(n55_wire2);

  n200_before.color = kP6Red;
  n200_after.color = kP6Blue;
  n200_wire1.color = kP6Grape;
  n200_wire2.color = kP6Yellow;
  
  std::vector<RunFile> runs200;
  runs200.push_back(n200_before);
  runs200.push_back(n200_after);
  runs200.push_back(n200_wire1);
  runs200.push_back(n200_wire2);

  TCanvas *c55toa = new TCanvas();
  TCanvas *c55toa_rate = new TCanvas();
  TCanvas *c55tot = new TCanvas();
  TCanvas *c55xy = new TCanvas();
  c55xy->Divide(2,2);
  
  TCanvas *c200toa = new TCanvas();
  TCanvas *c200toa_rate = new TCanvas();
  TCanvas *c200tot = new TCanvas();
  TCanvas *c200xy = new TCanvas();
  c200xy->Divide(2,2);
  
  int iter=1;
  Option_t* drawopts = "hist";
  for(auto& f : runs55){
    AnalyseRun(f, fout);
    if(iter>1) drawopts = "hist same";
    c55toa->cd();
    f.htoa->DrawCopy(drawopts);
    c55toa_rate->cd();
    f.htoa_rate->DrawCopy(drawopts);
    c55tot->cd();
    f.htot->DrawCopy(drawopts);
    c55xy->cd(iter);
    f.hxy->Draw("colz");
    iter++;
  }
  iter=1;
  drawopts = "hist";
  for(auto& f : runs200){
    AnalyseRun(f, fout);
    if(iter>1) drawopts = "hist same";
    c200toa->cd();
    f.htoa->DrawCopy(drawopts);
    c200toa_rate->cd();
    f.htoa_rate->DrawCopy(drawopts);
    c200tot->cd();
    f.htot->DrawCopy(drawopts);
    c200xy->cd(iter);
    f.hxy->Draw("colz");
    iter++;
  }
  
  float legx1 = 0.6; float legx2 = 0.85; float legy1 = 0.6; float legy2 = 0.85;
  c55tot->cd();
  gPad->BuildLegend(legx1, legy1, legx2, legy2);
  
  TCanvas *c55summary = new TCanvas("c55summary","N55 Summary",2000,1200);
  c55summary->Divide(2,2);
  
  c55summary->cd(1);
  c55tot->DrawClonePad();

  c55summary->cd(2);
  c55xy->DrawClonePad();
  
  c55summary->cd(3);
  c55toa->DrawClonePad();
  
  c55summary->cd(4);
  c55toa_rate->DrawClonePad();
  
  c55summary->Print("scapa_data_600seconds_N55_summary.png");
  
  c55toa->Close();
  c55toa_rate->Close();
  c55tot->Close();
  c55xy->Close();
  
  c200tot->cd();
  gPad->BuildLegend(legx1, legy1, legx2, legy2);
  
  TCanvas *c200summary = new TCanvas("c200summary","N200 Summary",2000,1200);
  c200summary->Divide(2,2);
  
  c200summary->cd(1);
  c200tot->DrawClonePad();

  c200summary->cd(2);
  c200xy->DrawClonePad();
  
  c200summary->cd(3);
  c200toa->DrawClonePad();
  
  c200summary->cd(4);
  c200toa_rate->DrawClonePad();
  
  c200summary->Print("scapa_data_600seconds_N200_summary.png");
  
  c200toa->Close();
  c200toa_rate->Close();
  c200tot->Close();
  c200xy->Close();
}



//std::vector<double> xpos_vec = df.Filter("mystring_getpulse").Take<double>("xpos");
//maybe auto =
