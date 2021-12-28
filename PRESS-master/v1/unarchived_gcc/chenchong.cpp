#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

//����:
int segCount = 4;	//�߶�����
int hs[] = {1, 0, 1, 1};	//hs[i]Ϊ0��ʾ�߶�iΪˮƽ��1��ʾΪ��ֱ
double cx1[] = {1, 2.5, 4, 5};	//��i���߶ζ˵�����
double cy1[] = {0.5, 3, 3.5, 4.5};	//ˮƽ����˵����ꣻ��ֱ���¶ϵ�����
double cx2[] = {1, 3.5, 4, 5};	//��i���߶ε���һ���˵�����
double cy2[] = {1.5, 3, 4.5 ,5.5};
int cstreet[] = {1, 2, 2, 2};	//��i���߶������ֵ�ID

//�����
int ansCount;	//�����ĸ���

double getSlope(double x1, double y1, double x2, double y2)
{
	//δ���Ǵ�ֱ���!
	return (y2 - y1) / (x2 - x1);
}

void nonmateralized()
{
	if (segCount == 1)
	{
		ansCount = 1;
		return;
	}

	vector<int> turnSegs;
	turnSegs.push_back(0);

	double slopeUB = 1e10;
	double slopeLB = -1e10;

	//ȡ��һ���߶�Ϊ��ʼ��׼�߶�
	int baseSegID = 0;
	double r1, r2;	//��׼�߶ε���Ч����[r1,r2]
	if (hs[baseSegID] == 0)
	{
		r1 = cx1[0];
		r2 = cx2[0];
	}
	else
	{
		r1 = cy1[0];
		r2 = cy2[0];
	}
	
	for (int currSegID=1; currSegID!=segCount; ++currSegID)
	{
		//��ǰ�����߶����׼�߶β���ͬһstreet�����������ת��
		if (cstreet[currSegID] != cstreet[baseSegID])
		{
			int lastSegID = currSegID - 1;
			if (lastSegID != baseSegID)
			{
				turnSegs.push_back(lastSegID);
			}
			turnSegs.push_back(currSegID);

			baseSegID = currSegID;
			if (hs[currSegID] == 0)
			{
				r1 = cx1[currSegID];
				r2 = cx2[currSegID];
			}
			else
			{
				r1 = cy1[currSegID];
				r2 = cy2[currSegID];
			}

			slopeUB = 1e10;
			slopeLB = -1e10;
			continue;
		}

		//���㵱ǰ�߶����׼�߶ε�б�����½�
		double currSlopeUB;
		double currSlopeLB;
		if (hs[baseSegID] == 1)
		{
			if (hs[currSegID] == 0)
			{
				currSlopeUB = getSlope(cx1[baseSegID], r1, cx1[currSegID], cy1[currSegID]);
				currSlopeLB = getSlope(cx2[baseSegID], r2, cx2[currSegID], cy2[currSegID]);
			}
			else
			{
				currSlopeUB = getSlope(cx1[baseSegID], r1, cx2[currSegID], cy2[currSegID]);
				currSlopeLB = getSlope(cx2[baseSegID], r2, cx1[currSegID], cy1[currSegID]);
			}
		}
		else
		{
			if (hs[currSegID] == 0)
			{
				currSlopeUB = getSlope(r2, cy2[baseSegID], cx1[currSegID], cy1[currSegID]);
				currSlopeLB = getSlope(r1, cy1[baseSegID], cx2[currSegID], cy2[currSegID]);
			}
			else
			{
				currSlopeUB = getSlope(r2, cy2[baseSegID], cx2[currSegID], cy2[currSegID]);
				currSlopeLB = getSlope(r1, cy1[baseSegID], cx1[currSegID], cy1[currSegID]);
			}
			
		}

		//б�ʿռ���
		if ((currSlopeUB<slopeLB||currSlopeLB>slopeUB))	//��Ҫ����ת��
		{
			//������ֱ�߱�ʾ��y=kx+b����ʽ��ͨ����׼�߶ε������˵���b��ֵ
			double bUB, bLB;
			if (hs[baseSegID] == 1)
			{
				bUB = r1 - slopeUB * cx1[baseSegID];
				bLB = r2 - slopeLB * cx2[baseSegID];
			}
			else
			{
				bUB = cy2[baseSegID] - slopeUB * r2;
				bLB = cy1[baseSegID] - slopeLB * r1;
			}

			//�����һ���߶ε���Ч���䣬ת�۷��������һ���߶�
			int lastSegID = currSegID - 1;
			if (hs[lastSegID] == 0)
			{
				double xUB = (cy1[lastSegID] - bUB) / slopeUB;
				double xLB = (cy1[lastSegID] - bLB) / slopeLB;
				if (xUB < xLB)
				{
					swap(xUB, xLB);
				}
				double valid_x1 = (xLB > cx1[lastSegID])?xLB:cx1[lastSegID];
				double valid_x2 = (cx2[lastSegID] > xUB)?xUB:cx2[lastSegID];

				turnSegs.push_back(lastSegID);
				r1 = valid_x1;
				r2 = valid_x2;
			}
			else
			{
				double yUB = slopeUB * cx1[lastSegID] + bUB;
				double yLB = slopeLB * cx1[lastSegID] + bLB;
				if (yUB < yLB)
				{
					swap(yUB, yLB);
				}
				double valid_y1 = (yLB > cy1[lastSegID])?yLB:cy1[lastSegID];
				double valid_y2 = (yUB < cy2[lastSegID])?yUB:cy2[lastSegID];
				
				turnSegs.push_back(lastSegID);
				r1 = valid_y1;
				r2 = valid_y2;
			}

			//������ת�۵��߶���Ϊ�µĻ�׼�߶�
			baseSegID = lastSegID;
			currSegID--;
			slopeUB = 1e10;
			slopeLB = -1e10;
			
		}
		else
		{
			if (currSegID == segCount - 1)
			{
				turnSegs.push_back(currSegID);
				break;
			}

			if (currSlopeUB < slopeUB)
			{
				slopeUB = currSlopeUB;
			}
			if (currSlopeLB > slopeLB)
			{
				slopeLB = currSlopeLB;
			}
		}

	}// end for

	ansCount = turnSegs.size();

	cout << turnSegs.size() << endl;
	for (int i=0; i!=turnSegs.size(); ++i)
	{
		cout << turnSegs[i] << " ";
	}
	cout << endl;
}

int main()
{
	nonmateralized();

	return 0;
}