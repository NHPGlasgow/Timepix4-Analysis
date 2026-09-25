#define trackTree_cxx
#include "trackTree.h"

//Example on how to read track info for root file


void trackReader(const char *fname){
  TFile *f = TFile::Open(fname);
  trackTree t((TTree*)f->Get("trackTree"));
  t.Loop();
}

void trackTree::Loop(){
  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  int hit_index = 0;
  double mag=0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {     //for all entries (each entry is an event with multiple tracks)
    Long64_t ientry = LoadTree(jentry);                   //get the event
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    hit_index=0;                                          //init the hit index (all hits on all layers are in std vec)

    //print out what's in the event  
    cout << "Event No: "<< event_no << endl;                                         //event number 
    for(int tr=0;tr<track_type[0].size();tr++){                                      //for each track in the event
      cout << "  Tr Indx, Type: " << tr << ", " << track_type[0][tr] << endl;        //print the index,type and position (x,y,z in mm)
      cout << "      Point:" << ppx[0][tr] << ", " << ppy[0][tr] << ", " << ppz[0][tr] << endl;
      if(track_type[0][tr] < 2) {                                                    //types 0 (signal track) and 1 (bg track) have unit direction vector.
	mag = TMath::Sqrt(vpx[0][tr]*vpx[0][tr]+vpy[0][tr]*vpy[0][tr]+vpz[0][tr]*vpz[0][tr]);
	cout << "      Direction (x,y,z): "  << vpx[0][tr] << ", " << vpy[0][tr] << ", " << vpz[0][tr] << ", Mag: " << mag << endl;
      }
      else{                                                                          //type 2 is a single and only has a position
	cout << "      Direction : N/A" << endl;
      }
      for(int l=0;l<track_nhits[0][tr]; l++){                                        // for all hits in the track, print layer, row, column numbers.
	cout <<"       Layer, row, col: " << lay[0][hit_index] << ", " << row[0][hit_index]  << ", " << col[0][hit_index] << endl;
	hit_index++;
      }
    }      
  }
}
