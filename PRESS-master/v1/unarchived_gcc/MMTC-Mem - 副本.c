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
	
	//cout << nodeUseful << endl;
	
	//============��������ļ�=====================
	//ȫ�ڴ��������·
	loadSPAll2();
	
	
	
	//2 ��ȡ�켣������
	FILE* file = fopen(argv[1],"rb");
	
	
	int tjNumber = readInt(file);	//�켣����
	cout << tjNumber << endl;

	FILE* result = fopen(argv[2],"wb");

	writeInt(result,tjNumber);
	
	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(file);	//·������
		
		//--
		//printf("%d\n",spNumber);
		//ansSize = 0;
		
		int j;
		for (j = 0; j < spNumber; ++j)
			path[j] = readInt(file);	//·���ϵı�
		
		//--
		//for (j = 0; j < spNumber; ++j)
		//printf("%d ",path[j]);	//·���ϵı�
		//{
		//	fout << path[j] << " ";
		//}
		//printf("\n");
		//fout<<endl;
		
		
		int lastOpt = 0;
		int node = thisSide[path[0]];	//�켣��ʼ��
		int nowEdge = 0;
		while (nowEdge < spNumber) {
			//���뵱ǰλ�õ����·
			//loadSP(node);
			
			//�����һ���Ͳ�������
			//--
			//printf("\n%d %d\n",spPre[other[path[nowEdge]]],path[nowEdge]);
			if (SPLINK[MAP[node]][MAP[other[path[nowEdge]]]]!=path[nowEdge]) {
				//printf("\n%d %d %d %d\n",spPre[other[path[nowEdge]]],path[nowEdge],node,thisSide[spPre[other[path[nowEdge]]]]);
				//ֱ����ԭ·������
				//printf("01:%d; ",path[nowEdge]);
				//fout << "01:"<< path[nowEdge] <<"; ";
				//ansList[ansSize][0] = 1;
				//ansList[ansSize][1] = path[nowEdge];
				//++ansSize;
				//{ʵ�ֶ�����д}
				writeInt(result,(1 << 30) | path[nowEdge]);
				lastOpt = 1;
				//��һ����
				node = other[path[nowEdge]];
				++nowEdge;
			}else{
				//�ҵ����·���������
				while (nowEdge < spNumber && SPLINK[MAP[node]][MAP[other[path[nowEdge]]]]==path[nowEdge]) 
					++nowEdge;
				--nowEdge;
				if (lastOpt == 0){
					//�������·
					//printf("00:%d; ",node);
					//fout<<"00:"<<node<<": ";
					//ansList[ansSize][0] = 0;
					//ansList[ansSize][1] = node;
					//++ansSize;
					//{ʵ�ֶ�����д}
					writeInt(result,node);
				}

				lastOpt = 0;
				node = other[path[nowEdge]];
				++nowEdge;
			}
		}
		//�������������Id
		//printf("10:%d;\n",other[path[spNumber-1]]);
		//fout << "10:" << other[path[spNumber-1]] << ":"<<endl;
		//ansList[ansSize][0] = 2;
		//ansList[ansSize][1] = other[path[spNumber-1]];
		//{ʵ�ֶ�����д}
		writeInt(result,(1 << 31) | other[path[spNumber-1]]);
		
		
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
	//============Part 2 ��Streetѹ���켣=====================
	
	//============Part 3 ��Ƶ���ӹ켣ѹ���켣=================
	
	//============Part 4 �ۺ�����ѹ���켣��DP��С���=========

	
	
	
	return 0;
}
