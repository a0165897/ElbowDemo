#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H


struct PathPointStructure //精度高,可用于计算过程
{
	double theta;      //张角绝对坐标
	double phi;   //中心角绝对坐标
	double alpha;   //缠绕有向角
    //double arc_length;      //弧长绝对坐标
};

struct fPathDataStructure  //降低精度,float 足以
{
	float theta;      //张角绝对坐标
	float phi;        //中心角绝对坐标
	float alpha;     //缠绕有向角
    //double arc_length;      //弧长绝对坐标
};

enum SearchDirection
{
	LeftToRight=1,
	RightToLeft=2
};

enum Quadrant           // Declare enum type Quadrant
{
   First=1,            // First Quadrant
   Second=2,          // Second Quadrant
   Third=3,           // Third Quadrant
   Fourth=4           // Fourth Quadrant
};

enum CatchFlag            // Declare enum type Quadrant
{
   ExceedY0=1,           // out range from the side of Y0
   ExceedYobj=2,         // out range from the side of Yobj
   NoCatch=3,             // No object Y is catched
   ReachIillCondition=4
};

enum Sign
{
   Negative=-1,
   Positive=1,
   Zero=0
};

enum WindingPathType
{
	GeodesicToGeodesic=1,
    GeodesicToSemi=2,
    SemiToGeodesic=3
};

struct Pressure
{
	float alpha_f;
	float alpha_b;
	float thickness_ave;
};
struct Site
{
	float x, y, z;
	float tx, ty, tz;
	float nx, ny, nz;
};
struct Mandrel
{
	float a, b, spanAngle;
	float cylinderLength, deviation;
};
struct Miscellence
{
	float highPayeye, zCarriage;
	float bar, epsilon;
	float widePayeye, wideCenter;
};


#endif