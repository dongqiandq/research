import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Vector;

public class MyVector {
    private boolean isNow;
    private boolean isFirstBessel;//表示矢量函数是否是由一阶贝塞尔生成的
    private int unitime = 1;
    private Data dataStart;
    private Data dataEnd;
    private Date startT;
    private Date endT;
    Position position;
    List<Position> control = new ArrayList<>();
    /**
     * vector中存储的矢量函数是通过一阶和三阶贝塞尔曲线求得的
     * 因为求出来的位置坐标是（X，Y），其中X是关于t的函数，Y也是关于t的函数，所以求取的position也是关于t的函数
     * 这里的t不是指的时间，而是比值
     */

    /**
     * 通过输入的矢量起始点和比例t，求取起始中间的对应点的坐标
     * @param t：比例值
     * @param sequence:矢量序列
     * @return ：t比例对应下的位置坐标
     */
    public Position firstBessel(double t,VectorSequence sequence){
        MyVector vector = sequence.getVetors().get(0);
        sequence.getVetors().remove(0);
        Data start = vector.getDataStart();
        Data end = vector.getDataEnd();
        double y = (1-t)*start.getLongitude()+t*end.getLongitude();
        double x = (1-t)*start.getLatitude()+t*end.getLatitude();
        Position pos = new Position(x,y);
        vector.setNow(true);
        vector.setFirstBessel(true);
        vector.setStartT(start.getGpsTime());
        vector.setEndT(end.getGpsTime());
        sequence.getVetors().add(vector);
        return pos;
    }

    /**
     * 根据三个数据点求贝塞尔曲线
     * 根据输入的三个点求取这三个点构成的贝塞尔曲线，先根据这三个点求取两个控制点，再根据控制点和起始点，四个点关于t的矢量函数position
     * 其中因为position的x,y是随着t变化的,因此他是矢量函数
     * @param ?????t的值可以用时间之比吗
     * @param a：输入的常数参数
     * @param data1：数据点
     * @param data2：数据点
     * @param data3：数据点
     * @return :输出data2对应时间的坐标
     */
    public Position thirdBessel(double a,Data data1,Data data2,Data data3){
        double p1X = data1.getLatitude();
        double p1Y = data1.getLongitude();
        double p2X = data2.getLatitude();
        double p2Y = data2.getLongitude();
        double p3X = data3.getLatitude();
        double p3Y = data3.getLongitude();
        double d12 = Math.sqrt((p2X-p1X)*(p2X-p1X)+(p1Y-p2Y)*(p1Y-p2Y));
        double d23 = Math.sqrt((p2X-p3X)*(p2X-p3X)+(p3Y-p2Y)*(p3Y-p2Y));
        double fa = a*(d12/(d12+d23));
        double fb = a*(d23/(d12+d23));
        double pAX = p2X-fa*(p3X-p1X);
        double pAY = p2Y-fa*(p3Y-p1Y);
        Position co1 = new Position(pAX,pAY);
        double pBX = p2X+fb*(p3X-p1X);
        double pBY = p2Y+fb*(p3Y-p1Y);
        Position co2 = new Position(pBX,pBY);
        control.add(co1);
        control.add(co2);
        Position pos = new Position();
        double t = seekT(data1,data2,data3);
        double x = (1-t)*(1-t)*(1-t)*p1X+3*t*(1-t)*(1-t)*pAX+3*t*t*(1-t)*pBX+t*t*t*p3X;
        double y = (1-t)*(1-t)*(1-t)*p1Y+3*t*(1-t)*(1-t)*pAY+3*t*t*(1-t)*pBY+t*t*t*p3Y;
        pos.setY(y);
        pos.setX(x);
        return pos;
    }

