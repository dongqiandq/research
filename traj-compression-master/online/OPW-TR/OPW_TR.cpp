#include<iostream>
#include<stdio.h>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<math.h>
#include<algorithm>
#include <time.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
  time_t clock;
  struct tm tm;
  SYSTEMTIME wtm;
  GetLocalTime(&wtm);
  tm.tm_year   = wtm.wYear - 1900;
  tm.tm_mon   = wtm.wMonth - 1;
  tm.tm_mday   = wtm.wDay;
  tm.tm_hour   = wtm.wHour;
  tm.tm_min   = wtm.wMinute;
  tm.tm_sec   = wtm.wSecond;
  tm. tm_isdst  = -1;
  clock = mktime(&tm);
  tp->tv_sec = clock;
  tp->tv_usec = wtm.wMilliseconds * 1000;
  return (0);
}
#endif
using namespace std;
typedef struct Point{
	double lat;
	double lon;
	double time;
}Point;
vector<Point> points;

void gpsreader(string filename){
	ifstream fin(filename.c_str());
	if (!fin){
		cout << "open file error !" << endl;
		exit(0);
	}
	while (fin){
		string line;
		getline(fin, line);
		stringstream sin(line);
		double lat, lon, time;
		struct Point point;
		sin >> point.lat;
		sin >> point.lon;
		sin >> point.time;
		points.push_back(point);

	}
	points.pop_back();
	return;
}
double cacl_SED(Point s, Point m, Point e){
  double numerator = m.time - s.time;
  double denominator = e.time - s.time;
  double time_ratio = denominator == 0 ? 1 : numerator / denominator;
  double lat = s.lat + (e.lat - s.lat)*time_ratio;
  double lon = s.lon + (e.lon - s.lon)*time_ratio;
  double lat_diff = lat - m.lat;
  double lon_diff = lon - m.lon;
  return sqrt(lat_diff*lat_diff + lon_diff*lon_diff);
}

vector<int> OPW_TR(double eplision){
	int e = 0;
	int originalIndex = 0;
	vector<int> simplified;
	simplified.push_back(originalIndex);

	e = originalIndex + 2;
	while (e < points.size()){
		int i = originalIndex + 1;
		bool condOPW = true;
		while (i < e && condOPW){
			if (cacl_SED(points[originalIndex], points[i], points[e]) > eplision)
				condOPW = false;
			else
				i++;
		}
		if (!condOPW){
			originalIndex = i;
			simplified.push_back(originalIndex);
			e = originalIndex + 2;
		}
		else
			e++;
	}
	simplified.push_back(points.size() - 1);
	
	return simplified;
}

int main(int argc, char** args){
	string filename = "E:\\DongQian\\research\\traj-compression-master\\online\\OPW-TR\\20081023025304-0.csv";
	double epsilon = atof("8");//��ֵ���ַ�����ʽת��Ϊ������
	char* save_filename = "E:\\DongQian\\research\\traj-compression-master\\online\\OPW-TR\\result.csv";
	gpsreader(filename);
	struct timeval start, end;
	gettimeofday(&start, NULL);
	vector<int> cmp_index = OPW_TR(epsilon);
	gettimeofday(&end, NULL);
	long timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	FILE* s_fp = fopen(save_filename, "w+");
	for (int i = 0; i < cmp_index.size(); i++)
		fprintf(s_fp, "%lf %lf %lf\n", points[cmp_index[i]].lat, points[cmp_index[i]].lon, points[cmp_index[i]].time);
	fprintf(s_fp, "%lf\n", timeuse / 1000000.0);
	fclose(s_fp);
	/*gpsreader("C://Users//Ding//Desktop//20081023025304-0.plt");*/
	//for (int i = 0; i < cmp_index.size(); i++)
	//	cout << cmp_index[i] << " ";
	//cout << endl;
}
