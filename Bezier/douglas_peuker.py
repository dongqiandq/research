# 1.点间距离
import math
import matplotlib.pyplot as plt
import os


def Rad(d):
    return d * math.pi / 180


def Geodist(point1, point2):
    """
    返回两个点之间的距离
    :param point1:
    :param point2:
    :return:
    """
    radLat1 = Rad(point1[1])
    radLat2 = Rad(point2[1])
    delta_lon = Rad(point1[0] - point2[0])
    top_1 = math.cos(radLat2) * math.sin(delta_lon)
    top_2 = math.cos(radLat1) * math.sin(radLat2) - math.sin(radLat1) * math.cos(radLat2) * math.cos(delta_lon)
    top = math.sqrt(top_1 * top_1 + top_2 * top_2)
    bottom = math.sin(radLat1) * math.sin(radLat2) + math.cos(radLat1) * math.cos(radLat2) * math.cos(delta_lon)
    delta_sigma = math.atan2(top, bottom)
    distance = delta_sigma * 6378137.0
    return round(distance, 3)


# 2.点弦距离
def get_vertical_dist(pointA, pointB, pointX):
    """
    返回点X与弦AB的垂直距离,点弦距离计算使用的是海伦公式
    :param pointA:
    :param pointB:
    :param pointX:
    :return:
    """
    a = math.fabs(Geodist(pointA, pointB))
    # 当弦两端重合时,点到弦的距离变为点间距离
    if a == 0:
        return math.fabs(Geodist(pointA, pointX))
    b = math.fabs(Geodist(pointA, pointX))
    c = math.fabs(Geodist(pointB, pointX))
    p = (a + b + c) / 2
    S = math.sqrt(math.fabs(p * (p - a) * (p - b) * (p - c)))
    vertical_dist = S * 2 / a
    return vertical_dist


# 3.递归压缩
def DP_compress(point_list, output_point_list, Dmax):
    """
    按照阈值Dmax将压缩后的point_list输出到output_point_list中
    :param point_list:
    :param output_point_list:
    :param Dmax:
    :return:
    """
    start_index = 0
    end_index = len(point_list) - 1
    # 起止点必定是关键点,但是作为递归程序此步引入了冗余数据,后期必须去除
    output_point_list.append(point_list[start_index])
    output_point_list.append(point_list[end_index])
    if start_index < end_index:
        index = start_index + 1  # 工作指针,遍历除起止点外的所有点
        max_vertical_dist = 0  # 路径中离弦最远的距离
        key_point_index = 0  # 路径中离弦最远的点,即划分点
        while index < end_index:
            cur_vertical_dist = get_vertical_dist(point_list[start_index], point_list[end_index], point_list[index])
            if cur_vertical_dist > max_vertical_dist:
                max_vertical_dist = cur_vertical_dist
                key_point_index = index  # 记录划分点
            index += 1

        # 递归划分路径
        if max_vertical_dist >= Dmax:
            DP_compress(point_list[start_index:key_point_index], output_point_list, Dmax)
            DP_compress(point_list[key_point_index:end_index], output_point_list, Dmax)


# 4.平均误差
def get_MeanErr(point_list, output_point_list):
    """
    返回输出相对于输入的平均误差(压缩时忽略的那些点到对应线段的距离之和除以总点数)
    压缩率：（压缩后的点数/总点数）*100%
    :param point_list:
    :param output_point_list:
    :return:
    """
    Err = 0
    start_index = 0
    end_index = len(output_point_list) - 1
    while start_index < end_index:  # 遍历所有关键点
        # 选取两相邻关键点
        pointA_id = int(output_point_list[start_index][2])
        pointB_id = int(output_point_list[start_index + 1][2])
        id = pointA_id + 1  # 工作指针,用于遍历非关键点
        while id < pointB_id:  # 遍历两关键点之间的非关键点
            Err += get_vertical_dist(output_point_list[start_index], output_point_list[start_index + 1], point_list[id])
            id += 1
        start_index += 1
    return Err / len(point_list)


