
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "HeFeng.h"

HeFeng::HeFeng() {
}

void HeFeng::fans(HeFengCurrentData *data, String id) {  //获取粉丝数
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  String url = "https://api.bilibili.com/x/relation/stat?vmid=" + id;
  Serial.print("[HTTPS] begin...bilibili\n");
  if (https.begin(*client, url)) {  // HTTPS
    // start connection and send HTTP header
    int httpCode = https.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        Serial.println(payload);
        DynamicJsonDocument  jsonBuffer(2048);
        deserializeJson(jsonBuffer, payload);
        JsonObject root = jsonBuffer.as<JsonObject>();

        String follower = root["data"]["follower"];
        data->follower = "666";
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      data->follower = "-1";
    }

    https.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
    data->follower = "-1";
  }
}

void HeFeng::doUpdateCurr(HeFengCurrentData *data, String key, String location) {  //获取天气

  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  String url = "https://free-api.heweather.net/s6/weather/now?lang=en&location=" + location + "&key=" + key;
  Serial.print("[HTTPS] begin...now\n");
  if (https.begin(*client, url)) {  // HTTPS
    // start connection and send HTTP header
    int httpCode = https.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        Serial.println(payload);
        DynamicJsonDocument  jsonBuffer(2048);
        deserializeJson(jsonBuffer, payload);
        JsonObject root = jsonBuffer.as<JsonObject>();

        String tmp = root["HeWeather6"][0]["now"]["tmp"];
        data->tmp = tmp;
        String fl = root["HeWeather6"][0]["now"]["fl"];
        data->fl = fl;
        String hum = root["HeWeather6"][0]["now"]["hum"];
        data->hum = hum;
        String wind_sc = root["HeWeather6"][0]["now"]["wind_sc"];
        data->wind_sc = wind_sc;
        String cond_code = root["HeWeather6"][0]["now"]["cond_code"];
        String meteoconIcon = getMeteoconIcon(cond_code);
        String cond_txt = root["HeWeather6"][0]["now"]["cond_txt"];
        data->cond_txt = cond_txt;
        data->iconMeteoCon = meteoconIcon;

      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      data->tmp = "-1";
      data->fl = "-1";
      data->hum = "-1";
      data->wind_sc = "-1";
      data->cond_txt = "no network";
      data->iconMeteoCon = ")";
    }

    https.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
    data->tmp = "-1";
    data->fl = "-1";
    data->hum = "-1";
    data->wind_sc = "-1";
    data->cond_txt = "no network";
    data->iconMeteoCon = ")";
  }

}

//void HeFeng::doUpdateFore(HeFengForeData *data, String key, String location) {  //获取预报
//
//  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
//  client->setInsecure();
//  HTTPClient https;
//  String url = "https://devapi.qweather.com/v7/weather/3d?location=" + location + "&key=" + key + "&gzip=n";
////  String url = "https://devapi.qweather.com/v7/weather/3d?location=101190405&key=462982c4a5d24593b8dc1e90c034b004&gzip=n";
//  Serial.print("[HTTPS] begin...forecast\n");
//  if (https.begin(*client, url)) {  // HTTPS
//    // start connection and send HTTP header
//    int httpCode = https.GET();
//    // httpCode will be negative on error
//    if (httpCode > 0) {
//      // HTTP header has been send and Server response header has been handled
//      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
//
//      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
//        String payload = https.getString();
//        Serial.println(payload);
//        DynamicJsonDocument  jsonBuffer(8192);
//        deserializeJson(jsonBuffer, payload);
//        JsonObject root = jsonBuffer.as<JsonObject>();
//        int i;
//
//        for (i = 0; i < 3; i++) {
//          String tmp_min = root["daily"][i]["tempMin"];
//          data[i].tmp_min = tmp_min;
//          Serial.print("tmp_min:");
//          Serial.println(tmp_min);
//          String tmp_max = root["daily"][i]["tempMax"];
//          data[i].tmp_max = tmp_max;
//          Serial.print("tmp_max:");
//          Serial.println(tmp_max);
//          String datestr = root["daily"][i]["fxDate"];
//          data[i].datestr = datestr.substring(5, datestr.length());
//          String cond_code = root["daily"][i]["iconDay"];
//          Serial.print("天气:");
//          Serial.println(cond_code);
//          String meteoconIcon = getMeteoconIcon(cond_code);
//          data[i].iconMeteoCon = meteoconIcon;
//          if(tmp_min=="null")
//          {
//            String tmp_min = root["HeWeather6"][0]["daily_forecast"][i]["tmp_min"];
//            data[i].tmp_min = tmp_min;
//            Serial.print("tmp_min1:");
//            Serial.println(tmp_min);
//            String tmp_max = root["HeWeather6"][0]["daily_forecast"][i]["tmp_max"];
//            data[i].tmp_max = tmp_max;
//            Serial.print("tmp_max1:");
//            Serial.println(tmp_max);
//            String datestr = root["HeWeather6"][0]["daily_forecast"][i]["date"];
//            data[i].datestr = datestr.substring(5, datestr.length());
//            String cond_code = root["HeWeather6"][0]["daily_forecast"][i]["cond_code_d"];
//            Serial.print("天气1:");
//            Serial.println(cond_code);
//            String meteoconIcon = getMeteoconIcon(cond_code);
//            data[i].iconMeteoCon = meteoconIcon;
//          } 
//        
//          
//        }
//      }
//    } else {
//      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
//      int i;
//      for (i = 0; i < 3; i++) {
//        data[i].tmp_min = "-1";
//        data[i].tmp_max = "-1";
//        data[i].datestr = "N/A";
//        data[i].iconMeteoCon = ")";
//      }
//    }
//    https.end();
//  } else {
//    Serial.printf("[HTTPS] Unable to connect\n");
//    int i;
//    for (i = 0; i < 3; i++) {
//      data[i].tmp_min = "-1";
//      data[i].tmp_max = "-1";
//      data[i].datestr = "N/A";
//      data[i].iconMeteoCon = ")";
//    }
//  }
//
//}

