import my_vector
import util


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

    # /**
    #  * 矢量的提取
    #  * @param data：新来的数据点
    #  */
    def vectorExtraction(self, data, t):
        if len(self.vectors) == 0:  # 当前没有矢量活动,来的是第一个点
            self.addFirstData(data)  # 添加第一个矢量活动的开始结点
        elif (len(self.vectors) == 1) and (self.vectors[0].dataEnd is None):  # 新来的点是第二个点
            self.addSecondData(data, t)
        else:
            index = len(self.vectors) - 1
            vector = self.vectors[index]
            if util.Util().judge(data, self, 0.02):
                # 需要更新
                self.vectorUpdate(data)
            else:
                # 不需要更新
                vector.endT = data.gpsTime
        return self

    # /**
    #  * 矢量的更新
    #  * @param data:新来的数据点
    #  */
    def vectorUpdate(self, data):
        index = len(self.vectors) - 1  # 当前矢量的位置
        vectorNow = self.vectors[index]
        vector = my_vector.MyVector()
        position = vector.thirdBessel(0.1, vectorNow.dataStart, vectorNow.dataEnd, data)
        vector.position = position
        vector.dataStart = vectorNow.dataEnd
        vector.dataEnd = data
        vector.startT = vectorNow.endT
        vector.endT = data.gpsTime
        vector.isNow = True
        vector.isFirstBessel = False
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
