#include "TimepixTree.C"

void DrawGraphs(){

  //Tilt vs Clus Mult
  double tilt[3] = {0,6,12};

  double mult55[3] = {1.38376, 1.37648, 1.50983};
  double mult200[3] = {1.4483, 1.41088, 1.522341};

  TGraph *gMultTilt55 = new TGraph(3,tilt,mult55);
  gMultTilt55->SetName("gMultTitle55");
  gMultTilt55->SetTitle("Chip N55; Detector Tilt [deg]; Mean Cluster Multiplicity");
  gMultTilt55->SetMarkerStyle(8);
  gMultTilt55->SetMarkerColor(kBlue);
  
  TGraph *gMultTilt200 = new TGraph(3,tilt,mult200);
  gMultTilt200->SetName("gMultTitle200");
  gMultTilt200->SetTitle("Chip N200; Detector Tilt [deg]; Mean Cluster Multiplicity");
  gMultTilt200->SetMarkerStyle(8);
  gMultTilt200->SetMarkerColor(kRed);

  TMultiGraph *mg_MultTilt = new TMultiGraph();
  mg_MultTilt->Add(gMultTilt55);
  mg_MultTilt->Add(gMultTilt200);
  
  mg_MultTilt->SetName("gMultTilt");
  mg_MultTilt->SetTitle("VThreshold 200, GAINVThreshold 8; Detector Tilt [deg]; Mean Cluster Multiplicity");
  mg_MultTilt->Draw("ap");
  gPad->BuildLegend(0.2,0.7,0.4,0.8);
  gPad->Print("MultTilt.png");
  

  //vthreshold vs Clus Mult
  double thresh[3] = {2000,2040,2047};
  double mult55_1[3] = {1.50672, 1.81217, 1.87421};
  double mult200_1[3] = {1.5217, 1.85199, 1.92818};

  TGraph *gMultThresh55 = new TGraph(3,thresh,mult55_1);
  gMultThresh55->SetName("gMultTitle55");
  gMultThresh55->SetTitle("Chip N55; Detector Threshold [arb]; Mean Cluster Multiplicity");
  gMultThresh55->SetMarkerStyle(8);
  gMultThresh55->SetMarkerColor(kBlue);

  TGraph *gMultThresh200 = new TGraph(3,thresh,mult200_1);
  gMultThresh200->SetName("gMultTitle200");
  gMultThresh200->SetTitle("Chip N200; Detector Threshold [arb]; Mean Cluster Multiplicity");
  gMultThresh200->SetMarkerStyle(8);
  gMultThresh200->SetMarkerColor(kRed);

  TMultiGraph *mg_MultThresh = new TMultiGraph();
  mg_MultThresh->Add(gMultThresh55);
  mg_MultThresh->Add(gMultThresh200);
  
  mg_MultThresh->SetName("gMultThresh");
  mg_MultThresh->SetTitle("GainVThreshold 8, 12 deg tilt; VThreshold [arb]; Mean Cluster Multiplicity");
  mg_MultThresh->Draw("ap");
  gPad->BuildLegend(0.2,0.7,0.4,0.8);
  gPad->Print("MultThresh.png");
  

  //vthreshold vs Raw Rate
  double curr[11] = {1, 1, 1, 1, 1, 1, 5, 5, 10, 10, 10};
  double rawrate200[11] = {538.787, 633.865, 627.221, 624.157, 632.913, 633.792, 2612.53, 2568.42, 5458.17, 5454.07, 5450.12};   
  double rawrate55[11] = {509.165, 601.061, 594.095, 590.494, 599.59, 600.226, 2637.33, 2617.82, 5365.49, 5361.22, 5374.93};
  double rawrate_nodigi200[11] = {477.308, 561.466, 555.598, 552.784, 560.483, 561.34, 2348.04, 2308.58, 4966.55, 4963.08, 4957.97};
  double rawrate_nodigi55[11] = {449.307, 530.359, 523.882, 520.683, 528.775, 529.411, 2363.3, 2346.38, 4867.89, 4863.58, 4876.79};
  double clusrate200[11] = {275.047, 324.017, 320.586, 319.178, 323.745, 324.198, 1338.08, 1316.03, 2830.17, 2829.7, 2830.13};
  double clusrate55[11] = {265.473, 312.838, 309.347, 308.011, 312.569, 312.821, 1373.39, 1364.05, 2834.13, 2833.51, 2833.53};
  
  TGraph *gRawRate200 = new TGraph(11,curr,rawrate200);
  gRawRate200->SetName("gRawRate200");
  gRawRate200->SetTitle("Raw; Beam Current [nA]; Rate [kHz]");
  gRawRate200->SetMarkerStyle(8);
  gRawRate200->SetMarkerColor(kRed);
  TGraph *gRawRate_nodigi200 = new TGraph(11,curr,rawrate_nodigi200);
  gRawRate_nodigi200->SetName("gRawRate_nodigi200");
  gRawRate_nodigi200->SetTitle("Raw (No Digi Hits); Beam Current [nA]; Rate [kHz]");
  gRawRate_nodigi200->SetMarkerStyle(8);
  gRawRate_nodigi200->SetMarkerColor(kBlue);
  TGraph *gClusRate200 = new TGraph(11,curr,clusrate200);
  gClusRate200->SetName("gClusRate200");
  gClusRate200->SetTitle("Offline Clustering; Beam Current [nA]; Rate [kHz]");
  gClusRate200->SetMarkerStyle(8);
  gClusRate200->SetMarkerColor(kMagenta);
  
  TGraph *gRawRate55 = new TGraph(11,curr,rawrate55);
  gRawRate55->SetName("gRawRate55");
  gRawRate55->SetTitle("Raw; Beam Current [nA]; Rate [kHz]");
  gRawRate55->SetMarkerStyle(8);
  gRawRate55->SetMarkerColor(kRed);
  TGraph *gRawRate_nodigi55 = new TGraph(11,curr,rawrate_nodigi55);
  gRawRate_nodigi55->SetName("gRawRate_nodigi55");
  gRawRate_nodigi55->SetTitle("Raw (No Digi Hits); Beam Current [nA]; Rate [kHz]");
  gRawRate_nodigi55->SetMarkerStyle(8);
  gRawRate_nodigi55->SetMarkerColor(kBlue);
  TGraph *gClusRate55 = new TGraph(11,curr,clusrate55);
  gClusRate55->SetName("gClusRate55");
  gClusRate55->SetTitle("Offline Clustering; Beam Current [nA]; Rate [kHz]");
  gClusRate55->SetMarkerStyle(8);
  gClusRate55->SetMarkerColor(kMagenta);
  
  TMultiGraph *mg_rates200 = new TMultiGraph();
  mg_rates200->Add(gRawRate200);
  mg_rates200->Add(gRawRate_nodigi200);
  mg_rates200->Add(gClusRate200);
  mg_rates200->SetTitle("Efficiency Studies Chip N200; Beam Current [nA]; Rate [kHz]");
  mg_rates200->Draw("ap");
  TLegend *leg_rate200 = new TLegend(0.15,0.65,0.4,0.8);
  leg_rate200->AddEntry(gRawRate200);
  leg_rate200->AddEntry(gRawRate_nodigi200);
  leg_rate200->AddEntry(gClusRate200);
  leg_rate200->Draw("same");
  gPad->Print("EffRates-N200.png");

  
  TMultiGraph *mg_rates55 = new TMultiGraph();
  mg_rates55->Add(gRawRate55);
  mg_rates55->Add(gRawRate_nodigi55);
  mg_rates55->Add(gClusRate55);
  mg_rates55->SetTitle("Efficiency Studies Chip N55; Beam Current [nA]; Rate [kHz]");
  mg_rates55->Draw("ap");
  TLegend *leg_rate55 = new TLegend(0.15,0.65,0.4,0.8);
  leg_rate55->AddEntry(gRawRate55);
  leg_rate55->AddEntry(gRawRate_nodigi55);
  leg_rate55->AddEntry(gClusRate55);
  leg_rate55->Draw("same");
  gPad->Print("EffRates-N55.png");

}

