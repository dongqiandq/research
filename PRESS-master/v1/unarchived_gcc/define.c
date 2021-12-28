#define DATA_INPUT_FILE "./input_01.txt"				//���������ļ�
#define N_MAX_NODE 1000000
#define EARTHRAD 6372797												//����뾶��ƽ����
#define PI180 0.0174532925199432957694

double BETA_ARR[31]={
	0,
	0.490376731, 0.82918373, 1.24364564, 1.67079581, 2.00719298,
	2.42513007, 2.81248831, 3.15745473, 3.52645392, 4.09511775,
	4.67319795, 5.41088180, 6.47666590, 6.29010734, 7.80752112,
	8.09074504, 8.08550528, 9.09405065, 11.09090603, 11.87752824,
	12.55107715, 15.82820829, 17.69496773, 18.07655652, 19.63438911,
	25.40832185, 23.76001877, 28.43289797, 32.21683062, 34.56991141
};

long double dp[5000][500];								//��̬�滮����
int dpSize[5000];									//ÿ���㸽���ж��ٱ�
int dpEdge[5000][500];								//��̬�滮��¼�ߵ�Id
double dpLeft[5000][500];							//��̬�滮ʣ��
double dpLen[5000][500];							//��̬�滮ÿ���ߵļ��μ��㳤��
int dpPre[5000][500];								//��̬�滮��¼·��

long double dp2[5000][500];								//��̬�滮����
int dpSize2[5000];									//ÿ���㸽���ж��ٱ�
int dpEdge2[5000][500];								//��̬�滮��¼�ߵ�Id
double dpLeft2[5000][500];							//��̬�滮ʣ��
double dpLen2[5000][500];							//��̬�滮ÿ���ߵļ��μ��㳤��
int dpPre2[5000][500];								//��̬�滮��¼·��



/**
 * 2-dimensional array stores the coordinary nodes
 */
double *coordNode;

/**
 * The beginning index of the line segment of each road segment
 */
int *edgeStart;

/**
 * Coordinate sequence of each road segment of the road network
 */
double *coordNet;

/**
 * The Up, Bottom, Left and Right Bolder of the coordinate system
 */
double maxLat = -1e50, minLat = 1e50, maxLong = -1e50, minLong = 1e50;

/**
 * the Number of Nodes, Edges, Line Segment
 */
int nodeNumber = 0;
int edgeNumber = 0;
int lineNumber = 0;



/**
 * Adjacency list
 */
int *pre, *other, *thisSide, *last, *cost, *typeList;

///**
// * list of Length
// */
double *len;




int originNodeNumber=1000000;						//ԭʼ�ڵ����
int originEdgeNumber=2000000;						//ԭʼ�߸���
int originLineNumber=20000000;						//ԭʼ�߶θ���

/**
 * the Number of RoadType
 */
int roadTypeNumber = 15;

/**
 * Sampling Rate
 */
int rate[1000];

/**
 * Time Stamp Sequence of one test file
 */
int timeSequence[200][10000];

/**
 * the number of Time Stamps in one test file
 */
int nowTime[1000];

/**
 * Sampling point coordinate sequence
 * (the nth sampling point's Lat is [2 * n], Lng is [2 * n + 1])
 */
double nodeSequence[200][20000];

/**
 * Adjacency list for Net Grid
 */
int indexPre[20000000], indexOther[20000000], indexLast[55000000];
int indexPre2[20000000], indexOther2[20000000], indexLast2[55000000];

int crossSize=0;									//��������������
int crossSize2=0;									//��������������

/**
 * Answer List
 */
int ans[200][10000];

//Things below are thread related
int nodeInsertedToIndex[1000000];					//ÿ�����Ƿ񱻲������ڽӱ�
int indexCreateFlag=0;
int doneFlag=0;
int inputLoaded[200];
int dataNumber=0;
int gridFlag=0;
char inPathSet[1000][1000];
char outPathSet[1000][1000];

/**
 * DijkstraNode Array Index Array
 */
int DijkstraIndex[N_MAX_NODE];
int DijkstraIndex2[N_MAX_NODE];
