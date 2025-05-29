#include "TimepixTree.C"
void ClusSizeScan(){
  
  //doesnt open files that arent closed yet
  gEnv->SetValue("TFile.Recover",0);
  //doesnt draw canvas'
  gROOT->SetBatch(0);
  
  //loop over all the "thursday" data in the new temporary output directory
  string dir = "/w/work5/eic/Timepix/MainzTests/trees/";
  //using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
  //for (const auto& file : recursive_directory_iterator(dir)){
    
  //actually, i dont like this method. Lets loop over the list of
  //output files so that we can get them in "order"
  string filelist="../data_management/250206_replayed_list.txt";
  string currentline;
  ifstream filesin(filelist);
  TCanvas *c = new TCanvas();
  c->cd();
  
  while(getline(filesin,currentline)){
    //string infile =  file.path();
    string infile = currentline;
    TFile *f = TFile::Open(infile.c_str(),"READ");
    string det = infile.substr(dir.size()+1, 4);
    
    if (!f){
      cout << "0" << endl;
      continue;
    }
    TTree *T = (TTree*) f->Get("clusterTree");
    //TimepixTree *Tpx = new TimepixTree(T);
    long nev = T->GetEntries();
    
    if (nev==0){
      cout << "Bad Run Skipping" << endl;
      continue;
    }
    
    long nev_draw = 1000000; //a million events should be enough
    long first_ev = 0; //can start at event 0 unless any issues arise
   
    /*
    //get mean cluster size
    T->Draw("nhits>>hsize(20,0,20)","digitalInput==-1","",nev_draw,first_ev);
    TH1D *hsize = (TH1D*)gDirectory->Get("hsize");
    double mean = hsize->GetMean();
    
    //get threshold for run
    T->Draw("threshold>>hthresh()","","",nev_draw,first_ev);
    TH1D *hthresh = (TH1D*)gDirectory->Get("hthresh");
    double threshold = hthresh->GetMean();
    
    
    //raw rate
    int rate_ev = 10000000;
    T->Draw("toa*(25.0/128.0)/pow(10,9)>>htoa_raw(10,0,10)","digitalInput==-1","",rate_ev,first_ev);
    TH1D *htoa_raw = (TH1D*)gDirectory->Get("htoa_raw");
    int binlow = htoa_raw->FindFirstBinAbove(0);
    int binhigh = htoa_raw->FindLastBinAbove(0);
    double fitlow = htoa_raw->GetBinCenter(binlow);
    double fithigh = htoa_raw->GetBinCenter(binhigh);
    TF1 *frate = new TF1("frate","pol0",fitlow,fithigh);
    htoa_raw->Fit(frate,"Q","",fitlow,fithigh);
    double rate_raw = frate->GetParameter(0) / htoa_raw->GetBinWidth(1);
    //htoa_raw->Draw();
    cout << rate_raw/1000.0 << endl;
    */
    
    int rate_ev = 10000000;
    T->Draw("toa[0]*(25.0/128.0)/pow(10,9)>>htoa_clus(10,0,10)","digitalInput==-1","",rate_ev,first_ev);
    TH1D *htoa_clus = (TH1D*)gDirectory->Get("htoa_clus");
    int binlow = htoa_clus->FindFirstBinAbove(0);
    int binhigh = htoa_clus->FindLastBinAbove(0);
    double fitlow = htoa_clus->GetBinCenter(binlow);
    double fithigh = htoa_clus->GetBinCenter(binhigh);
    TF1 *frate = new TF1("frate","pol0",fitlow,fithigh);
    htoa_clus->Fit(frate,"Q","",fitlow,fithigh);
    double rate_clus = frate->GetParameter(0) / htoa_clus->GetBinWidth(1);
    //htoa_clus->Draw();
    cout << rate_clus/1000.0 << endl;
  
  } 
}
