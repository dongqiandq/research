import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class VectorSequence {
    private Util util = new Util();
    private List<MyVector> vetors = new ArrayList<>();
    private int vSchema;//标识属于采样值哪个分量的状态序列


    public Position inquire(Data data){
        Date time = data.getGpsTime();
        Long tNum = time.getTime();
        for(MyVector v:vetors){
            Long start = v.getStartT().getTime();
            Long end = v.getEndT().getTime();
            if(tNum>=start && tNum<=end){
                double t = v.seekT(v.getDataStart(),data,v.getDataEnd());
                if(v.isFirstBessel()){
                    return v.firstBessel(t,this);
                }else{
                    return v.inquireThird(v,data.getGpsTime());
                }
            }
        }
        System.out.println("此时改点不在目前的存储范围");
        return null;
    }

    /**
     * 矢量的提取
     * @param data：新来的数据点
     */
    public VectorSequence vectorExtraction(Data data,double t){
        if(vetors.size()==0){//当前没有矢量活动,来的是第一个点
            addFirstData(data);//添加第一个矢量活动的开始结点
        }else if(vetors.size()==1 && vetors.get(0).getDataEnd()==null){//新来的点是第二个点
            addSecondData(data,t);
        }else{
            int index = vetors.size()-1;
            MyVector vector = vetors.get(index);
            if(util.judge(data,this,0.02)){
                //需要更新
                vectorUpdate(data);
            }else{
                //不需要更新
                vector.setEndT(data.getGpsTime());
            }

        }
        return this;
    }

    /**
     * 矢量的更新
     * @param data:新来的数据点
     */
    public VectorSequence vectorUpdate(Data data){
        int index = this.getVetors().size()-1;//当前矢量的位置
        MyVector vectorNow = this.getVetors().get(index);
        MyVector vector = new MyVector();
        Position position = vector.thirdBessel(0.1,vectorNow.getDataStart(),vectorNow.getDataEnd(),data);
        vector.setPosition(position);
        vector.setDataStart(vectorNow.getDataEnd());
        vector.setDataEnd(data);
        vector.setStartT(vectorNow.getEndT());
        vector.setEndT(data.getGpsTime());
        vector.setNow(true);
        vector.setFirstBessel(false);
        vectorNow.setNow(false);
        vetors.add(vector);
        return this;
    }

    /**
     * 添加第一个结点作为第一个矢量的起始点
     * @param data
     */
    public void addFirstData(Data data){
        MyVector vector = new MyVector();
        data.setNumber(1);
        vector.setDataStart(data);
        vector.setNow(true);
        vector.setStartT(data.getGpsTime());
        vetors.add(vector);
    }

    /**
     * 添加第二个结点为第一个矢量的终止点，并使用一阶贝塞尔曲线求取 f(t)
     * @param data
     */
    public void addSecondData(Data data,double t){//使用一阶贝塞尔生成时间函数
        data.setNumber(2);
        MyVector vector = vetors.get(0);
        vector.setDataEnd(data);
        vector.setEndT(data.getGpsTime());
        Position position = vector.firstBessel(t,this);
        vector.setPosition(position);
        vector.setNow(true);
    }

    public List<MyVector> getVetors() {
        return vetors;
    }

    public void setVetors(List<MyVector> vetors) {
        this.vetors = vetors;
    }

    public int getvSchema() {
        return vSchema;
    }

    public void setvSchema(int vSchema) {
        this.vSchema = vSchema;
    }

    @Override
    public String toString() {
        return "VectorSequence{" +
                "vetors=" + vetors +
                ", vSchema=" + vSchema +
                '}';
    }


}
