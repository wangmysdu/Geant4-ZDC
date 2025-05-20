//generate geometry and run mac files for the simulation

void ScriptGenerator()
{
    
    const int NFILE = 10;
    
    for (int i=0; i<NFILE; i++){
        ofstream runFile;
        ofstream geoFile;
        
        runFile.open(Form("run%d.mac", i+1));
        geoFile.open(Form("geometry%d.mac", i+1));
        //for the run file
        runFile<<Form("/control/execute geometry%d.mac", i+1)<<endl;
	runFile<<"/run/initialize"<<endl;
        runFile<<endl;
        runFile<<"/gps/particle neutron"<<endl;
        runFile<<"/gps/pos/type Plane"<<endl;
        runFile<<"/gps/pos/shape Circle"<<endl;
        runFile<<"/gps/pos/centre 0. 0. -200. cm"<<endl;
        runFile<<"/gps/pos/radius 1. um"<<endl;
        runFile<<endl;
        runFile<<"/gps/energy 10. GeV"<<endl;
        runFile<<"/gps/direction 0 0 1"<<endl;
        runFile<<endl;
        runFile<<"/run/beamOn 100"<<endl;
        runFile<<endl;
        
        //for the geometry file
        geoFile<<"#for the WSci sector"<<endl;
        geoFile<<"/evt/WSci/NModule      5"<<endl;
        geoFile<<"/det/WSci/NLayer       42"<<endl;
        geoFile<<"/det/WSci/Size         50   mm"<<endl;
        geoFile<<"/det/WSci/WThick       3.5  mm"<<endl;
        geoFile<<"/det/WSci/SciThick     1.0  mm"<<endl;
        geoFile<<endl;
        geoFile<<"#for the PbWO4 sector"<<endl;
        geoFile<<"/evt/Shash/NModule     4"<<endl;
        geoFile<<"/det/Shash/NLayer      15"<<endl;
        geoFile<<"/det/Shash/ScinThick   6.0  mm"<<endl;
        geoFile<<Form("/det/Shash/LeadThick   %0.1f mm", 20.+i)<<endl;
        geoFile<<"/det/Shash/Size        120  mm"<<endl;
        geoFile<<endl;
        geoFile<<"#for fiber"<<endl;
        geoFile<<"/det/NFiberHole        16"<<endl;
        geoFile<<endl;
        geoFile<<"/det/InitGeo"<<endl;

        
        runFile.close();
        geoFile.close();
    }
}
