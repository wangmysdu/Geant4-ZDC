#include "AnaZDC.h"

//user defined global histogram pointers
TH1D* SecEdepSenHist[NSector];
TH1D* SecEdepAbsHist[NSector];
TH1D* ModEdepSenHist[NSector];
TH1D* ModEdepAbsHist[NSector];
TH1D* ParEdepSenHist[NSector];
TH1D* ParEdepAbsHist[NSector];

TH1D* hX_EdepLow;
TH1D* hX_EdepMid;
TH1D* hX_EdepHigh;

TH1D* hSen;
TH1D* hAbs;
TH1D* HSen;
TH1D* HAbs;

TH1D* hThetaLow;
TH1D* hThetaHigh;

TH1D* TotalEdepHistCorrSum = nullptr;

double SumEdepByLayer[2][30] = {0};

TH2D* Sec3VsSec12EdepHist;
TH2D* EdepXVSEdepYHist;

TGraph* EdepByLayer[2]; 

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

        //initial
        const double InEnergy = 10000;
        double CaliConst[NSector] = {25, 25, 1}; 
        double totalEDep[NSector] = {0., 0., 0.};
        double ReconE = 0.;

        
        //fill the sector level info
        for (int i = 0; i < NSector; i++){
            SecEdepSenHist[i]->Fill(SecEdepSecSen[i]);
            SecEdepAbsHist[i]->Fill(SecEdepSecAbs[i]);
        }
        //Sec3VsSec12EdepHist->Fill(SecEdepSecSen[0] + SecEdepSecSen[1], SecEdepSecSen[2]);
        
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

            if (id[kSectorID] == 2) continue;

            int LayerID = id[kLayerID];
            if (id[kSectorID] == 0) LayerID += 15;

            SumEdepByLayer[id[kTypeID]][LayerID] += ParEdep->at(i);

            
            //hist->Fill(ParInX->at(i), ParEdep->at(i));
            
            if (id[kTypeID] == 0){
                //this is a sensitive layer
                if (id[kSectorID] >= 0 && id[kSectorID] < NSector)
                SumParSen[id[kSectorID]]+=ParEdep->at(i);
                // if (id[kLayerID]>=0 && id[kLayerID]<NLayer)
                // SumParSen[id[kLayerID]]+=PartEdep ->at([kLayer]);
            }
            else if (id[kTypeID] == 1){
                //this is an absorber layer
                if (id[kSectorID] >= 0 && id[kSectorID] < NSector)
                SumParAbs[id[kSectorID]]+=ParEdep->at(i);
                // if (id[kLayerID]>=0 && id[kLayerID]<NLayer)
                // SumParAbs[id[kLayerID]]+=PartEdep ->at([kLayer]);
            }
            
        }
        for (int i=0; i<NSector; i++){
            ParEdepSenHist[i]->Fill(SumParSen[i]);
            ParEdepAbsHist[i]->Fill(SumParAbs[i]);
        }
    
        for (unsigned int i = 0; i < ParDetID->size(); i++) {
            vector<int> id;
            GetDetInfo(ParDetID->at(i), id);
            int sector = id[kSectorID];
            if (sector != 0 && sector != 1) continue;
            double x = ParInX->at(i);
            double y = ParInY->at(i);

            EdepXVSEdepYHist->Fill(x, y, ParEdep->at(i));

            if (id[kTypeID] == 0) {
                hSen->Fill(x);
                HSen->Fill(ParInX->at(i), ParEdep->at(i));
            }
            else if (id[kTypeID] == 1) {
                hAbs->Fill(x);
                HAbs->Fill(ParInX->at(i), ParEdep->at(i));
            }
            
            
            
            double edep = ParEdep->at(i);
            if (id[kTypeID] == 0) { 
                if (edep < 10) hX_EdepLow->Fill(x);
                else if (edep < 100) hX_EdepMid->Fill(x);
                else hX_EdepHigh->Fill(x);
            }  
            // const double InEnergy  = 10000;
            // double CaliConst[NSector] = {0};
            // double totalEDep[NSector] = {0., 0., 0.};
            // TH1D* TotalEdepHistCorrSum;
            // double ReconE = 0.;
            // //int sector = id[kSectorID];
            // for(int i=0; i<sector; i++)
            // if (sector != 0 && sector != 1) continue;
            // totalEDep[i] += ParEdep->at(i);
            // ReconE +=totalEDep[i] * CaliConst[i];
            
            // TotalEdepHistCorrSum = new TH1D("Recon_E_over_E", "Recon_E_over_E", 200, 0, 3);
            // TotalEdepHistCorrSum->Fill(ReconE / InEnergy);
            // TotalEdepHistCorrSum->GetXaxis()->SetTitle("E_{recon}/E");
            // TotalEdepHistCorrSum->GetYaxis()->SetTitle("count");
            if (sector < 0 || sector >= NSector) continue; 
            if (sector == 2) continue;
            if (id[kTypeID] == 0) totalEDep[sector] += ParEdep->at(i);    
            
            
            // if( !( (x >=120.0 && x<=122.0) || (x <=-120.0 && x>=-122.0) ) ) continue;
            // double px = ParInPx->at(i);
            // double py = ParInPy->at(i);
            // double pz = ParInPz->at(i);
    
            // double theta = atan2(sqrt(px*px + py*py), abs(pz)) * 1000.0; // mrad
            // //double edep = ParEdep->at(i);
            // if(edep < 10.0){hThetaLow->Fill(theta);
            // } 
            // else if(edep >= 100.0){
            //     hThetaHigh->Fill(theta);
            // }


        } 
        for (int i = 0; i < NSector; i++) {ReconE += totalEDep[i] * CaliConst[i];
        }
        if (TotalEdepHistCorrSum) {TotalEdepHistCorrSum->Fill(ReconE / InEnergy);
        }
        
    }

    for (int i=0; i<2; i++){
        for (int j=0; j<30; j++){
            SumEdepByLayer[i][j] /= t->GetEntries();
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
    
    //TCanvas* c3 = new TCanvas("c3", "c3", 800, 600);
    //Sec3VsSec12EdepHist->Draw();

    TCanvas* c4 = new TCanvas("c4", "c4", 1200, 600);
    c4->Divide(2, 1);
    double x[30];
    for (int i=0; i<30; i++) x[i] = i;

    EdepByLayer[0] = new TGraph(30, x, SumEdepByLayer[0]);
    EdepByLayer[0]->SetMarkerStyle(20); 
    EdepByLayer[0]->SetTitle("Sensitive Layers;Layer;Edep (MeV)");

    EdepByLayer[1] = new TGraph(30, x, SumEdepByLayer[1]);
    EdepByLayer[1]->SetMarkerStyle(20);
    EdepByLayer[1]->SetTitle("Absorber Layers;Layer;Edep (MeV)");
    
    c4->cd(1);
    EdepByLayer[0]->Draw("AP"); 
    c4->cd(2);
    EdepByLayer[1]->Draw("AP");

    TCanvas* c5 = new TCanvas("c5", "c5", 1200, 600);
    c5->Divide(2, 1);
    
    c5->cd(1);
    hSen->Draw("HIST");
    
    c5->cd(2);
    hAbs->Draw("HIST");
    

    TCanvas* c6 = new TCanvas("c6", "c6", 1200, 600);
    c6->Divide(2, 1);
    
    c6->cd(1);
    HSen->Draw("HIST");

    c6->cd(2);
    HAbs->Draw("HIST");

    TCanvas* c7 = new TCanvas("c7", "c7", 1200, 400);
    c7->Divide(3, 1);
    c7->cd(1); hX_EdepLow->Draw("HIST");
    c7->cd(2); hX_EdepMid->Draw("HIST");
    c7->cd(3); hX_EdepHigh->Draw("HIST");

    TCanvas* c8 = new TCanvas("c8", "c8", 1200, 600);
    TotalEdepHistCorrSum->Draw("");
    //TotalEdepHistCorrSum->Fit("gaus");
    TotalEdepHistCorrSum->Fit("gaus", "S"); 
    TF1* fitFunc = TotalEdepHistCorrSum->GetFunction("gaus");

    if (fitFunc) {
    double sigma = fitFunc->GetParameter(2); 
    double sigma_error = fitFunc->GetParError(2); 

    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.035);
    latex->DrawLatex(0.7, 0.65, Form("#sigma = %.6f #pm %.8f", sigma, sigma_error));

    cout << "Sigma = " << sigma << " ± " << sigma_error << endl;
    } 
    else {
    cerr << "Error: Gaussian fit failed!" << endl;
    }

    TCanvas* c9 =new TCanvas("c9", "XY Energy Deposit", 800, 600);
    c9->cd();
    EdepXVSEdepYHist->Draw("COLZ");
    EdepXVSEdepYHist->GetZaxis()->SetTitle("Edep [MeV]");

    // TCanvas* cTheta = new TCanvas("cTheta","Theta by Momentum",1200,600);
    // cTheta->Divide(2,1);

    // cTheta->cd(1);
    // hThetaLow->SetLineColor(kBlue);
    // hThetaLow->Draw("HIST");
    // gPad->SetLogy();  
    // //gStyle->SetOptStat(1101); 

    // cTheta->cd(2);
    // hThetaHigh->SetLineColor(kRed); 
    // hThetaHigh->Draw("HIST");
    // gPad->SetLogy();

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

        hX_EdepLow = new TH1D(Form("EdepLow%d", i), "energy (Edep < 10 MeV);X [mm];Count", 300, -300, 300);
        hX_EdepMid = new TH1D(Form("EdepMid%d", i), "energy (10 < Edep < 100 MeV);X [mm];Count", 300, -300, 300);
        hX_EdepHigh = new TH1D(Form("EdepHigh%d", i), "energy (Edep > 100 MeV);X [mm];Count", 300, -300, 300);                               
    
    }
    //Sec3VsSec12EdepHist = Init2DHist("Edep_sec3_vs_sec12", 100, 0, 500, 100, 0, 500, "Edep [MeV]", "Edep [MeV]");

    hSen = new TH1D("hSen", "Sen_Layer_count; X [mm];Particle Count", 200, -400.0, 400.0);
    hAbs = new TH1D("hAbs", "Abs_Layer_count; X [mm];Particle Count", 200, -400.0, 400.0);

    HSen = new TH1D("HSen", "Sen_Layer_Edep; X [mm];Edep", 200, -400.0, 400.0);
    HAbs = new TH1D("HAbs", "Abs_Layer_Edep; X [mm];Edep", 200, -400.0, 400.0);

    TotalEdepHistCorrSum = new TH1D("Recon_E_over_E", "Recon_E_over_E;E_{recon}/E;count", 100, 0, 2);
    
    EdepXVSEdepYHist = Init2DHist("Edep_X_vs_Y", 300, -300, 300, 300, -300, 300,"X [mm]", "Y [mm]");

    // hThetaLow = new TH1D("hThetaLow", "Low Energy (Edep<10MeV) @X:120-122mm;#theta [mrad];Count", 200, 0, 20); 
    
    // hThetaHigh = new TH1D("hThetaHigh", "High Energy (Edep>=100MeV) @X:120-122mm;#theta [mrad];Count", 200, 0, 20);
}