void HeFeng::doUpdateFore(HeFengForeData *data, String key, String location) {

  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure(); // 不进行服务器身份认证
  HTTPClient https;
  String payload;
 
  Serial.print("[HTTPS] begin...\n");
 
  String url = "https://devapi.heweather.net/v7/weather/3d?location=" + location +"&key=" + key  + "&lang=en" + "&gzip=n";
  if (https.begin(*client, url)) {  // HTTPS连接成功
   
    Serial.print("[HTTPS] GET...\n");
   
    int httpCode = https.GET(); // 请求

    if (httpCode > 0) { // 错误返回负值
     
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
     
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) { // 服务器响应
         payload = https.getString();
       
        Serial.println(payload);

        const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(2) +
                          JSON_OBJECT_SIZE(5) + 3*JSON_OBJECT_SIZE(26) + 1320;
        DynamicJsonDocument doc(capacity);
      
        deserializeJson(doc, payload);
        
        const char* code = doc["code"];
        const char* updateTime = doc["updateTime"];
        JsonArray daily = doc["daily"];
        JsonObject daily_0 = daily[0];
        JsonObject daily_1 = daily[1];
        JsonObject daily_2 = daily[2];
      
        _response_code = doc["code"].as<String>();
        _last_update_str = doc["updateTime"].as<String>();
        String datestr = daily_0["fxDate"].as<String>();          //获取第一天日期
        data[0].datestr = datestr.substring(5, datestr.length());
        datestr = daily_1["fxDate"].as<String>();                 //获取第二天日期
        data[1].datestr = datestr.substring(5, datestr.length());
        datestr = daily_2["fxDate"].as<String>();                 //获取第三天日期
        data[2].datestr = datestr.substring(5, datestr.length());
        data[0].tmp_max = daily_0["tempMax"].as<int>();           //获取第一天高温
        data[1].tmp_max = daily_1["tempMax"].as<int>();           //获取第二天高温
        data[2].tmp_max = daily_2["tempMax"].as<int>();           //获取第三天高温
         data[0].tmp_min = daily_0["tempMin"].as<int>();          //获取第一天低温
         data[1].tmp_min = daily_1["tempMin"].as<int>();          //获取第二天低温
         data[2].tmp_min = daily_2["tempMin"].as<int>();          //获取第三天低温

        _daily_iconDay_int[0] = daily_0["iconDay"].as<int>();                   //获取第一天天气图标
        String meteoconIcon = getMeteoconIcon(String(_daily_iconDay_int[0]));
        data[0].iconMeteoCon = meteoconIcon;
        _daily_iconDay_int[1] = daily_1["iconDay"].as<int>();                   //获取第二天天气图标
        meteoconIcon = getMeteoconIcon(String(_daily_iconDay_int[1]));
        data[1].iconMeteoCon = meteoconIcon;
        _daily_iconDay_int[2] = daily_2["iconDay"].as<int>();                   //获取第三天天气图标
        meteoconIcon = getMeteoconIcon(String(_daily_iconDay_int[2]));
        data[2].iconMeteoCon = meteoconIcon;
//        _daily_textDay_str[0] = daily_0["textDay"].as<String>();
//        _daily_textDay_str[1] = daily_1["textDay"].as<String>();
//        _daily_textDay_str[2] = daily_2["textDay"].as<String>();
//        _daily_windDirDay_str[0] = daily_0["windDirDay"].as<String>();
//        _daily_windDirDay_str[1] = daily_1["windDirDay"].as<String>();
//        _daily_windDirDay_str[2] = daily_2["windDirDay"].as<String>();
//        _daily_windScaleDay_str[0] = daily_0["windScaleDay"].as<String>();
//        _daily_windScaleDay_str[1] = daily_1["windScaleDay"].as<String>();
//        _daily_windScaleDay_str[2] = daily_2["windScaleDay"].as<String>();
//        _daily_humidity_int[0] = daily_0["humidity"].as<int>();
//        _daily_humidity_int[1] = daily_1["humidity"].as<int>();
//        _daily_humidity_int[2] = daily_2["humidity"].as<int>();
//        _daily_precip_float[0] = daily_0["precip"].as<float>();
//        _daily_precip_float[1] = daily_1["precip"].as<float>();
//        _daily_precip_float[2] = daily_2["precip"].as<float>();
//        _daily_uvIndex_int[0] = daily_0["uvIndex"].as<int>();
//        _daily_uvIndex_int[1] = daily_1["uvIndex"].as<int>();
//        _daily_uvIndex_int[2] = daily_2["uvIndex"].as<int>();



       
//        _parseNowJson(payload);
//        return true;
      }
    } else { // 错误返回负值
     
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      int i;
      for (i = 0; i < 3; i++) {
        data[i].tmp_min = "-1";
        data[i].tmp_max = "-1";
        data[i].datestr = "N/A";
        data[i].iconMeteoCon = ")";
      }

    }
    https.end();
  } else { // HTTPS连接失败

      Serial.printf("[HTTPS] Unable to connect\n");
      int i;
      for (i = 0; i < 3; i++) {
        data[i].tmp_min = "-1";
        data[i].tmp_max = "-1";
        data[i].datestr = "N/A";
        data[i].iconMeteoCon = ")";
      }
  }
  

  
}



