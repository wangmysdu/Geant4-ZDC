#include "ZDCMaterials.hh"

#include "G4SystemOfUnits.hh"

namespace ZDC
{

ZDCMaterials *ZDCMaterials::fInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ZDCMaterials::ZDCMaterials()
{
	fNistMan = G4NistManager::Instance();

	fNistMan->SetVerbose(2);

	CreateMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ZDCMaterials::~ZDCMaterials()
{
	delete fPMMA;
	delete fPethylene;
	delete fFPethylene;
	delete fPolystyrene;
	delete fSilicone;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ZDCMaterials *ZDCMaterials::GetInstance()
{
	if (fInstance == 0)
	{
		fInstance = new ZDCMaterials();
	}
	return fInstance;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material *ZDCMaterials::GetMaterial(const G4String material)
{
	G4Material *mat = fNistMan->FindOrBuildMaterial(material);

	if (!mat)
		mat = G4Material::GetMaterial(material);
	if (!mat)
	{
		std::ostringstream o;
		o << "Material " << material << " not found!";
		G4Exception("ZDCMaterials::GetMaterial", "",
					FatalException, o.str().c_str());
	}

	return mat;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ZDCMaterials::CreateMaterials()
{
	/*** Material list  ***/
	//**** defined basic material
	// PMMA: 		C5H8O2, fiber core
	// Cladding: 	C2H4, first cladding, polyethylene
	// D-cladding: 	C4H8, fluorinated polyethylene
	// Polystyrene:	C8H8, scintillator
	// Silicone:	C2H6, scin-PMT
	// Scintillator Coating
	// Tyvek: 		C2H4ox
	// Photocathode:K2CsSb
	// other: G4_Air, G4_TITANIUM_DIOXIDE, G4_SILICON_DIOXIDE, G4_Pb, G4_Al, G4_STAINLESS-STEEL, G4_CESIUM_IODIDE


	G4double density;
	G4int ncomponents;
	G4double fractionmass;
	std::vector<G4int> natoms;
	std::vector<G4double> fractionMass;
	std::vector<G4String> elements;

	//--------------------------------------------------
	// WLSfiber PMMA
	//--------------------------------------------------
	elements.push_back("C"); natoms.push_back(5);
	elements.push_back("H"); natoms.push_back(8);
	elements.push_back("O"); natoms.push_back(2);

	density = 1.190 * g / cm3;
	fPMMA = fNistMan->ConstructNewMaterial("PMMA", elements, natoms, density); //有警告提示

	elements.clear();natoms.clear();

	//--------------------------------------------------
	// Cladding (polyethylene)
	//--------------------------------------------------
	elements.push_back("C"); natoms.push_back(2);
	elements.push_back("H");natoms.push_back(4);

	density = 1.200 * g / cm3;
	fPethylene = fNistMan->ConstructNewMaterial("Pethylene", elements, natoms, density);

	elements.clear(); natoms.clear();

	//--------------------------------------------------
	// Double Cladding (fluorinated polyethylene), changed from C2H4 to C4H8
	//--------------------------------------------------
	elements.push_back("C"); natoms.push_back(4);
	elements.push_back("H"); natoms.push_back(8);

	density = 1.400 * g / cm3;
	fFPethylene = fNistMan->ConstructNewMaterial("FPethylene", elements, natoms, density);

	elements.clear(); natoms.clear();

	//--------------------------------------------------
	// Polystyrene, or G4_PLASTIC_SC_VINYLTOLUENE
	//--------------------------------------------------
	elements.push_back("C"); natoms.push_back(8);
	elements.push_back("H"); natoms.push_back(8);

	density = 1.050 * g / cm3;
	fPolystyrene = fNistMan->ConstructNewMaterial("Polystyrene", elements, natoms, density);

	elements.clear(); natoms.clear();
	
	//--------------------------------------------------
	// Silicone (Template for Optical Grease)
	//--------------------------------------------------
	elements.push_back("C"); natoms.push_back(2);
	elements.push_back("H"); natoms.push_back(6);

	density = 1.060 * g / cm3;
	fSilicone = fNistMan->ConstructNewMaterial("Silicone", elements, natoms, density);

	elements.clear(); natoms.clear();

	//--------------------------------------------------
	// Scintillator Coating - 15% TiO2 and 85% polystyrene by weight.  ????????????? why contain polystyrene
	//--------------------------------------------------
	density = 1.52 * g / cm3;
	fCoating = new G4Material("Coating", density, ncomponents = 2);

	fTiO2 = fNistMan->FindOrBuildMaterial("G4_TITANIUM_DIOXIDE");
	fCoating->AddMaterial(fTiO2, fractionmass = 15 * perCent);
	fCoating->AddMaterial(fPolystyrene, fractionmass = 85 * perCent);

	//--------------------------------------------------
	// Tyvek      from EicC(pandaRoot) material.geo
	//--------------------------------------------------
	elements.push_back("C");     natoms.push_back(2);
    elements.push_back("H");     natoms.push_back(4);
	density = 0.94*g/cm3;
    fTyvek = fNistMan->ConstructNewMaterial("Tyvek", elements, natoms, density);

	elements.clear(); natoms.clear();

	//--------------------------------------------------
	// Photocathod material: PMT window
	//--------------------------------------------------
	elements.push_back("K");     natoms.push_back(2);
    elements.push_back("Cs");    natoms.push_back(1);
	elements.push_back("Sb");    natoms.push_back(1);

	density = 2.00*g/cm3;
    fPhotocathode = fNistMan->ConstructNewMaterial("Photocathode", elements, natoms, density);

	elements.clear(); natoms.clear();

	//--------------------------------------------------
	// carbon fiber, and epoxy
	//--------------------------------------------------
	elements.push_back("H");     natoms.push_back(32);
    elements.push_back("N");    natoms.push_back(2);
	elements.push_back("O");    natoms.push_back(4);
	elements.push_back("C");    natoms.push_back(15);

	density = 1.16*g/cm3;
    fEpoxy = fNistMan->ConstructNewMaterial("Epoxy", elements, natoms, density);

	elements.clear(); natoms.clear();

	fCarbonFiber = new G4Material("CarbonFiber",  density =  1.750*g/cm3, 2);
	fCarbonFiber->AddMaterial(fNistMan->FindOrBuildMaterial("G4_C"), 90*perCent);  // Carbon
	fCarbonFiber->AddMaterial(fEpoxy,                    10*perCent);  // Epoxy (scotch)

	//--------------------------------------------------
	// Lead Glass: from JLab Hall C, HMS TF-1(SHMS, F-101, Fb2O4 not exist???)
	//--------------------------------------------------
	G4Material *fPbO;
	elements.push_back("Pb");     natoms.push_back(1);
    elements.push_back("O");    natoms.push_back(1);
	density = 9.53*g/cm3;
    fPbO = fNistMan->ConstructNewMaterial("PbO", elements, natoms, density);
	elements.clear(); natoms.clear();

	G4Material *fSiO2 = fNistMan->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
	
	G4Material *fK2O = fNistMan->FindOrBuildMaterial("G4_POTASSIUM_OXIDE");

	G4Material *fNa2O = fNistMan->FindOrBuildMaterial("G4_SODIUM_MONOXIDE");;

	density = 3.86 * g / cm3;
	fLeadGlass = new G4Material("LeadGlass", density, ncomponents = 4);
	fLeadGlass->AddMaterial(fPbO, fractionmass = 51.45 * perCent); // mass ratio, +0.25 four values are sumed to normalzie to 1
	fLeadGlass->AddMaterial(fSiO2, fractionmass = 41.55 * perCent); //+0.25
	fLeadGlass->AddMaterial(fK2O, fractionmass = 3.5 * perCent);
	fLeadGlass->AddMaterial(fNa2O, fractionmass = 3.5 * perCent);

	//////////////////////////////////////////////////////////////////
	// optical property
	// ------------ Generate & Add Material Properties Table ------------
	//
	//*** Materrial list:
	// Air
	// PMMA
	// Polyethylene
	// Fluorinated Polyethylene
	// Plastic scintillator, Polystyrene
	// Photocathode
	// Tyvek
	// Light guide(Nylon)
	// silicone
	// G4_CESIUM_IODIDE (CsI)

	G4double photonEnergy_simple[] =
		{2.00 * eV, 2.26 * eV, 2.49 * eV, 2.76 * eV, 3.11 * eV, 3.47 * eV}; // 能量*波长=1243
	const G4int nEntries_simple = 6;

	G4double photonEnergy[] =
		{2.00 * eV, 2.03 * eV, 2.06 * eV, 2.09 * eV, 2.12 * eV,
		 2.15 * eV, 2.18 * eV, 2.21 * eV, 2.24 * eV, 2.27 * eV,
		 2.30 * eV, 2.33 * eV, 2.36 * eV, 2.39 * eV, 2.42 * eV,
		 2.45 * eV, 2.48 * eV, 2.51 * eV, 2.54 * eV, 2.57 * eV,
		 2.60 * eV, 2.63 * eV, 2.66 * eV, 2.69 * eV, 2.72 * eV,
		 2.75 * eV, 2.78 * eV, 2.81 * eV, 2.84 * eV, 2.87 * eV,
		 2.90 * eV, 2.93 * eV, 2.96 * eV, 2.99 * eV, 3.02 * eV,
		 3.05 * eV, 3.08 * eV, 3.11 * eV, 3.14 * eV, 3.17 * eV,
		 3.20 * eV, 3.23 * eV, 3.26 * eV, 3.29 * eV, 3.32 * eV,
		 3.35 * eV, 3.38 * eV, 3.41 * eV, 3.44 * eV, 3.47 * eV};

	const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double); //50

	//--------------------------------------------------
	// Air
	//--------------------------------------------------
	G4double refractiveIndex[] =
		{1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
		 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
		 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
		 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
		 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00};

	assert(sizeof(refractiveIndex) == sizeof(photonEnergy));

	G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable();
	mpt->AddProperty("RINDEX", photonEnergy, refractiveIndex, nEntries);
	fAir = GetMaterial("G4_AIR");
	fAir->SetMaterialPropertiesTable(mpt);

	//--------------------------------------------------
	//  PMMA for WLSfibers
	//--------------------------------------------------
	G4double refractiveIndexWLSfiber[] =
		{1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
		 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
		 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
		 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
		 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60};

	G4double Attenuation_length[] =
		{3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m,
		 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m,
		 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m,
		 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m,
		 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m, 3.28 * m};

	assert(sizeof(refractiveIndexWLSfiber) == sizeof(photonEnergy));

	// acquired form someone's ppt
	const G4int num_energy2 = 42;
	G4double photon_energy_WLSABSLENGTH_v1[num_energy2] =
		{3.539 * eV, 3.477 * eV, 3.340 * eV, 3.321 * eV, 3.291 * eV,
		 3.214 * eV, 3.162 * eV, 3.129 * eV, 3.091 * eV, 3.086 * eV,
		 3.049 * eV, 3.008 * eV, 2.982 * eV, 2.958 * eV, 2.928 * eV,
		 2.905 * eV, 2.895 * eV, 2.890 * eV, 2.858 * eV, 2.813 * eV,
		 2.774 * eV, 2.765 * eV, 2.752 * eV, 2.748 * eV, 2.739 * eV,
		 2.735 * eV, 2.731 * eV, 2.723 * eV, 2.719 * eV, 2.698 * eV,
		 2.674 * eV, 2.626 * eV, 2.610 * eV, 2.583 * eV, 2.556 * eV,
		 2.530 * eV, 2.505 * eV, 2.480 * eV, 2.455 * eV, 2.431 * eV,
		 2.407 * eV, 2.384 * eV};

	G4double WLS_ABSLENGTHBCF91A_v1[num_energy2] =
		{0.28 * cm, 0.28 * cm, 0.26 * cm, 0.25 * cm, 0.24 * cm,
		 0.21 * cm, 0.19 * cm, 0.16 * cm, 0.13 * cm, 0.13 * cm,
		 0.14 * cm, 0.11 * cm, 0.08 * cm, 0.05 * cm, 0.02 * cm,
		 0.05 * cm, 0.08 * cm, 0.10 * cm, 0.13 * cm, 0.10 * cm,
		 0.08 * cm, 0.07 * cm, 0.08 * cm, 0.11 * cm, 0.13 * cm,
		 0.16 * cm, 0.19 * cm, 0.21 * cm, 0.24 * cm, 0.27 * cm,
		 0.30 * cm, 2.69 * cm, 3.49 * cm, 3.99 * cm, 5.00 * cm,
		 11.6 * cm, 21.6 * cm, 33.1 * cm, 175. * cm, 393. * cm,
		 617. * cm, 794. * cm};

	G4double photon_energy_WLSABSLENGTH[num_energy2];
	G4double WLS_ABSLENGTHBCF91A[num_energy2];

	for(int iii = 0; iii<42; iii++){
		photon_energy_WLSABSLENGTH[iii] = photon_energy_WLSABSLENGTH_v1[41-iii];
		WLS_ABSLENGTHBCF91A[iii] = WLS_ABSLENGTHBCF91A_v1[41-iii];
	}

	const G4int num_energy3 = 50;
	G4double photon_energy_WLSEMISSION[num_energy3] =
		{2.00 * eV, 2.03 * eV, 2.06 * eV, 2.09 * eV, 2.12 * eV,
		 2.15 * eV, 2.18 * eV, 2.21 * eV, 2.24 * eV, 2.27 * eV,
		 2.30 * eV, 2.33 * eV, 2.36 * eV, 2.39 * eV, 2.42 * eV,
		 2.45 * eV, 2.48 * eV, 2.51 * eV, 2.54 * eV, 2.57 * eV,
		 2.60 * eV, 2.63 * eV, 2.66 * eV, 2.69 * eV, 2.72 * eV,
		 2.75 * eV, 2.78 * eV, 2.81 * eV, 2.84 * eV, 2.87 * eV,
		 2.90 * eV, 2.93 * eV, 2.96 * eV, 2.99 * eV, 3.02 * eV,
		 3.05 * eV, 3.08 * eV, 3.11 * eV, 3.14 * eV, 3.17 * eV,
		 3.20 * eV, 3.23 * eV, 3.26 * eV, 3.29 * eV, 3.32 * eV,
		 3.35 * eV, 3.38 * eV, 3.41 * eV, 3.44 * eV, 3.47 * eV};
	G4double WLSEMISSION_BCF91A[num_energy3] =
		{0.000, 0.01, 0.02, 0.03, 0.065, 0.10, 0.135, 0.17, 0.205, 0.25,
		 0.295, 0.39, 0.53, 0.65, 0.740, 0.71, 0.690, 0.79, 0.910, 1.00,
		 0.810, 0.52, 0.24, 0.17, 0.096, 0.05, 0.034, 0.03, 0.020, 0.00,
		 0.000, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
		 0.000, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};

	// Add entries into properties table
	G4MaterialPropertiesTable *mptWLSfiber = new G4MaterialPropertiesTable();
	mptWLSfiber->AddProperty("RINDEX", photonEnergy, refractiveIndexWLSfiber, nEntries);
	mptWLSfiber->AddProperty("ABSLENGTH", photonEnergy, Attenuation_length, nEntries);						//attenuation length
	mptWLSfiber->AddProperty("WLSABSLENGTH", photon_energy_WLSABSLENGTH, WLS_ABSLENGTHBCF91A, num_energy2); // absorb length?
	mptWLSfiber->AddProperty("WLSCOMPONENT", photon_energy_WLSEMISSION, WLSEMISSION_BCF91A, num_energy3);	// emission ratio?
	mptWLSfiber->AddConstProperty("WLSTIMECONSTANT", 0.5 * ns);												//not check

	fPMMA->SetMaterialPropertiesTable(mptWLSfiber);

	//--------------------------------------------------
	//  Polyethylene
	//--------------------------------------------------
	G4double refractiveIndexClad1[] =
		{1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
		 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
		 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
		 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
		 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49};

	assert(sizeof(refractiveIndexClad1) == sizeof(photonEnergy));

	G4double absClad[] =
		{20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m,
		 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m,
		 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m,
		 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m,
		 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m, 20.0 * m};

	assert(sizeof(absClad) == sizeof(photonEnergy));

	// Add entries into properties table
	G4MaterialPropertiesTable *mptClad1 = new G4MaterialPropertiesTable();
	mptClad1->AddProperty("RINDEX", photonEnergy, refractiveIndexClad1, nEntries);
	mptClad1->AddProperty("ABSLENGTH", photonEnergy, absClad, nEntries); // seems too long

	fPethylene->SetMaterialPropertiesTable(mptClad1);

	//--------------------------------------------------
	// Fluorinated Polyethylene
	//--------------------------------------------------
	G4double refractiveIndexClad2[] =
		{1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
		 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
		 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
		 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
		 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42};

	assert(sizeof(refractiveIndexClad2) == sizeof(photonEnergy));

	// Add entries into properties table
	G4MaterialPropertiesTable *mptClad2 = new G4MaterialPropertiesTable();
	mptClad2->AddProperty("RINDEX", photonEnergy, refractiveIndexClad2, nEntries);
	mptClad2->AddProperty("ABSLENGTH", photonEnergy, absClad, nEntries);

	fFPethylene->SetMaterialPropertiesTable(mptClad2);

	//--------------------------------------------------
	// Plastic scintillator, Polystyrenes
	// -------------------------------------------------
	const G4int num_energy4 = 12;
	G4double photonEnergy_scin_v1[num_energy4] =
		{3.44 * eV, 3.26 * eV, 3.1 * eV, 3.02 * eV, 2.95 * eV,
		 2.92 * eV, 2.82 * eV, 2.76 * eV, 2.7 * eV, 2.58 * eV,
		 2.38 * eV, 2.08 * eV};
	G4double refractiveIndex_scin_v1[num_energy4] = {1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58};
	G4double AbsorptionLength_scin_v1[num_energy4] = {380 * cm, 380 * cm, 380 * cm, 380 * cm, 380 * cm, 380 * cm, 380 * cm, 380 * cm, 380 * cm, 380 * cm, 380 * cm, 380 * cm};
	G4double scintilFast_v1[num_energy4] =
		{0.04, 0.07, 0.20, 0.49, 0.84,
		 1.00, 0.83, 0.55, 0.40, 0.17,
		 0.03, 0};

	G4double photonEnergy_scin[num_energy4];
        G4double refractiveIndex_scin[num_energy4];
        G4double AbsorptionLength_scin[num_energy4];
        G4double scintilFast[num_energy4];

	for(int iii=0; iii<num_energy4; iii++){
		photonEnergy_scin[iii] = photonEnergy_scin_v1[num_energy4-1-iii];
		refractiveIndex_scin[iii] = refractiveIndex_scin_v1[num_energy4-1-iii];
		AbsorptionLength_scin[iii] = AbsorptionLength_scin_v1[num_energy4-1-iii];
		scintilFast[iii] = scintilFast_v1[num_energy4-1-iii];
	}

	G4MaterialPropertiesTable *myMPT_scin = new G4MaterialPropertiesTable();
	myMPT_scin->AddProperty("RINDEX", photonEnergy_scin, refractiveIndex_scin, num_energy4);	 //refractive index
	myMPT_scin->AddProperty("ABSLENGTH", photonEnergy_scin, AbsorptionLength_scin, num_energy4); //absorption length / decay length

	myMPT_scin->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy_scin, scintilFast, num_energy4);		 // fast component ratio

	myMPT_scin->AddConstProperty("SCINTILLATIONYIELD", 10. / MeV); // 8000/MeV
	myMPT_scin->AddConstProperty("RESOLUTIONSCALE", 1.0);
	myMPT_scin->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2. * ns); // fast and slow ???
	//myMPT_scin->AddConstProperty("SLOWTIMECONSTANT",10.*ns);
	myMPT_scin->AddConstProperty("SCINTILLATIONYIELD1", 1);
	myMPT_scin->DumpTable();

	fPolystyrene->SetMaterialPropertiesTable(myMPT_scin);
	fPolystyrene->GetIonisation()->SetBirksConstant(0.126*mm/MeV);


	//--------------------------------------------------
	// Photocathode(glass of PMT)
	//--------------------------------------------------
	G4double refractiveIndexSiO2[] =
	{ 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
		1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
		1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
		1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
		1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46};
	G4MaterialPropertiesTable *mpt_SiO2 = new G4MaterialPropertiesTable();
	mpt_SiO2->AddProperty("RINDEX", photonEnergy, refractiveIndexSiO2, nEntries);
	fPhotocathode->SetMaterialPropertiesTable(mpt_SiO2);

	//--------------------------------------------------
	// Nylon: temp use as Light Guide
	//--------------------------------------------------	
	fNylon = fNistMan->FindOrBuildMaterial("G4_NYLON-8062");
	G4MaterialPropertiesTable *mpt_Nylon = new G4MaterialPropertiesTable();
	mpt_Nylon->AddProperty("RINDEX", photonEnergy_scin, refractiveIndex_scin, num_energy4);
	mpt_Nylon->AddProperty("ABSLENGTH", photonEnergy_scin, AbsorptionLength_scin, num_energy4);
	fNylon->SetMaterialPropertiesTable(mpt_Nylon);

	//--------------------------------------------------
	// Silicone
	//--------------------------------------------------
	G4double refractiveIndexSilicone[] =
	{ 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
		1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
		1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
		1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
		1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46};

	assert(sizeof(refractiveIndexSilicone) == sizeof(photonEnergy));

	// Add entries into properties table
	G4MaterialPropertiesTable* mptSilicone = new G4MaterialPropertiesTable();
	mptSilicone->AddProperty("RINDEX",photonEnergy,refractiveIndexSilicone,nEntries);
	mptSilicone->AddProperty("ABSLENGTH",photonEnergy,absClad,nEntries);

	fSilicone->SetMaterialPropertiesTable(mptSilicone);

	//--------------------------------------------------
	// CsI
	//--------------------------------------------------
	fCsI= fNistMan->FindOrBuildMaterial("G4_CESIUM_IODIDE");

	G4double photonEnergy_CsI[2] = {2.0*eV, 3.47*eV}; //最小波长320nm
	G4double refractiveIndex_CsI[2] = {1.79, 1.79};
	G4double AbsorptionLength_CsI[2] = {50 * cm, 50 * cm};
	G4double CsIFast[2] = {1,1};


	G4MaterialPropertiesTable *myMPT_CsI = new G4MaterialPropertiesTable();
	myMPT_CsI->AddProperty("RINDEX", photonEnergy_CsI, refractiveIndex_CsI, 2);	 //refractive index
	myMPT_CsI->AddProperty("ABSLENGTH", photonEnergy_CsI, AbsorptionLength_CsI, 2); //absorption length / decay length
	myMPT_CsI->AddConstProperty("SCINTILLATIONYIELD", 5000. / MeV);
	myMPT_CsI->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy_CsI, CsIFast, 2);
	myMPT_CsI->AddConstProperty("RESOLUTIONSCALE", 1.0);
	myMPT_CsI->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2. * ns); // fast and slow ???
	//myMPT_scin->AddConstProperty("SLOWTIMECONSTANT",10.*ns);
	myMPT_CsI->AddConstProperty("SCINTILLATIONYIELD1", 1);
	myMPT_CsI->DumpTable();

