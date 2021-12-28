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
#define mymax(x,y) ((x) > (y) ? (x) : (y))
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
int ansList[100000][3];			//ѹ����Ĺ켣�����Խ�ѹ��
double acc[100000];

int SPNUM[40000];
int** SPLINK;
int MAP[2000000];

int save[10000];

int road1Number = 0;
int comRoad1[100000];
int road2Number = 0;
int comRoad2[100000];

bool scan[50000];

void writePath(int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);
	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[MAP[node1]][MAP[node2]];
		if (save[count] == -1) return;
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i)
		//printf("%d ",save[i]);
		fout << save[i] << " ";
}

//���·
void loadSPAll2() {
	int t;
	int s;
	int f;
	int c = 0;

	SPLINK = new int*[30000];

	for (t = 0; t < 30000; ++t){
		SPLINK[t] = new int[30000];
	}
	for (t = 0; t < 30000; ++t)
		for (s = 0; s < 30000; ++s) 
			SPLINK[t][s] = -1;
	char fileName[100]="F:/�¼��¹켣/ShortestPath/SP00.txt";
	
	//////
	for (f = 0; f < 21; ++f) {
		fileName[29] = (f / 10) + 48;
		fileName[30] = (f % 10) + 48;
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
				SPLINK[MAP[id]][MAP[q]] = readInt(file);
			}
		}
		fclose(file);
	}
	
}

//�ֵ�
int streetNumber;
int road2Street[2000000][2];	//i��·�εĽֵ���źͽֵ�ƫ��
int street2road[50000][200];	//i�Žֵ�j��ƫ������Ӧ·��

void loadStreet() {
	memset(street2road,255,sizeof(street2road));
	memset(road2Street,255,sizeof(road2Street));
	FILE* file = fopen("./Street/Street.txt","r");
	while (1) {
		int number;
		fscanf(file,"%d",&number);
		if (number == -1) break;

		int q = 0;
		while (1) {
			int p;
			fscanf(file,"%d",&p);
			street2road[number][q] =p;
			road2Street[p][0]=number;
			road2Street[p][1] = q;
			++q;
			if (p ==-1) break;
		}
	}
	fclose(file);
}

int cmp(const void* a, const void* b) {
	return (*(int*)a) - (*(int*)b);
}

//Ƶ���켣
int fre_pre[200000], fre_last[60000], fre_other[200000][2];//0:�ĸ��ӹ켣 1���ӹ켣�еڼ�����
int fre_number, item = 0;

//0�����ĸ��ӹ켣 1����ʼƫ�� 2����ֹƫ��
int tmp_item[200000][3],tmp_mm[200000][3];
int tmp_num, tmp_num2;
int fre[10000][200];
int writeList[1000000];

void loadFrequency() {
	FILE* file = fopen("./Subtrajectory/sub_final.txt","r");
	memset(fre_last,255,sizeof(fre_last));
	fscanf(file,"%d",&fre_number);
	int i;
	for (i = 0; i < fre_number; ++i) {
		int num;
		fscanf(file,"%d",&num);
		int j,e;
		for (j = 0; j < num; ++j) {
			fscanf(file, "%d",&e);
			fre[i][j] = e;
			fre_other[item][0] = i;
			fre_other[item][1] = j;
			fre_pre[item] = fre_last[e];
			fre_last[e] = item;
			++item;
		}
	}
	
	fclose(file);
}

//��������
int timing[50000];
int dist[50000];
int validing[50000];
double timeStamp[50000];
double timeMap[50000];

