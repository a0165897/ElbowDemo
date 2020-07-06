
// pay.cpp: implementation of the pay class.
//
//////////////////////////////////////////////////////////////////////

  //#include ""
#include "pay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

pay::pay()
{

}

pay::~pay()
{

}

site* elbowsite(float a,float b,float thick,float deviation,
			struct PathPoint path,float& gn)
{
	site* psite;
	float phi,theta,alpha,e,bn,dbn,en;
	phi=path.phi; theta=path.theta; alpha=path.alpha;
	e=a+b*cos(phi);
	bn=b+path.nolayer*thick*(a+b)/e;
	dbn=path.nolayer*thick*(a+b)*b*sin(phi)/e/e;
	en=a+bn*cos(phi);
	gn=sqrt(bn*bn+dbn*dbn);
	psite->x=en*cos(theta)-deviation;
	psite->y=en*sin(theta);
	psite->z=bn*sin(phi);
	psite->xtangent=-sin(theta)*cos(alpha)+
		(-bn*sin(phi)+dbn*cos(phi))*cos(theta)*sin(alpha)/gn;
	psite->ytangent=cos(theta)*cos(alpha)+
		(-bn*sin(phi)+dbn*cos(phi))*sin(theta)*sin(alpha)/gn;
	psite->ztangent=(bn*cos(phi)+dbn*sin(phi))*sin(alpha)/gn;
	psite->xnormal=(bn*cos(phi)+dbn*sin(phi))*cos(theta)/gn;
	psite->ynormal=(bn*cos(phi)+dbn*sin(phi))*sin(theta)/gn;
	psite->znormal=-(-bn*sin(phi)+dbn*cos(phi))/gn;
	return psite;
}

struct site* cylindersite(float a,float deviation,
				float m_span_angle,struct PathPoint path)
{
	struct site* psite;
	float phi,y,alpha,bn;
	bn=path.bn;
	phi=path.phi; y=path.theta; alpha=path.alpha;
	psite->x=a*cos(m_span_angle)-deviation+
		bn*cos(phi)*cos(m_span_angle)-y*sin(m_span_angle);
	psite->y=path.flag*(a*sin(m_span_angle)+
		bn*cos(phi)*sin(m_span_angle)+y*cos(m_span_angle));
	psite->z=bn*sin(phi);
	psite->xtangent=-sin(m_span_angle)*cos(alpha)-
		sin(phi)*cos(m_span_angle)*sin(alpha);
	psite->ytangent=(cos(m_span_angle)*cos(alpha)-
		sin(phi)*sin(m_span_angle)*sin(alpha))*path.flag;
	psite->ztangent=cos(phi)*sin(alpha);
	psite->xnormal=cos(phi)*cos(m_span_angle);
	psite->ynormal=path.flag*cos(phi)*sin(m_span_angle);
	psite->znormal=sin(phi);
	return psite;
}

void UpDown(int icountstart,//i:sequence number of this beginning
		int countPathPoint,
		int counttau,//angle in spindle_axis at PI/2 times
		int rotatedirect,//spindle_axis rotate direction
		float psistart,//angle psi initial value
		float psidown,//angle psi ending value
		float yplane,//track plane y_coord.
		float xcenter,float ycenter,float zcenter,//coord. center
		float deviation,//spindle_axis shift about mandrel
		float a,float b,float thick,float m_span_angle,
				//torus parameter,thick of yarn,span initial angle
		struct PathPoint *pPath,
		int& iwindup,
		float& psiwindup,//for DownUp
		float& lengthwindup,
		struct Track *ptrack)
{
	int i;
	float dist,psi,xt,yt,zt,ro,tau,yarnlength,gn;
	struct site *fsite;

	dist=rotatedirect*counttau*PI/2;
	i=icountstart;
	do{
		i++;
		if(i>countPathPoint)
		{iwindup=i-1;
		 return;
		}
		if(pPath[i].flag==0)
			fsite=elbowsite(a,b,thick,deviation,pPath[i],gn);
		else fsite=cylindersite(a,deviation,m_span_angle,pPath[i]);
		psi=psistart-pPath[i].phi+pPath[icountstart].phi;
		if(psi>psidown) psi=psidown;
		xt=fsite->xtangent;
		yt=fsite->ytangent;
		zt=fsite->ztangent;
		ro=asin(xt/sqrt(xt*xt+zt*zt));
		if(zt<0) ro=PI-ro;
		tau=psi-ro+(counttau-1)*PI/2;
		yarnlength=(yplane-ycenter+fsite->x*sin(tau)-
			fsite->z*cos(tau))/(-xt*sin(tau)+zt*cos(tau));
		ptrack[i].x=xcenter+fsite->y+yarnlength*yt;
	//	ptrack[i].y=ycenter-(fsite->x+yarnlength*xt)*sin(tau)+
	//		(fsite->z+yarnlength*zt)*cos(tau);
		ptrack[i].z=zcenter+(fsite->x+yarnlength*xt)*cos(tau)+
			(fsite->z+yarnlength*zt)*sin(tau);
		ptrack[i].angle_spindle=tau;
		ptrack[i].angle_swing=atan(fsite->ynormal/
			(fsite->xnormal*sin(tau)-fsite->znormal*cos(tau)));
		ptrack[i].dx=ptrack[i].x-ptrack[i-1].x;
		ptrack[i].dz=ptrack[i].z-ptrack[i-1].z;
		ptrack[i].dangle_spindle=ptrack[i].angle_spindle-
			ptrack[i-1].angle_spindle;
		ptrack[i].dangle_swing=ptrack[i].angle_swing-
			ptrack[i-1].angle_swing;
	}while (rotatedirect*tau<dist);
	iwindup=i;
	psiwindup=psi;
	lengthwindup=yarnlength;
}

