float PtRatio(float pt1, float pt2, float pt3)
{
	if(pt2!=0&&pt3!=0){
		return pt1*pt1/(pt2*pt3);
	}else{
		return 0;
	}
}