#if _MSC_VER>1000
#pragma once
#endif //_MSC_VER>1000

struct Pressure
{
	float alpha_f;
	float alpha_b;
	float thickness_ave;
};
struct Site
{
	float x,y,z;
	float tx,ty,tz;
	float nx,ny,nz;
};
struct Track
{
	float x,z;
	float swingAngle,spindleAngle;
};
struct Mandrel
{
	float a,b,spanAngle;
	float cylinderLength,deviation;
};
struct Miscellence
{
	float highPayeye,zCarriage;
	float bar,epsilon;
	float widePayeye,wideCenter;
};