void DownUp(int icountstart,//next point begins to be calculated
		int countPathPoint,//Total winding point number
		int counttau,//initial spindle angle about PI/2 times
		float lengthstart,//initial yarn length
		float yplane,float xcenter,float ycenter,float zcenter,
		float deviation,int rotatedirect,
		float a,float b,float thick,float m_span_angle,
		struct PathPoint *pPath,
		int &iwindup,
		float &psiwindup,
		float &lengthwindup,
		struct Track *ptrack)
{
	struct site *fsite;
	int i,counttaubegin;
	float dist,lformer,yarnlength,xt,yt,zt,ds,aa,bb,tau,t,gn;
	float tauformer;

	counttaubegin=counttau-2;
	dist=rotatedirect*counttau*PI/2;
	tauformer=dist-PI;
	i=icountstart;
	lformer=lengthstart;
	do{
		i++;
		if(i>countPathPoint)
		{
			iwindup=i-1;
			return;
		}
		if(pPath[i].flag==0)
			fsite=elbowsite(a,b,thick,deviation,pPath[i],gn);
		else fsite=cylindersite(a,deviation,m_span_angle,pPath[i]);
		xt=fsite->xtangent;
		yt=fsite->ytangent;
		zt=fsite->ztangent;
		ds=(1.0/fabs(sin(pPath[i].alpha))+1.0/fabs(sin(pPath[i-1]
			.alpha)))*fabs(pPath[i].phi-pPath[i-1].phi)*gn/2;
		aa=fsite->x+(lformer-ds)*xt;
		bb=fsite->z+(lformer-ds)*zt;
		tau=-acos(bb/sqrt(aa*aa+bb*bb))+
			rotatedirect*counttaubegin*PI/2;
		t=(yplane-ycenter)/sqrt(aa*aa+bb*bb);
		if(t<=1) tau=acos(t)+tau;
		if(tau<tauformer)tau=tauformer;
		yarnlength=(yplane-ycenter+fsite->x*sin(tau)-
			fsite->z*cos(tau))/(-xt*sin(tau)+zt*cos(tau));
		ptrack[i].x=xcenter+fsite->y+yarnlength*yt;
	//	ptrack[i].y=ycenter-(fsite->x+yarnlength*xt)*sin(tau)+
	//		(fsite->z+yarnlength*zt)*cos(tau);
		ptrack[i].z=zcenter+(fsite->x+yarnlength*xt)*cos(tau)+
			(fsite->z+yarnlength*zt)*sin(tau);
		ptrack[i].angle_spindle=tau;
		ptrack[i].angle_swing=atan(fsite->ynormal/
			(fsite->xnormal*sin(tau)-fsite->znormal*cos(tau)));
		ptrack[i].dx=ptrack[i].x-ptrack[i-1].x;
		ptrack[i].dz=ptrack[i].z-ptrack[i-1].z;
		ptrack[i].dangle_spindle=ptrack[i].angle_spindle-
			ptrack[i-1].angle_spindle;
		ptrack[i].dangle_swing=ptrack[i].angle_swing-
			ptrack[i-1].angle_swing;
	}while(rotatedirect*tau<dist);
	iwindup=i;
	psiwindup=tau+asin(xt/sqrt(xt*xt+zt*zt));
	lengthwindup=yarnlength;
}

void payeye(float m_a,float m_b,float thick,
			float m_span_angle,
			int rotatedirect,float deviation,float yplane,
			float xcenter,float ycenter,float zcenter,
			float countPathPoint,struct PathPoint *pPath,
			struct Track *ptrack)
{
	struct site *fsite;
	int istart,iwindup,counttau;
	float psidown,psistart,xt,yt,zt,t,ro,tau,l,gn;
	float taustart,tauwindup,lengthstart,lengthwindup,
		  psiwindup;

	//set initial position
	psidown=5.0*PI/180.0;
	fsite=elbowsite(m_a,m_b,thick,deviation,pPath[0],gn);
	xt=fsite->xtangent;
	yt=fsite->ytangent;
	zt=fsite->ztangent;
	ro=asin(xt/sqrt(xt*xt+zt*zt));
    tau=psidown-ro;
	l=(yplane-ycenter+fsite->x*sin(tau)-fsite->z*cos(tau))/
		(-xt*sin(tau)+zt*cos(tau));
	ptrack[0].x=xcenter+fsite->y+l*fsite->ytangent;
	ptrack[0].y=ycenter-(fsite->x+l*xt)*sin(tau)+
		(fsite->z+l*zt)*cos(tau);
	ptrack[0].z=zcenter+(fsite->x+l*xt)*cos(tau)+
		(fsite->z+l*zt)*sin(tau);
	ptrack[0].angle_spindle=tau;
	ptrack[0].angle_swing=atan(fsite->ynormal
		/(fsite->xnormal*sin(tau)-fsite->znormal*cos(tau)));

	//trejectory
	istart=0;
	counttau=1;
	fsite->y=ycenter+1;
	psistart=psidown;

	do{
		if(fsite->y>ycenter)
			UpDown(istart,countPathPoint,counttau,rotatedirect,
			psistart,psidown,yplane,xcenter,ycenter,zcenter,
			deviation,m_a,m_b,thick,m_span_angle,
			pPath,
			iwindup,psiwindup,lengthwindup,
			ptrack);
		else DownUp(istart,countPathPoint,counttau,lengthstart,
			yplane,xcenter,ycenter,zcenter,deviation,
			rotatedirect,m_a,m_b,thick,m_span_angle,
			pPath,
			iwindup,psiwindup,lengthwindup,
			ptrack);
		istart=iwindup;
		psistart=psiwindup;
		lengthstart=lengthwindup;
		counttau=counttau+2;
	}while(iwindup>=countPathPoint);
}


