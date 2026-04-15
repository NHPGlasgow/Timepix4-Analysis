void TimepixDF(TString infile = ""){
  
  auto df = ROOT::RDataFrame("clusterTree",infile);
  auto df_tagger = df.Filter("digitalInput==0");
  auto df_tpx = df.Filter("digitalInput!=0");
  
  ROOT::RDF::RSnapshotOptions opts;
  opts.fLazy = true;
  opts.fMode = "RECREATE";
  std::vector<std::string> colnames = df.GetColumnNames();
  
  TString diginame = infile;
  diginame.ReplaceAll(".root","_digi.root");
  TString nodiginame = infile;
  nodiginame.ReplaceAll(".root","_nodigi.root");
  //auto snap_tagger = df_tagger.Snapshot("clusterTree",diginame,colnames,opts);
  //auto snap_tpx = df_tpx.Snapshot("clusterTree",nodiginame,colnames,opts);
  auto x = df.Count();
  df.Count().GetValue();
  cout << *x << endl;
}
