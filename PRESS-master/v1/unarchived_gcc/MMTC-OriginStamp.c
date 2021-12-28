#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include "geometry.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>

union Int2Binary {
	int value;
	unsigned char data[4];
};

union Int2Binary int2Binary;

union Double2Binary {
	double value;
	unsigned char data[8];
};

union Double2Binary double2Binary;

void writeInt(FILE* file, int value) {
	int2Binary.value = value;
	fwrite(int2Binary.data,sizeof(unsigned char), 4,file);
}

int readInt(FILE* file) {
	//unsigned char buf[4];
	if (fread(int2Binary.data,sizeof(unsigned char), 4,file) < 4) return -1;
	return int2Binary.value;
}

void writeDouble(FILE* file, double value) {
	double2Binary.value = value;
	fwrite(double2Binary.data,sizeof(unsigned char), 8,file);
}

double readDouble(FILE* file) {
	if (fread(double2Binary.data,sizeof(unsigned char), 8,file) < 8) return -1;
	return double2Binary.value;
}

//����������
double cosTangle(double x1, double y1, double x2, double y2) {
	return (x1*x2+y1*y2)/(sqrt(x1*x1+y1*y1)*sqrt(x2*x2+y2*y2));
}

//����·ͬ�켣�ĸ��ʹ�ʽ
double direction(int r1, int r2) {
	double x1 = coordNode[other[r1]*2]-coordNode[thisSide[r1]*2];
	double y1 = coordNode[other[r1]*2+1]-coordNode[thisSide[r1]*2+1];
	double x2 = coordNode[other[r2]*2]-coordNode[thisSide[r2]*2];
	double y2 = coordNode[other[r2]*2+1]-coordNode[thisSide[r2]*2+1];
	//printf("%.4lf %d\n",len[r2],cost[r2]);
	return ((cosTangle(x1, y1, x2, y2) + 1) + 1e-4) * len[r2] / cost[r2];
}

int roadSet[6000][100];
double probMul[6000][100];
int preLink[6000][100];

double velocity = 10;

//int tmpList[10000],roadList[10000],timeList[10000],spCount[10000];
//double prob[10000];

char inPath[20000][100];
char inPath2[20000][100];

int path[10000000];
double accDist[1000000];

int ansSize = 0;
int timeList[1000000];//0:ʱ��	1:����
double distList[1000000];

//����·��
void getInputPathSet(char* path)
{
	dataNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPath[dataNumber],path);
		strcat(inPath[dataNumber],"/");
		strcat(inPath[dataNumber],ent->d_name);
		++dataNumber;
	}
	closedir(dir);
}

//����·��
void getInputPathSet2(char* path)
{
	dataNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPath2[dataNumber],path);
		strcat(inPath2[dataNumber],"/");
		strcat(inPath2[dataNumber],ent->d_name);
		++dataNumber;
	}
	closedir(dir);
}

//���㼸��
/**
 * Get Distance between two points
 */
double circleDistance(double lat1,double long1,double lat2,double long2)
{
	double deltaLat=lat1-lat2;
	double deltaLong=(long2-long1)*cos(lat1 * PI180);
	return LENGTH_PER_RAD*sqrt(deltaLat*deltaLat+deltaLong*deltaLong);
}

/**
 *description:��������һ�㵽һ��·���ľ���
 *param: double double ������  int �߱��  double* ��������ߵ�ǰ�˵����  double* ��·����
 *return:��������룬��λ��
 */
double nodeToEdgeDistanceAndNodeSide(double nodeX,double nodeY,int edgeId,double *sideLen,double *roadLen){
	int i,j;
	double tmpSideLen=0,tmpRoadLen=0;
	double result=1e80,tmp=0;
	for (i=edgeStart[edgeId];i<edgeStart[edgeId+1];++i){
		double x=coordNet[i << 1],y=coordNet[(i << 1)+1],x2=coordNet[(i << 1)+2],y2=coordNet[(i << 1)+3];
		double dist=circleDistance(x,y,nodeX,nodeY);
		if (dist<result){
			result=dist;
			tmpSideLen=tmp;
		}
		if (i<edgeStart[edgeId+1]-1) {
			double vecX1=x2-x,vecY1=y2-y,
					vecX2=nodeX-x,vecY2=nodeY-y,
					vecX3=nodeX-x2,vecY3=nodeY-y2;
			if (vecX1*vecX2+vecY1*vecY2>0 && -vecX1*vecX3-vecY1*vecY3>0 && (vecX1!=0 || vecY1!=0)){
				double rate=((nodeX-x2)*vecX1+(nodeY-y2)*vecY1)/(-vecX1*vecX1-vecY1*vecY1);
				double nearX=rate*x+(1-rate)*x2,nearY=rate*y+(1-rate)*y2;
				double dist=circleDistance(nearX,nearY,nodeX,nodeY);
				if (dist<result){
					result=dist;
					tmpSideLen=tmp+circleDistance(x,y,nearX,nearY);
				}
			}
			tmpRoadLen+=circleDistance(x,y,x2,y2);
		}
		tmp+=circleDistance(x,y,x2,y2);
	}

	*sideLen=tmpSideLen;
	*roadLen=tmpRoadLen;
	return result;
}

int main(int argc, char *argv[]){

	if (argc < 4) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	loadData(argv[1]);
	
	//1 ����켣�ļ�
	FILE* spatial = fopen(argv[2],"rb");
	FILE* stamp = fopen(argv[3],"wb");
	
	int traNumber = readInt(spatial);
	//printf("%d\n",traNumber);
	writeInt(stamp, traNumber);
	
	int i;
	for (i = 0; i < traNumber; ++i) {
		//puts(inPath[i]);
		int spNumber = readInt(spatial);
		
		writeInt(stamp,spNumber);
		int j;
		accDist[0] = 0;
		double roadLen,sideLen;
		
		for (j = 0; j < spNumber; ++j) { 
			path[j] = readInt(spatial);
			//printf("%d ",path[j]);
			nodeToEdgeDistanceAndNodeSide(0,0,path[j],&sideLen,&roadLen);
			writeInt(stamp,path[j]);
			writeDouble(stamp,roadLen);
		}
		
		//������
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	fclose(spatial);
	fclose(stamp);
	
	return 0;
}