	fCsI->SetMaterialPropertiesTable(myMPT_CsI);

	//--------------------------------------------------
	// Lead Glass
	//--------------------------------------------------
	G4double photonEnergy_LG[2] = {2.0*eV, 3.47*eV}; //最小波长320nm
	G4double refractiveIndex_LG[2] = {1.66, 1.66};
	G4double AbsorptionLength_LG[6] = {800 * cm, 600 * cm, 300 * cm, 175 * cm, 50 * cm, 10 * cm};
	G4double LGFast[6] = {1,1,1,1,1,1};

	G4MaterialPropertiesTable *myMPT_LG = new G4MaterialPropertiesTable();
	myMPT_LG->AddProperty("RINDEX", photonEnergy_LG, refractiveIndex_LG, 2);	 //refractive index
	myMPT_LG->AddProperty("ABSLENGTH", photonEnergy_simple, AbsorptionLength_LG, 6); //absorption length / decay length
	//myMPT_LG->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy_simple, LGFast, 6); //fast和slow都是基于闪烁光的，没有加入闪烁光，所以报错
	myMPT_LG->AddConstProperty("RESOLUTIONSCALE", 1.0);
	//myMPT_LG->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2. * ns); // fast and slow ???
	//myMPT_scin->AddConstProperty("SLOWTIMECONSTANT",10.*ns);
	//myMPT_LG->AddConstProperty("SCINTILLATIONYIELD1", 1);
	myMPT_LG->DumpTable();