String HeFeng::getMeteoconIcon(String cond_code) {  //获取天气图标
  if (cond_code == "100" || cond_code == "9006") {
    return "B";
  }
  if (cond_code == "999") {
    return ")";
  }
  if (cond_code == "104") {
    return "D";
  }
  if (cond_code == "500") {
    return "E";
  }
  if (cond_code == "503" || cond_code == "504" || cond_code == "507" || cond_code == "508") {
    return "F";
  }
  if (cond_code == "499" || cond_code == "901") {
    return "G";
  }
  if (cond_code == "103") {
    return "H";
  }
  if (cond_code == "502" || cond_code == "511" || cond_code == "512" || cond_code == "513") {
    return "L";
  }
  if (cond_code == "501" || cond_code == "509" || cond_code == "510" || cond_code == "514" || cond_code == "515") {
    return "M";
  }
  if (cond_code == "102") {
    return "N";
  }
  if (cond_code == "213") {
    return "O";
  }
  if (cond_code == "302" || cond_code == "303") {
    return "P";
  }
  if (cond_code == "305" || cond_code == "308" || cond_code == "309" || cond_code == "314" || cond_code == "399") {
    return "Q";
  }
  if (cond_code == "306" || cond_code == "307" || cond_code == "310" || cond_code == "311" || cond_code == "312" || cond_code == "315" || cond_code == "316" || cond_code == "317" || cond_code == "318") {
    return "R";
  }
  if (cond_code == "200" || cond_code == "201" || cond_code == "202" || cond_code == "203" || cond_code == "204" || cond_code == "205" || cond_code == "206" || cond_code == "207" || cond_code == "208" || cond_code == "209" || cond_code == "210" || cond_code == "211" || cond_code == "212") {
    return "S";
  }
  if (cond_code == "300" || cond_code == "301") {
    return "T";
  }
  if (cond_code == "400" || cond_code == "408") {
    return "U";
  }
  if (cond_code == "407") {
    return "V";
  }
  if (cond_code == "401" || cond_code == "402" || cond_code == "403" || cond_code == "409" || cond_code == "410") {
    return "W";
  }
  if (cond_code == "304" || cond_code == "313" || cond_code == "404" || cond_code == "405" || cond_code == "406") {
    return "X";
  }
  if (cond_code == "101") {
    return "Y";
  }
  return ")";
}
