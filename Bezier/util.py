import entity
import my_vector
import vector_sequence
import datetime
import time
import csv
import codecs
import folium
import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.spatial.distance import pdist
import sys
import douglas_peuker

sys.setrecursionlimit(5000)


def readGeoLifeDir(path, count):
    """
    批量读取文件夹下的所有文件
    :param path: 文件夹路径（截止到文件夹名）
    :param count: 统计总共的轨迹点个数
    :return:
    """
    count = 0
    files = os.listdir(path)  # 得到文件夹下的所有文件名称
    datas = []
    for file in files:  # 遍历文件夹
        position = path + '\\' + file  # 构造绝对路径，"\\"，其中一个'\'为转义符
        with open(position, "r", encoding='utf-8') as f:  # 打开文件
            for line in f.readlines()[6:]:
                line = line.strip()
                data = entity.Data()
                data.id = count
                count += 1
                data.longitude = float(line.split(",")[1])
                data.latitude = float(line.split(",")[0])
                data.gpsTime = string_datetime(line.split(",")[5] + " " + line.split(",")[6])
                datas.append(data)
            f.close()
    return datas, count


def readGeoLife(path, count):
    """
    # 读取一个GeoLife文件的datas
    :param path: 文件路径
    :param count: GeoLife数据集个数（id）
    :return: 轨迹点集合，当前count累加个数
    """
    fd = open(path, 'r')
    datas = []
    for line in fd.readlines()[6:]:
        line = line.strip()
        data = entity.Data()
        data.id = count
        count += 1
        data.longitude = float(line.split(",")[1])
        data.latitude = float(line.split(",")[0])
        data.gpsTime = string_datetime(line.split(",")[5] + " " + line.split(",")[6])
        datas.append(data)
    fd.close()
    return datas, count


def readDguiji(path):  # 没有时间无法使用
    # 读取Dguiji数据集的数据封装成集合
    point_list = []
    fd = open(path, 'r')
    for line in fd:
        line = line.strip()
        id = int(line.split(",")[0])
        longitude = float(line.split(",")[1])
        latitude = float(line.split(",")[2])
        data = entity.Data()
        data.id = id
        data.longitude = longitude
        data.latitude = latitude
        data.gpsTime = "2013-5-31 23:25:59"  # 不起实际作用，只是防止报错
        data.speed = 100
        point_list.append(data)
    fd.close()
    return point_list


def get_compression_data(sequence):
    """
    得到一辆车压缩后的数据点
    :param sequence:
    :return:
    """
    vectors = sequence.vectors
    datas = [vectors[0]]
    for i in range(0, len(vectors)):
        datas.append(vectors[i])


def output_compression_data(file_name, sequence):
    """
    将一辆车压缩后的矢量起止点存储到文件中，包括第一个矢量的起止点和后面矢量的终点
    :param file_name: 输出的文件名
    :param sequence: 矢量序列
    :return:
    """
    vectors = sequence.vectors
    datas = [vectors[0].dataStart.trans_csv()]
    for i in range(0, len(vectors)):
        datas.append(vectors[i].dataEnd.trans_csv())
    file_csv = codecs.open(file_name, 'w+', 'utf-8')  # 追加
    writer = csv.writer(file_csv, delimiter=',', quotechar=',', quoting=csv.QUOTE_MINIMAL)
    for data in datas:
        writer.writerow(data)


def read_gps_data(path):
    P = pd.read_csv(path, header=None, dtype=np.str).values  # 读取csv文件，输出为narray
    locations = P[:, 2:4].tolist()  # narray转换成list
    for i in range(0, len(locations)):
        temp = float(locations[i][0])
        locations[i][0] = float(locations[i][1])
        locations[i][1] = temp
    return locations


