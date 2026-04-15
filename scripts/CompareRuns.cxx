struct RunFile{
  std::string chip;
  std::string name;
  std::string file;
  std::string filepath;
  std::string datadir = "/scratch1/tpx4_config/data/trees/";
  RunFile(std::string fchip,
	  std::string ffile,
	  std::string fname)
    : chip(fchip), file(ffile), name(fname);
  {
    filepath = datadir+ffile;
  }
};

void AnalyseRun(const RunFile& f,
		TFile& outfile)
{
  
  ROOT::RDataFrame df("clusterTree",f.filepath.c_str());
  auto htot = df.Histo1D({(""+f.name),"",100,0,40},"tot");
  htot->DrawCopy("same");
  
}
void CompareRuns(){

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
  }
  
}
