struct RunFile{
  std::string chip;
  std::string name;
  std::string label;
  std::string file;
  std::string filepath;
  double      toff;
  //std::string datadir = "/scratch1/tpx4_config/data/trees/";
  std::string datadir = "/w/work6/home/kl13k/scapa/trees/";

  TH1D *htoa;
  TH1D *htot;
  TH2D *hxy;
  
  Color_t color;
  
  RunFile(std::string fchip,
	  std::string ffile,
	  std::string fname,
	  std::string flabel,
	  double      ftoff )
    : chip(fchip), file(ffile), name(fname), label(flabel),toff(ftoff)
  {
    filepath = datadir+ffile;
  }
};

//Hists for information based on combining the layers somehow 
TH2F *trackDiff[4];
TH2F *trackPlane[4];
TGraph *pulseRatio[4];


void TrackerSimple(int nevents=100, int start=0){
  //nevents = noof pulses to process from each pair of files
  //start   = pulse to start with
  
  //layer 0 files for each run
  RunFile L0_before( "N55",  "N55-260415-142248.root",  "L0_before",  "L0 Before Shielding",   0.0);
  RunFile L0_after(  "N55",  "N55-260415-150214.root",  "L0_after",   "L0 After Shielding",    0.0 );
  RunFile L0_wire1(  "N55",  "N55-260415-153211.root",  "L0_wire1",   "L0 Thin Wire",          0.0);
  RunFile L0_wire2(  "N55",  "N55-260415-155916.root",  "L0_wire2",   "L0 Thick Wire",         0.0);
    
  //layer 1 files for each run
  RunFile L1_before("N200",  "N200-260415-142234.root", "L1_before",  "L1 Before Shielding",  14.4);
  RunFile L1_after( "N200",  "N200-260415-150214.root", "L1_after",   "L1 After Shielding",    0.2);
  RunFile L1_wire1( "N200",  "N200-260415-153209.root", "L1_wire1",   "L1 Thin Wire",          3.5);
  RunFile L1_wire2( "N200",  "N200-260415-155922.root", "L1_wire2",   "L1 Thick Wire",        -6.5);
  //...
  //more layers here ....

  //vectors for all the run files for each layer.
		       
  std::vector<RunFile> Layer0;
  std::vector<RunFile> Layer1;
  std::vector<RunFile> Layer2;
  std::vector<RunFile> Layer3;
  
  Layer0.push_back(L0_before); Layer1.push_back(L1_before);
  Layer0.push_back(L0_after);  Layer1.push_back(L1_after);
  Layer0.push_back(L0_wire1);  Layer1.push_back(L1_wire1);
  Layer0.push_back(L0_wire2);  Layer1.push_back(L1_wire2);

  //to get the times of the pulses for each layer
  TSpectrum *s = new TSpectrum(300); //find up to 300 peaks 
  int pulseSize[300];
  int nPulses;

  double pTime0 = 0.0;
  double d1=120.0;
  double d2=3000.0;
  double x0,x1,x2;
  double y0,y1,y2;
  //for all runs of interest
  for(int n=0;n<4;n++){
    //Go over the layers in the run 2 for now, but maybe 4 later
    auto l0=Layer0[n];
    auto l1=Layer1[n];
    //auto l2=Layer0[n];
    //auto l3=Layer1[n];
    char fstring[100];

    // ***** Layer 0 *********************************************************** //
    //make dataframes of each layer, add a seconds column and stop after 590s.
    ROOT::RDataFrame df_raw0("clusterTree",l0.filepath.c_str());
    auto df_seconds0 = df_raw0.Define("toa_seconds", "toa[0]*25.0/(1e9*128.0)");

    //filter on something of interest
    auto df0 = df_seconds0.Filter("(toa_seconds<590)&&(tot[0]>4.0)");
    auto htoa0 = df0.Histo1D({("htoa_"+l0.name).c_str(), (l0.label+";Time [s];Hits / bin").c_str(), 2500,0,600},"toa_seconds");
    l0.htoa = (TH1D*)htoa0->Clone(htoa0->GetName());
    // ***** Layer 0 *********************************************************** //
   
    
    // ***** Layer 1 *********************************************************** //
    //make dataframes of each layer, add a seconds column and stop after 590s.
     ROOT::RDataFrame df_raw1("clusterTree",l1.filepath.c_str());
    auto df_seconds1 = df_raw1.Define("toa_seconds", "toa[0]*25.0/(1e9*128.0)");

    //filter on something of interest
    auto df1 = df_seconds1.Filter("(toa_seconds<590)&&(tot[0]>4.0)");
    auto htoa1 = df1.Histo1D({("htoa_"+l1.name).c_str(),(l1.label+";Time [s];Hits / bin").c_str(),2500,0,600},"toa_seconds");
    l1.htoa= (TH1D*)htoa1->Clone(htoa1->GetName());
    // ***** Layer 1 *********************************************************** //

    //find the times of the pulses in the 1st nevents in layer 0.
    nPulses = s->Search(l0.htoa,1,"",0.01);
    //sort the pulses in ascending order of size  
    TMath::Sort(nPulses,s->GetPositionY(),pulseSize,0);
    
    //create global hists etc
    pulseRatio[n] = new TGraph();
    pulseRatio[n]->SetName(("pulseRatio_"+l0.name+"_"+l1.name).c_str());
    
    trackDiff[n]  = new TH2F(("trackDiff_"+l0.name+"_"+l1.name).c_str(), (l0.label+"_"+l1.label+" diff;dx (mm);dy (mm)").c_str(),510,-25,25,510,-25,25);
    trackPlane[n] = new TH2F(("trackPlane_"+l0.name+"_"+l1.name).c_str(), (l0.label+"_"+l1.label+" plane;x (mm);y (mm)").c_str(),510,-250,250,510,-250,250);
     
    for(int p = start; p<nevents+start;p++){ //do required no of pulses
      //first pulse from layer 0
      pTime0 = s->GetPositionX()[pulseSize[p]];
      cout << "Using pulse at " << s->GetPositionX()[pulseSize[p]] <<" s, with "
	   <<  s->GetPositionY()[pulseSize[p]] << " counts in Run " << l0.name << endl;
      sprintf(fstring,"(toa_seconds>=%f)&&(toa_seconds<=%f)",pTime0-1.0,pTime0+1.0);
      //Get x,y arrays for this pulse.
      auto df0p =  df_seconds0.Filter(fstring);
      auto xpos0 = df0p.Take<double>("xpos");
      auto ypos0 = df0p.Take<double>("ypos");
      
      //apply toff to other layers to find filter time.
      cout << "Applying offset of " << l1.toff << " s to Layer 1 file: "<< l1.name << endl;
      cout << "Using pulse at " << pTime0+l1.toff <<" s, with "
	   << l1.htoa->Integral(l1.htoa->FindBin(pTime0+l1.toff-1.0),l1.htoa->FindBin(pTime0+l1.toff+1.0))
	   << " counts in Run " << l1.name << endl << endl;
      sprintf(fstring,"(toa_seconds>=%f)&&(toa_seconds<=%f)",pTime0+l1.toff-1.0,pTime0+l1.toff+1.0); 
      auto df1p =  df_seconds1.Filter(fstring);
      auto xpos1 = df1p.Take<double>("xpos");
      auto ypos1 = df1p.Take<double>("ypos");

      //graph the ration of pulse size L1/L0 to try and check that we'lre getting the correct matching between L0 and L1.
      pulseRatio[n]->AddPoint(p,s->GetPositionY()[pulseSize[p]]/l1.htoa->Integral(l1.htoa->FindBin(pTime0+l1.toff-1.0),l1.htoa->FindBin(pTime0+l1.toff+1.0)));
      
      //do the combinatorics for 
      for(int pxl0=0;pxl0<xpos0->size();pxl0++){
	//cout << xpos0->at(pxl0) <<", "  << ypos0->at(pxl0) << endl;
	x0=xpos0->at(pxl0);
	y0=ypos0->at(pxl0);
	for(int pxl1=0;pxl1<xpos1->size();pxl1++){
	  //cout << n << " " << pxl0 << " " << pxl1 << " " <<  xpos1->at(pxl1)-xpos0->at(pxl0) << " " << ypos1->at(pxl1)-ypos0->at(pxl0) << endl;
	  x1=xpos1->at(pxl1);
	  y1=ypos1->at(pxl1);

	  trackDiff[n]->Fill(x0-x1,y0-y1);
	  x2 = x1 + (x0-x1)*(d2/d1);
	  y2 = y1 + (y0-y1)*(d2/d1);
	  trackPlane[n]->Fill(x2,y2);
	}
      }
    }
  }
}
