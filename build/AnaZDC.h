#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

//need to adjust the following constants based on the simulation
const bool UsePbWO4EMCal = false;
const int NSector = 3;
const int NModule[NSector] = {16, 16, 25};
const int NLayer[NSector]  = {15, 15, 42};
const string SectorName[NSector] = {"PbSci1", "PbSci2", "WSci"};

enum DetectorIndex{
    kSectorID = 0,
    kModuleID,
    kTypeID,
    kLayerID
};

//Declaration of leaf types
//sector level variables
Int_t           EventID;
Double_t        SecEdepEMC;
Double_t        SecEdepSecSen[NSector];
Double_t        SecEdepSecAbs[NSector];
Int_t           SecNPEEmc;
Int_t           SecNPESec[NSector];
//module level variables
vector<double>  *ModEdepEmc     = 0;
vector<double>  *ModEdepSecAbs[NSector];
vector<double>  *ModEdepSecSen[NSector];
vector<int>     *ModNPEEmc      = 0;
vector<int>     *ModNPESec[NSector];
//sipm variables
vector<int>     *SipmID         = 0;
vector<double>  *SipmTime       = 0;
//particle level variables
vector<int>     *ParDetID       = 0;
vector<int>     *ParPDG         = 0;
vector<double>  *ParEdep        = 0;
vector<double>  *ParTrackLength = 0;
vector<double>  *ParInPx        = 0;
vector<double>  *ParInPy        = 0;
vector<double>  *ParInPz        = 0;
vector<double>  *ParOutPx       = 0;
vector<double>  *ParOutPy       = 0;
vector<double>  *ParOutPz       = 0;
vector<double>  *ParInX         = 0;
vector<double>  *ParInY         = 0;
vector<double>  *ParInZ         = 0;
vector<double>  *ParOutx        = 0;
vector<double>  *ParOuty        = 0;
vector<double>  *ParOutz        = 0;
vector<double>  *ParTime        = 0;

vector<TH1D*>   HistContainer1D;
vector<TH2D*>   HistContainer2D;

void GetDetInfo(const int & id, vector<int> & v)
{
    //from detector id, get sector ID, module ID, layer ID, typeID
    //current program store detector information into 7 digit integer
    
    //sec: sector number, starting from 1
    //mod: module number, starting from 1
    //typ: type number, 1 for sensitive, 2 for absorber, 3 for reflector
    //lay: layer number,  starting from 0
    //otr: used for store other id for other parts, 
    
    //  _   _   _   _   _   _   _
    // sec mod mod typ lay lay otr
    
    //NOTE: type ID should never be 0, based on the output information right now
    
    v.clear();
    
    int secID  = id / 1000000;
    int tmp    = id - secID*1000000;
    
    int modID  = (tmp) / 10000;
    tmp = tmp  - modID * 10000;
    
    int typeID = tmp   / 1000;
    tmp = tmp  - typeID * 1000;
    
    //should never happen
    assert(typeID == 0);
    
    int layerID = tmp / 10;
    tmp = tmp - layerID*10;
    
    //tmp should now always be 0 based on the output information right now
    assert(tmp != 0);
    
    //setting sector, module and layer ID all starting from 0
    v.push_back(secID   - 1);
    v.push_back(modID   - 1);
    v.push_back(typeID  - 1);
    v.push_back(layerID );
}

