
class Data:
    number = 0  # 标记是第一个还是第二个采样点
    id = 0
    centerId = ""  # 标识调度中心
    simCode = ""  # 终端号
    vehicleCode = ""  # 车牌号
    gpsTime = None  # 数据生成时间
    longitude = 0.0  # 经度
    latitude = 0.0  # 纬度
    speed = 0.0  # 速度
    vehState = 0  # 车辆状态
    gpsState = 1  # 经纬度定位是否有效，0无效，1有效
    aspect = 0  # 方向，正北偏东的方向角范围（0-359）

    def __str__(self):
        info = "Data{" + "id=" + self.id + ", VehicleCode='" + self.vehicleCode + '\'' + ", gpsTime=" + self.gpsTime + ", longitude=" + self.longitude + ", latitude=" + self.latitude + ", speed=" + self.speed
        return info


class ObjectRecord:  # 数据分布式存储
    vectorSequences = []  # 不同分量的状态序列集合
    raw_addr_vector = []  # 服务器地址表
    objId = 0
    obj_descript = ""


class Position:
    x = 0.0
    y = 0.0

    def construct1(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return "Position{" + "x=" + str(self.x) + ", y=" + str(self.y) + '}'
