float LightLeptonDXY(int i, float MuonDXY0, float MuonDXY1, float MuonDXY2, float ElectronDXY0,float ElectronDXY1,float ElectronDXY2, int LightLeptonFlavor, int LightLeptonNativeIndex )
{
	float dxy = 0.0;
	
	//dxy = (LightLeptonFlavor==2)*MuonDXY2 + (LightLeptonFlavor==1)*ElectronDXY2;
	if(i==0){
		dxy = (LightLeptonFlavor==2)*MuonDXY0 + (LightLeptonFlavor==1)*ElectronDXY0;
	}else if(i==1){
		dxy = (LightLeptonFlavor==2)*MuonDXY1 + (LightLeptonFlavor==1)*ElectronDXY1;
	}else if(i==2){
		dxy = (LightLeptonFlavor==2)*MuonDXY2 + (LightLeptonFlavor==1)*ElectronDXY2;
	}

	return dxy;

	//dxy = (LightLeptonFlavor==2)*Alt$(MuonDXY[LightLeptonNativeIndex],0) + (LightLeptonFlavor==1)*Alt$(ElectronDXY[LightLeptonNativeIndex,0);

}