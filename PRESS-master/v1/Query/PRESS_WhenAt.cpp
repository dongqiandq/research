/**
	���ĳ��򣬽���MMTC��SPATIAl���ֵ�ѹ���ĳ���
*/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAXN 95000000

using namespace std;

ofstream fout("E:\\TC\\OUTPUT.TXT");

int nodeInRegion[2000000],edgeInRegion[2000000];
//double latMax2 = 47.30329376225394, latMin2 = 46.83389173208538;
//double longMax2 = -121.827392578125, longMin2 = -121.9557373046875;
double latMax2 = 47.86523461231467, latMin2 = 47.427622733649606;
double longMax2 = -122.213037109375, longMin2 =  -122.43576049804688;

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

//============
int nodeIndex[1000000][2];		//0���ļ�   1��ƫ��
int spSize;						//�����ٸ��������·
int spPre[1000000];				//ÿ�������һ����,-1��ʾ������
int path[100000];				//��¼ԭʼSPATIAL·��
int ansSize;					//ѹ����ʵ�����
int ansList[10000];			//ѹ����Ĺ켣�����Խ�ѹ��

int SPNUM[40000];
int** SPLINK;/*-1��ʾ����䲻��ͨ*/
int MAP[2000000];
int save[10000];

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

//��·�ڼ�·������
double pathLen(int node1, int node2) {/*����֮�����̾���*/
	//printf("\n%d %d\n",node1,node2);

	double res = 0, sideLen,roadLen;

	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[node1][node2];
		if (save[count] == -1) break;
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i) {
		nodeToEdgeDistanceAndNodeSide(0,0,save[i],&sideLen,&roadLen);
		res += roadLen;
	}
	return res;
}

//��·�ڼ�mbr
void edgeMBR(int edgeId, double* x1, double* x2, double* y1, double* y2) {
	int i,j;
	double minX = 1e100, minY = 1e100, maxX = -1e100, maxY = -1e100;

	for (i=edgeStart[edgeId];i<edgeStart[edgeId+1];++i){
		double x=coordNet[i << 1],y=coordNet[(i << 1)+1],x2=coordNet[(i << 1)+2],y2=coordNet[(i << 1)+3];
		if (x > maxX) maxX = x;
		if (x < minX) minX = x;
		if (x2 > maxX) maxX = x2;
		if (x2 < minX) minX = x2;
		if (y > maxY) maxY = y;
		if (y < minY) minY = y;
		if (y2 > maxY) maxY = y2;
		if (y2 < minY) minY = y2;
	}
	*x1 = minX;
	*x2 = maxX;
	*y1 = minY;
	*y2 = maxY;
}
void pathMBR(int node1, int node2, double* x1, double* x2, double* y1, double* y2) {
	//printf("\n%d %d\n",node1,node2);
	double mx1, mx2, my1, my2;
	double res = 0, sideLen,roadLen;
	double minX = 1e100, minY = 1e100, maxX = -1e100, maxY = -1e100;
	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[node1][node2];
		if (save[count] == -1) break;
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i) {
		edgeMBR(save[i], &mx1, &mx2, &my1, &my2);
		if (mx1 < minX) minX = mx1;
		if (mx2 > maxX) maxX = mx2;
		if (my1 < minY) minY = my1;
		if (my2 > maxY) maxY = my2;
	}
	*x1 = minX;
	*x2 = maxX;
	*y1 = minY;
	*y2 = maxY;
}

