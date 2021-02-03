
#pragma once
#include <ArduinoJson.h>

typedef struct HeFengCurrentData {

  String cond_txt;
  String fl;
  String tmp;
  String hum;
  String wind_sc;
  String iconMeteoCon;
  String follower;
}
HeFengCurrentData;
typedef struct HeFengForeData {
  String datestr;
  String tmp_min;
  String tmp_max;
  String iconMeteoCon;

}
HeFengForeData;
class HeFeng {
  private:
    String getMeteoconIcon(String cond_code);
  public:
    HeFeng();
    void doUpdateCurr(HeFengCurrentData *data, String key, String location);
    void doUpdateFore(HeFengForeData *data, String key, String location);
    void fans(HeFengCurrentData *data, String id);
    
    String _response_code = "no_init";           // API状态码
    String _last_update_str = "no_init";         // API最近更新时间
    String _daily_sunrise_str[3] = {"no_init", "no_init", "no_init"}; // 日出时间
    int _daily_tempMax_int[3] = {999, 999, 999}; // 最高气温
    int _daily_tempMin_int[3] = {999, 999, 999}; // 最低气温
    int _daily_iconDay_int[3] = {999, 999, 999}; // 白天天气状况的图标代码
    String _daily_textDay_str[3] = {"no_init", "no_init", "no_init"};      // 天气状况的文字描述
    String _daily_windDirDay_str[3] = {"no_init", "no_init", "no_init"};   // 白天风向
    String _daily_windScaleDay_str[3] = {"no_init", "no_init", "no_init"}; // 白天风力等级
    int _daily_humidity_int[3] = {999, 999, 999};   // 相对湿度百分比数值
    float _daily_precip_float[3] = {999, 999, 999}; // 降水量，毫米
    int _daily_uvIndex_int[3] = {999, 999, 999};    // 紫外线强度指数
};