int main(int argc, char *argv[])
{
	
	if (argc < 3) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	//char sss[20] = "";
	
	loadData(argv[1]);
	
	//===============
	int nodeUseful = 0;
	int i;

	for (i = 0; i < nodeNumber; ++i) {
		nodeInRegion[i] = 1;
		MAP[i] = nodeUseful;
		++nodeUseful;
	}
	
	//============��������ļ�=====================
	//ȫ�ڴ��������·
	loadSPAll2();
	cout << "SP-";
	//����ֵ����
	loadStreet();
	cout << "ST-";
	//����Ƶ���켣
	loadFrequency();
	
	cout << "FR"<< endl;

	long bbb = clock();

	int tolerance, tolerance2;

	for (tolerance = 9000; tolerance < 100000; tolerance += 10000) {
		for (tolerance2 = 9000; tolerance2 < 100000; tolerance2 += 10000){
	//2 ��ȡ�켣������
	FILE* file = fopen(argv[2],"rb");	//����
	
	
	int tjNumber = readInt(file);	//�켣����
	cout << tjNumber << endl;

	//FILE* temporal = fopen(argv[2],"rb");
	
	//readInt(temporal);	//�켣����

	FILE* stamp = fopen(argv[3],"r");	//time stamp of each intersection

	//ʱ��ͬ��·���������������Ͻ�
	char* ttt = argv[4];
;	/*
	int tolerance = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance = tolerance * 10 + ttt[i] - 48;
	printf("%d\n",tolerance);
	
	//·��ͬ��ʱ�����
	ttt = argv[5];
	int tolerance2 = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance2 = tolerance2 * 10 + ttt[i] - 48;
	printf("%d\n",tolerance2);
	*/

	//FILE* result = fopen(argv[2],"wb");

	//writeInt(result,tjNumber);
	int total = 0, compress = 0;

	double diff = 0;

	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(file);	//·������
		memset(validing, 0, sizeof(validing));
		//����������·
		int j;
		for (j = 0; j < spNumber; ++j) {
			path[j] = readInt(file);	//·���ϵı�
			acc[j] = readDouble(file);
			if (j > 0) acc[j] += acc[j-1];
		}
		int dddd;
		for (j = 0; j < spNumber; ++j) {
			fscanf(stamp,"%d",&dddd);
			fscanf(stamp,"%lf",&timeStamp[j]);
		}
		//�𰸴�С
		ansSize = 0;
		
		//���ַ�ʽ�Ŀ��Լ�����Ч�ı��
		int sp = 1,st = 1, fr =1;	//���·���ֵ���Ƶ������
		int valid = 3;	//���м��ַ�ʽ��Ч
		
		//��һ����·��ƫ��
		int nowStreet = road2Street[path[0]][0];	//��ʼ��·
		int nowBias = road2Street[path[0]][1];	//��ʼƫ��
		
		//��һ����·��Ƶ������
		tmp_num = 0;
		int p = fre_last[path[0]];
		while (p > -1) {
			tmp_item[tmp_num][0] = fre_other[p][0];
			tmp_item[tmp_num][1] = fre_other[p][1];
			tmp_item[tmp_num][2] = fre_other[p][1];
			++tmp_num;
			p = fre_pre[p];
		}
		if (tmp_num == 0) {fr = 0;--valid;}

		//��һ����·����һ���˵�
		int node = other[path[0]];	//�켣��ʼ��
		
		//��ǰ�ĵ�·
		int nowEdge;
		//ѹ����ǰ����ε���ʼ��·
		int preEdge = 0;
		
		for (nowEdge = 1; nowEdge < spNumber; ++nowEdge) {
			
			//3��Ƶ���
			if (fr == 1) {
				//��һ�������
				tmp_num2 = 0;
				int p = fre_last[path[nowEdge]];
				while (p > -1) {
					tmp_mm[tmp_num2][0] = fre_other[p][0];
					tmp_mm[tmp_num2][2] = fre_other[p][1];
					++tmp_num2;
					p = fre_pre[p];
				}
				//�����
				int tp = 0;
				int l,r;
				for (r = 0;r < tmp_num2; ++r) 
					for (l = 0; l < tmp_num; ++l)
						if (tmp_item[l][0] == tmp_mm[r][0] && tmp_item[l][2] + 1 == tmp_mm[r][2]) {
							tmp_mm[tp][0] = tmp_mm[r][0];
							tmp_mm[tp][1] = tmp_item[l][1];
							tmp_mm[tp][2] = tmp_mm[r][2];
							++tp;
							break;
						}
				//���û�м���άϵ���ˣ���ô�����һ��·��
				if (tp == 0) {
					fr = 0;--valid;
					
					if (valid == 0) {
						validing[preEdge] = 1;
						//ansList[ansSize][0] = 3;
						//ansList[ansSize++][1] = preEdge;
						//ansList[ansSize++][1] = ((tmp_item[0][0] & 8191) << 14) | ((tmp_item[0][1] & 127) << 7) | (tmp_item[0][2] & 127);
						preEdge = nowEdge;
						
						//��ʼ��=======
						//���ַ�ʽ�Ŀ��Լ�����Ч�ı��
						sp = st = fr =1;	//���·���ֵ���Ƶ������
						valid = 3;	//���м��ַ�ʽ��Ч
						//��һ����·��ƫ��
						nowStreet = road2Street[path[nowEdge]][0];	//��ʼ��·
						nowBias = road2Street[path[nowEdge]][1];	//��ʼƫ��
						//��һ����·��Ƶ������
						tmp_num = 0;
						int p = fre_last[path[nowEdge]];
						while (p > -1) {
							tmp_item[tmp_num][0] = fre_other[p][0];
							tmp_item[tmp_num][1] = fre_other[p][1];
							tmp_item[tmp_num][2] = fre_other[p][1];
							++tmp_num;
							p = fre_pre[p];
						}
						if (tmp_num == 0) {fr = 0;--valid;}

						//��һ����·����һ���˵�
						node = other[path[nowEdge]];	//�켣��ʼ��
						//=============
						
						continue;
					}
					
				}else {	//���Լ����ӳ�
					tmp_num = tp;
					for (j = 0; j < tmp_num; ++j) {
						tmp_item[j][0] = tmp_mm[j][0];
						tmp_item[j][1] = tmp_mm[j][1];
						tmp_item[j][2] = tmp_mm[j][2];
					}
				}
			}

			//1�����·�ж�
			if (sp == 1) {	//���·����Ч
				int node2 = other[path[nowEdge]];
				//�������·
				if (SPLINK[MAP[node]][MAP[node2]]!=path[nowEdge] || other[path[nowEdge-1]]!=thisSide[path[nowEdge]]) {
					sp = 0;--valid;
					//ȫ�����������������
					if (valid == 0) {
						//ansList[ansSize][0] = 1;//ansList[ansSize][1] = path[preEdge];ansList[ansSize++][2] = path[nowEdge - 1];
						//ansList[ansSize++][1] = preEdge;
						validing[preEdge] = 1;
						preEdge = nowEdge;
						
						//��ʼ��=======
						//���ַ�ʽ�Ŀ��Լ�����Ч�ı��
						sp = st = fr =1;	//���·���ֵ���Ƶ������
						valid = 3;	//���м��ַ�ʽ��Ч
						//��һ����·��ƫ��
						nowStreet = road2Street[path[nowEdge]][0];	//��ʼ��·
						nowBias = road2Street[path[nowEdge]][1];	//��ʼƫ��
						//��һ����·��Ƶ������
						tmp_num = 0;
						int p = fre_last[path[nowEdge]];
						while (p > -1) {
							tmp_item[tmp_num][0] = fre_other[p][0];
							tmp_item[tmp_num][1] = fre_other[p][1];
							tmp_item[tmp_num][2] = fre_other[p][1];
							++tmp_num;
							p = fre_pre[p];
						}
						if (tmp_num == 0) {fr = 0;--valid;}

						//��һ����·����һ���˵�
						node = other[path[nowEdge]];	//�켣��ʼ��
						//=============
						
						continue;
					}
				}
			}
		
			//2���ֵ��ж�
			if (st == 1) {
				++nowBias;
				if (street2road[nowStreet][nowBias]!=path[nowEdge]) {
					st = 0;--valid;
					//ȫ�����������������
					if (valid == 0) {
						//ansList[ansSize][0] = 2;//ansList[ansSize][1] = path[preEdge];ansList[ansSize++][2] = nowEdge - preEdge;
						//ansList[ansSize++][1] = preEdge;
						validing[preEdge] = 1;
						preEdge = nowEdge;
						
						//��ʼ��=======
						//���ַ�ʽ�Ŀ��Լ�����Ч�ı��
						sp = st = fr =1;	//���·���ֵ���Ƶ������
						valid = 3;	//���м��ַ�ʽ��Ч
						//��һ����·��ƫ��
						nowStreet = road2Street[path[nowEdge]][0];	//��ʼ��·
						nowBias = road2Street[path[nowEdge]][1];	//��ʼƫ��
						//��һ����·��Ƶ������
						tmp_num = 0;
						int p = fre_last[path[nowEdge]];
						while (p > -1) {
							tmp_item[tmp_num][0] = fre_other[p][0];
							tmp_item[tmp_num][1] = fre_other[p][1];
							tmp_item[tmp_num][2] = fre_other[p][1];
							++tmp_num;
							p = fre_pre[p];
						}
						if (tmp_num == 0) {fr = 0;--valid;}

						//��һ����·����һ���˵�
						node = other[path[nowEdge]];	//�켣��ʼ��
						//=============
						
						continue;
					}	
				}
			
			}
		}




		validing[preEdge] = 1;
		
		//spNumber = readInt(temporal);	//·������
		double upper, downer;

		validing[0] = validing[spNumber - 1] = 1;
		
		for (j = 0; j < spNumber; ++j) {
			dist[j] = (int)acc[j];				//vertical
			if (j > 0 && dist[j] < dist[j-1]) dist[j] = dist[j-1];
			timing[j] = (int) timeStamp[j];		//horizon
		}
		qsort(timing,spNumber,sizeof(int),cmp);
		for (j = 1; j < spNumber; ++j)
			if (timing[j] <= timing[j-1]) timing[j] = timing[j-1] + 1;
		
		for (j = 0; j < spNumber - 1; ++j)
			if (timing[j] >= timing[j+1] || dist[j] > dist[j+1]) cout << "Fuck!" << endl;

		int preNode = 0; upper = 1e100, downer = -1e100;
		while (preNode < spNumber - 1) {
			//printf("%d %d\t",preNode, spNumber);
			
			int flag = 1;
			for (j = preNode + 1; j < spNumber; ++j) {
				if (timing[j] > timing[preNode]) {
					double d = (double)(dist[j] - dist[preNode]) / (timing[j] - timing[preNode]);
					if (d < downer || d > upper) {
						flag = 0;break;
					}
					d = (double)(dist[j] - dist[preNode] - tolerance) / (timing[j] - timing[preNode]);
					if (d > downer) downer = d;
					d = (double)(dist[j] - dist[preNode] + tolerance) / (timing[j] - timing[preNode]);
					if (d < upper) upper = d;
				
					d = (double)(dist[j] - dist[preNode])/ (timing[j] - timing[preNode] + tolerance2);
					if (d > downer) downer = d;
					if (timing[j] - timing[preNode] > tolerance2) {
						d = (double)(dist[j] - dist[preNode])/ (timing[j] - timing[preNode] - tolerance2);
						if (d < upper) upper = d;
					}
				}
				/*
				if (upper < downer) {
					flag = 0;
					break;
				}*/
			}
			if (!flag) {
				validing[j-1] = 1;
				preNode = j-1;
				upper = 1e100; downer = -1e100;
			}else break;
		}

		int ansSize = 0;
		for (j = 0; j < spNumber; ++j) if (validing[j]) ++ansSize;
		int preOne = 0,nextOne;
		for (j = 0; j < spNumber; ++j) {
			//timeMap[j] = timeStamp[j];
			//if (timeStamp[j] >= timeStamp[preNode]) {
				if (validing[j]) {
					timeMap[j] = timing[j];
					preOne = j;
					int k;
					for (k = j+1;k < spNumber; ++k) if (validing[k]) {
						nextOne = k;
						break;
					}
				}else {
					timeMap[j] = timing[preOne] + (double)(timing[nextOne] - timing[preOne]) *(dist[j]-dist[preOne])/(dist[nextOne] - dist[preOne]);
				}
				//if ( fabs(timeMap[j] - timing[j]) > 10000)
				//printf("%.4lf\t",fabs(timeMap[j]-timing[j]));
				//diff += fabs(timeMap[j] - timing[j]);
			//}
		}
		
		for (j = 0; j < spNumber - 1; ++j) {
			double d1 = fabs((double)timing[j+1] - timing[j]);
			double d2 = fabs((double)timeMap[j+1] - timeMap[j]);
			diff += fabs(d1 - d2) / mymax(d1, d2);
		}
		
		total += spNumber;
		compress += ansSize;
		//printf("%.8lf\n",(double)ansSize/spNumber);
		/*for (j = 0; j < spNumber; ++j) if (validing[j]) {
			printf("%d\t",path[j]);
			//writeInt(result,dist[j]);
			//writeInt(result,timing[j]);
		}
		puts("");*/

		//������
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	FILE*fpp =  fopen(argv[6],"a+");
	fprintf(fpp,"%.4lf\t%.4lf\n", (double)compress/total, 1 - diff / (total - tjNumber)/2);
	fclose(file);
	fclose(fpp);
	fclose(stamp);
	}
	}
	
	//FILE* fpp =fopen(argv[6],"a+");
	//fprintf(fpp,"%ld\n",clock() - bbb);
	//fclose(fpp);
	return 0;
}
