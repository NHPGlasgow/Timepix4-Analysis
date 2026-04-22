struct RunFile{
  std::string chip;
  std::string name;
  std::string file;
  std::string filepath;
  //std::string datadir = "/scratch1/tpx4_config/data/trees/";
  std::string datadir = "/w/work6/kl13k/scapa/trees/";

  TH1D *htoa;
  TH1D *htot;
  
  RunFile(std::string fchip,
	  std::string ffile,
	  std::string fname)
    : chip(fchip), file(ffile), name(fname)
  {
    filepath = datadir+ffile;
  }
};

void AnalyseRun(RunFile& f,
		TFile* outfile)
{
  
  ROOT::RDataFrame df_raw("clusterTree",f.filepath.c_str());
  auto df = df_raw.Define("toa_seconds", "toa[0]*25.0/(1e9*128.0)");


  //first 600 seconds ~ 200 pulses for beam "stability"
  auto htoa = df.Histo1D({("htoa_"+f.name).c_str(),"",100,0,600},"toa_seconds");
  auto htot = df.Histo1D({("htot_"+f.name).c_str(),"",100,0,40},"tot");

  htoa->DrawCopy("same");
  htot->DrawCopy("same");

  f.htoa = (TH1D*)htoa->Clone(htoa->GetName());
  f.htot = (TH1D*)htot->Clone(htot->GetName());
}

void CompareRuns(){

  std::string outfilename = "scapa_ana.root";
  TFile *fout = new TFile(outfilename.c_str(), "RECREATE");
  fout->cd();
  
  RunFile n55_before("N55","N55-260415-142248.root","before");
  RunFile n55_after("N55","N55-260415-150214.root","after");
  RunFile n55_wire1("N55","N55-260415-153211.root","wire1");
  RunFile n55_wire2("N55","N55-260415-155916.root","wire2");

  std::vector<RunFile> runs55;
  runs55.push_back(n55_before);
  runs55.push_back(n55_after);
  runs55.push_back(n55_wire1);
  runs55.push_back(n55_wire2);

  TCanvas *ctot = new TCanvas();
  for(auto f : runs55){
    AnalyseRun(f, fout);
  }
  
}
