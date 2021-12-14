import entity
import math
import util
import sympy as sp


def calculate_pos_fistbezier(t, vector):
    """
    通过比例t和找到的vector求取对应坐标
    :param t:
    :param vector:
    :return: 该比例t下对应的位置坐标
    """
    start = vector.dataStart
    end = vector.dataEnd
    y = (1 - t) * start.latitude + t * end.latitude
    x = (1 - t) * start.longitude + t * end.longitude
    pos = entity.Position()
    pos.construct1(x, y)
    return pos


class MyVector:
    isNow = False
    isFirstBessel = False  # 表示矢量函数是否是由一阶贝塞尔生成的
    unitime = 1
    dataStart = None  # 矢量的开始数据状态
    dataEnd = None
    startT = None  # 矢量开始的时间,初始化为当前时间
    endT = None
    position = None
    control = []  # Position，控制点

    # /**
    #  * vector中存储的矢量函数是通过一阶和三阶贝塞尔曲线求得的
    #  * 因为求出来的位置坐标是（X，Y），其中X是关于t的函数，Y也是关于t的函数，所以求取的position也是关于t的函数
    #  * 这里的t不是指的时间，而是比值
    #  */

    # /**
    #  * 根据前一段矢量生成新的矢量
    #  * @param t：比例值
    #  * @param sequence:矢量序列
    #  * @return ：t比例对应下的位置坐标
    #  */

    def firstBessel(self, t, sequence):
        index = len(sequence.vectors) - 1
        vector = sequence.vectors[index]
        # del sequence.vectors[0]
        start = vector.dataStart
        end = vector.dataEnd
        y = (1 - t) * start.latitude + t * end.latitude
        x = (1 - t) * start.longitude + t * end.longitude
        pos = entity.Position()
        pos.construct1(x, y)
        return pos

    # /**
    #  * 根据三个数据点求贝塞尔曲线
    #  * 根据输入的三个点求取这三个点构成的贝塞尔曲线，先根据这三个点求取两个控制点，再根据控制点和起始点，四个点关于t的矢量函数position
    #  * 其中因为position的x,y是随着t变化的,因此他是矢量函数
    #  * @param ?????t的值可以用时间之比吗
    #  * @param a：输入的常数参数
    #  * @param data1：数据点
    #  * @param data2：数据点
    #  * @param data3：数据点
    #  * @return :输出data2对应时间的坐标
    #  */
    def thirdBessel(self, a, data1, data2, data3):
        p1X = data1.longitude
        p1Y = data1.latitude
        p2X = data2.longitude
        p2Y = data2.latitude
        p3X = data3.longitude
        p3Y = data3.latitude
        d12 = math.sqrt((p2X - p1X) * (p2X - p1X) + (p1Y - p2Y) * (p1Y - p2Y))
        d23 = math.sqrt((p2X - p3X) * (p2X - p3X) + (p3Y - p2Y) * (p3Y - p2Y))
        fa = a * (d12 / (d12 + d23))
        fb = a * (d23 / (d12 + d23))
        pAX = p2X - fa * (p3X - p1X)
        pAY = p2Y - fa * (p3Y - p1Y)
        co1 = entity.Position()
        co1.construct1(pAX, pAY)
        pBX = p2X + fb * (p3X - p1X)
        pBY = p2Y + fb * (p3Y - p1Y)
        co2 = entity.Position()
        co2.construct1(pBX, pBY)
        self.control.clear()
        # 两者的区别是，self.l = blablabla 这是一个赋值语句，self.l.append(blablabla)这不是一个赋值语句。两者的区别是，self.l = blablabla 这是一个赋值语句，self.l.append(blablabla)这不是一个赋值语句。
        self.control.append(co1)
        self.control.append(co2)
        pos = entity.Position()
        t = sp.symbols('t')
        x = (1 - t) * (1 - t) * (1 - t) * p1X + 3 * t * (1 - t) * (1 - t) * pAX + 3 * t * t * (
                1 - t) * pBX + t * t * t * p3X
        y = (1 - t) * (1 - t) * (1 - t) * p1Y + 3 * t * (1 - t) * (1 - t) * pAY + 3 * t * t * (
                1 - t) * pBY + t * t * t * p3Y
        # 求data2对应的曲率
        # t = sp.symbols('t')
        xt = sp.diff(x, t)
        xtt = sp.diff(xt, t)
        yt = sp.diff(y, t)
        ytt = sp.diff(yt, t)
        t1 = self.seekT(data1, data2, data3)
        xt_t = float(xt.evalf(subs={t: t1}))
        xtt_t = float(xtt.evalf(subs={t: t1}))
        yt_t = float(yt.evalf(subs={t: t1}))
        ytt_t = float(ytt.evalf(subs={t: t1}))
        k = abs(xt_t * ytt_t - yt_t* xtt_t) / ((xt_t ** 2 + yt_t ** 2) ** (3 / 2))
        pos.y = y
        pos.x = x
        # print("xt_t:"+str(xt_t))
        # print("xtt_t:" + str(xtt_t))
        # print("yt_t:" + str(yt_t))
        # print("ytt_t:" + str(ytt_t))
        print("k:" + str(k))
        return pos, k

    # /**
    #  * 通过给定的时间求取三阶贝塞尔曲线上的对应点坐标，使用矢量之前求取的控制点
    #  * @param vector:所在的矢量范围
    #  * @param date：输入要查询的坐标对应的时间
    #  * @return :该时间下所对应的坐标
    #  */
    def inquireThird(self, vector, date):
        start = vector.dataStart
        end = vector.dataEnd
        fa = vector.control[0]
        fb = vector.control[1]
        pos = entity.Position()
        p1X = start.longitude
        p1Y = start.latitude
        p3X = end.longitude
        p3Y = end.latitude
        da = entity.Data()
        da.gpsTime = date
        t = self.seekT(start, da, end)
        x = (1 - t) * (1 - t) * (1 - t) * p1X + 3 * t * (1 - t) * (1 - t) * fa.x + 3 * t * t * (
                1 - t) * fb.x + t * t * t * p3X
        y = (1 - t) * (1 - t) * (1 - t) * p1Y + 3 * t * (1 - t) * (1 - t) * fa.y + 3 * t * t * (
                1 - t) * fb.y + t * t * t * p3Y
        pos.x = x
        pos.y = y
        return pos

    # /**
    #  * 求取t的值
    #  * @param data1：左
    #  * @param data2：中
    #  * @param data3：右
    #  * @return
    #  */
    def seekT(self, data1, data2, data3):
        zi = util.datetime_time(data2.gpsTime) - util.datetime_time(data1.gpsTime)
        mu = util.datetime_time(data3.gpsTime) - util.datetime_time(data1.gpsTime)
        t = zi / mu
        return t

    # /**求得的点在之后,即要将新来的点带入前一个矢量函数，求出函数在当前时间点对应的坐标，此时的t应该是大于一的，因为新来的点在终止点之后
    #  * 当前矢量活动是三阶贝塞尔曲线时，保持原来的控制点，加上后两个点，带入当前的时间求得t，然后求出保持原函数对应的位置坐标
    #  * @param a
    #  * @param vector:前一个矢量
    #  * @param data3：新来的数据
    #  * @return
    #  */
    def verify(self, t, a, vector, data3):
        pos = entity.Position()
        start = vector.dataStart
        end = vector.dataEnd
        fa = vector.control[0]
        fb = vector.control[1]
        x = (1 - t) * (1 - t) * (1 - t) * start.longitude + 3 * t * (1 - t) * (1 - t) * fa.x + 3 * t * t * (
                1 - t) * fb.x + t * t * t * data3.longitude
        y = (1 - t) * (1 - t) * (1 - t) * start.latitude + 3 * t * (1 - t) * (1 - t) * fa.y + 3 * t * t * (
                1 - t) * fb.y + t * t * t * data3.latitude
        pos.y = y
        pos.x = x
        return pos

    def __str__(self):
        return "MyVector{" + "isNow=" + str(self.isNow) + ", isFirstBessel=" + str(
            self.isFirstBessel) + ", startT=" + str(self.startT) + ", endT=" + str(self.endT) + ", position=" + str(
            self.position) + '}'
