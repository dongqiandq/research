import util
import entity
import vector_sequence

# 标有2的是第二次修改后使用的一阶贝塞尔直线实现的，区别于不加数字标记的一阶三阶贝塞尔
# See PyCharm help at https://www.jetbrains.com/help/pycharm/

util_obj = util.Util()
count = 0


def fun1():
    errordist = 0
    # datas = util_obj.readerByCarNumber("E:\\DongQian\\trafficData\\test.CSV", "京B08807")
    datas = util.readGeoLife("E:/DongQian/trafficData/Geolife Trajectories 1.3/Data/000/Trajectory/20081023025304.plt", count)[0]

    # datas = util.readGeoLifeDir("E:/DongQian/trafficData/Geolife Trajectories 1.3/Data/000/Trajectory",count)[0]
    sequence = vector_sequence.VectorSequence()
    vectors = sequence.vectors
    for i in range(0, len(datas)):
        errordist += sequence.vectorExtraction(datas[i], 0.1)
        # print(datas[i])
    # for i in range(0, len(vectors)):
    #     print(vectors[i])
    # print("*******************通过输入时间求得的预测位置坐标**********************")
    # dataTest = entity.Data()
    # t = util.string_datetime("2013-5-31 23:56:34")
    # dataTest.gpsTime = t
    # p = sequence.inquire2(dataTest)
    # print(p)
    # util.output_compression_data("E:/DongQian/research/output/new.csv", sequence)  # 以文件形式输出压缩后的点
    press_data = util_obj.draw_path(datas, sequence)  # 对比压缩前后的轨迹图
    rate = (press_data/len(datas))*100
    print("压缩率为：" + str(rate) + "%")
    print("平均误差为：" + str(errordist/len(datas)))
    errordist/len(datas)
    # *************************将轨迹放到地图上显示***************
    # path1 = 'E:/DongQian/research/output/new.csv'  # 要显示地图中的文件路径
    # locations = util.read_gps_data(path1)  # 经纬度坐标集合[[lat,lon]...]
    # print(locations)
    # util.draw_map_gps(locations, 'E:/DongQian/research/output/', 'new.html')  # 将轨迹在地图中显示


fun1()
