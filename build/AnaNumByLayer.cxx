#include "AnaNumByLayer.h"

double SumPhotonByLayer[2][30] = {0}; // [0:sensitive layer, 1:absober]

TGraph* PhotonByLayer[2];

bool MyAnaFunc(const string & fileName);
void InitHist();
void PlotGraph();
void WritePlotToFile(const string & fileName);

void AnaNumByLayer(int mode = 0)
{
    vector<string> InputFiles;
    InputFiles.push_back("ZDC.root");
    
    memset(SumPhotonByLayer, 0, sizeof(SumPhotonByLayer));

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
    cout << "analyzing file " << fileName << endl;
    
    TTree* t = (TTree*)f->Get("ZDC");
    if (!t) return false;
    InitTree(t);

    const Long64_t nEntries = t->GetEntries();
    for (Long64_t entry = 0; entry < nEntries; entry++){
        if (entry % 100 == 0) cout << "Processing entry " << entry << "/" << nEntries << endl;
        t->GetEntry(entry);

        for (unsigned int i = 0; i < ParDetID->size(); i++){
            // (PDG=22)
            if (ParPDG->at(i) != 22) continue;

            vector<int> detInfo;
            GetDetInfo(ParDetID->at(i), detInfo);

            if (detInfo[kSectorID] == 2) continue;

            int layerID = detInfo[kLayerID];
            if (detInfo[kSectorID] == 1) layerID += 15;

            if (layerID < 0 || layerID >= 30) {
                cerr << "Invalid layer ID: " << layerID << endl;
                continue;
            }

            SumPhotonByLayer[detInfo[kTypeID]][layerID] += 1;
        }
    }

    f->Close();
    delete f;
    return true;
}

void PlotGraph()
{
    TCanvas* c1 = new TCanvas("c1", "Photon Distribution", 1200, 600);
    c1->Divide(2, 1);

    double x[30];
    for (int i = 0; i < 30; i++) x[i] = i;

    PhotonByLayer[0] = new TGraph(30, x, SumPhotonByLayer[0]);
    PhotonByLayer[0]->SetTitle("Sensitive Layers; Layer Number; Photon Count");
    PhotonByLayer[0]->SetMarkerStyle(20);
    PhotonByLayer[0]->SetMarkerColor(kBlue);

    PhotonByLayer[1] = new TGraph(30, x, SumPhotonByLayer[1]);
    PhotonByLayer[1]->SetTitle("Absorber Layers; Layer Number; Photon Count");
    PhotonByLayer[1]->SetMarkerStyle(20);
    PhotonByLayer[1]->SetMarkerColor(kRed);

    c1->cd(1);
    PhotonByLayer[0]->Draw("AP");
    c1->cd(2);
    PhotonByLayer[1]->Draw("AP");
}
