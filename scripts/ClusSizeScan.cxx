#include "TimepixTree.C"
void ClusSizeScan(){
  
  //doesnt open files that arent closed yet
  gEnv->SetValue("TFile.Recover",0);
  //doesnt draw canvas'
  gROOT->SetBatch(1);
  
  //loop over all the "thursday" data in the new temporary output directory
  //string dir = "/w/work5/eic/Timepix/MainzTests/trees/";
  //using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
  //for (const auto& file : recursive_directory_iterator(dir)){
    
  //actually, i dont like this method. Lets loop over the list of
  //output files so that we can get them in "order"
  string filelist="../data_management/250206_replayed_list.txt";
  string currentline;
  ifstream filesin(filelist);
    while(getline(filesin,currentline)){
      //string infile =  file.path();
      string infile = currentline;
      TFile *f = TFile::Open(infile.c_str(),"READ");
    if (!f){
      cout << "0" << endl;
      continue;
    }
    //cout << infile << endl;
    TTree *T = (TTree*) f->Get("clusterTree");
    TimepixTree *Tpx = new TimepixTree(T);
    long nev = T->GetEntries();
    if (nev==0){
      cout << "0" << endl;
      continue;
    }
    
    long nev_draw = 1000000; //a million events should be enough
    long first_ev = 0; //can start at event 0 unless any issues arise
    T->Draw("nhits>>htemp(20,0,20)","digitalInput==-1","",nev_draw,first_ev);
    TH1D *htemp = (TH1D*)gDirectory->Get("htemp");
    double mean = htemp->GetMean();
    cout << mean << endl;
    }
}
