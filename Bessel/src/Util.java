import java.io.*;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Vector;

public class Util {

    /**
     * 对新来的数据点进行带入，判断求得的值与实际值的差 与阈值的关系
     * @param newData：新数据点
     * @param sequence：矢量序列，可以得到最新的矢量（最后一个）
     * @param dist：阈值
     * @return :如果两者的差大于阈值，则返回TRUE，表示需要更新，否则返回FALSE，表示不需要更新，继续延续当前矢量，更改截止时间
     */
    public boolean judge(Data newData,VectorSequence sequence,double dist){
        boolean sign = false;
        int index = sequence.getVetors().size()-1;
        MyVector vector = sequence.getVetors().get(index);
        Data start = vector.getDataStart();
        Data end = vector.getDataEnd();
        double dist1 = 0.0;
        double dist2 = 0.0;
        double zi = newData.getGpsTime().getTime()-start.getGpsTime().getTime();
        double mu = end.getGpsTime().getTime()-start.getGpsTime().getTime();
        double t = zi/mu;
        double funValueX = 0.0 ,funValueY = 0.0;
        if(vector.isFirstBessel()){//如果当前的矢量活动是由一阶贝塞尔生成的，将新来的数据带入一阶判断
            Position pos = vector.firstBessel(t,sequence);
            funValueX = pos.getX();
            funValueY = pos.getY();
            dist1 = Math.abs(funValueX-newData.getLatitude());
            dist2 = Math.abs(funValueY-newData.getLongitude());
        }else{//否则，将新数据带入三阶判断
            Position pos = vector.verify(t,0.1,vector,newData);
            funValueX = pos.getX();
            funValueY = pos.getY();
            dist1 = Math.abs(funValueX-end.getLatitude());
            dist2 = Math.abs(funValueY-end.getLongitude());
        }
        if(dist1>dist || dist2>dist){//带入当前矢量函数所求的值超出了阈值
            sign = true;//需要更新
            vector.setNow(false);//要创建新的矢量，把当前的矢量标记为非当前
        }
        return sign;
    }


    /**
     * 筛选某辆车的信息并进行封装
     * @param path：文件路径
     * @param carNumber：如果要筛选，输入完整的车牌号；不筛选，输入空字符串
     * @throws IOException
     */
    public List<Data> readerByCarNumber(String path, String carNumber) throws IOException {
//        File file=new File(path);
//        String myCharset = System.getProperty("file.encoding");
//        System.out.println(myCharset );
        List<Data> list = new ArrayList<>();
        InputStream fis = new FileInputStream(path);
        InputStreamReader isr = new InputStreamReader(fis, "UTF-8"); //设置utf8编码
        BufferedReader br = new BufferedReader(isr);
        String line = "";
        try {
            while ((line = br.readLine()) != null) {
                String[] items = line.split(",");
                for (int j = 0; j < items.length; j++) {
                    items[j] = items[j].trim();//这是关键，那些乱码就是空格，这一步清除空格就好了
                    if (items[j].equals("")) {
                        continue;
                    }
                    if (items[j].equals(carNumber)) {
                        Data data = constructor(items);
                        list.add(data);
                        System.out.println(data.toString());
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            fis.close();
            isr.close();
            br.close();
        }
        return list;
    }

    /**
     * 按照读取的一行创建一个data对象
     * @param s
     * @return
     */
    public Data constructor(String[] s){
        Data data = new Data();
        data.setId(Long.parseLong(s[1]));
        data.setVehicleCode(s[2]);
        data.setGpsTime(conversion(s[3]));
        data.setLongitude(Double.parseDouble(s[4]));
        data.setLatitude(Double.parseDouble(s[5]));
        data.setSpeed(Double.parseDouble(s[6]));
        data.setVehState(Integer.parseInt(s[7]));
        data.setGpsState(Integer.parseInt(s[8]));
        data.setAspect(Integer.parseInt(s[9]));
        return data;
    }

    /**
     * 将时间字符串转化为date类
     * @param time
     * @return
     */
    public Date conversion(String time){
        Date date=null;
        SimpleDateFormat formatter=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        try {
            date=formatter.parse(time);
        } catch (ParseException e) {
            e.printStackTrace();
        }
        return date;
    }

}
