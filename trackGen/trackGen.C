// Ken Livingston   -   Created May 2026 
// Toy Timepix4 tracker model 
// signal events from target disk or ring LTARG to L0
// BG events isotropic with hits in layers L0 and L1
// .... something like this                                                                   
//                                                                                     
//    ^ y-axis                                                      
//
//    :   Tracker                                                                                        
//   L0   L1   L2   L3                                                                            
//    |    |    |    |                                                                      LTARG ||
//    |    |    |    |                                                                            ||
//    |    |    |    |                                                                            ||  
//____|____|____|____|_____________________________________//______________________________________:___ z axis
//   0:(x,y,z) = (0,0,0)                                                                           :
//    :<-------------------------------------------------- // ------------------------------------>:
//    :  layerz[0-3]                         layerz[4]                                              
//                                                                                                 


// To run:
// root[] .L trackSim.C
// root[] trackSim(n_signal, n_bg, comb)
// where:
// n_signal (default = 100)
// n_bg     (default = 100)
// n_singles (default = 0 )
// comb     (defauls = 1 ie do combinatorials)
//
// For signal event generation:
// generates uniform randoms on both L0 and Target - joins the dots to make the track,
// then projects to the detector planes of other layers. All saved in arrays, and detector_target histos. 
//
// For BG event generation:
// Generates uniform randoms on both L0 and L1 - joins the dots to make the track,
// then projects to the detector planes of other layers (including the target plane).
// all saved in arrays, and detector_bg histos.
// .... plenty room for improvement with that BG generation!
//
// Analysis:
// For every layer (Ln):
//   Pair with every other layer (Lm)
//      Make all track combinations for hits on Ln with all hist on Lm pairs and project them onto the planes
//      of the other layers (including the target).
//      For each of the tracks:
//          Plot xy diff on the layer-Lm-Ln-all histogram
//          If both hits were from target event: Plot xy diff on the layer-Lm-Ln-sig histogram
//          If both hits were from BG event: Plot xy diff on the layer-Lm-Ln-BG histogram
//          Project it to the target plane.

#include <TFile.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TRandom.h>
#include <TMath.h>
#include <TStyle.h>
#include <stdio.h>



const int xpixels  = 448;                     //Timepix4 data
const int ypixels  = 512;                      
const double pixmm = 0.055;                   //pixel size in mm

const double detx  = xpixels*pixmm;           //det dimensions in mm       
const double dety  = ypixels*pixmm;

int targtype   = 0;                           //0 = cross, 1=ring or disk  
//double targr =  0.0;                        //target radius, or length of cross
double targr =  5.0;                          //target radius,  or length of cross
double targt =  2.5;                          //targ thickness

enum                      {L0,  L1,   L2,  L3,  LTARG};   //if we want to refer to layers by name

//const double   layerz[] = {0,   1,    2,   3,   3000};  //xy plane distances in mm. Front is the origin, last is target 
const double   layerz[]   = {0, 100,  200, 300, 3000};  //xy plane distances in mm. Front is the origin, last is target 
const char* layerName[]   = {"0", "1", "2", "3", "targ"};  //and their names

//for efficient near neighbour searching
int nnx[] = {0,-1, 1, 0, 0, -1, -1,  1, 1, 2,  2, 2, -2,-2,-2,-1, 0, 1,-1, 0, 1};
int nny[] = {0, 0, 0,-1, 1, -1,  1, -1, 1, -1, 0, 1, -1, 0, 1,-2,-2,-2, 2, 2, 2}; 

enum                     {PX,PY};                            //to refer to x and y layer coords and target coords
enum                     { ALL,   SIG,   BG,   SING };       //types of hit
const char *hitType[4] = {"all", "sig", "bg", "singles"};    //and their names 
double hit[20000][5][2];                                     //array for up to 2000 hits in each layer (5 = target plane)
                                                             //[nhit][layer][PX] = xcoord, [nhit][layer][PY] = y coord
double hitp[20000][5][2];                                    //as above, but pixel centre values.
                                                             //
int    hind[20000][5];                                       //bin index of hit in hist

int ntr=0;                                                   //total no of tracks passing through at least one layer

//canvases
TCanvas *layerHitCanvas;
TCanvas *planeWCanvas;

