#include <ROOT/RDataFrame.hxx>
#include <TCanvas.h>
#include <TH1F.h>

void BeamTestAnalysis() {
  // Create a ROOT DataFrame from a CSV file or a TTree
  ROOT::RDataFrame df1("clusterTree", "/w/work5/eic/Timepix/MainzTests/trees/N55-250206-123249.root");
  ROOT::RDataFrame df2("clusterTree", "/w/work5/eic/Timepix/MainzTests/trees/N200-250206-123235.root");
  
  auto df1_1000 = df1.Define("toa0", "toa[0]*(25.0/128.0)").Define("col0", "col[0]").Define("row0", "row[0]").Define("tot0", "tot[0]*25.0");
  auto df2_1000 = df2.Define("toa0", "toa[0]*(25.0/128.0)").Define("col0", "col[0]").Define("row0", "row[0]").Define("tot0", "tot[0]*25.0");
  
  int nev=100000;
  
  // Define a new column taking just the first element of the array
  auto df1_digi   = df1_1000.Filter("digitalInput==0").Range(nev);
  auto df1_sensor = df1_1000.Filter("digitalInput==-1").Range(nev);
  auto df2_digi   = df2_1000.Filter("digitalInput==0").Range(nev);
  auto df2_sensor = df2_1000.Filter("digitalInput==-1").Range(nev);

  auto timeDigi1   = df1_digi.Take<Double_t>("toa0");
  auto timeSensor1 = df1_sensor.Take<Double_t>("toa0");
  auto totSensor1 = df1_sensor.Take<Double_t>("tot0");
    
  auto timeDigi2   = df2_digi.Take<Double_t>("toa0");
  auto timeSensor2 = df2_sensor.Take<Double_t>("toa0");
  auto totSensor2 = df2_sensor.Take<Double_t>("tot0");
    
  auto colDigi1    = df1_digi.Take<int>("col0");
  auto rowDigi1    = df1_digi.Take<int>("row0");
  auto colDigi2    = df2_digi.Take<int>("col0");
  auto rowDigi2    = df2_digi.Take<int>("row0");

  auto colSensor1 = df1_sensor.Take<int>("col0");
  auto rowSensor1 = df1_sensor.Take<int>("row0");
  auto colSensor2 = df2_sensor.Take<int>("col0");
  auto rowSensor2 = df2_sensor.Take<int>("row0");
    
  auto xpos1 = df1_sensor.Take<double>("xpos");
  auto xpos2 = df2_sensor.Take<double>("xpos");
  auto ypos1 = df1_sensor.Take<double>("ypos");
  auto ypos2 = df2_sensor.Take<double>("ypos");
    
  // Create a histogram of col and row
  TH2F *hColRow      = new TH2F("hColRow", "Col vs Row", 512, 0, 512, 512, 0, 512);
  TH2F *hColRowDigi1 = new TH2F("hColRowDigi1", "Col vs Row Digi1 Coincidence", 512, 0, 512, 512, 0, 512);
  TH2F *hColRowDigi2 = new TH2F("hColRowDigi2", "Col vs Row Digi2 Coincidence", 512, 0, 512, 512, 0, 512);

  // Filtered col and row
  TH2F *hColRowDiff       = new TH2F("hColRowDiff", "Col Row Diff Raw", 512, -512, 512, 512, -512, 512);
  TH2F *hColRowDiffSignal = new TH2F("ColRowDiffSignal", "Col Row Diff Raw Coin", 512, -512, 512, 512, -512, 512);



  // Loop over the timeArray calcuating the difference but not when the index is the same
  int jStartDigi1 = 0;
  int jStartDigi2 = 0;
  int jStartSensor2 = 0;
  long timeWinMin = -100;
  long timeWinMax = 100;
  long plotWinMin = -500;
  long plotWinMax = 500;
  long timeLimMin = -10000;
  long timeLimMax = 10000;

  TH1F *hTimeDiffDigi1   = new TH1F("hTimeDiffDigi1", "Chip1-Tagger Coincidence; t_{diff} [ns]", 400, plotWinMin, plotWinMax);
  TH1F *hTimeDiffDigi2   = new TH1F("hTimeDiffDigi2", "Chip2-Tagger Coincidence; t_{diff} [ns]", 400, plotWinMin, plotWinMax);
  TH1F *hTimeDiffSensor2 = new TH1F("hTimeDiffSensor2", "Chip1-Chip2 Coincidence; t_{diff} [ns]", 400, plotWinMin, plotWinMax);
    
  //for workshop
  TH1F *hTimeDiffSensor = new TH1F("hTimeDiffSensor", "Chip1-Chip2 Coincidence; t_{diff} [ns]", 100, -100, 100);
  TH1F *hdxSensor = new TH1F("hdxSensor","Chip1-Chip2 xpos Coincidence; x_{diff} [55#mum pixel]", 100, -400, 400);
  TH1F *hdySensor = new TH1F("hdySensor","Chip1-Chip2 ypos Coincidence; y_{diff} [55#mum pixel]", 100, -400, 400);
    

  //timewalk using tagger as "trigger reference"
  TH2F *hTW1 = new TH2F("hTW1","Chip1-Tagger Timewalk;ToT [ns]; ToA [ns]",100,0,1000,100,-300,-50);
    
  for (size_t i = 0; i < timeSensor1->size(); i++) {

    hColRow->Fill(colSensor1->at(i), rowSensor1->at(i));

    //Digi1 differences
    for (size_t j = jStartDigi1; j < timeDigi1->size(); j++) {
      // hColRow->Fill(colSensor->at(j), rowSensor->at(j));
      long timeDiff = timeSensor1->at(i) - timeDigi1->at(j);
      if(timeDiff > timeLimMax) {
	jStartDigi1 = j;
	continue;
      }
      if(timeDiff < timeLimMin) {
	break;
      }
      hTimeDiffDigi1->Fill(timeDiff);
      hTW1->Fill(totSensor1->at(i), timeDiff);
      // hColRow->Fill(colSensor1->at(i), rowSensor1->at(i));

      if (timeDiff >timeWinMin && timeDiff < timeWinMax) {
	hColRowDigi1->Fill(colSensor1->at(i), rowSensor1->at(i));
      }
    }

    //Digi2 differences
    for (size_t j = jStartDigi2; j < timeDigi2->size(); j++) {
      long timeDiff = timeSensor1->at(i) - timeDigi2->at(j);
      if(timeDiff > timeLimMax) {
	jStartDigi2 = j;
	continue;
      }
      if(timeDiff < timeLimMin) {
	break;
      }
      hTimeDiffDigi2->Fill(timeDiff);
            
      if (timeDiff >timeWinMin && timeDiff < timeWinMax) {
	hColRowDigi2->Fill(colSensor1->at(i), rowSensor1->at(i));
      }
    }

    //Sensor2 differences
    for (size_t j = jStartSensor2; j < timeSensor2->size(); j++) {
      long timeDiff = timeSensor1->at(i) - timeSensor2->at(j);
      if(timeDiff > timeLimMax) {
	jStartSensor2 = j;
	continue;
      }
      if(timeDiff < timeLimMin) {
	break;
      }
      hTimeDiffSensor->Fill(timeDiff);
      hTimeDiffSensor2->Fill(timeDiff);
      int collDiff = colSensor1->at(i) - colSensor2->at(j);
      int rowDiff = rowSensor1->at(i) - rowSensor2->at(j);
      int xdiff = xpos1->at(i) - xpos2->at(j);
      int ydiff = ypos1->at(i) - ypos2->at(j);
      hColRowDiff->Fill(collDiff, rowDiff);
      if (timeDiff >timeWinMin && timeDiff < timeWinMax) {
	hColRowDiffSignal->Fill(collDiff, rowDiff);
	//hdxSensor->Fill(xdiff);
	//hdySensor->Fill(ydiff);
	hdxSensor->Fill(collDiff);
	hdySensor->Fill(rowDiff);
	      
      }
    }
  }
    
  gStyle->SetOptStat(0);
  //Angus Start
  // --- Timewalk Correction using (ToA_sensor - ToA_digital) ---
  std::vector<double> timewalkOffsets, totMatched;
  long matchWindow = 100; // ns
  size_t jStart = 0;

  // Match sensor hits to digital triggers
  for (size_t i = 0; i < timeSensor1->size(); ++i) {
    double toa_sensor = timeSensor1->at(i);
    double tot = totSensor1->at(i);

    for (size_t j = jStart; j < timeDigi1->size(); ++j) {
      double toa_digi = timeDigi1->at(j);
      double diff = toa_sensor - toa_digi;

      if (diff > matchWindow) {
	jStart = j;
	continue;
      }
      if (diff < -matchWindow) break;

      timewalkOffsets.push_back(diff);
      totMatched.push_back(tot);
      break;
    }
  }

  std::cout << "Matched events: " << timewalkOffsets.size() << std::endl;

  // --- Step 1: Timewalk vs ToT
  TGraph* grTimewalk = new TGraph(timewalkOffsets.size());
  for (size_t i = 0; i < timewalkOffsets.size(); ++i)
    grTimewalk->SetPoint(i, totMatched[i], timewalkOffsets[i]);

  grTimewalk->SetTitle("Timewalk vs ToT;ToT [ns];ToA - Digital Input [ns]");
  grTimewalk->SetMarkerStyle(20);
  grTimewalk->Draw();
  
  // --- Step 2: Fit with linear model
  cout << "Fitting timewalk graph here" << endl;
  TF1* fTW = new TF1("fTW", "[0] + [1]/pow(x, 0.5) +[2]/x",50, 600);
  grTimewalk->Fit(fTW, "");  // Quiet mode

  //double p0 = fTW->GetParameter(0);
  //double p1 = fTW->GetParameter(1);
  //std::cout << "Fit: p0 = " << p0 << ", p1 = " << p1 << std::endl;

  // --- Step 3: Apply correction
  std::vector<double> correctedOffsets;
  for (size_t i = 0; i < timewalkOffsets.size(); ++i) {
    double fitval = fTW->Eval(totMatched[i]);
    correctedOffsets.push_back(timewalkOffsets[i] - fitval);
  }



  // --- Step 5: 2D Histogram of Corrected Timewalk
  TH2F* hTW1Corrected = new TH2F("hTW1Corrected", "Corrected Timewalk;ToT [ns];(ToA - Digi) - Fit [ns]", 100, 0, 1000, 100, -100, 100);
  for (size_t i = 0; i < correctedOffsets.size(); ++i)
    hTW1Corrected->Fill(totMatched[i], correctedOffsets[i]);

  // --- Step 6: Plot All


  TCanvas* cTimewalk2D = new TCanvas("cTimewalk2D", "2D Timewalk Maps", 1600, 800);
  cTimewalk2D->Divide(2, 1);
  cTimewalk2D->cd(1); gPad->SetLeftMargin(0.15); hTW1->Draw("COLZ");
  cTimewalk2D->cd(2); gPad->SetLeftMargin(0.15); hTW1Corrected->Draw("COLZ");
  cTimewalk2D->Print("TimewalkCorrection_2D.png");
  //Angus End

  TCanvas *c00 = new TCanvas("c00", "c00", 1500, 500);
  c00->Divide(3, 1);
  c00->cd(1);
  hTimeDiffDigi1->Draw();
  c00->cd(2);
  hTimeDiffDigi2->Draw();
  c00->cd(3);
  hTimeDiffSensor2->Draw();
  c00->Print("TimeDifferenc00es1.png");
  c00->Close();
  
  TCanvas *c01 = new TCanvas("c01", "c01", 1500, 500);
  c01->Divide(3, 1);
  c01->cd(1);
  hColRow->Draw("colz");
  c01->cd(2);
  hColRowDigi1->Draw("colz");
  c01->cd(3);
  hColRowDigi2->Draw("colz");
  c01->Print("PositionCoincidences.png");
  c01->Close();
  
  TCanvas *c02 = new TCanvas("c02", "c02", 1500, 500);
  c02->Divide(2, 1);
  c02->cd(1);
  hColRowDiff->Draw("colz");
  c02->cd(2);
  hColRowDiffSignal->Draw("colz");
  c02->Print("PositionDifferences.png");
  c02->Close();
  
  //for the workshop
  TCanvas *c03 = new TCanvas("c03","c03",800,800);
  c03->cd();
  double roughmean=hTimeDiffSensor->GetMean();
  double roughsig=hTimeDiffSensor->GetStdDev();
  TF1 *fcoinc = new TF1("fcoinc","gaus");
  hTimeDiffSensor->Fit(fcoinc,"","",roughmean-roughsig,roughmean+roughsig);
  hTimeDiffSensor->Draw();
  TLatex tex;
  tex.SetNDC(1);
  double tcoin_sig = fcoinc->GetParameter(2);
  tex.DrawLatex(0.2,0.8,Form("#sigma = %1.2f ns",tcoin_sig));
  c03->Print("TimeRes.png");
    
  gPad->Clear();
  c03->cd();
  roughmean=hdxSensor->GetMean();
  roughsig=hdxSensor->GetStdDev();
  TF1 *fdx = new TF1("fdx","gaus");
  hdxSensor->Fit(fdx,"","",roughmean-roughsig,roughmean+roughsig);
  hdxSensor->Draw();
  double dx_sig = fdx->GetParameter(2);
  dx_sig *= 55 * pow(10,-3);
  tex.DrawLatex(0.15,0.7,Form("#sigma = %1.2f mm",dx_sig));
  c03->Print("xposRes.png");
    
  gPad->Clear();
  c03->cd();
  roughmean=hdySensor->GetMean();
  roughsig=hdySensor->GetStdDev();
  TF1 *fdy = new TF1("fdy","gaus");
  hdySensor->Fit(fdy,"","",roughmean-roughsig,roughmean+roughsig);
  hdySensor->Draw();
  double dy_sig = fdy->GetParameter(2);
  dy_sig *= 55 * pow(10,-3);
  tex.DrawLatex(0.15,0.7,Form("#sigma = %1.2f mm ",dy_sig));
  c03->Print("yposRes.png");
  c03->Close();
  
  TCanvas *c04 = new TCanvas("c04","");
  c04->cd();
  hTW1->Draw("colz");
  c04->Close();
  


 
 



  //Save the histograms to a root file
  //TFile *f = new TFile("TimeDifferences1.root", "RECREATE");
  // hTimeDiffDigi1->Write();
  // hTimeDiffDigi2->Write();
  // hTimeDiffSensor2->Write();
  // hColRow->Write();
  // hColRowDigi1->Write();
  // hColRowDigi2->Write();
  // hColRowDiff->Write();
  // hColRowDiffSignal->Write();
  //f->cd();
  //f->Write();
  //f->Close();

  
  //gApplication->Run();
    
  // // Draw the histogram
  // TCanvas c(1000,1000);
  // c.Divide(1, 3);
  // c.cd(1);
  // hTimeDiffs->Draw();
  // c.cd(2);
  // hColRow->Draw("colz");
  // c.cd(3);
  // hColRowDigi->Draw("colz");
  // c.Print("TimeDifferences1.png");
}