def readdir(path):
    """
    读取文件夹下的全部文件，批处理文件
    :param path: 文件夹目录，截止到目录名
    :return:
    """
    count = 0
    files = os.listdir(path)  # 得到文件夹下的所有文件名称
    # point_list = []
    for file in files:  # 遍历文件夹
        position = path + '\\' + file  # 构造绝对路径，"\\"，其中一个'\'为转义符
        with open(position, "r", encoding='utf-8') as f:  # 打开文件
            for line in f.readlines()[6:]:
                line = line.strip()
                id = count
                count += 1
                longitude = float(line.split(",")[1])
                latitude = float(line.split(",")[0])
                point_list.append((longitude, latitude, id))
            f.close()


# readdir("E:/DongQian/trafficData/Geolife Trajectories 1.3/Data/000/Trajectory")


# 5.主程序
def fun():
    point_list = []
    output_point_list = []
    # 将处理后的数据读入内存,单个文件处理
    fd = open(r"E:/DongQian/trafficData/Geolife Trajectories 1.3/Data/000/Trajectory/20081023025304.plt", 'r')
    count = 0
    for line in fd.readlines()[6:]:
        line = line.strip()
        id = count
        count += 1
        longitude = float(line.split(",")[1])
        latitude = float(line.split(",")[0])
        point_list.append((longitude, latitude, id))
    fd.close()


    # fd = open(r"E:/DongQian/trafficData/Geolife Trajectories 1.3/Data/000/Trajectory/20081024020959.plt", 'r')
    # for line in fd.readlines()[6:]:
    #     line = line.strip()
    #     id = count
    #     count += 1
    #     longitude = float(line.split(",")[1])
    #     latitude = float(line.split(",")[0])
    #     point_list.append((longitude, latitude, id))
    # fd.close()

    # fd = open(r"E:/DongQian/trafficData/Geolife Trajectories 1.3/Data/000/Trajectory/20081026134407.plt", 'r')
    # for line in fd.readlines()[6:]:
    #     line = line.strip()
    #     id = count
    #     count += 1
    #     longitude = float(line.split(",")[1])
    #     latitude = float(line.split(",")[0])
    #     point_list.append((longitude, latitude, id))
    # fd.close()

    # fd = open(r"E:/DongQian/trafficData/Geolife Trajectories 1.3/Data/000/Trajectory/20081027115449.plt", 'r')
    # for line in fd.readlines()[6:]:
    #     line = line.strip()
    #     id = count
    #     count += 1
    #     longitude = float(line.split(",")[1])
    #     latitude = float(line.split(",")[0])
    #     point_list.append((longitude, latitude, id))
    # fd.close()

    # fd = open(r"E:/DongQian/trafficData/Geolife Trajectories 1.3/Data/000/Trajectory/20081028003826.plt", 'r')
    # for line in fd.readlines()[6:]:
    #     line = line.strip()
    #     id = count
    #     count += 1
    #     longitude = float(line.split(",")[1])
    #     latitude = float(line.split(",")[0])
    #     point_list.append((longitude, latitude, id))
    # fd.close()

    DP_compress(point_list, output_point_list, Dmax=8)

    output_point_list = list(set(output_point_list))  # 去除递归引入的冗余数据
    output_point_list = sorted(output_point_list, key=lambda x: x[2])  # 按照id排序

    # 将压缩数据写入输出文件
    fd = open(r"E:/DongQian/trafficData/output.txt", 'w')
    for point in output_point_list:
        fd.write("{},{},{}\n".format(point[2], point[0], point[1]))
    fd.close()

    print("compression rate={}/{}={}".format(len(output_point_list), len(point_list),
                                             len(output_point_list) / len(point_list)))
    print("mean error:{}".format(get_MeanErr(point_list, output_point_list)))

    # 6.可视化
    ####point_list

    uncompressed = [[], []]
    for point in point_list[:]:  # point_list
        uncompressed[0].append(point[0])
        uncompressed[1].append(point[1])

    plt.plot(uncompressed[0], uncompressed[1], "g")
    plt.xlabel("longitude")
    plt.ylabel("latitude")
    plt.title("uncompressed")
    plt.show()

    compressed = [[], []]
    for point in output_point_list:  # output_point_list
        compressed[0].append(point[0])
        compressed[1].append(point[1])

    plt.plot(compressed[0], compressed[1], 'r')
    plt.xlabel("longitude")
    plt.ylabel("latitude")
    plt.title("compressed")
    plt.show()


# fun()
