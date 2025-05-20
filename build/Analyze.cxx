#include "AnaZDC.h"

TH1D* TotalEdepHistCorrSum = nullptr;
TH1D* TotalEdepHist[NSector];

const double InEnergy  = 10000;

// 添加函数声明
void InitHist();

void Analyze()
{
    const int NTYPE = 2;
    TFile* f = new TFile("ZDC.root", "READ");
    TTree* t = (TTree*)f->Get("ZDC");
    InitTree(t);
    InitHist();
    
    TLinearFitter *lf=new TLinearFitter(NSector);
    lf->SetFormula("x0++x1");
    double CaliConst[NSector] = {28.267, 28.267, 54.56};
    
    unsigned int NEvent = t->GetEntries();

    double* FitX = new double [NEvent*NTYPE];
    double* FitY = new double [NEvent];
    double* FitE = new double [NEvent];
    
    for (unsigned int entry = 0; entry < NEvent; entry++){
        t->GetEntry(entry);
        if (entry % 100 == 0) cout<<"analyzing event "<<entry<<endl;
        
        double totalEDep[NSector] = {0., 0., 0.};

        // 使用sector级别的数据
        for (int i = 0; i < NSector; i++) {
            totalEDep[i] = SecEdepSecSen[i];
        }

        // 删除重复的内层循环，直接使用当前entry的数据
        double ReconE = 0.0;
        for (int i = 0; i < NSector; i++) {
            ReconE += SecEdepSecSen[i] * CaliConst[i];
        }
        TotalEdepHistCorrSum->Fill(ReconE / InEnergy);

        for (int i=0; i<NSector; i++) 
            TotalEdepHist[i]->Fill(totalEDep[i]);
        
        FitX[0 + entry*NTYPE] = totalEDep[0] + totalEDep[1];
        FitX[1 + entry*NTYPE] = totalEDep[2];
        FitY[entry] = InEnergy;
        FitE[entry] = 10;
    }
    
    TFile* outFile = new TFile("output_analysis.root", "RECREATE");
    outFile->cd();
    for (int i=0; i<NSector; i++){
        TotalEdepHist[i]->Write();
    }
    TotalEdepHistCorrSum->Write();
    outFile->Close();
    
    lf->AssignData(NEvent, NTYPE, FitX, FitY, FitE);
    lf->Eval();
    TVectorD params;
    TVectorD errors;
    lf->GetParameters(params);
    lf->GetErrors(errors);
    for (Int_t i=0; i<2; i++)
       printf("par[%d]=%f+-%f\n", i, params(i), errors(i));
    Double_t chisquare=lf->GetChisquare();
    printf("chisquare=%f\n", chisquare);
};

void InitHist()
{
    double MaxE[NSector] = {1000, 1000, 500};
    for (int i=0; i<NSector; i++){
        // 将DETNAME改为SectorName
        TotalEdepHist[i] = new TH1D(Form("%s_total_edep", SectorName[i].c_str()), 
                                   Form("%s_total_edep", SectorName[i].c_str()), 
                                   200, 0, MaxE[i]);
        TotalEdepHist[i]->GetXaxis()->SetTitle("Edep [MeV]");
        TotalEdepHist[i]->GetYaxis()->SetTitle("count");
    }
    TotalEdepHistCorrSum = new TH1D("Recon_E_over_E", "Recon_E_over_E", 200, 0, 3);
    TotalEdepHistCorrSum->GetXaxis()->SetTitle("E_{recon}/E");
    TotalEdepHistCorrSum->GetYaxis()->SetTitle("count");
};