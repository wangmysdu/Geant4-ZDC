#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include <vector>
#include "AnalyzeNPE_ZDC.h"

TH1D* SecNPEHist[NSector]; 
TH1D* ModNPEHist[NSector]; 

bool AnalyzeNPE(const std::string &fileName);
void InitNPEHist();
void PlotNPE();
void WriteNPEToFile(const std::string fileName);


void AnalyzeNPE_ZDC(int mode = 0) {
    std::vector<std::string> inputFiles = {"ZDC.root"};
    InitNPEHist();

    for (const auto& file : inputFiles) {
        AnalyzeNPE(file);
    }

    if (mode == 0) PlotNPE();
    else WriteNPEToFile("NPE_Analysis.root");
}


void InitNPEHist() {
    for (int i = 0; i < NSector; ++i) {
        SecNPEHist[i] = new TH1D(Form("Sec%d_TotalNPE", i+1), 
                                Form("Section %d Total NPE;NPE;Counts", i+1),
                                40, 0, 40);  
        ModNPEHist[i] = new TH1D(Form("Sec%d_ModuleNPE", i+1), 
                                Form("Section %d Module NPE;NPE;Counts", i+1),
                                40, 0, 40); 
    }
}


bool AnalyzeNPE(const std::string &fileName) {
    TFile *file = TFile::Open(fileName.c_str(), "READ");
    if (!file || file->IsZombie()) return false;

    TTree *tree = (TTree*)file->Get("ZDC");
    if (!tree) return false;

    
    std::vector<int> *modNPE[NSector] = {nullptr, nullptr, nullptr};
    tree->SetBranchAddress("Mod.NPESec1", &modNPE[0]);
    tree->SetBranchAddress("Mod.NPESec2", &modNPE[1]);
    tree->SetBranchAddress("Mod.NPESec3", &modNPE[2]);

    for (Long64_t entry = 0; entry < tree->GetEntries(); ++entry) {
        tree->GetEntry(entry);

        int totalNPE[NSector] = {0};
        
        for (int sec = 0; sec < NSector; ++sec) {
            if (!modNPE[sec]) continue;
            
            for (const auto &npe : *modNPE[sec]) {
                totalNPE[sec] += npe;
                ModNPEHist[sec]->Fill(npe); 
            }
            SecNPEHist[sec]->Fill(totalNPE[sec]);
        }
    }

    file->Close();
    return true;
}


void PlotNPE() {
    TCanvas *c1 = new TCanvas("c1", "NPE Distribution", 1200, 600);
    c1->Divide(2, 1);

    for (int i = 0; i < 2/*NSector*/; ++i) {
        c1->cd(i+1);
        SecNPEHist[i]->SetLineColor(kBlue);
        SecNPEHist[i]->Draw();
        ModNPEHist[i]->SetLineColor(kRed);
        ModNPEHist[i]->Draw("same");
    }


}


void WriteNPEToFile(const std::string fileName) {
    TFile outFile(fileName.c_str(), "RECREATE");
    for (int i = 0; i < NSector; ++i) {
        SecNPEHist[i]->Write();
        ModNPEHist[i]->Write();
    }

    outFile.Close();
}