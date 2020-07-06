#include<math.h>
#include "publicfun.h"
#include <stdafx.h>

void FindAbsoluteValueMinMaxInArray(double *pArray, int cell_num, double &min, int &min_index,double &max, int &max_index )
{
   int i;
   double curr;
   min=fabs(pArray[0]);
   min_index=0;
   max=min;
   max_index=0;

   for(i=1;i<cell_num;i++)
   {
	   curr=fabs(pArray[i]);
		if(curr<min)
		{
			min=curr;
			min_index=i;
		}
   }

    for(i=1;i<cell_num;i++)
   {
	   curr=fabs(pArray[i]);
		if(curr>max)
		{
			max=curr;
			max_index=i;
		}
   }
}

void  Prompt(char *message)
{
	AfxMessageBox(message,MB_OK||MB_APPLMODAL); 
}
