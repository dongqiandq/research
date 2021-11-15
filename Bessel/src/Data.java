import java.util.Date;

public class Data {
    private int number;//标记是第一个还是第二个采样点
    private long id;
    private String centerId;//标识调度中心
    private String simCode;//终端号
    private String vehicleCode;//车牌号
    private Date gpsTime;//数据生成时间
    private double longitude;//经度
    private double latitude;//纬度
    private double speed;//速度
    private int vehState;//车辆状态
    private int gpsState;//经纬度定位是否有效，0无效，1有效
    private int aspect;//方向，正北偏东的方向角范围（0-359）

    public int getNumber() {
        return number;
    }

    public void setNumber(int number) {
        this.number = number;
    }

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    public String getCenterId() {
        return centerId;
    }

    public void setCenterId(String centerId) {
        this.centerId = centerId;
    }

    public String getSimCode() {
        return simCode;
    }

    public void setSimCode(String simCode) {
        this.simCode = simCode;
    }

    public String getVehicleCode() {
        return vehicleCode;
    }

    public void setVehicleCode(String vehicleCode) {
        this.vehicleCode = vehicleCode;
    }

    public Date getGpsTime() {
        return gpsTime;
    }

        public void setGpsTime(Date gpsTime) {
        this.gpsTime = gpsTime;
    }

    public double getLongitude() {
        return longitude;
    }

    public void setLongitude(double longitude) {
        this.longitude = longitude;
    }

    public double getLatitude() {
        return latitude;
    }

    public void setLatitude(double latitude) {
        this.latitude = latitude;
    }

    public double getSpeed() {
        return speed;
    }

    public void setSpeed(double speed) {
        this.speed = speed;
    }

    public int getVehState() {
        return vehState;
    }

    public void setVehState(int vehState) {
        this.vehState = vehState;
    }

    public int getGpsState() {
        return gpsState;
    }

    public void setGpsState(int gpsState) {
        this.gpsState = gpsState;
    }

    public int getAspect() {
        return aspect;
    }

    public void setAspect(int aspect) {
        this.aspect = aspect;
    }

    @Override
    public String toString() {
        return "Data{" +
                "id=" + id +
                ", VehicleCode='" + vehicleCode + '\'' +
                ", gpsTime=" + gpsTime +
                ", longitude=" + longitude +
                ", latitude=" + latitude +
                ", speed=" + speed +
                '}';
    }

}
