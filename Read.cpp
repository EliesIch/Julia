
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TFrame.h"
#include "TH1F.h"
#include "TBenchmark.h"
#include "TSystem.h"

void Read()
{
    TFile *f = TFile::Open("Cd.root", "UPDATE");
    TTree *t;
    f->GetObject("WFM", t);
    std::vector<float> *WFM = 0;
    TBranch *Energy = 0;
    t->SetBranchAddress("WFM", &WFM, &Energy);
    // create  histogram
    TH1F *h = new TH1F("h", "Energy Spectrum", 300, -10, 150);
    h->GetXaxis()->SetTitle("Energy/keV");
    h->GetYaxis()->SetTitle("Counts");
    //--------------read all entries and fill the histograms---------
    Int_t nentries = (Int_t)t->GetEntries();
    // cout << nentries << endl;

    for (Int_t i = 0; i < nentries; i++)
    {
        Long64_t tentry = t->LoadTree(i);
        Energy->GetEntry(tentry);
        Float_t Area = 0;
        Float_t Back = 0; //Background

        Back = -std::accumulate(WFM->begin(), WFM->begin() + 1000, 0.0) / 1000;
        Area = (-std::accumulate(WFM->begin(), WFM->end(), 0.0) - Back)*1.54-3.354;
        h->Fill(Area);
        //cout << Back << endl;
        std::cout << i << "\r" << std::flush;
    }
    
    //----------------Fit the Histogram
    Double_t par[9];
    TF1 *g1 = new TF1("g1", "gaus", 0,20 );
    TF1 *g2 = new TF1("g2", "gaus", 20, 60);
    TF1 *g3 = new TF1("g2", "gaus", 80, 150);
    TF1 *total = new TF1("total", "gaus(0)+gaus(3)+gaus(6)", 0, 150);
    total->SetLineColor(4);
    h->Fit(g1, "R");
    h->Fit(g2, "R+");
    h->Fit(g3, "R+");
    g1->GetParameters(&par[0]);
    g2->GetParameters(&par[3]);
    g3->GetParameters(&par[6]);
    total->SetParameters(par);
    h->Fit(total, "R+");
    
    h->Draw();
    f->Write();
}