void InitTree(TTree* t)
{
    //init vector first
    for (int i=0; i<NSector; i++){
        ModEdepSecAbs[i] = 0;
        ModEdepSecSen[i] = 0;
        ModNPESec[i]     = 0;
    }
    
    //assign variables to branches
    t->SetBranchAddress("EventID", &EventID);
    //let EMC branches
    if (UsePbWO4EMCal){
        t->SetBranchAddress("Sec.EdepEMC",  &SecEdepEMC );
        t->SetBranchAddress("Sec.NPEEmc",   &SecNPEEmc  );
        t->SetBranchAddress("Mod.EdepEmc",  &ModEdepEmc );
        t->SetBranchAddress("Mod.NPEEmc",   &ModNPEEmc  );
    }
    //let hadronic calorimeter branches
    for (int i = 0; i < NSector; i++){
        t->SetBranchAddress(Form("Sec.EdepSecSen%d", i+1), &SecEdepSecSen[i]);
        t->SetBranchAddress(Form("Sec.EdepSecAbs%d", i+1), &SecEdepSecAbs[i]);
        t->SetBranchAddress(Form("Sec.NPESec%d",     i+1), &SecNPESec[i]    );
        t->SetBranchAddress(Form("Mod.EdepSecSen%d", i+1), &ModEdepSecSen[i]);
        t->SetBranchAddress(Form("Mod.EdepSecAbs%d", i+1), &ModEdepSecAbs[i]);
        t->SetBranchAddress(Form("Mod.NPESec%d",     i+1), &ModNPESec[i]    );
    }
    
    //Sipm level branches
    t->SetBranchAddress("Sipm.ID",   &SipmID  );
    t->SetBranchAddress("Sipm.Time", &SipmTime);
    
    //particle level branches
    t->SetBranchAddress("Par.DetID",       &ParDetID       );
    t->SetBranchAddress("Par.PDG",         &ParPDG         );
    t->SetBranchAddress("Par.Edep",        &ParEdep        );
    t->SetBranchAddress("Par.TrackLength", &ParTrackLength );
    t->SetBranchAddress("Par.InPx",        &ParInPx        );
    t->SetBranchAddress("Par.InPy",        &ParInPy        );
    t->SetBranchAddress("Par.InPz",        &ParInPz        );
    t->SetBranchAddress("Par.OutPx",       &ParOutPx       );
    t->SetBranchAddress("Par.OutPy",       &ParOutPy       );
    t->SetBranchAddress("Par.OutPz",       &ParOutPz       );
    t->SetBranchAddress("Par.InX",         &ParInX         );
    t->SetBranchAddress("Par.InY",         &ParInY         );
    t->SetBranchAddress("Par.InZ",         &ParInZ         );
    t->SetBranchAddress("Par.Outx",        &ParOutx        );
    t->SetBranchAddress("Par.Outy",        &ParOuty        );
    t->SetBranchAddress("Par.Outz",        &ParOutz        );
    t->SetBranchAddress("Par.Time",        &ParTime        );
}

TH1D* Init1DHist(const string & name,  const int & nbin, const double & min, const double & max, 
                 const string & xaxis, const string & yaxis, const int & color)
{
    TH1D* h = new TH1D(name.c_str(), name.c_str(), nbin, min, max);
    h->GetXaxis()->SetTitle(xaxis.c_str());
    h->GetYaxis()->SetTitle(yaxis.c_str());
    h->SetLineWidth(2);
    h->SetLineColor(color);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->CenterTitle();
    h->GetXaxis()->SetTitleSize(0.045);
    h->GetYaxis()->SetTitleSize(0.045);
    h->GetXaxis()->SetLabelSize(0.04);
    h->GetYaxis()->SetLabelSize(0.04);
    HistContainer1D.push_back(h);
    return h;
}

TH2D* Init2DHist(const string & name,
                 const int & nbinx,    const double & minx, const double & maxx,
                 const int & nbiny,    const double & miny, const double & maxy,
                 const string & xaxis, const string & yaxis)
{
    TH2D* h = new TH2D(name.c_str(), name.c_str(), nbinx, minx, maxx, nbiny, miny, maxy);
    h->GetXaxis()->SetTitle(xaxis.c_str());
    h->GetYaxis()->SetTitle(yaxis.c_str());
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->CenterTitle();
    h->GetXaxis()->SetTitleSize(0.045);
    h->GetYaxis()->SetTitleSize(0.045);
    h->GetXaxis()->SetLabelSize(0.04);
    h->GetYaxis()->SetLabelSize(0.04);
    h->SetOption("colz");
    HistContainer2D.push_back(h);
    return h; 
}

void WritePlotToFile(const string fileName)
{
    TFile* f = new TFile(fileName.c_str(), "RECREATE");
    f->cd();
    
    for (unsigned int i = 0; i < HistContainer1D.size(); i++)
    HistContainer1D[i]->Write();
    
    for (unsigned int i = 0; i < HistContainer2D.size(); i++)
    HistContainer2D[i]->Write();
    
    f->Close();
}