//histograms
TH2F *Layer[4][5];                             //hit distributions on layers (from all,signal,bg)
TH2F *PlaneW[4][3];                            //coord recon at target plane with weight for no of layers


//
TFile* fTracks;

// Create a TTree
TTree *trackTree;

//to write it all out as a root tree
int                event_no;                  //event no
std::vector<int>   track_type;                   //track type
std::vector<float> ppx;                          //point of track
std::vector<float> ppy;	       
std::vector<float> ppz;	       
std::vector<float> vpx;                          //direction of track (unit vector)
std::vector<float> vpy;	       
std::vector<float> vpz;	       
std::vector<int>   track_nhits;                  //track no of hits in layers
std::vector<int>   row;                          //row, col, layer of each hit
std::vector<int>   col;                          
std::vector<int>   lay;        


TAxis *xaxis, *yaxis;
TRandom *rn = NULL;                            //for general randoms

//Called to check for presence of hit on layer near coord x,y
int hitNear(double px, double py, TH2F *lhist, int width=2, double *hx=NULL, double *hy=NULL){   //look for a hit within "width" pixels around x y
  int binx,biny,binz;
  int xybin;
  int nnn=0;

  //cout << "Real: " << px << ", " << py  << endl;
  if((px<0)||(px>detx)||(py<0)||(py>dety)){
    //cout << "     Miss - off det" << endl;
    return 0;	              //check hit on layer detector
  }
  if(width==1) nnn=9;
  else         nnn=21;
  
  for(int n=0;n<nnn;n++){
    xybin=lhist->FindFixBin(px+nnx[n]*pixmm,py+nny[n]*pixmm);
    lhist->GetBinXYZ(xybin,binx,biny,binz);
    *hx=xaxis->GetBinCenter(binx);
    *hy=yaxis->GetBinCenter(biny);
    //cout << "     Tried  " << nnx[n] << "," << nny[n] << ": " <<  *hx  << ", " << *hy << endl;     
    if(lhist->GetBinContent(xybin)>0.5){
      //cout << "     Hit  " << nnx[n] << "," << nny[n] << ": " <<  *hx  << ", " << *hy << endl;
      return 1;
    }
    else{
      //cout << "     Miss  " << nnx[n] << "," << nny[n] << ": " <<  *hx  << ", " << *hy << endl;     
    }
  }
  //cout << "     Miss - no near:" << endl;  
  return 0;
}		  


