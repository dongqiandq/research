//��ѯĳʱ��ÿ���켣�������������Լ���

#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include "geometry.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

//���㼸��
double circleDistance(double lat1,double long1,double lat2,double long2)
{
	double deltaLat=lat1-lat2;
	double deltaLong=(long2-long1)*cos(lat1 * PI180);
	return LENGTH_PER_RAD*sqrt(deltaLat*deltaLat+deltaLong*deltaLong);
}

int main(int argc, char *argv[]){

	if (argc < 4) {
		puts("Argument Exception!");
		return 1;
	}
	
	FILE* precise = fopen(argv[1],"r");
	FILE* prox = fopen(argv[2],"r");
	
	//���������Ͻ�
	char* ttt = argv[3];
	int i;
	int tolerance = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance = tolerance * 10 + ttt[i] - 48;
	
	int dataNumber,del;
	fscanf(precise,"%d",&dataNumber);
	fscanf(prox,"%d",&del);

	double max = 0,all = 0;
	for (i = 0; i < dataNumber; ++i) {
		double x1,x2,y1,y2;
		fscanf(precise,"%d %lf %lf",&del,&x1,&y1);
		fscanf(prox,"%d %lf %lf",&del,&x2,&y2);
		double tmp = circleDistance(x1,y1,x2,y2);
		if (tmp > max) max = tmp;
		if (tmp < tolerance) all += tmp;
	}
	
	printf("%.4lf\n",all / dataNumber);
	
	fclose(precise);
	fclose(prox);
	return 0;
}
