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
int ansList[100000][2];			//ѹ����Ĺ켣�����Խ�ѹ��

int SPNUM[40000];
int** SPLINK;
int MAP[2000000];

int save[10000];
void writePath(int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);
	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[MAP[node1]][MAP[node2]];
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

//Ƶ���켣
int fre_pre[200000], fre_last[60000], fre_other[200000][2];//0:�ĸ��ӹ켣 1���ӹ켣�еڼ�����
int fre_number, item = 0;

//0�����ĸ��ӹ켣 1����ʼƫ�� 2����ֹƫ��
int tmp_item[200000][3],tmp_mm[200000][3];
int tmp_num, tmp_num2;
int fre[10000][200];

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

int main(int argc, char *argv[])
{
	
	if (argc < 3) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	char sss[20] = "";
	
	loadData(sss);
	
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
	//����ֵ����
	loadStreet();
	//����Ƶ���켣
	loadFrequency();
	
	//2 ��ȡ�켣������
	FILE* file = fopen(argv[1],"rb");
	
	
	int tjNumber = readInt(file);	//�켣����
	cout << tjNumber << endl;

	FILE* result = fopen(argv[2],"wb");

	writeInt(result,tjNumber);
	
	for (i = 0; i < 100/*tjNumber*/; ++i) {
		int spNumber = readInt(file);	//·������

		//����������·
		int j;
		for (j = 0; j < spNumber; ++j)
			path[j] = readInt(file);	//·���ϵı�
		
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
		
		//��һ����·����һ���˵�
		int node = other[path[0]];	//�켣��ʼ��
		
		//��ǰ�ĵ�·
		int nowEdge;
		//ѹ����ǰ����ε���ʼ��·
		int preEdge = 0;
		
		for (nowEdge = 1; nowEdge < spNumber; ++nowEdge) {
			
			//1�����·�ж�
			if (sp == 1) {	//���·����Ч
				int node2 = other[path[nowEdge]];
				//�������·
				if (SPLINK[MAP[node]][MAP[node2]]!=path[nowEdge]) {
					sp = 0;--valid;
					//ȫ�����������������
					if (valid == 0) {
						ansList[ansSize][0] = 1;ansList[ansSize++][1] = preEdge;
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
						ansList[ansSize][0] = 2;ansList[ansSize++][1] = preEdge;
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
						//��һ����·����һ���˵�
						node = other[path[nowEdge]];	//�켣��ʼ��
						//=============
						
						continue;
					}	
				}
			
			}
			
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
						ansList[ansSize][0] = 3;
						ansList[ansSize++][1] = (1 << 31) | ((tmp_item[0][0] % 32768) * 65536) + ((tmp_item[0][1] % 255) * 255) + (tmp_item[0][2] % 255);
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
		
		
		}
		
		for (j = 0; j < ansSize; ++j) printf("[%d %d]\t",ansList[j][0],ansList[j][1]);
		puts("");
		
		//��ѹ�켣����
		//--
		//for (j = 0; j <=ansSize; ++j){
		//	if (j > 0 && ansList[j-1][0] == 1) {
		//		int node1 = other[ansList[j-1][1]],node2;
		//		if (ansList[j][0] !=1 ) node2 = ansList[j][1];else node2 = thisSide[ansList[j][1]];
		//		if (node1!=node2) {
		//			//loadSP(node1);
		//			writePath(node1,node2);
		//		}
		//	}
		//	if (j<ansSize){
		//		if (ansList[j][0] == 1) {
		//			//printf("%d ",ansList[j][1]);
		//			fout << ansList[j][1]<<" ";
		//		}else if (ansList[j][0] == 0)	{
		//			int node1,node2;
		//			node1 = ansList[j][1];
		//			if (ansList[j+1][0] != 1) node2 = ansList[j+1][1];
		//			else node2 = thisSide[ansList[j+1][1]];
		//			//loadSP(node1);
		//			writePath(node1,node2);
		//		}
		//	}
		//}
		

		//printf("\n");
		//fout<<endl;
		
		//������
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	fclose(result);
	fclose(file);
	system("pause");
	
	return 0;
}