//main function: generate tracks and do tracking on them.
void trackSim(int tr=10, int bg=100, int singles=0, int comb=1, int tgttype=0, double tgtr=10.0, double tgtt=2.0){
  char hname[100];
  char htitle[100];
  ntr=0;                                 //total no of tracks passing through det0
  double theta=0,r=0;
  double dx=0,dy=0, dz=0; 
  double px=0,py=0;
  double pnx,pny,pmx,pmy;
  double plx[3],ply[3]; 
  int l=0;
  double mag=0.0;
  //  double dx,dy,dz;
  int nlayer=0;
  
  track_type.clear(); 
  ppx.clear();        
  ppy.clear();	       
  ppz.clear();	       
  vpx.clear();        
  vpy.clear();	       
  vpz.clear();	       
  track_nhits.clear();
  row.clear();        
  col.clear();        
  lay.clear();        
  
  targtype = tgttype;
  targr    = tgtr;
  targt    = tgtt;

  //Make all the histograms
  for(int t=0;t<4;t++){                                                          //for all types (all, sig,bg)
    for(int n=0;n<5;n++){                                                        //for all layers
      if(Layer[t][n]){
	Layer[t][n]->Reset("ICESM");
      }
      else{
	sprintf(hname,"Layer_%s_Hits_from_%s",layerName[n],hitType[t]);
	sprintf(htitle,"Layer %s: Hits from_%s",layerName[n],hitType[t]);
	if(n==4){
	  Layer[t][n] = new TH2F(hname,htitle,201,-100,100,201,-100,100);        // target plane hists
	}
	else{
	  Layer[t][n] = new TH2F(hname,htitle,xpixels,0,detx,ypixels,0,dety);    //layer hists - need 1 bin per pixel in x,y
	}                                                                        //since they double as a store of the hit pattern for tracking
      }                                                                      
    }
    
    for(int w=0;w<3;w++){
      if(PlaneW[t][w])PlaneW[t][w]->Reset("ICESM");
      else{
	sprintf(hname,"TargetPlane_from_layer0and%d_tracks_%s",w+1,hitType[t]);
	sprintf(htitle,"Target Plane from layer 0 and %d tracks_%s",w+1,hitType[t]);
	PlaneW[t][w]=new TH2F(hname,htitle,201,-100,100,201,-100,100);
      }
    }
  }
  xaxis = Layer[0][0]->GetXaxis();                                   //Get the axes for layers to find the pixel centres for hits
  yaxis = Layer[0][0]->GetYaxis();                                   //as opposed to the generated positions 
 
  
  // ---------------------------- Make signal tracks from target to Layer0 -------------------------------------
  if(!rn) rn = new TRandom();
  int flip=0;
  for(int t=0; t<tr;t++){                                             //make tr tracks from target
    nlayer=0;
    track_type.push_back(0);                                          //and type = 0 for signal
    
    hit[ntr][L0][PX]  = rn->Uniform(detx);                            //generate uniform random hit on layer 0.
    hit[ntr][L0][PY]  = rn->Uniform(dety);
    hitp[ntr][L0][PX] = xaxis->GetBinCenter(xaxis->FindBin(hit[ntr][L0][PX]));
    hitp[ntr][L0][PY] = yaxis->GetBinCenter(yaxis->FindBin(hit[ntr][L0][PY]));
    hind[ntr][L0] = Layer[SIG][L0]->Fill(hit[ntr][L0][PX],hit[ntr][L0][PY]);          //fill layer0 hit histos
    Layer[ALL][L0]->Fill(hit[ntr][L0][PX],hit[ntr][L0][PY]);          

    //hit position vector (layer 0)
    ppx.push_back(hit[ntr][L0][PX]);
    ppy.push_back(hit[ntr][L0][PY]);
    ppz.push_back(layerz[0]);
    //hit row,col
    row.push_back(xaxis->FindBin(hit[ntr][L0][PX]));
    col.push_back(yaxis->FindBin(hit[ntr][L0][PY]));
    lay.push_back(0);
    nlayer++;
    
    if(targtype == 0){                                                   //cross wire
      if(flip){                                                          //alternate between vertical and horizontal wire
	hit[ntr][LTARG][PX]  = detx*0.5 + (rn->Uniform(-targr,targr));   //centred on the tracker centreline 
	hit[ntr][LTARG][PY]  = dety*0.5 + (rn->Uniform(-targt,+targt));
      }
      else{
	hit[ntr][LTARG][PX] = detx*0.5 + (rn->Uniform(-targt,targt));     
	hit[ntr][LTARG][PY] = dety*0.5 + (rn->Uniform(-targr,+targr));
      }
      flip=!flip;
    }
    else{                                                               //ring, or disk
      r     = rn->Uniform(targr,targr+targt);                           //generate uniform random somewhere on target 
      theta = rn->Uniform(0,TMath::TwoPi());                            // a ring / disk of targr radius and targr thickness
      hit[ntr][LTARG][PX] = detx*0.5 + r*TMath::Sin(theta);             // centred on the tracker centreline 
      hit[ntr][LTARG][PY] = dety*0.5+ r*TMath::Cos(theta);
    }
    hitp[ntr][LTARG][PX] = hit[ntr][LTARG][PX];                        //duplicate for convenience
    hitp[ntr][LTARG][PY] = hit[ntr][LTARG][PY];
    hind[ntr][LTARG] = Layer[SIG][LTARG]->Fill(hit[ntr][LTARG][PX],hit[ntr][LTARG][PY]); //fill target plane hit histos
    Layer[ALL][LTARG]->Fill(hit[ntr][LTARG][PX],hit[ntr][LTARG][PY]);
    
    //unit direction vector
    dx=hit[ntr][LTARG][PX]-hit[ntr][L0][PX];
    dy=hit[ntr][LTARG][PY]-hit[ntr][L0][PY];
    dz=layerz[LTARG]-layerz[L0];
    
    mag=TMath::Sqrt(dx*dx+dy*dy+dz*dz);
    dx/=mag;
    dy/=mag;
    dz/=mag;
    //cout << dx <<", " << dy << ", " << dz << ", Mag " << TMath::Sqrt(dx*dx+dy*dy+dz*dz) << endl;
    vpx.push_back(dx);
    vpy.push_back(dy);
    vpz.push_back(dz);
		  
    
    for(int l=1;l<4;l++){                                             //project from target through layer0 onto other layers
      dx = hit[ntr][LTARG][PX]-hit[ntr][L0][PX];
      dy = hit[ntr][LTARG][PY]-hit[ntr][L0][PY];
      hit[ntr][l][PX] = hit[ntr][L0][PX] + (layerz[l]/layerz[LTARG])*dx;
      hit[ntr][l][PY] = hit[ntr][L0][PY] + (layerz[l]/layerz[LTARG])*dy;
      hitp[ntr][l][PX] = xaxis->GetBinCenter(xaxis->FindBin(hit[ntr][l][PX]));
      hitp[ntr][l][PY] = yaxis->GetBinCenter(yaxis->FindBin(hit[ntr][l][PY]));
      hind[ntr][l] = Layer[SIG][l]->Fill(hit[ntr][l][PX],hit[ntr][l][PY]);
      row.push_back(xaxis->FindBin(hit[ntr][l][PX]));
      col.push_back(yaxis->FindBin(hit[ntr][l][PY]));
      lay.push_back(l);
      nlayer++;
      Layer[ALL][l]->Fill(hit[ntr][l][PX],hit[ntr][l][PY]); 
    }
    track_nhits.push_back(nlayer);
    ntr++;
  }
  
  // ---------------------------- Make tracks from background (randoms L0 to L1 for now) -----------------------------
  for(int t=0; t<bg;t++){
    track_type.push_back(1);                                  //type = 1 for bg track
    nlayer=0;
    hit[ntr][L0][PX] = rn->Uniform(detx);                     //generate uniform random hit on layer 0.
    hit[ntr][L0][PY] = rn->Uniform(dety);
    hitp[ntr][L0][PX] = xaxis->GetBinCenter(xaxis->FindBin(hit[ntr][L0][PX]));
    hitp[ntr][L0][PY] = yaxis->GetBinCenter(yaxis->FindBin(hit[ntr][L0][PY]));

    hind[ntr][L0] = Layer[BG][L0]->Fill(hit[ntr][L0][PX],hit[ntr][L0][PY]);   //fill layer0 hit histos
    Layer[ALL][L0]->Fill(hit[ntr][L0][PX],hit[ntr][L0][PY]);       

    //hit position vector (layer 0)
    ppx.push_back(hit[ntr][L0][PX]);
    ppy.push_back(hit[ntr][L0][PY]);
    ppz.push_back(layerz[0]);
    row.push_back(xaxis->FindBin(hit[ntr][0][PX]));
    col.push_back(yaxis->FindBin(hit[ntr][0][PY]));
    lay.push_back(0);
    nlayer++;

    hit[ntr][L1][PX] = rn->Uniform(detx);                     //generate uniform random hit on layer 1 
    hit[ntr][L1][PY] = rn->Uniform(dety);
    hitp[ntr][L1][PX] = xaxis->GetBinCenter(xaxis->FindBin(hit[ntr][L1][PX]));
    hitp[ntr][L1][PY] = yaxis->GetBinCenter(yaxis->FindBin(hit[ntr][L1][PY]));
    row.push_back(xaxis->FindBin(hit[ntr][1][PX]));
    col.push_back(yaxis->FindBin(hit[ntr][1][PY]));
    lay.push_back(1);
    nlayer++;
    hind[ntr][L1] = Layer[BG][L1]->Fill(hit[ntr][L1][PX],hit[ntr][L1][PY]);   //fill layer1 hit histo
    Layer[ALL][L1]->Fill(hit[ntr][L1][PX],hit[ntr][L1][PY]);       

    //unit direction vector
    dx=hit[ntr][L1][PX]-hit[ntr][L0][PX];
    dy=hit[ntr][L1][PY]-hit[ntr][L0][PY];
    dz=layerz[L1]-layerz[L0];

    mag=TMath::Sqrt(dx*dx+dy*dy+dz*dz);
    dx/=mag;
    dy/=mag;
    dz/=mag;
    //cout << dx <<", " << dy << ", " << dz << ", Mag " << TMath::Sqrt(dx*dx+dy*dy+dz*dz) << endl;
    vpx.push_back(dx);
    vpy.push_back(dy);
    vpz.push_back(dz);

    //project from layer0,1 onto other layers including target plane
    for(int l=2;l<5;l++){
      dx = hit[ntr][L1][PX]-hit[ntr][L0][PX];
      dy = hit[ntr][L1][PY]-hit[ntr][L0][PY];
      hit[ntr][l][PX] = hit[ntr][L0][PX] + (layerz[l]/layerz[L1])*dx;
      hit[ntr][l][PY] = hit[ntr][L0][PY] + (layerz[l]/layerz[L1])*dy;
      hitp[ntr][l][PX] = xaxis->GetBinCenter(xaxis->FindBin(hit[ntr][l][PX]));
      hitp[ntr][l][PY] = yaxis->GetBinCenter(yaxis->FindBin(hit[ntr][l][PY]));
      if(l<4){
	row.push_back(xaxis->FindBin(hit[ntr][l][PX]));
	col.push_back(yaxis->FindBin(hit[ntr][l][PY]));
	lay.push_back(l);
	nlayer++;
      }
      hind[ntr][l] = Layer[BG][l]->Fill(hit[ntr][l][PX],hit[ntr][l][PY]); 
      Layer[ALL][l]->Fill(hit[ntr][l][PX],hit[ntr][l][PY]); 
    }
    track_nhits.push_back(nlayer);
    ntr++;
    
  }

  //make random singles on all detectors
  for(int n=0; n<singles; n++){
    track_type.push_back(2);                                          //and type = 2 for singles track
    l  =  rn->Integer(4);
    px =  rn->Uniform(detx);
    py =  rn->Uniform(dety);
    hit[ntr][l][PX]=px;
    hit[ntr][l][PY]=py;
    ppx.push_back(hit[ntr][l][PX]);
    ppy.push_back(hit[ntr][l][PY]);
    ppz.push_back(layerz[l]);
    hitp[ntr][l][PX] = xaxis->GetBinCenter(xaxis->FindBin(px));
    hitp[ntr][l][PY] = yaxis->GetBinCenter(yaxis->FindBin(py));
    row.push_back(xaxis->FindBin(px));
    col.push_back(yaxis->FindBin(py));
    lay.push_back(l);
    hind[ntr][l] = Layer[SING][l]->Fill(hit[ntr][l][PX],hit[ntr][l][PY]); 
    Layer[ALL][l]->Fill(hit[ntr][l][PX],hit[ntr][l][PY]);
    track_nhits.push_back(1);
    ntr++;
  }
    
    
  //Layer[ALL][l]->Fill(px,py);
  // }
  //Analysis ---------------------------------------------------------------------------------------------

  

  //now make all combis from layer0 and layer1
  //
  for(int d0=0;d0<ntr;d0++){                                                 //for hits in layer 0 (all originate there in generator) 
    pnx = hitp[d0][L0][PX];                                                  //use centre of pixel
    pny = hitp[d0][L0][PY];
    for(int d1=0;d1<ntr;d1++){                                               //for hits in layer 1
      if((comb)||(d0==d1)){                                                  //if no  combos only do real tracks (for debugging)
	if(!hind[d1][L1]) continue;
	pmx = hitp[d1][L1][PX];                                              //use centre of pixel
	pmy = hitp[d1][L1][PY];
	for(int t=0;t<4;t++){
	  for(int l=0;l<4;l++){	  
	    dx = pmx - pnx;
	    dy = pmy - pny;
	    px = pnx + (layerz[l]/layerz[L1])*dx;
	    py = pny + (layerz[l]/layerz[L1])*dy;
	    if( hitNear(px,py,Layer[t][l],2,&plx[t],&ply[t])){
	      dx = plx[t] - pnx;
	      dy = ply[t] - pny;
	      px = pnx + (layerz[LTARG]/layerz[l])*dx;
	      py = pny + (layerz[LTARG]/layerz[l])*dy;
	      
	      PlaneW[t][l-1]->Fill(px,py);
	    }
	  }
	}
      }
    }
  }


  //plot results
  int pad=0;

  if(!layerHitCanvas){
    layerHitCanvas=new TCanvas("layerHitCanvas","Hits on Layers",5,5,1200,900);
    layerHitCanvas->Divide(5,4);
    gStyle->SetOptStat("i");
  }
  
  for(int t=0;t<4;t++){
    for(int l=0;l<5;l++){
      pad = 1 + ((t*5)+l);
      //      if(l==4) pad = 1 + ((t*5)+l);
      //      else     pad = 1 + ((t*5)+(3-l));
      layerHitCanvas->cd(pad);
      Layer[t][l]->Draw("colbox");
    }
  }
  
  if(!planeWCanvas){
    planeWCanvas=new TCanvas("planeWCanvas"," Targ plane recon from Multiple Layers",50,50,900,900);
    planeWCanvas->Divide(3,4);
    gStyle->SetOptStat("i");
  }
  pad=1;
  for(int t=0;t<4;t++){
    for(int l=0;l<3;l++){
      planeWCanvas->cd(pad++);
      PlaneW[t][l]->Draw("colbox");
    }
  }
}