void loadSPAll2() {
	int t;
	int s;
	int f;
	int c = 0;

	//memset(SPLINK,255,sizeof(SPLINK));
	//SPLINK = (int**)malloc(40000*sizeof(int));
	SPLINK = new int*[30000];

	for (t = 0; t < 30000; ++t) {
		SPLINK[t] = new int[30000];
	}
	for (t = 0; t < 30000; ++t)
		for (s = 0; s < 30000; ++s) 
			SPLINK[t][s] = -1;
	char fileName[100]="../ShortestPath/SP00.txt";
	
	//////
	for (f = 0; f < 21; ++f) {
		//--
		//cout << f << endl;

		fileName[18] = (f / 10) + 48;
		fileName[19] = (f % 10) + 48;
		cout << fileName << endl;
		//--
		//printf("%d %s\n",nodeId,fileName);
		FILE* file = fopen(fileName,"rb");
		
		//����
		int number = readInt(file);
		int j;
		for (j = 0; j < number; ++j) {
			//id
			int id = readInt(file);
			//MAP[id] = c;
			//����
			int many = readInt(file);
			int i;
			//memset(spPre,255,sizeof(spPre));
			for (i = 0; i < many; ++i){
				int q = readInt(file);
				SPLINK[id][q] = readInt(file);
			}
			//++c;
		}
		//fseek(file,nodeIndex[nodeId][1],0);
		//spSize = readInt(file);
		fclose(file);
	}
	
}

int* son/*�ڵ㺢�ӱ��*/, *righting/*���ֵܱ��*/, *value/*·�α��*/, counting, *father/*���ڵ���*/, *father2/*��Զ���ȱ��,����ֱ�Ӻ��Ӵ���Ǳ�����*/, *L, *R, *F, *map/*-1������Ҷ�ӽڵ�*/, *rvsmap, *fail, depth;
double *trielen/*trie���ϼ�¼�ľ���*/,*mbr1,*mbr2,*mbr3,*mbr4/*xxyy*/;
int e = 0,p,q;
int halfRoot;/*�����������ڵ��ţ�L[i]=i�ڵ����ӱ��,R[i]=i�ڵ��Һ��ӱ�ţ�F[i]=i�ڵ㸸�ױ�ţ�map[i]=trie���ڵ���j��rvsmap[j]=��������i�ڵ�*/

double tmpDist[100000], tmpTime[100000]; 
char spCode[100000];