	fLeadGlass->SetMaterialPropertiesTable(myMPT_LG);

	//--------------------------------------------------
	// PbWO4
	//--------------------------------------------------
	fPbWO4 = fNistMan->FindOrBuildMaterial("G4_PbWO4");

	const G4int PbWO4_points = 20;
	G4double photonEnergy_PbWO4[PbWO4_points] = {1.55*eV, 1.6*eV, 1.65*eV, 1.71*eV, 1.77*eV, 1.84*eV, 1.91*eV, 1.98*eV, 2.07*eV, 2.16*eV, 2.25*eV, 2.36*eV, 2.48*eV, 2.61*eV, 2.76*eV, 2.92*eV, 3.1*eV, 3.31*eV, 3.54*eV, 3.81*eV}; // wavelength 800 - 325(25nm interval)
	G4double refractiveIndex_PbWO4[PbWO4_points] = {2.25, 2.25, 2.25, 2.25, 2.25, 2.25, 2.25, 2.25, 2.25, 2.25, 2.25, 2.25, 2.25, 2.25, 2.26, 2.21, 2.17, 2.12, 2.1, 2.1}; //Investigation of linear and nonlinear optical properties of PbWO4 single crystal, S.Delice, 2022
	G4double AbsorptionLength_PbWO4[PbWO4_points] = {200*cm, 200 * cm, 200 * cm, 200 * cm, 200 * cm, 200 * cm, 200 * cm, 200 * cm, 200 * cm, 200 * cm, 200 * cm, 200 * cm, 200 * cm, 50 * cm, 50 * cm, 40 * cm, 15 * cm, 10 * cm, 0 * cm, 0 * cm}; //From "A study on the properties of Lead Tungstate Crystals, R.Y. Zhu, 1996", transmittance(fig.5 728) -> Absorption length(fig.1)
	G4double PbWO4Fast[PbWO4_points] = {0.01, 0.01, 0.015, 0.02, 0.03, 0.04, 0.07, 0.08, 0.14, 0.2, 0.23, 0.24, 0.2, 0.11, 0.05, 0.03, 0.01, 0.005, 0., 0.}; //From "A study on the properties of Lead Tungstate Crystals, R.Y. Zhu, 1996"