void runTracks(const char *fname,int nevt=100, int tr=10, int bg=100, int singles=0, int comb=1, int tgttype=0, double tgtr=10.0, double tgtt=2.0){

  //gROOT->SetBatch(kTRUE);
  // Create a TTree
  TH1::AddDirectory(false);  //don't write the hists to file.
  TH2::AddDirectory(false);

  char textfile[200];
  char rootfile[200];
  sprintf(textfile,"%s.txt",fname);
  sprintf(rootfile,"%s.root",fname);
  
  FILE *fp = fopen(textfile,"w");
  fprintf(fp,"Events: %d\n", nevt);
  fprintf(fp,"Tracks-Targ-per-event: %d\n", tr);
  fprintf(fp,"Tracks-Non-Targ-per-event: %d\n", bg);
  fprintf(fp,"Hits-singles-per-event: %d\n", singles);
  fprintf(fp,"Layer0_z_mm: %5.0f\n",layerz[0]);
  fprintf(fp,"Layer1_z_mm: %5.0f\n",layerz[1]);
  fprintf(fp,"Layer2_z_mm: %5.0f\n",layerz[2]);
  fprintf(fp,"Layer3_z_mm: %5.0f\n",layerz[3]);
  fprintf(fp,"Targ_z_mm: %5.0f\n",layerz[4]);
  if(!targtype)  fprintf(fp,"Targ_Type: Cross\n");
  else           fprintf(fp,"Targ_Type: Ring\n");
  fprintf(fp,"Targ_Radius_or_length_of_cross_mm:%3.1f\n", targr);
  fprintf(fp,"Targ_Thickness:\t\t\t%3.1f\n", targr);
  fprintf(fp,"Timepix_pixel_mm:\t\t%5.3lf\n", pixmm);
  fprintf(fp,"Timepix_nx_pixels_rows:\t\t%d\n", xpixels);
  fprintf(fp,"Timepix_ny_pixels_cols:\t\t%d\n", ypixels);
  fclose(fp);
  
  
  fTracks   = TFile::Open(rootfile,"RECREATE");
  trackTree = new TTree("trackTree","Tree with track vectors");
  trackTree->Branch("event_no",&event_no);                    //event no
  trackTree->Branch("track_type",&track_type);                //track type (SIGNAL track =0, BG track = 1, Single hit = 2)
  trackTree->Branch("ppx",&ppx);                              //point coords
  trackTree->Branch("ppy",&ppy);
  trackTree->Branch("ppz",&ppz);
  trackTree->Branch("vpx",&vpx);                              //unit vector
  trackTree->Branch("vpy",&vpy);
  trackTree->Branch("vpz",&vpz);
  trackTree->Branch("track_nhits",&track_nhits);               //track no of hits in layers
  trackTree->Branch("row",&row);                              //row of hit
  trackTree->Branch("col",&col);                              //column 
  trackTree->Branch("lay",&lay);                              //layer
  
  
  for(int n=0;n<nevt;n++){
    event_no=n;
    trackSim(tr, bg, singles, comb, tgttype, tgtr, tgtt);
    trackTree->Fill();
    trackTree->Write("", TObject::kWriteDelete);
  }
  
  fTracks->Write();
  fTracks->Close();
}
