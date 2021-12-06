import util
import entity
import vector_sequence

# 标有2的是第二次修改后使用的一阶贝塞尔直线实现的，区别于不加数字标记的一阶三阶贝塞尔
# See PyCharm help at https://www.jetbrains.com/help/pycharm/

util_obj = util.Util()


def fun1():
    datas = util_obj.readerByCarNumber("E:\\DongQian\\trafficData\\test.CSV", "京01144513")
    sequence = vector_sequence.VectorSequence()
    vectors = sequence.vectors
    for i in range(0, len(datas)):
        sequence.vectorExtraction(datas[i], 0.1)
    for i in range(0, len(vectors)):
        print(vectors[i])
    print("*******************通过输入时间求得的预测位置坐标**********************")
    dataTest = entity.Data()
    t = util.string_datetime("2013-5-31 23:50:36")
    dataTest.gpsTime = t
    p = sequence.inquire2(dataTest)
    print(p)


fun1()
