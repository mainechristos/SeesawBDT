float DeltaPhi( float phi1, float phi2 )
{
  	float deltaphi = phi2-phi1;
  	if(      deltaphi>  TMath::Pi() ){
   		deltaphi -= 2.0*TMath::Pi();
   		deltaphi=TMath::Abs(deltaphi);
   		return deltaphi;
	}
  	else if( deltaphi<=-TMath::Pi() ){
  		deltaphi += 2.0*TMath::Pi();
  		deltaphi=TMath::Abs(deltaphi);
  		return deltaphi;
  	}
  	deltaphi=TMath::Abs(deltaphi);
  	deltaphi=0;
  	return deltaphi;
}

