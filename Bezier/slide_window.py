from math import *
import sys

sys.setrecursionlimit(10000)


def dfTodu(s):
    s = str.split('.')
    d = (float(str[0][:-2])) + (float(str[0][-2:] + str[1]) / 600000)
    return d


# 将文件中的经纬度提取出保存到数组
def createDataSet():
    rfile = open("2007-10-14-GPS.log")
    time = []
    jwd = []
    index = 0
    for line in rfile:
        strlist = line.split(' ', 8)
        time.append(strlist[2])
        jwd.append([dfTodu(strlist[5]), dfTodu(strlist[3]), index])
        index += 1
    rfile.close()
    return time, jwd


# 将压缩后的轨迹写入数组
def Write(enpArrayFilter):
    wfile = open('2016-10-28.log', "w")
    for i in enpArrayFilter:
        wfile.write(str(i) + "\n")


def geoDist(pA, pB):
    radLat1 = Rad(pA[0])
    radLat2 = Rad(pB[0])
    delta_lon = Rad(pB[1] - pA[1])
    top_1 = cos(radLat2) * sin(delta_lon)
    top_2 = cos(radLat1) * sin(radLat2) - sin(radLat1) * cos(radLat2) * cos(delta_lon)
    top = sqrt(top_1 * top_1 + top_2 * top_2)
    bottom = sin(radLat1) * sin(radLat2) + cos(radLat1) * cos(radLat2) * cos(delta_lon)
    delta_sigma = atan2(top, bottom)
    distance = delta_sigma * 6378137.0
    return distance


def Rad(d):
    return d * pi / 180.0


def distToSegment(pA, pB, pX):
    a = abs(geoDist(pA, pB))
    b = abs(geoDist(pA, pX))
    c = abs(geoDist(pB, pX))
    p = (a + b + c) / 2.0
    s = sqrt(abs(p * (p - a) * (p - b) * (p - c)))
    d = s * 2.0 / a
    return d


# 滑动窗口算法
# enpInitenpInit初始轨迹点数组
# enpArrayFilter过滤数组
# start窗口内的起始点
# end窗口内的终点
# cur当前点
# m目前误差最大的点
# DMax最大误差
# count
def SlideWindow(enpInit, enpArrayFilter, start, end, cur, m, DMax, count):
    if end < count:
        d_cur = distToSegment(enpInit[start], enpInit[end], enpInit[cur])  # 当前点到对应线段的距离
        d_m = distToSegment(enpInit[start], enpInit[end], enpInit[m])  # 当前点到对应线段的距离
        if d_cur > DMax or d_m > DMax:
            enpArrayFilter.append(enpInit[cur])  # 将当前点加入到过滤数组中
            start = cur
            cur = start + 1
            end = start + 2
            m = cur
            d_m = 0
            SlideWindow(enpInit, enpArrayFilter, start, end, cur, m, DMax, count)
        elif (d_cur <= DMax) and (d_m <= DMax):
            if d_cur > d_m:
                m = cur
            cur = end
            end = end + 1
            SlideWindow(enpInit, enpArrayFilter, start, end, cur, m, DMax, count)


# 平均误差计算函数
def getMeanDistError(enpInit, enpArrayFilter):
    sumDist = 0
    for i in range(1, len(enpArrayFilter)):
        start = enpArrayFilter[i - 1][2]
        end = enpArrayFilter[i][2]
        for j in range(start + 1, end):
            sumDist += distToSegment(enpInit[start], enpInit[end], enpInit[j])
    return sumDist / len(enpInit)


time, jwd = createDataSet()
start = 0
end = 2
cur = 1
DMax = 30
enpInit = jwd
m = 1
count = len(enpInit) - 1
enpArrayFilter = []
enpArrayFilter.append(enpInit[0])  # 获取第一个原始经纬度点坐标并添加到过滤后的数组中
SlideWindow(enpInit, enpArrayFilter, start, end, cur, m, DMax, count)
Write(enpArrayFilter)
c_enpInit = len(enpInit)
c_enpArrayFilter = len(enpArrayFilter)
print("压缩前的点数=" + str(c_enpInit))
print("压缩后的点数=" + str(c_enpArrayFilter))
print("压缩率=" + str(c_enpArrayFilter / c_enpInit * 100) + "%")
print("平均误差=" + str(getMeanDistError(enpInit, enpArrayFilter)))
