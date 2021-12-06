import entity
import vector_sequence
import datetime
import time
import csv
import folium


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
            dist1 = abs(funValueX - newData.latitude)
            dist2 = abs(funValueY - newData.longitude)
        else:  # 否则，将新数据带入三阶判断
            pos = vector.verify(t, 0.1, vector, newData)
            funValueX = pos.x
            funValueY = pos.y
            dist1 = abs(funValueX - end.latitude)
            dist2 = abs(funValueY - end.longitude)
        if (dist1 > dist) or (dist2 > dist):  # 带入当前矢量函数所求的值超出了阈值
            sign = True  # 需要更新
            vector.isNow = False  # 要创建新的矢量，把当前的矢量标记为非当前
        return sign

    def judge2(self, newData, sequence, distX, distY):  # 只带入一阶贝塞尔判断

        sign = [False, False]
        # 两类更新，第一类是主要由于速度改变（x的差超出了阈值，y的差没有超出阈值），方向改变小或没改变 ——sign[0]
        # 第二类是方向超出了阈值，此时要按照道路口处理，求道路口的拐点，改成两段——sign[1]
        # sequence = vector_sequence.VectorSequence()
        index = len(sequence.vectors) - 1  # 得到矢量序列最后一个矢量的下标
        vector = sequence.vectors[index]  # 得到当前序列的最后一个矢量
        start = vector.dataStart
        end = vector.dataEnd
        dist1 = 0.0  # 实际横坐标误差
        dist2 = 0.0  # 实际纵坐标误差
        zi = datetime_time(newData.gpsTime) - datetime_time(start.gpsTime)
        mu = datetime_time(end.gpsTime) - datetime_time(start.gpsTime)
        t = zi / mu
        funValueX = 0.0  # 如果不更新带入前一段矢量函数对应的x坐标
        funValueY = 0.0
        pos = vector.firstBessel(t, sequence)
        funValueX = pos.x
        funValueY = pos.y
        dist1 = abs(funValueX - newData.latitude)
        dist2 = abs(funValueY - newData.longitude)
        if dist1 > distX:  # 带入当前矢量函数所求的值超出了阈值
            sign[0] = True  # 需要普通更新
            vector.isNow = False  # 要创建新的矢量，把当前的矢量标记为非当前
        if dist2 > distY:
            sign[1] = True  # 需要道路口更新
            vector.isNow = False  # 要创建新的矢量，把当前的矢量标记为非当前
        return sign

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
