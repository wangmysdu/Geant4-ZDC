#include "AnaEdepByLayer.h"

double SumEdepByLayer[2][30] = {0};

TGraph* EdepByLayer[2];

bool MyAnaFunc(const string & fileName);
void InitHist();
void PlotGraph();
void WritePlotToFile(const string & fileName);

void AnaEdepByLayer(int mode = 0)
{
    vector<string> InputFiles;
    InputFiles.push_back("ZDC.root");
    
    for (unsigned int ifile = 0; ifile < InputFiles.size(); ifile++){
        MyAnaFunc(InputFiles[ifile]);
    }
    if (mode == 0) PlotGraph();
    else WritePlotToFile("output_AnaZDC.root");
}

bool MyAnaFunc(const string & fileName)
{
    TFile* f = new TFile(fileName.c_str(), "READ");
    if (!f->IsOpen() || f->IsZombie()) return false;
    if (f->TestBit(TFile::kRecovered)) return false;
    cout<<"analyzing file "<<fileName<<endl;
    TTree* t = (TTree*)f->Get("ZDC");
    InitTree(t);

    for (unsigned int entry = 0; entry < t->GetEntries(); entry++){
        if (entry % 100 == 0) cout<<entry<<endl;
        t->GetEntry(entry);

        //initial
        const double InEnergy = 10000;

        for (unsigned int i = 0; i < ParDetID->size(); i++){
            vector<int> id;
            GetDetInfo(ParDetID->at(i), id);
            
            if (id[kSectorID] == 2) continue;
            
            int LayerID = id[kLayerID];

            if (id[kSectorID] == 1) LayerID += 15;

            SumEdepByLayer[id[kTypeID]][LayerID] += ParEdep->at(i);
            }
        }    

    for (int i = 0; i<2; i++){
        for (int j = 0; j < 30 ; j++){
            SumEdepByLayer[i][j] /= t->GetEntries();
        }
    }

    f->Close();
    delete f;
    return true;
}

void InitHist()
{

}

void PlotGraph()
{
    TCanvas* c1 = new TCanvas("c1", "c1", 1200, 600);
    c1->Divide(2, 1);
    double x[30];
    for (int i = 0; i < 30; i++) x[i] = i;

    EdepByLayer[0] = new TGraph(30, x, SumEdepByLayer[0]);
    EdepByLayer[0]->SetMarkerStyle(20);
    EdepByLayer[0]->SetTitle("Sensitive Layers; LaLayers; Edep(MeV)");

    EdepByLayer[1] = new TGraph(30, x, SumEdepByLayer[1]);
    EdepByLayer[1]->SetMarkerStyle(20);
    EdepByLayer[1]->SetTitle("Absorber Layers; LaLayers; Edep(MeV)");

    c1->cd (1);
    EdepByLayer[0]->Draw("Ap");
    c1->cd (2);
    EdepByLayer[1]->Draw("Ap");
}