	G4MaterialPropertiesTable *myMPT_PbWO4 = new G4MaterialPropertiesTable();
	myMPT_PbWO4->AddProperty("RINDEX", photonEnergy_PbWO4, refractiveIndex_PbWO4, PbWO4_points);     //refractive index
	myMPT_PbWO4->AddProperty("ABSLENGTH", photonEnergy_PbWO4, AbsorptionLength_PbWO4, PbWO4_points); //absorption length / decay length
	myMPT_PbWO4->AddConstProperty("SCINTILLATIONYIELD", 40. / MeV); //undoped 200/MeV, doped 300-500, suggest to use 400
	myMPT_PbWO4->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy_PbWO4, PbWO4Fast, PbWO4_points); //SCINTILLATIONCOMPONET1,2,3
	myMPT_PbWO4->AddConstProperty("RESOLUTIONSCALE", 1.0);
	myMPT_PbWO4->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 50. * ns); 
	myMPT_PbWO4->AddConstProperty("SCINTILLATIONYIELD1", 1);
	myMPT_PbWO4->DumpTable();

	fPbWO4->SetMaterialPropertiesTable(myMPT_PbWO4);

	/****************************/
	// SURFACE 
	/****************************/

	//--------------------------------------------------
	// Tyvek  
	//--------------------------------------------------
	TyvekSurface = new G4OpticalSurface("TyvekSurface",
            glisur,
            ground,
            dielectric_metal,
			0.3);

    G4MaterialPropertiesTable* TyvekSurfaceProperty = new G4MaterialPropertiesTable();

    G4double p_Tyvek[2] = {2.0*eV, 3.47*eV};
    G4double refl_Tyvek[2] = {0.9, 0.9}; //实际很高（0.96），在此减少
    G4double effi_Tyvek[2] = {1-refl_Tyvek[0], 1-refl_Tyvek[1]};
    TyvekSurfaceProperty -> AddProperty("REFLECTIVITY",p_Tyvek,refl_Tyvek,2);
	
    TyvekSurfaceProperty -> AddProperty("EFFICIENCY",p_Tyvek,effi_Tyvek,2);
    TyvekSurface -> SetMaterialPropertiesTable(TyvekSurfaceProperty);

	//--------------------------------------------------
	// PMT  (fPhotocathode)
	//--------------------------------------------------
	PMTSurface = new G4OpticalSurface("PMTSurface", glisur, polished, dielectric_metal);

	//G4double refl_PMT[6] = { 0.045, 0.1, 0.1, 0.1, 0.1, 0.1};
	G4double refl_PMT[6] = { 0.045, 0.1, 0.2, 0.25, 0.29, 0.28};
	G4double effi_PMT[6];
	for(int i=0; i<6; i++){
		effi_PMT[i] = 1- refl_PMT[i];
	}
	G4MaterialPropertiesTable *PMTSurfaceProperty = new G4MaterialPropertiesTable();
	PMTSurfaceProperty->AddProperty("REFLECTIVITY", photonEnergy_simple, refl_PMT, 6);
	PMTSurfaceProperty->AddProperty("EFFICIENCY", photonEnergy_simple, effi_PMT, 6);
	PMTSurface->SetMaterialPropertiesTable(PMTSurfaceProperty);

	//--------------------------------------------------
	// Sipm, 这些数据从哪得到的？pengxiaoyu 的也是。
	//--------------------------------------------------
	SipmSurface = new G4OpticalSurface("SipmSurface",
														 glisur,
														 polished,
														 dielectric_metal);

	G4MaterialPropertiesTable *SipmSurfaceProperty = new G4MaterialPropertiesTable();

	G4double p_Sipm[50] = {2.00 * eV, 2.03 * eV, 2.06 * eV, 2.09 * eV, 2.12 * eV,
						   2.15 * eV, 2.18 * eV, 2.21 * eV, 2.24 * eV, 2.27 * eV,
						   2.30 * eV, 2.33 * eV, 2.36 * eV, 2.39 * eV, 2.42 * eV,
						   2.45 * eV, 2.48 * eV, 2.51 * eV, 2.54 * eV, 2.57 * eV,
						   2.60 * eV, 2.63 * eV, 2.66 * eV, 2.69 * eV, 2.72 * eV,
						   2.75 * eV, 2.78 * eV, 2.81 * eV, 2.84 * eV, 2.87 * eV,
						   2.90 * eV, 2.93 * eV, 2.96 * eV, 2.99 * eV, 3.02 * eV,
						   3.05 * eV, 3.08 * eV, 3.11 * eV, 3.14 * eV, 3.17 * eV,
						   3.20 * eV, 3.23 * eV, 3.26 * eV, 3.29 * eV, 3.32 * eV,
						   3.35 * eV, 3.38 * eV, 3.41 * eV, 3.44 * eV, 3.47 * eV};

	G4double refl_Sipm[50] = {0.1500, 0.1600, 0.163, 0.172, 0.178, 0.182, 0.1900, 0.198, 0.2050, 0.2100,
							  0.2150, 0.2180, 0.222, 0.229, 0.232, 0.240, 0.2410, 0.242, 0.2434, 0.2437,
							  0.2439, 0.2439, 0.250, 0.2439, 0.2439, 0.2438, 0.2437, 0.2435, 0.243, 0.242,
							  0.24, 0.239, 0.238, 0.235, 0.233, 0.232, 0.23, 0.223, 0.222, 0.216,
							  0.211, 0.203, 0.2, 0.197, 0.195, 0.193, 0.19, 0.186, 0.183, 0.179};
	

	G4double effi_Sipm[50];
	for(int i=0; i<50; i++){
		effi_Sipm[i] = 1- refl_Sipm[i];
	}
	SipmSurfaceProperty->AddProperty("REFLECTIVITY", p_Sipm, refl_Sipm, 50);
	SipmSurfaceProperty->AddProperty("EFFICIENCY", p_Sipm, effi_Sipm, 50);
	SipmSurface->SetMaterialPropertiesTable(SipmSurfaceProperty);

	//--------------------------------------------------
	//  Polystyrene
	//--------------------------------------------------
	/*
	G4double refractiveIndexPS[] =
	{ 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
		1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
		1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
		1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
		1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50};

	assert(sizeof(refractiveIndexPS) == sizeof(photonEnergy));

	G4double absPS[] =
	{2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
		2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
		2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
		2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
		2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm};

	assert(sizeof(absPS) == sizeof(photonEnergy));

	G4double scintilFast[] =
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

	assert(sizeof(scintilFast) == sizeof(photonEnergy));

	// Add entries into properties table
	G4MaterialPropertiesTable* mptPolystyrene = new G4MaterialPropertiesTable();
	mptPolystyrene->AddProperty("RINDEX",photonEnergy,refractiveIndexPS,nEntries);
	mptPolystyrene->AddProperty("ABSLENGTH",photonEnergy,absPS,nEntries);
	mptPolystyrene->AddProperty("SCINTILLATIONCOMPONENT1",photonEnergy, scintilFast,nEntries);
	mptPolystyrene->AddConstProperty("SCINTILLATIONYIELD",10./keV);
	mptPolystyrene->AddConstProperty("RESOLUTIONSCALE",1.0);
	mptPolystyrene->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 10.*ns);

	fPolystyrene->SetMaterialPropertiesTable(mptPolystyrene);

	// Set the Birks Constant for the Polystyrene scintillator

	fPolystyrene->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
*/
}
}
