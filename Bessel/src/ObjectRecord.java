import java.util.ArrayList;
import java.util.List;

public class ObjectRecord {//数据分布式存储
    private List<VectorSequence> vectorSequences = new ArrayList<>();//不同分量的状态序列集合
    private List<Integer> rawAddrVector = new ArrayList<>();//服务器地址表
    private int objId;
    private String objDescript;
}
