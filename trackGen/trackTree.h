//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Aug 20 14:14:04 2026 by ROOT version 6.36.04
// from TTree trackTree/Tree with track vectors
// found on file: test0.root
//////////////////////////////////////////////////////////

#ifndef trackTree_h
#define trackTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class trackTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           event_no;
   vector<int>     *track_type;
   vector<float>   *ppx;
   vector<float>   *ppy;
   vector<float>   *ppz;
   vector<float>   *vpx;
   vector<float>   *vpy;
   vector<float>   *vpz;
   vector<int>     *track_nhits;
   vector<int>     *row;
   vector<int>     *col;
   vector<int>     *lay;

   // List of branches
   TBranch        *b_event_no;   //!
   TBranch        *b_track_type;   //!
   TBranch        *b_ppx;   //!
   TBranch        *b_ppy;   //!
   TBranch        *b_ppz;   //!
   TBranch        *b_vpx;   //!
   TBranch        *b_vpy;   //!
   TBranch        *b_vpz;   //!
   TBranch        *b_track_nhits;   //!
   TBranch        *b_row;   //!
   TBranch        *b_col;   //!
   TBranch        *b_lay;   //!

   trackTree(TTree *tree=0);
   virtual ~trackTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef trackTree_cxx
trackTree::trackTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("test0.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("test0.root");
      }
      f->GetObject("trackTree",tree);

   }
   Init(tree);
}

trackTree::~trackTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t trackTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t trackTree::LoadTree(Long64_t entry)
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

void trackTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   track_type = 0;
   ppx = 0;
   ppy = 0;
   ppz = 0;
   vpx = 0;
   vpy = 0;
   vpz = 0;
   track_nhits = 0;
   row = 0;
   col = 0;
   lay = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("event_no", &event_no, &b_event_no);
   fChain->SetBranchAddress("track_type", &track_type, &b_track_type);
   fChain->SetBranchAddress("ppx", &ppx, &b_ppx);
   fChain->SetBranchAddress("ppy", &ppy, &b_ppy);
   fChain->SetBranchAddress("ppz", &ppz, &b_ppz);
   fChain->SetBranchAddress("vpx", &vpx, &b_vpx);
   fChain->SetBranchAddress("vpy", &vpy, &b_vpy);
   fChain->SetBranchAddress("vpz", &vpz, &b_vpz);
   fChain->SetBranchAddress("track_nhits", &track_nhits, &b_track_nhits);
   fChain->SetBranchAddress("row", &row, &b_row);
   fChain->SetBranchAddress("col", &col, &b_col);
   fChain->SetBranchAddress("lay", &lay, &b_lay);
   Notify();
}

bool trackTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void trackTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t trackTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef trackTree_cxx
