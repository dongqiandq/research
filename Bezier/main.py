import util
import entity
import vector_sequence

# See PyCharm help at https://www.jetbrains.com/help/pycharm/

util_obj = util.Util()


def fun():
    datas = util_obj.readerByCarNumber("E:\\DongQian\\trafficData\\test.CSV", "京01144513")
    sequence = vector_sequence.VectorSequence()
    vectors = sequence.vectors
    for i in range(0, len(datas)):
        sequence.vectorExtraction(datas[i], 0.1)
    for i in range(0, len(vectors)):
        print(vectors[i])
    print("*******************通过输入时间求得的预测位置坐标**********************")
    #         for(int j=0;j<datas.size();j++){
    #            Position pos = sequence.inquire(datas.get(j));
    #            System.out.println(pos);
    #         }
    dataTest = entity.Data()
    t = util.string_datetime("2013-5-31 23:31:36")
    dataTest.gpsTime = t
    p = sequence.inquire(dataTest)
    print(p)


fun()