    /**
     * 通过给定的时间求取三阶贝塞尔曲线上的对应点坐标，使用矢量之前求取的控制点
     * @param vector:所在的矢量范围
     * @param date：输入要查询的坐标对应的时间
     * @return :改时间下所对应的坐标
     */
    public Position inquireThird(MyVector vector,Date date){
        Data start = vector.getDataStart();
        Data end = vector.getDataEnd();
        Position fa = vector.control.get(0);
        Position fb = vector.control.get(1);
        Position pos = new Position();
        double p1X = start.getLatitude();
        double p1Y = start.getLongitude();
        double p3X = end.getLatitude();
        double p3Y = end.getLongitude();
        Data da = new Data();
        da.setGpsTime(date);
        double t = seekT(start,da,end);
        double x = (1-t)*(1-t)*(1-t)*p1X+3*t*(1-t)*(1-t)*fa.getX()+3*t*t*(1-t)* fb.getX()+t*t*t*p3X;
        double y = (1-t)*(1-t)*(1-t)*p1Y+3*t*(1-t)*(1-t)* fa.getY()+3*t*t*(1-t)* fb.getY()+t*t*t*p3Y;
        pos.setX(x);
        pos.setY(y);
        return pos;
    }

    /**
     * 求取t的值
     * @param data1：左
     * @param data2：中
     * @param data3：右
     * @return
     */
    public double seekT(Data data1,Data data2,Data data3){
        double zi = data2.getGpsTime().getTime()-data1.getGpsTime().getTime();
        double mu = data3.getGpsTime().getTime()-data1.getGpsTime().getTime();
        double t = zi/mu;
        return t;
    }

    /**求得的点在之后,即要将新来的点带入前一个矢量函数，求出函数在当前时间点对应的坐标，此时的t应该是大于一的，因为新来的点在终止点之后
     * 当前矢量活动是三阶贝塞尔曲线时，保持原来的控制点，加上后两个点，带入当前的时间求得t，然后求出保持原函数对应的位置坐标
     * @param a
     * @param vector:前一个矢量
     * @param data3：新来的数据
     * @return
     */
    public Position verify(double t,double a, MyVector vector,Data data3){
        Position pos = new Position();
        Data start = vector.getDataStart();
        Data end = vector.getDataEnd();
        Position fa = this.control.get(0);
        Position fb = this.control.get(1);
        double x = (1-t)*(1-t)*(1-t)*start.getLatitude()+3*t*(1-t)*(1-t)* fa.getX()+3*t*t*(1-t)* fb.getX()+t*t*t*data3.getLatitude();
        double y = (1-t)*(1-t)*(1-t)*start.getLongitude()+3*t*(1-t)*(1-t)* fa.getY()+3*t*t*(1-t)* fb.getY()+t*t*t*data3.getLongitude();
        pos.setY(y);
        pos.setX(x);
        return pos;
    }


    public Position getPosition() {
        return position;
    }

    public void setPosition(Position position) {
        this.position = position;
    }

    public boolean isFirstBessel() {
        return isFirstBessel;
    }

    public void setFirstBessel(boolean firstBessel) {
        isFirstBessel = firstBessel;
    }

    public boolean isNow() {
        return isNow;
    }

    public void setNow(boolean now) {
        isNow = now;
    }

    public int getUnitime() {
        return unitime;
    }

    public void setUnitime(int unitime) {
        this.unitime = unitime;
    }

    public Data getDataStart() {
        return dataStart;
    }

    public void setDataStart(Data dataStart) {
        this.dataStart = dataStart;
    }

    public Data getDataEnd() {
        return dataEnd;
    }

    public void setDataEnd(Data dataEnd) {
        this.dataEnd = dataEnd;
    }

    public Date getStartT() {
        return startT;
    }

    public void setStartT(Date startT) {
        this.startT = startT;
    }

    public Date getEndT() {
        return endT;
    }

    public void setEndT(Date endT) {
        this.endT = endT;
    }

    @Override
    public String toString() {
        return "MyVector{" +
                "isNow=" + isNow +
                ", isFirstBessel=" + isFirstBessel +
                ", startT=" + startT +
                ", endT=" + endT +
                ", position=" + position +
                '}';
    }
}
