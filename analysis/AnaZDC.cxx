#include "AnaZDC.h"

//user defined global histogram pointers
TH1D* SecEdepSenHist[NSector];
TH1D* SecEdepAbsHist[NSector];
TH1D* ModEdepSenHist[NSector];
TH1D* ModEdepAbsHist[NSector];
TH1D* ParEdepSenHist[NSector];
TH1D* ParEdepAbsHist[NSector];

TH2D* Sec3VsSec12EdepHist;

//my example analysis code
bool MyAnaFunc(const string & fileName);
void InitHist();
void PlotGraph();
void WritePlotToFile(const string fileName);

//the main function of this script
//mode == 0, plotting histograms
//mode != 0, writing all histograms to root file
void AnaZDC(int mode = 0)
{
    //put input files here
    vector<string> InputFiles;
    InputFiles.push_back("ZDC.root");
    
    //initialize the histogram
    InitHist();
    
    //analysing the files
    for (unsigned int ifile = 0; ifile < InputFiles.size(); ifile++){
        MyAnaFunc(InputFiles[ifile]);
    }
    
    if (mode == 0) PlotGraph();                 //plot histograms
    else WritePlotToFile("output_AnaZDC.root"); //write histograms
}

bool MyAnaFunc(const string & fileName)
{
    TFile* f = new TFile(fileName.c_str(), "READ");
    
    //make sure the file is closed properly
    if (!f->IsOpen() || f->IsZombie()) return false;
    if (f->TestBit(TFile::kRecovered)) return false;
    
    cout<<"analyzing file "<<fileName<<endl;
    TTree* t = (TTree*)f->Get("ZDC");
    InitTree(t);

    for (unsigned int entry = 0; entry < t->GetEntries(); entry++){
        if (entry % 100 == 0) cout<<entry<<endl;
        t->GetEntry(entry);
        
        //fill the sector level info
        for (int i = 0; i < NSector; i++){
            SecEdepSenHist[i]->Fill(SecEdepSecSen[i]);
            SecEdepAbsHist[i]->Fill(SecEdepSecAbs[i]);
        }
        Sec3VsSec12EdepHist->Fill(SecEdepSecSen[0] + SecEdepSecSen[1], SecEdepSecSen[2]);
        
        //fill the module level info
        for (int i = 0; i < NSector; i++){
            double SumModSen = 0.;
            for (unsigned int j=0; j<ModEdepSecSen[i]->size(); j++) SumModSen += ModEdepSecSen[i]->at(j);
            ModEdepSenHist[i]->Fill(SumModSen);
            
            double SumModAbs = 0.;
            for (unsigned int j=0; j<ModEdepSecAbs[i]->size(); j++) SumModAbs += ModEdepSecAbs[i]->at(j);
            ModEdepAbsHist[i]->Fill(SumModAbs);
        }
        
        //fill the particle level info
        double SumParSen[NSector] = {0., 0., 0.};
        double SumParAbs[NSector] = {0., 0., 0.};
        
        for (unsigned int i = 0; i < ParDetID->size(); i++){
            //extract detector information from detector ID
            vector<int> id;
            GetDetInfo(ParDetID->at(i), id);
            
            if (id[kTypeID] == 1){
                //this is a sensitive layer
                if (id[kSectorID] >= 0 && id[kSectorID] < NSector)
                SumParSen[id[kSectorID]]+=ParEdep->at(i);
            }
            else if (id[kTypeID] == 2){
                //this is an absorber layer
                if (id[kSectorID] >= 0 && id[kSectorID] < NSector)
                SumParAbs[id[kSectorID]]+=ParEdep->at(i);
            }
        }
        for (int i=0; i<NSector; i++){
            ParEdepSenHist[i]->Fill(SumParSen[i]);
            ParEdepAbsHist[i]->Fill(SumParAbs[i]);
        }
    }
    
    f->Close();
    return true;
}

void PlotGraph()
{
    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 600);
    c1->Divide(3, 1);
    
    for (int i=0; i<NSector; i++){
        c1->cd(i+1);
        SecEdepSenHist[i]->Draw();
        ModEdepSenHist[i]->Draw("histsame");
        ParEdepSenHist[i]->Draw("histsame");
    }
    
    
    TCanvas* c2 = new TCanvas("c2", "c2", 1600, 600);
    c2->Divide(3, 1);
    
    for (int i=0; i<NSector; i++){
        c2->cd(i+1);
        SecEdepAbsHist[i]->Draw();
        ModEdepAbsHist[i]->Draw("histsame");
        ParEdepAbsHist[i]->Draw("histsame");
    }
    
    TCanvas* c3 = new TCanvas("c3", "c3", 800, 600);
    Sec3VsSec12EdepHist->Draw();
}
void InitHist()
{   
    for (int i=0; i<NSector; i++){
        //sector level edep hists for sensitive and absorber
        SecEdepSenHist[i] = Init1DHist(Form("Sec_Edep_Sen_%s", SectorName[i].c_str()), 
                                       100, 0, 500, "Edep [MeV]", "count", 1);
        SecEdepAbsHist[i] = Init1DHist(Form("Sec_Edep_Abs_%s", SectorName[i].c_str()), 
                                       100, 0, 8000, "Edep [MeV]", "count", 1);
        //module level edep hists for sensitive and absorber
        ModEdepSenHist[i] = Init1DHist(Form("Mod_Edep_Sen_%s", SectorName[i].c_str()), 
                                       100, 0, 500, "Edep [MeV]", "count", 2);
        ModEdepAbsHist[i] = Init1DHist(Form("Mod_Edep_Abs_%s", SectorName[i].c_str()), 
                                       100, 0, 8000, "Edep [MeV]", "count", 2);
        //particle level edep hists for sensitive and absorber
        ParEdepSenHist[i] = Init1DHist(Form("Par_Edep_Sen_%s", SectorName[i].c_str()), 
                                       100, 0, 500, "Edep [MeV]", "count", 4);
        ParEdepAbsHist[i] = Init1DHist(Form("Par_Edep_Abs_%s", SectorName[i].c_str()), 
                                       100, 0, 8000, "Edep [MeV]", "count", 4);
    }
    Sec3VsSec12EdepHist = Init2DHist("Edep_sec3_vs_sec12", 100, 0, 500, 100, 0, 500, "Edep [MeV]", "Edep [MeV]");
}
