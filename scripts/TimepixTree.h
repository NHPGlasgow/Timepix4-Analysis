//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Jan 25 14:22:45 2025 by ROOT version 6.32.08
// from TTree clusterTree/Pixel packets
// found on file: N55-250122-145225.root
//////////////////////////////////////////////////////////

#ifndef TimepixTree_h
#define TimepixTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class TimepixTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           nhits;
   Int_t           col[1];   //[nhits]
   Int_t           row[1];   //[nhits]
   Long64_t        toa[1];   //[nhits]
   Double_t        tot[1];   //[nhits]
   Int_t           minCol;
   Int_t           maxCol;
   Int_t           minRow;
   Int_t           maxRow;
   Int_t           digitalInput;
   Bool_t          pileup[1];   //[nhits]
   Double_t        xpos;
   Double_t        ypos;
   Double_t        cltot;
   Int_t           ctoa[1];   //[nhits]
   Int_t           ctot[1];   //[nhits]
   Int_t           ftoaRise[1];   //[nhits]
   Int_t           ftoaFall[1];   //[nhits]
   Int_t           uftoaStart[1];   //[nhits]
   Int_t           uftoaStop[1];   //[nhits]
   Long64_t        heartbeat[1];   //[nhits]
   ULong64_t       packet[1];   //[nhits]
   Int_t           ccltot;
   Double_t        threshold;

   // List of branches
   TBranch        *b_nhits;   //!
   TBranch        *b_col;   //!
   TBranch        *b_row;   //!
   TBranch        *b_toa;   //!
   TBranch        *b_tot;   //!
   TBranch        *b_minCol;   //!
   TBranch        *b_maxCol;   //!
   TBranch        *b_minRow;   //!
   TBranch        *b_maxRow;   //!
   TBranch        *b_digitalInput;   //!
   TBranch        *b_pileup;   //!
   TBranch        *b_xpos;   //!
   TBranch        *b_ypos;   //!
   TBranch        *b_cltot;   //!
   TBranch        *b_ctoa;   //!
   TBranch        *b_ctot;   //!
   TBranch        *b_ftoaRise;   //!
   TBranch        *b_ftoaFall;   //!
   TBranch        *b_uftoaStart;   //!
   TBranch        *b_uftoaStop;   //!
   TBranch        *b_heartbeat;   //!
   TBranch        *b_packet;   //!
   TBranch        *b_ccltot;   //!
   TBranch        *b_threshold;   //!

   TimepixTree(TTree *tree=0);
   virtual ~TimepixTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TimepixTree_cxx
TimepixTree::TimepixTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("N55-250122-145225.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("N55-250122-145225.root");
      }
      f->GetObject("clusterTree",tree);

   }
   Init(tree);
}

TimepixTree::~TimepixTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TimepixTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TimepixTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TimepixTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("nhits", &nhits, &b_nhits);
   fChain->SetBranchAddress("col", col, &b_col);
   fChain->SetBranchAddress("row", row, &b_row);
   fChain->SetBranchAddress("toa", toa, &b_toa);
   fChain->SetBranchAddress("tot", tot, &b_tot);
   fChain->SetBranchAddress("minCol", &minCol, &b_minCol);
   fChain->SetBranchAddress("maxCol", &maxCol, &b_maxCol);
   fChain->SetBranchAddress("minRow", &minRow, &b_minRow);
   fChain->SetBranchAddress("maxRow", &maxRow, &b_maxRow);
   fChain->SetBranchAddress("digitalInput", &digitalInput, &b_digitalInput);
   fChain->SetBranchAddress("pileup", pileup, &b_pileup);
   fChain->SetBranchAddress("xpos", &xpos, &b_xpos);
   fChain->SetBranchAddress("ypos", &ypos, &b_ypos);
   fChain->SetBranchAddress("cltot", &cltot, &b_cltot);
   fChain->SetBranchAddress("ctoa", ctoa, &b_ctoa);
   fChain->SetBranchAddress("ctot", ctot, &b_ctot);
   fChain->SetBranchAddress("ftoaRise", ftoaRise, &b_ftoaRise);
   fChain->SetBranchAddress("ftoaFall", ftoaFall, &b_ftoaFall);
   fChain->SetBranchAddress("uftoaStart", uftoaStart, &b_uftoaStart);
   fChain->SetBranchAddress("uftoaStop", uftoaStop, &b_uftoaStop);
   fChain->SetBranchAddress("heartbeat", heartbeat, &b_heartbeat);
   fChain->SetBranchAddress("packet", packet, &b_packet);
   fChain->SetBranchAddress("ccltot", &ccltot, &b_ccltot);
   fChain->SetBranchAddress("threshold", &threshold, &b_threshold);
   Notify();
}

bool TimepixTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void TimepixTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TimepixTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TimepixTree_cxx