def draw_gps(locations, output_path, file_name):
    """
    绘制gps轨迹图
    :param locations: list, 需要绘制轨迹的经纬度信息，格式为[[lat1, lon1], [lat2, lon2], ...]
    :param output_path: str, 轨迹图保存路径
    :param file_name: str, 轨迹图保存文件名
    :return: None
    """
    m = folium.Map(locations[0], zoom_start=15, attr='default')  # 中心区域的确定

    folium.PolyLine(  # polyline方法为将坐标用线段形式连接起来
        locations,  # 将坐标点连接起来
        weight=3,  # 线的大小为3
        color='orange',  # 线的颜色为橙色
        opacity=0.8  # 线的透明度
    ).add_to(m)  # 将这条线添加到刚才的区域m内

    # 起始点，结束点
    folium.Marker(locations[0], popup='<b>Starting Point</b>').add_to(m)
    folium.Marker(locations[-1], popup='<b>End Point</b>').add_to(m)

    m.save(os.path.join(output_path, file_name))  # 将结果以HTML形式保存到指定路径


def draw_map_gps(locations, output_path, file_name):
    """
    绘制gps轨迹图
    :param locations: list, 需要绘制轨迹的经纬度信息，格式为[[lat1, lon1], [lat2, lon2], ...]
    :param output_path: str, 轨迹图保存路径
    :param file_name: str, 轨迹图保存文件名
    :return: None
    """
    m = folium.Map(locations[0], zoom_start=30,
                   tiles="http://webrd02.is.autonavi.com/appmaptile?lang=zh_cn&size=1&scale=1&style=7&x={x}&y={y}&z={z}",
                   # 设置高德底图
                   attr='default')  # 中心区域的确定
    for i in range(0, len(locations)):
        folium.CircleMarker(
            location=locations[i],
            radius=5,
            popup='popup',
            color='#DC143C',  # 圈的颜色
            fill=True,
            fill_color='#6495ED'  # 填充颜色
        ).add_to(m)
    folium.PolyLine(  # polyline方法为将坐标用实线形式连接起来
        locations,  # 将坐标点连接起来
        weight=4,  # 线的大小为4
        color='red',  # 线的颜色为红色
        opacity=0.8,  # 线的透明度
    ).add_to(m)  # 将这条线添加到刚才的区域m内
    # 起始点，结束点
    folium.Marker(locations[0], popup='<b>Starting Point</b>').add_to(m)
    folium.Marker(locations[-1], popup='<b>End Point</b>').add_to(m)
    m.save(os.path.join(output_path, file_name))  # 将结果以HTML形式保存到指定路径


def line_k(data_a, data_b):
    """
    求data_a和data_b之间的斜率（y2-y1）/(x2-x1)
    :param data_a:
    :param data_b:
    :return:
    """
    if data_b.longitude - data_a.longitude != 0:
        return (data_b.latitude - data_a.latitude) / (data_b.longitude - data_a.longitude)


# 字符串转日期
def string_datetime(s):
    return datetime.datetime.strptime(s, "%Y-%m-%d %H:%M:%S")


# 日期转时间戳
def datetime_time(dt):
    return time.mktime(dt.timetuple())


# 日期转字符串
def date_string(dt):
    return dt.strftime("%Y-%m-%d %H:%M:%S")


# 时间戳转日期
def time_datetime(t):
    s = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(t))
    return string_datetime(s)


# 绘制矢量序列的轨迹路径


