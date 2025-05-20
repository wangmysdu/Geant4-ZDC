void readZDCTree(){

   TFile *fin = new TFile("ZDC.root");

   TTree *tTest = (TTree*)fin->Get("ZDC");

   int eventID;
   double Eabs, Labs;

   std::vector<int>*    detectorID          = new std::vector<int>();
   std::vector<int>*    particlePDG         = new std::vector<int>();
   std::vector<double>* particleEdep        = new std::vector<double>();
   std::vector<double>* particleTrackLength = new std::vector<double>();
   std::vector<double>* particleInPx        = new std::vector<double>();
   std::vector<double>* particleInPy        = new std::vector<double>();
   std::vector<double>* particleInPz        = new std::vector<double>();
   std::vector<double>* particleOutPx       = new std::vector<double>();
   std::vector<double>* particleOutPy       = new std::vector<double>();
   std::vector<double>* particleOutPz       = new std::vector<double>();
   std::vector<double>* particleInPosx      = new std::vector<double>();
   std::vector<double>* particleInPosy      = new std::vector<double>();
   std::vector<double>* particleInPosz      = new std::vector<double>();
   std::vector<double>* particleOutPosx     = new std::vector<double>();
   std::vector<double>* particleOutPosy     = new std::vector<double>();
   std::vector<double>* particleOutPosz     = new std::vector<double>();

   tTest->SetBranchAddress("EventID",&eventID);
   tTest->SetBranchAddress("EabsTot",&Eabs);
   tTest->SetBranchAddress("LabsTot",&Labs);

   tTest->SetBranchAddress("Particle_detID",      &detectorID);
   tTest->SetBranchAddress("Particle_PDG",        &particlePDG);
   tTest->SetBranchAddress("Particle_Edep",       &particleEdep);
   tTest->SetBranchAddress("Particle_TrackLength",&particleTrackLength);
   tTest->SetBranchAddress("Particle_InPx",       &particleInPx);
   tTest->SetBranchAddress("Particle_InPy",       &particleInPy);
   tTest->SetBranchAddress("Particle_InPz",       &particleInPz);
   tTest->SetBranchAddress("Particle_OutPx",      &particleOutPx);
   tTest->SetBranchAddress("Particle_OutPy",      &particleOutPy);
   tTest->SetBranchAddress("Particle_OutPz",      &particleOutPz);
   tTest->SetBranchAddress("Particle_Inx",     &particleInPosx);
   tTest->SetBranchAddress("Particle_Iny",     &particleInPosy);
   tTest->SetBranchAddress("Particle_Inz",     &particleInPosz);
   tTest->SetBranchAddress("Particle_Outx",    &particleOutPosx);
   tTest->SetBranchAddress("Particle_Outy",    &particleOutPosy);
   tTest->SetBranchAddress("Particle_Outz",    &particleOutPosz);

   int totalNumber=tTest->GetEntries();

/* Structure: front plate + (reflector + scintillator + reflector + lead) + back plate
 *
 * Detector ID
 *
 * (Front) 2nd detector, 1st detector (End with SiPM)
 * ID:     2000000        1000000
 * 
 * Module:   10000*(i+1) 
 *
 * Layer:  
 *     Sintillator: 1000+i*10
 *     Lead:        2000+i*10
 *     relector 1:  3000+20*i
 *     relector 2:  3000+10+20*i
 *
 *
 *     TiO2 coating: 1
 *     Front-end aluminum plate: 2
 *     Back-end aluminum plate:  3
 *
 * fiber structure: hole -> clad2 -> clad1 ->core  
 *     hole:  10+i (16 in total)
 *     clad2: 4
 *     clad1: 5
 *     core:  6
 *     mirror: 30+i
 *
 *     SimPM: 50+i
 *
 *     Rod hole: 70+i
 *     Rod: 7
 *
 *  Front   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14  SiPM
 *          ----------------------------------> z
 * 
 *
 *
 *                --- --- ---
 *   ^           | 6 | 7 | 8 |
 * y |            --- --- ---
 *   |           | 3 | 4 | 5 |
 *    ----> x     --- --- ---
 *               | 0 | 1 | 2 |
 *                --- --- ---
 *
*/   


   for (Long64_t i=0;i<totalNumber;i++) {
      if (i%10000 == 0) {
         std::cout << "--- ... Processing event: " << i << "/"<<totalNumber<<std::endl;
      }

      tTest->GetEntry(i);

      cout<<eventID<<" "<<Eabs<<" "<<Labs<<" "<<detectorID->size()<<endl;
      for(int j=0; j<detectorID->size(); j++){
	      if(detectorID->at(j) > 3000000) cout<<detectorID->at(j)<<" "<<particlePDG->at(j)<<" "<<particleEdep->at(j)<<" "<<particleTrackLength->at(j)<<endl;
      }
   };
}
