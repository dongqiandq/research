import entity
import my_vector
import util
import math


class VectorSequence:
    vectors = []  # MyVectors
    vSchema = 0  # 标识属于采样值哪个分量的状态序列

    # /**
    #  * 通过输入的数据获取时间，来求取改时间点下对因为的位置
    #  * @param data
    #  * @return
    #  */
    def inquire(self, data):
        dt = data.gpsTime
        tNum = util.datetime_time(dt)
        for v in self.vectors:
            start = util.datetime_time(v.startT)
            end = util.datetime_time(v.endT)
            if (tNum >= start) and (tNum <= end):
                t = v.seekT(v.dataStart, data, v.dataEnd)
                if v.isFirstBessel:
                    return v.firstBessel(t, self)
                else:
                    return v.inquireThird(v, data.gpsTime)
                print("此时改点不在目前的存储范围")
        return None

    def inquire2(self, data):
        dt = data.gpsTime
        tNum = util.datetime_time(dt)
        for v in self.vectors:
            start = util.datetime_time(v.startT)
            end = util.datetime_time(v.endT)
            if (tNum >= start) and (tNum <= end):
                t = v.seekT(v.dataStart, data, v.dataEnd)
                return my_vector.calculate_pos_fistbezier(t, v)
        print("此时改点不在目前的存储范围")
        return None

    # /**
    #  * 矢量的提取
    #  * @param data：新来的数据点
    #  */
    def vectorExtraction(self, data, t):
        errordist = 0
        if len(self.vectors) == 0:  # 当前没有矢量活动,来的是第一个点
            self.addFirstData(data)  # 添加第一个矢量活动的开始结点
        elif (len(self.vectors) == 1) and (self.vectors[0].dataEnd is None):  # 新来的点是第二个点
            t = 1
            self.addSecondData(data, t)
        else:
            index = len(self.vectors) - 1
            vector = self.vectors[index]
            sign, errordist = util.Util().judge2(data, self, 8, 10005300)
            if sign[1]:
                # 需要道路口更新
                # self.vectorUpdate(data)
                self.vectorUpdateTurn(data)
            elif sign[0]:
                # 需要普通更新
                self.vectorUpdate(data)
            else:
                # 不需要更新
                vector.endT = data.gpsTime
                vector.dataEnd = data
        return errordist

    def vectorUpdateTurn(self, data):
        """
        如果Y方向偏离过大，我们判断遇到了道路口，在道路口生成一点，生成两个垂直的矢量
        :param data: 新来的数据点
        :return: 返回该矢量序列
        """
        index = len(self.vectors) - 1  # 当前矢量的位置
        vectorNow = self.vectors[index]
        posA = vectorNow.dataStart
        posB = vectorNow.dataEnd
        # 根据AB的坐标，求AB直线方程一般式a = Y2 - Y1,B = X1 - X2,C = X2*Y1 - X1*Y2
        a = posB.latitude - posA.latitude
        b = posA.longitude - posB.longitude
        c = posB.longitude * posA.latitude - posA.longitude * posB.latitude
        # 求data到直线AB的垂足坐标 posD.
        # 设直线方程为ax+by+c=0,点坐标为(m,n)则垂足为((b*b*m-a*b*n-a*c)/(a*a+b*b),(a*a*n-a*b*m-b*c)/(a*a+b*b))
        posD = entity.Data()
        posD.speed = posB.speed
        posD.longitude = (b * b * data.longitude - a * b * data.latitude - a * c) / (a * a + b * b)
        posD.latitude = (a * a * data.latitude - a * b * data.longitude - b * c) / (a * a + b * b)
        x_BD = posD.longitude - posB.longitude
        y_BD = posD.latitude - posB.latitude
        d_BD = math.sqrt(x_BD * x_BD + y_BD * y_BD)
        x_Ddata = posD.longitude - data.longitude
        y_Ddata = posD.latitude - data.latitude
        d_Ddata = math.sqrt(x_Ddata * x_Ddata + y_Ddata * y_Ddata)
        t = d_BD / (d_Ddata + d_BD)
        timeD = util.datetime_time(posB.gpsTime) + t * (
                    util.datetime_time(data.gpsTime) - util.datetime_time(posB.gpsTime))
        posD.gpsTime = util.time_datetime(timeD)
        # 生成前半段BD
        vector1 = my_vector.MyVector()
        vector1.dataStart = posB
        vector1.dataEnd = posD
        vector1.startT = vectorNow.endT
        vector1.endT = posD.gpsTime
        vector1.isNow = False
        vector1.isFirstBessel = True
        vectorNow.isNow = False
        position1 = my_vector.calculate_pos_fistbezier(1, vector1)
        vector1.position = position1
        self.vectors.append(vector1)
        # 生成后半段DC
        vector2 = my_vector.MyVector()
        vector2.dataStart = posD
        vector2.dataEnd = data
        vector2.startT = posD.gpsTime
        vector2.endT = data.gpsTime
        vector2.isNow = True
        vector2.isFirstBessel = True
        position2 = my_vector.calculate_pos_fistbezier(1, vector2)
        vector2.position = position2
        self.vectors.append(vector2)
        return self

    # /**
    #  * 矢量的普通更新
    #  * @param data:新来的数据点
    #  */
    def vectorUpdate(self, data):
        index = len(self.vectors) - 1  # 当前矢量的位置
        vectorNow = self.vectors[index]
        vector = my_vector.MyVector()
        position = vector.firstBessel(1, self)
        vector.position = position
        vector.dataStart = vectorNow.dataEnd
        vector.dataEnd = data
        vector.startT = vectorNow.endT
        vector.endT = data.gpsTime
        vector.isNow = True
        vector.isFirstBessel = True
        vectorNow.isNow = False
        self.vectors.append(vector)
        return self

    # /**
    #  * 添加第一个结点作为第一个矢量的起始点
    #  * @param data
    #  */
    def addFirstData(self, data):
        vector = my_vector.MyVector()
        data.number = 1
        vector.dataStart = data
        vector.isNow = True
        vector.startT = data.gpsTime
        self.vectors.append(vector)

    # /**
    #  * 添加第二个结点为第一个矢量的终止点，并使用一阶贝塞尔曲线求取 f(t)
    #  * @param data
    #  */
    def addSecondData(self, data, t):  # 使用一阶贝塞尔生成时间函数
        data.number = 2
        vector = self.vectors[0]
        vector.dataEnd = data
        vector.endT = data.gpsTime
        position = vector.firstBessel(t, self)
        vector.position = position
        vector.isNow = True

    def __str__(self):
        return "VectorSequence{" + "vetors=" + self.vetors + ", vSchema=" + self.vSchema + "}"
