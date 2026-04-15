#include "TimepixTree.C"
#include "TimepixDF.cxx"

const TString rootfile_dir = "/scratch1/tpx4_config/data/trees/";
TString file1 = "N200-250204-171615.root";
TString file2 = "N55-250204-171615.root";
file1 = "N200-250204-170740.root";
file2 = "N55-250204-170739.root";
file1 = "N200-250204-181555.root";
file2 = "N55-250204-181554.root";


file1 = rootfile_dir+file1;
file2 = rootfile_dir+file2;

void GetOffset(){

  TString newfile1 = file1;
  newfile1.ReplaceAll(".root","_nodigi.root");
  TString newfile2 = file2;
  newfile2.ReplaceAll(".root","_nodigi.root");

  if(!std::filesystem::exists(newfile1.Data())){
    cout << "Snapshotting file 1 for real hits" << endl;
    TimepixDF(file1);
  }
  if(!std::filesystem::exists(newfile2.Data())){
    cout << "Snapshotting file 2 for real hits" << endl;
    TimepixDF(file2);
  }
  
  TFile *f1 = new TFile(newfile1,"OPEN");
  TFile *f2 = new TFile(newfile2,"OPEN");
  
  TTree* T1 = (TTree*) f1->Get("clusterTree");
  TTree* T2 = (TTree*) f2->Get("clusterTree");
  TimepixTree *Tpx1 = new TimepixTree(T1);
  TimepixTree *Tpx2 = new TimepixTree(T2);
  
  long nev1 = T1->GetEntries();
  long nev2 = T2->GetEntries();
  
  cout << Form("%ld events in file 1",nev1) << endl;
  cout << Form("%ld events in file 2",nev2) << endl;

  double xlim = 1;//pow(10,9);
  TH1D *htoa = new TH1D("htoa","Raw Time of Arrival; toa [25 / 128 ns]",250,-xlim,-xlim);

  //nev1=10000;
  //nev2=10000;
  long nev = nev1*nev2;
  int good_ev1 = 0;
  int good_ev2 = 0;
  int this_ev = 0;
  int ev2start=0;
  long toa1,toa2;
  long tmin=-1000;
  long tmax=1000;
  for(int ev1=0;ev1<nev1;ev1++){
    
    Tpx1->GetEntry(ev1);
    if(Tpx1->digitalInput==0)
      cout << "WHY ARE THERE DIGITAL HITS AAAAAHHHH" << endl;
    toa1 = Tpx1->toa[0];
    good_ev1++;
    good_ev2=0;
    for(int ev2=ev2start; ev2<nev2; ev2++){
      
      Tpx2->GetEntry(ev2);
      if(Tpx2->digitalInput==0) 
	cout << "WHY ARE THERE DIGITAL HITS AAAAAHHHH" << endl;
      
      //  if(this_ev%100000==0) cout << "Events: " << this_ev << " / " << nev << " " <<  (double)100.0* this_ev/ (double) nev << "%" << endl;

      toa2 = Tpx2->toa[0];
      long tdiff = toa1-toa2;
      
      if(toa1-toa2 > tmax){
	ev2start = ev2;
	continue;
      }
      
      if(tdiff<tmin){
	break;
      }
      
      good_ev2++;
      this_ev++;
      htoa->Fill(tdiff);
      
    }
  }

  // draw:
  htoa->Draw();

}