class Util:

    # /**
    #  * 对新来的数据点进行带入，判断求得的值与实际值的差 与阈值的关系
    #  * @param newData：新数据点
    #  * @param sequence：矢量序列，可以得到最新的矢量（最后一个）
    #  * @param distX：横坐标阈值
    #  * @param distY：纵坐标阈值
    #  * @return :如果两者的差大于阈值，则返回TRUE，表示需要更新，否则返回FALSE，表示不需要更新，继续延续当前矢量，更改截止时间
    #  */

    def judge(self, newData, sequence, dist):
        sign = False
        sequence = vector_sequence.VectorSequence()
        index = len(sequence.vectors) - 1
        vector = sequence.vectors[index]
        start = vector.dataStart
        end = vector.dataEnd
        dist1 = 0.0
        dist2 = 0.0
        zi = datetime_time(newData.gpsTime) - datetime_time(start.gpsTime)
        mu = datetime_time(end.gpsTime) - datetime_time(start.gpsTime)
        t = zi / mu
        funValueX = 0.0
        funValueY = 0.0
        if vector.isFirstBessel:  # 如果当前的矢量活动是由一阶贝塞尔生成的，将新来的数据带入一阶判断
            pos = vector.firstBessel(t, sequence)
            funValueX = pos.x
            funValueY = pos.y
            dist1 = abs(funValueX - newData.longitude)
            dist2 = abs(funValueY - newData.latitude)
        else:  # 否则，将新数据带入三阶判断
            pos = vector.verify(t, 0.1, vector, newData)
            funValueX = pos.x
            funValueY = pos.y
            dist1 = abs(funValueX - end.longitude)
            dist2 = abs(funValueY - end.latitude)
        if (dist1 > dist) or (dist2 > dist):  # 带入当前矢量函数所求的值超出了阈值
            sign = True  # 需要更新
            vector.isNow = False  # 要创建新的矢量，把当前的矢量标记为非当前
        return sign

    def judge2(self, newData, sequence, dmax, dist_angle):  # 只带入一阶贝塞尔判断
        sign = [False, False]
        errordist = 0
        # 两类更新，第一类是主要由于速度改变（x的差超出了阈值，y的差没有超出阈值），方向改变小或没改变 ——sign[0]
        # 第二类是方向超出了阈值，此时要按照道路口处理，求道路口的拐点，改成两段——sign[1]
        # sequence = vector_sequence.VectorSequence()
        index = len(sequence.vectors) - 1  # 得到矢量序列最后一个矢量的下标
        vector = sequence.vectors[index]  # 得到当前序列的最后一个矢量
        start = vector.dataStart
        end = vector.dataEnd
        # k = vector.thirdBessel(0.1, start, end, newData)[1]
        k = 10005300
        dist1 = 0.0  # 实际横坐标误差
        dist2 = 0.0  # 实际纵坐标误差
        dist_dir = 0.0  # 实际的角度误差
        zi = datetime_time(newData.gpsTime) - datetime_time(start.gpsTime)
        mu = datetime_time(end.gpsTime) - datetime_time(start.gpsTime)
        if mu != 0:
            t = zi / mu
        else:
            return sign, errordist
        funValueX = 0.0  # 如果不更新带入前一段矢量函数对应的x坐标
        funValueY = 0.0
        pos = vector.firstBessel(t, sequence)
        funValueX = pos.x
        funValueY = pos.y
        shiji = [newData.longitude, newData.latitude]
        yugu = [pos.x, pos.y]
        distance = douglas_peuker.Geodist(shiji, yugu)
        # dist1 = abs(funValueX - newData.longitude)
        # dist2 = abs(funValueY - newData.latitude)
        # 两直线的斜率为k1, k2,夹角为α, 求两直线所夹的锐角tanα = | (k2 - k1) / (1 + k1k2) |
        # k1 = abs(line_k(start, end))
        # k2 = abs(line_k(end, newData))
        # if k1 is not None and k2 is not None:
        #     dist_dir = abs((k2 - k1) / (1 + k1 * k2))
        # if 0.01 > dist1 > distX and 0.01 > dist2 > distY and dist_dir > dist_angle:  # 判断路口
        #     sign[1] = True  # 需要道路口更新
        #     vector.isNow = False
        if k > dist_angle:  # 判断路口,中间点的曲率大于某个值
            sign[1] = True  # 需要道路口更新
            vector.isNow = False
        # elif dist1 > distX or dist2 > distY:  # 带入当前矢量函数所求的值超出了阈值
        elif distance > dmax:
            sign[0] = True  # 需要普通更新
            vector.isNow = False  # 要创建新的矢量，把当前的矢量标记为非当前
        else:
            errordist += distance
        return sign, errordist

    # /**
    #  * 筛选某辆车的信息并进行封装
    #  * @param path：文件路径
    #  * @param carNumber：如果要筛选，输入完整的车牌号；不筛选，输入空字符串
    #  * @throws IOException
    #  */
    def readerByCarNumber(self, path, carNumber):
        li = []
        file_path = path
        with open(file_path, newline='', encoding='utf-8') as f:  # 参数encoding = 'utf-8'防止出现乱码
            reader = csv.reader(f)  # 使用csv的reader()方法，创建一个reader对象  csv.reader（）读取结果是列表
            for row in reader:  # 遍历reader对象的每一行
                for col in row:
                    if col == "":
                        continue
                    if col == carNumber:
                        data = self.constructor(row)
                        li.append(data)
        return li

    # /**
    #  * 按照读取的一行创建一个data对象
    #  * @param s
    #  * @return
    #  */
    def constructor(self, s):
        data = entity.Data()
        data.id = int(s[1])
        data.vehicleCode = s[2]
        data.gpsTime = string_datetime(s[3])
        data.longitude = float(s[4])
        data.latitude = float(s[5])
        data.speed = float(s[6])
        data.vehState = int(s[7])
        data.gpsState = int(s[8])
        data.aspect = int(s[9])
        return data

    def draw_path(self, points, sequence):
        """
        根据轨迹点绘制轨迹路径
        :param points: 轨迹点集合
        :param sequence:根据矢量序列还原的路径
        :return:
        """
        lat = []
        lon = []
        t = []
        lat_reduction = []
        lon_reduction = []
        lat_vector = []
        lon_vector = []
        for data in points:
            lat.append(data.latitude)
            lon.append(data.longitude)
            t.append(data.gpsTime)
            # plt.text(data.longitude, data.latitude, (data.longitude, data.latitude), color='b')
        print("原始轨迹点个数：" + str(len(points)))
        plt.rcParams['font.sans-serif'] = ['SimHei']  # 用来正常显示中文标签
        plt.rcParams['axes.unicode_minus'] = False  # 用来正常显示负号

        plt.title("轨迹测试")
        plt.xlabel("经度")  # 定义x坐标轴名称
        plt.ylabel("纬度")  # 定义y坐标轴名称
        plt.plot(lon, lat, marker='1', label='原始')  # 绘制原始路径

        dataTest = entity.Data()

        # for i in range(0, len(points)):
        #     dataTest.gpsTime = t[i]
        #     p = sequence.inquire2(dataTest)
        #     lat_reduction.append(p.y)
        #     lon_reduction.append(p.x)
        # plt.plot(lon_reduction, lat_reduction, marker='.', label='据原时间求得')  # 还原后的路径

        lat_vector.append(sequence.vectors[0].dataStart.latitude)
        lon_vector.append(sequence.vectors[0].dataStart.longitude)
        for i in range(0, len(sequence.vectors)):
            lat_vector.append(sequence.vectors[i].dataEnd.latitude)
            lon_vector.append(sequence.vectors[i].dataEnd.longitude)
            plt.text(sequence.vectors[i].dataEnd.longitude, sequence.vectors[i].dataEnd.longitude,
                     (sequence.vectors[i].dataEnd.longitude, sequence.vectors[i].dataEnd.longitude), color='r')
        print("压缩轨迹点个数:" + str(len(lat_vector)))
        plt.plot(lon_vector, lat_vector, marker='.', label='压缩后')  # 还原后的路径
        plt.ylim(min(lat_vector) - 0.01, max(lat_vector) + 0.01)
        plt.xlim(min(lon_vector) - 0.01, max(lon_vector) + 0.01)
        plt.legend()
        plt.show()  # 展示
        return len(lat_vector)
