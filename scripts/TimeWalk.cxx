#include <ROOT/RDataFrame.hxx>
#include <TCanvas.h>
#include <TH1F.h>

void TimeWalk(const TString filename="/w/work5/eic/Timepix/MainzTests/trees/N55-250206-123249.root"){
  
  // Create a ROOT DataFrame from a CSV file or a TTree
  ROOT::RDataFrame df("clusterTree", filename);
  
  //toa*(1/0.1953) to get from bins into ns 
  auto df0 = df.Define("toa0", "toa[0]*(25.0/128.0)").Define("col0", "col[0]").Define("row0", "row[0]");
  
  // Define a new column taking just the first element of the array
  auto df1 = df0.Filter("digitalInput==-1").Range(1000000);
  auto timeSensor1 = df1_sensor.Take<Double_t>("toa0");
  auto colDigi1    = df1_digi.Take<int>("col0");
  auto rowDigi1    = df1_digi.Take<int>("row0");
    
  int nrows=512;
  int ncols=448;
  int npixels=nrows*ncols;
  
  //pixel_i = col_i + row_i*ncols
  auto findpixel(auto &row_i, auto&col_i){
    return col_i + row_i*ncols;
  }
  
  
  TH2D *hTW[npixels];
  for(auto &pix : npixels){
    hTW[pix] = new TH2D(Form("hTW_%i",pix),";TOT [25 ns]; TOA1-TOA2 [25/128 ns]",100,0,100,-300,-50);
  }


}