pair<double,double> query(int T, string code, int spNumber) {
	double d;
	int s,e;
	if(tmpTime[0] > T || tmpTime[spNumber-1] < T)
		return make_pair(-1,-1);
	for(int i = 0;i < spNumber;i++) {
		if(tmpTime[i] > T) {
			s = i-1;
			e = i;
			d = tmpDist[s] + (tmpDist[e]-tmpDist[s])/(tmpTime[e]-tmpTime[s])*(T-tmpTime[s]);
			break;
		}
	}
	int count = 0;

	int c = halfRoot;
	double curD = 0.0;
	double lastD = 0.0;
	bool mark = 0;
	int last;//��һ�����ڵ�
	int trieNum;
	int trieTarget;
	int result = -1;
	int final_s,final_t;
	for(int i = 0;i < code.length();i++) {
		int cur = code[i]-'0';
		if(cur == 0)
			c = L[c];
		else
			c = R[c];
		
		if(map[c] != -1) {
		//	cout << curD << endl;
		//	cout << map[c] << endl;
			if(mark == 0) {
				curD = trielen[map[c]];
				mark = 1;
				last = other[value[map[c]]]; 
				count++;
			}
			else {
				lastD = curD;
				curD += pathLen(last,thisSide[value[father2[map[c]]]]);
			//	cout << lastD << "\t" << curD << "\t 1"<< endl;
				count++;
				if(curD > d) {
					/*�������α�֮������·����*/
					final_s = last;
					final_t = thisSide[value[father2[map[c]]]];
					result = 0;
					break;
				}
				lastD = curD;
				curD += trielen[map[c]];
			//	cout << lastD << "\t" << curD << "\t 2" << endl;
				count++;
				if(curD > d) {
					/*����trie�����������*/
					result = 1;
					trieTarget = map[c];
					break;
				}
				last = other[value[map[c]]];
			}
			c = halfRoot;
		}
	}
	if(count <= 2)
		return make_pair(-1,-1);
	if(curD <= d) {
		return make_pair(coordNode[last*2],coordNode[last*2+1]);
	}
//	cout << result << endl;
	if(result == 0) {
		/*��final_s,final_t֮������·����*/
		int final_e;
		int curs,cure;
		double endD;
		while(curD > d) {
			final_e = SPLINK[final_s][final_t];
			curs = thisSide[final_e];
			cure = other[final_e];
			endD = curD;
			curD -= pathLen(curs,cure);
		}
		double x1 = coordNode[curs*2];
		double y1 = coordNode[curs*2+1];
		double x2 = coordNode[cure*2];
		double y2 = coordNode[cure*2+1];
		pair<double, double> re = make_pair((d-curD)/(endD-curD)*(x2-x1)+x1,(d-curD)/(endD-curD)*(y2-y1)+y1);
		
		return re;
	}
	else if(result == 1) {
		while(lastD+trielen[father[trieTarget]] > d) {
			trieTarget = father[trieTarget];
			//cout << lastD <<"\t" << trielen[father[trieTarget]] << endl;
		}
		if(lastD + trielen[father[trieTarget]] + pathLen(other[value[father[trieTarget]]],thisSide[value[trieTarget]]) > d) {
			/*��other[value[father[trieTarget]]],thisSide[trieTarget]֮��*/
	//		cout << 3 << endl;
			final_t = thisSide[value[trieTarget]];
	//		cout << final_s << "\t" << final_t << endl;
			if(pathLen(final_s,final_t) < 1e-3 || final_s == final_t)
				return make_pair(coordNode[final_s*2],coordNode[final_s*2+1]);
			int final_e;
			int curs,cure;
			double endD;
			int cc = 0;
			while(curD >= d) {
				cc++;
	//			system("pause");
				final_e = SPLINK[final_s][final_t];
				if(final_e == -1)
					return make_pair(-1,-1);
				final_t = thisSide[final_e];
				curs = thisSide[final_e];
				cure = other[final_e];
				endD = curD;
				curD -= pathLen(curs,cure);
	//			cout << final_e << endl;
	//			cout << curs << " " << cure << endl;
	//			cout << curD << endl;
			}
			double x1 = coordNode[curs*2];
			double y1 = coordNode[curs*2+1];
			double x2 = coordNode[cure*2];
			double y2 = coordNode[cure*2+1];
			pair<double, double> re = make_pair((d-curD)/(endD-curD)*(x2-x1)+x1,(d-curD)/(endD-curD)*(y2-y1)+y1);
			return re;
		}
		else {
			/*sonTarget����*/
			double startD = lastD + trielen[trieTarget] + pathLen(other[value[trieTarget]],thisSide[value[trieTarget]]);
			double endD =  lastD + trielen[trieTarget];
			double len = endD-startD;
			double x1 = coordNode[thisSide[value[trieTarget]]*2];
			double y1 = coordNode[thisSide[value[trieTarget]]*2+1];
			double x2 = coordNode[other[value[trieTarget]]*2];
			double y2 = coordNode[other[value[trieTarget]]*2+1];
			pair<double, double> re = make_pair((d-startD)/(endD-startD)*(x2-x1)+x1,(d-startD)/(endD-startD)*(y2-y1)+y1);
			return re;
		}
	}
} 
double dis(double x1,double y1,double x2,double y2,double x,double y) {
	if(x > x2) {
		if(y > y2)
			return (x-x2)*(x-x2)+(y-y2)*(y-y2);
		if(y < y1)
			return (x-x2)*(x-x2)+(y1-y)*(y1-y);
		return (x-x2)*(x-x2);
	}
	if(x < x1) {
		if(y > y2)
			return (x1-x)*(x1-x)+(y-y2)*(y-y2);
		if(y < y1)
			return (x1-x)*(x1-x)+(y1-y)*(y1-y);
		return (x1-x)*(x1-x);
	}
	if(y > y2)
		return (y-y2)*(y-y2);
	if(y < y1)
		return (y1-y)*(y1-y);
	return 0.0;
}
int queryWhenAt(string code, double x, double y, int spNumber) {
	int count = 0;
	int c = halfRoot;
	double curD = 0.0;
	bool mark = 0;
	int last;//��һ�����ڵ�
	double mindis = 1e10;
	double tdis;
	int trieNum;
	int trieTarget;
	int result = -1;
	int final_s,final_t;
	int inSP = -1;
	for(int i = 0;i < code.length();i++) {
		int cur = code[i]-'0';
		if(cur == 0)
			c = L[c];
		else
			c = R[c];
		if(map[c] != -1) {
			if(mark == 0) {
				if(mbr1[map[c]] <= x && mbr2[map[c]] >= x && mbr3[map[c]] <= y && mbr4[map[c]] >= y) {
					trieTarget = map[c];
					result = 0;
					break;
				}
				mindis = dis(mbr1[map[c]],mbr3[map[c]],mbr2[map[c]],mbr4[map[c]],x,y);
				curD = trielen[map[c]];
				last = other[value[map[c]]]; 
				mark = 1;
			}
			else {
				int endP = thisSide[value[father2[map[c]]]];
				double cx1,cy1,cx2,cy2;
				pathMBR(last,endP,&cx1,&cx2,&cy1,&cy2);
				if(cx1 <= x && cx2 >= x && cy1 <= y && cy2 >= y) {
	//				cout << 1 << endl;
					result = 1;
					final_s = last;
					final_t = endP;
					break;
				}
				tdis = dis(cx1,cy1,cx2,cy2,x,y);
				if(mindis > tdis) {
					result = 1;
					final_s = last;
					final_t = endP;
					mindis = tdis;
				}
				curD += pathLen(last,endP);
				if(mbr1[map[c]] <= x && mbr2[map[c]] >= x && mbr3[map[c]] <= y && mbr4[map[c]] >= y) {
	//				cout << 2 << endl;
					trieTarget = map[c];
					result = 0;
					break;
				}
				tdis = dis(mbr1[map[c]],mbr3[map[c]],mbr2[map[c]],mbr4[map[c]],x,y);
				if(mindis > tdis) {
					result = 0;
					trieTarget = map[c];
					mindis = tdis;
				}
				curD += trielen[map[c]];
				last = other[value[map[c]]]; 
			}
			c = halfRoot;
		}
	}
	//cout << result << endl;
	if(result == 1) {
		//��final_s��final_t�����·����
		curD += pathLen(final_s,final_t);
		double x1,x2,y1,y2;
		mindis = 1e10;
		int ts,tt;
		double td;
		int curE;
		while(thisSide[curE] != final_s) {
			curE = SPLINK[final_s][final_t];
			pathMBR(thisSide[curE],other[curE],&x1,&x2,&y1,&y2);
			tdis = dis(x1,y1,x2,y2,x,y);
			curD -= pathLen(thisSide[curE],other[curE]);
			if(tdis < mindis) {
				ts = thisSide[curE];
				tt = other[curE];
				mindis = tdis;
				td = curD;
			}
			final_t = thisSide[curE];
			
		//	cout << final_t << "\t" << final_s << endl;
			//system("pause");
		}
		curD = td;
		//cout << result << "\t" << trieTarget << endl;
	}
//	��final_s��final_t֮��ı��ϣ�final_s����ΪcurD
	for(int i = 0;i < spNumber;i++) {
		if(tmpDist[i] > curD) {
			return tmpTime[i-1] + (curD-tmpDist[i-1])/(tmpDist[i]-tmpDist[i-1]) * (tmpTime[i]-tmpTime[i-1]);
		}
	}
}
int main(int argc, char *argv[]) {

	//Load Road Network from File
	char sss[20] = "";
	
	//����·��
	loadData(sss);
	
	//===============
	int nodeUseful = 0;
	int i;

	
	//============Part 1 �����·ѹ���켣=====================
	//ȫ�ڴ��������·
	
	//���·��
	loadSPAll2();
	
	//����Trie
	FILE* fp = fopen("./suffix_03.txt","r");

	son = (int*)malloc(MAXN*sizeof(int));
	righting = (int*)malloc(MAXN*sizeof(int));
	value = (int*)malloc(MAXN*sizeof(int));
	father = (int*)malloc(MAXN*sizeof(int));
	father2 = (int*)malloc(MAXN*sizeof(int));

	mbr1 = (double*)malloc(MAXN*sizeof(double));
	mbr2 = (double*)malloc(MAXN*sizeof(double));
	mbr3 = (double*)malloc(MAXN*sizeof(double));
	mbr4 = (double*)malloc(MAXN*sizeof(double));
	trielen = (double*)malloc(MAXN*sizeof(double));
	fail = (int*)malloc(MAXN*sizeof(int));
	map = (int*)malloc(MAXN*sizeof(int));
	rvsmap = (int*)malloc(MAXN*sizeof(int));
	L = (int*)malloc(MAXN*sizeof(int));
	R = (int*)malloc(MAXN*sizeof(int));
	F = (int*)malloc(MAXN*sizeof(int));

	fscanf(fp,"%d",&e);
	for (int i = 0; i <= e; ++i)
		fscanf(fp,"%d%d%d%d%d",&son[i],&righting[i],&father[i],&value[i],&counting);
	fclose(fp);
	for (int i = 1; i <=e; ++i)
		if (father[i] == 0) father2[i] = i;
		else 
		father2[i] = father2[father[i]];

	//����failure function
	fp = fopen("./ac_03.txt","r");
	for (int i = 0; i <=e; ++i)
		fscanf(fp,"%d%d",&fail[i],&depth);
	fclose(fp);


	//Halfman������
	fp = fopen("./halfman_03.txt","r");

		// Huffman Root
	
	fscanf(fp,"%d",&halfRoot);
	for (int i = 0; i <= halfRoot; ++i) {
		int id;
		fscanf(fp,"%d",&id);
		fscanf(fp,"%d%d%d",&L[id],&R[id],&map[id]);
		if (L[id] > -1) F[L[id]] = id;
		if (R[id] > -1) F[R[id]] = id;
		rvsmap[map[id]] = id;
	}
	fclose(fp);

	//Len & MBR
	fp = fopen("./LenMBR.txt","r");
	for (int i = 0; i <=e; ++i)
		fscanf(fp,"%lf%lf%lf%lf%lf",&trielen[i],&mbr1[i],&mbr2[i],&mbr3[i],&mbr4[i]);
	fclose(fp);

	//����Spatial & Temporal Ȼ����
	

	long ttr = 0;

	fp =fopen(argv[1],"rb");
	FILE* spatial = fopen("./spatial.txt","r");
	//cout << 1 << endl;
	FILE* q = fopen("./WhenAt.txt","r");
	int queryNumber;
	fscanf(q,"%d",&queryNumber);
	int tjNumber = readInt(fp);
	int kk;
	fscanf(spatial,"%d\n",&kk);
	int qcount = 0; 
	int pNumber;

	long start = clock();

	//cout << 1 << endl;
	for(int j = 0;j < queryNumber;j++) {
		double qx,qy;
		int qNumber;    
		fscanf(q,"%d %lf %lf",&qNumber,&qx, &qy);
		while(qcount <= qNumber) {
			//cout << qcount << "\t" << qNumber << endl;
			fscanf(spatial, "%s", spCode);
			qcount ++;
			pNumber = readInt(fp);
			for(int i = 0;i < pNumber;i++) {
				tmpDist[i] = readInt(fp);
				tmpTime[i] = readInt(fp);
			}
		}
		//if(j == 5)
		//	cout << j << "\t" << queryWhenAt(spCode,qx,qy,pNumber) << endl;
		//system("pause");
	}
	fclose(fp);
	fclose(spatial);

	printf("%ld\n",clock()-start);

	FILE* fff = fopen("yao.txt","a+");
	fprintf(fff,"%ld\n",ttr);
	fclose(fff);
	//system("pause");
	return 0;
}
