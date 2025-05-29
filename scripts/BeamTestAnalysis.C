#include <ROOT/RDataFrame.hxx>
#include <TCanvas.h>
#include <TH1F.h>

void BeamTestAnalysis() {
  // Create a ROOT DataFrame from a CSV file or a TTree
  ROOT::RDataFrame df1("clusterTree", "/w/work5/eic/Timepix/MainzTests/trees/N55-250206-123249.root");
  ROOT::RDataFrame df2("clusterTree", "/w/work5/eic/Timepix/MainzTests/trees/N200-250206-123235.root");
  
  auto df1_1000 = df1.Define("toa0", "toa[0]*(25.0/128.0)").Define("col0", "col[0]").Define("row0", "row[0]");
  auto df2_1000 = df2.Define("toa0", "toa[0]*(25.0/128.0)").Define("col0", "col[0]").Define("row0", "row[0]");
  
  // Define a new column taking just the first element of the array
    auto df1_digi   = df1_1000.Filter("digitalInput==0").Range(1000000);
    auto df1_sensor = df1_1000.Filter("digitalInput==-1").Range(1000000);
    auto df2_digi   = df2_1000.Filter("digitalInput==0").Range(1000000);
    auto df2_sensor = df2_1000.Filter("digitalInput==-1").Range(1000000);

    auto timeDigi1   = df1_digi.Take<Double_t>("toa0");
    auto timeSensor1 = df1_sensor.Take<Double_t>("toa0");
    auto timeDigi2   = df2_digi.Take<Double_t>("toa0");
    auto timeSensor2 = df2_sensor.Take<Double_t>("toa0");

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
    
    //Save the histograms to a root file
    TFile *f = new TFile("TimeDifferences1.root", "RECREATE");
    hTimeDiffDigi1->Write();
    hTimeDiffDigi2->Write();
    hTimeDiffSensor2->Write();
    hColRow->Write();
    hColRowDigi1->Write();
    hColRowDigi2->Write();
    hColRowDiff->Write();
    hColRowDiffSignal->Write();
    f->Close();

    TCanvas c("c", "c", 1500, 500);
    c.Divide(3, 1);
    c.cd(1);
    hTimeDiffDigi1->Draw();
    c.cd(2);
    hTimeDiffDigi2->Draw();
    c.cd(3);
    hTimeDiffSensor2->Draw();
    c.SaveAs("TimeDifferences1.png");
    
    TCanvas c2("c2", "c2", 1500, 500);
    c2.Divide(3, 1);
    c2.cd(1);
    hColRow->Draw("colz");
    c2.cd(2);
    hColRowDigi1->Draw("colz");
    c2.cd(3);
    hColRowDigi2->Draw("colz");
    c2.SaveAs("PositionCoincidences.png");
    
    TCanvas c3("c3", "c3", 1500, 500);
    c3.Divide(2, 1);
    c3.cd(1);
    hColRowDiff->Draw("colz");
    c3.cd(2);
    hColRowDiffSignal->Draw("colz");
    c3.SaveAs("PositionDifferences.png");

    //for the workshop
    TCanvas cnew("cnew","cnew",800,800);
    cnew.cd();
    double roughmean=hTimeDiffSensor->GetMean();
    double roughsig=hTimeDiffSensor->GetStdDev();
    TF1 *fcoinc = new TF1("fcoinc","gaus");
    hTimeDiffSensor->Fit(fcoinc,"","",roughmean-roughsig,roughmean+roughsig);
    hTimeDiffSensor->Draw();
    TLatex tex;
    tex.SetNDC(1);
    double tcoin_sig = fcoinc->GetParameter(2);
    tex.DrawLatex(0.2,0.8,Form("#sigma = %1.2f ns",tcoin_sig));
    cnew.SaveAs("TimeRes.png");
    
    gPad->Clear();
    cnew.cd();
    roughmean=hdxSensor->GetMean();
    roughsig=hdxSensor->GetStdDev();
    TF1 *fdx = new TF1("fdx","gaus");
    hdxSensor->Fit(fdx,"","",roughmean-roughsig,roughmean+roughsig);
    hdxSensor->Draw();
    double dx_sig = fdx->GetParameter(2);
    dx_sig *= 55 * pow(10,-3);
    tex.DrawLatex(0.15,0.7,Form("#sigma = %1.2f mm",dx_sig));
    cnew.SaveAs("xposRes.png");
    
    gPad->Clear();
    cnew.cd();
    roughmean=hdySensor->GetMean();
    roughsig=hdySensor->GetStdDev();
    TF1 *fdy = new TF1("fdy","gaus");
    hdySensor->Fit(fdy,"","",roughmean-roughsig,roughmean+roughsig);
    hdySensor->Draw();
    double dy_sig = fdy->GetParameter(2);
    dy_sig *= 55 * pow(10,-3);
    tex.DrawLatex(0.15,0.7,Form("#sigma = %1.2f mm ",dy_sig));
    cnew.SaveAs("yposRes.png");
    
    // // Draw the histogram
    // TCanvas c(1000,1000);
    // c.Divide(1, 3);
    // c.cd(1);
    // hTimeDiffs->Draw();
    // c.cd(2);
    // hColRow->Draw("colz");
    // c.cd(3);
    // hColRowDigi->Draw("colz");
    // c.SaveAs("TimeDifferences1.png");
}
