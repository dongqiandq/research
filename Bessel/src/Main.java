import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

public class Main {
    private static Util util = new Util();
    public static void main(String[] args) throws IOException {
        List<Data> datas = util.readerByCarNumber("E:\\DongQian\\trafficData\\test.CSV","京01144513");
        VectorSequence sequence = new VectorSequence();
        List<MyVector> vectors = sequence.getVetors();
        for(int i=0;i<datas.size();i++){
            sequence.vectorExtraction(datas.get(i),0.1);
        }
        for(int i=0;i<vectors.size();i++){
            System.out.println(vectors.get(i).toString());
        }
        System.out.println("*******************通过输入时间求得的预测位置坐标**********************");
//        for(int j=0;j<datas.size();j++){
//           Position pos = sequence.inquire(datas.get(j));
//           System.out.println(pos);
//        }
        Data dataTest = new Data();
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        try {
            Date t = sdf.parse("2013/5/31 23:50:36");
            dataTest.setGpsTime(t);
            Position p = sequence.inquire(dataTest);
            System.out.println(p);
        } catch (ParseException e) {
            e.printStackTrace();
        }

    }

}
