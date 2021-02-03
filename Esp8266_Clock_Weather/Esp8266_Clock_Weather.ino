#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <DNSServer.h>//密码直连将其三个库注释
#include <ESP8266WebServer.h>
#include <CustomWiFiManager.h>

#include <time.h>                       
#include <sys/time.h>                  
#include <coredecls.h>      


//#include "SH1106Wire.h"   //1.3寸用这个
#include "SSD1306Wire.h"    //0.96寸用这个
#include "OLEDDisplayUi.h"
#include "HeFeng.h"
#include "WeatherStationFonts.h"
#include "WeatherStationImages.h"

/***************************
   Begin Settings
 **************************/

const char* WIFI_SSID = "Bobo";  //填写你的WIFI名称及密码
const char* WIFI_PWD = "tian842626";

const char* BILIBILIID = "232103885";  //填写你的B站账号

//由于太多人使用我的秘钥，导致获取次数超额，所以不提供秘钥了，大家可以到https://dev.heweather.com/获取免费的
const char* HEFENG_KEY = "462982c4a5d24593b8dc1e90c034b004";//填写你的和风天气秘钥
const char* HEFENG_LOCATION = "CN101190405";//填写你的城市ID,可到https://where.heweather.com/index.html查询
//const char* HEFENG_LOCATION = "auto_ip";//自动IP定位

#define TZ              8      // 中国时区为8
#define DST_MN          0      // 默认为0

const int UPDATE_INTERVAL_SECS = 5 * 60; // 5分钟更新一次天气
const int UPDATE_CURR_INTERVAL_SECS = 2 * 59; // 2分钟更新一次粉丝数

const int I2C_DISPLAY_ADDRESS = 0x3c;  //I2c地址默认
#if defined(ESP8266)
const int SDA_PIN = 0;  //引脚连接
const int SDC_PIN = 2;  //
#endif

const String WDAY_NAMES[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //星期
const String MONTH_NAMES[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};  //月份

// web配网页面自定义我的图标请随便使用一个图片转base64工具转换https://tool.css-js.com/base64.html, 64*64
const char Icon[] PROGMEM = "data:image/png;base64,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";

/***************************
   End Settings
 **************************/
 
//SH1106Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);   // 1.3寸用这个
SSD1306Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);   // 0.96寸用这个
OLEDDisplayUi   ui( &display );

HeFengCurrentData currentWeather; //实例化对象
HeFengForeData foreWeather[3];
HeFeng HeFengClient;

#define TZ_MN           ((TZ)*60)   //时间换算
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

time_t now; //实例化时间

bool readyForWeatherUpdate = false; // 天气更新标志
bool first = true;  //首次更新标志
long timeSinceLastWUpdate = 0;    //上次更新后的时间
long timeSinceLastCurrUpdate = 0;   //上次天气更新后的时间

String fans = "-1"; //粉丝数

void drawProgress(OLEDDisplay *display, int percentage, String label);   //提前声明函数
void updateData(OLEDDisplay *display);
void updateDatas(OLEDDisplay *display);
void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex);
void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
void setReadyForWeatherUpdate();
void webconnect();
//添加框架
//此数组保留指向所有帧的函数指针
//框架是从右向左滑动的单个视图
FrameCallback frames[] = { drawDateTime, drawCurrentWeather, drawForecast };
//页面数量
int numberOfFrames = 3;

OverlayCallback overlays[] = { drawHeaderOverlay }; //覆盖回调函数
int numberOfOverlays = 1;  //覆盖数

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // 屏幕初始化
  display.init();
  display.clear();
  display.display();

  display.flipScreenVertically(); //屏幕翻转
  display.setContrast(255); //屏幕亮度

  //Web配网，密码直连请注释
  webconnect();
  
  // 用固定密码连接，Web配网请注释
  //wificonnect();
 configTime(TZ_SEC, DST_SEC, "ntp.ntsc.ac.cn", "ntp1.aliyun.com"); //ntp获取时间，你也可用其他"pool.ntp.org","0.cn.pool.ntp.org","1.cn.pool.ntp.org","ntp1.aliyun.com"
  ui.setTargetFPS(30);  //刷新频率

  ui.setActiveSymbol(activeSymbole); //设置活动符号
  ui.setInactiveSymbol(inactiveSymbole); //设置非活动符号

  // 符号位置
  // 你可以把这个改成TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(TOP);

  // 定义第一帧在栏中的位置
  ui.setIndicatorDirection(LEFT_RIGHT);

  // 屏幕切换方向
  // 您可以更改使用的屏幕切换方向 SLIDE_LEFT, SLIDE_RIGHT, SLIDE_TOP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  ui.setFrames(frames, numberOfFrames); // 设置框架
  ui.setTimePerFrame(9000); //设置切换时间
  
  ui.setOverlays(overlays, numberOfOverlays); //设置覆盖

  // UI负责初始化显示
  ui.init();
  
//  display.mirrorScreen();//镜像
//  display.flipScreenVertically(); //屏幕反转
  display.mirrorScreen();//镜像
  //configTime(TZ_SEC, DST_SEC, "ntp.ntsc.ac.cn", "ntp1.aliyun.com"); //ntp获取时间，你也可用其他"pool.ntp.org","0.cn.pool.ntp.org","1.cn.pool.ntp.org","ntp1.aliyun.com"
  delay(200);

}

void loop() {
  if (first) {  //首次加载
    updateDatas(&display);
    first = false;
  }
  if (millis() - timeSinceLastWUpdate > (1000L * UPDATE_INTERVAL_SECS)) { //屏幕刷新
    setReadyForWeatherUpdate();
    timeSinceLastWUpdate = millis();
  }
//  if (millis() - timeSinceLastCurrUpdate > (1000L * UPDATE_CURR_INTERVAL_SECS)) { //粉丝数更新
//    HeFengClient.fans(&currentWeather, BILIBILIID);
//    fans = String(currentWeather.follower);
//    timeSinceLastCurrUpdate = millis();
//}

  if (readyForWeatherUpdate && ui.getUiState()->frameState == FIXED) { //天气更新
    updateData(&display);
  }

  int remainingTimeBudget = ui.update(); //剩余时间预算

  if (remainingTimeBudget > 0) {
    //你可以在这里工作如果你低于你的时间预算。
    delay(remainingTimeBudget);
  }
  
}

//void wificonnect() {  //WIFI密码连接，Web配网请注释
//  WiFi.begin(WIFI_SSID, WIFI_PWD);
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(80);
//    display.clear();
//    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_5);
//    display.display();
//    delay(80);
//    display.clear();
//    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_6);
//    display.display();
//    delay(80);
//    display.clear();
//    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_7);
//    display.display();
//    delay(80);
//    display.clear();
//    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_8);
//    display.display();
//    delay(80);
//    display.clear();
//    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_1);
//    display.display();
//    delay(80);
//    display.clear();
//    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_2);
//    display.display();
//    delay(80);
//    display.clear();
//    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_3);
//    display.display();
//    delay(80);
//    display.clear();
//    display.drawXbm(34, 0, bili_Logo_width, bili_Logo_height, bili_Logo_4);
//    display.display();
//  }
//  Serial.println("");
//  delay(500);
//}

void webconnect() {  ////Web配网，密码直连将其注释
//  display.clear();
//  display.drawXbm(0, 0, 128, 64, bilibili); //显示哔哩哔哩
//  display.display();
for(char i=0;i<5;i++)
{
  display.clear();
  display.drawXbm(34, 0, 60, 64, bili_Logo_1); //显示哔哩哔哩
  display.display();
  delay(100);
  display.clear();
  display.drawXbm(34, 0, 60, 64, bili_Logo_2); //显示哔哩哔哩
  display.display();
  delay(100);
   display.clear();
  display.drawXbm(34, 0, 60, 64, bili_Logo_3); //显示哔哩哔哩
  display.display();
  delay(100);
   display.clear();
  display.drawXbm(34, 0, 60, 64, bili_Logo_4); //显示哔哩哔哩
  display.display();
  delay(100);
   display.clear();
  display.drawXbm(34, 0, 60, 64, bili_Logo_5); //显示哔哩哔哩
  display.display();
  delay(100);
   display.clear();
  display.drawXbm(34, 0, 60, 64, bili_Logo_6); //显示哔哩哔哩
  display.display();
  delay(100);
   display.clear();
  display.drawXbm(34, 0, 60, 64, bili_Logo_7); //显示哔哩哔哩
  display.display();
  delay(100);
   display.clear();
  display.drawXbm(34, 0, 60, 64, bili_Logo_8); //显示哔哩哔哩
  display.display();
  delay(300);
}
  WiFiManager wifiManager;  //实例化WiFiManager
  wifiManager.setDebugOutput(false); //关闭Debug
  //wifiManager.setConnectTimeout(10); //设置超时
  wifiManager.setHeadImgBase64(FPSTR(Icon)); //设置图标
  wifiManager.setPageTitle("欢迎来到小田的WiFi配置页");  //设置页标题
  if (!wifiManager.autoConnect("XiaoTian-IOT-WIFI")) {  //AP模式
    Serial.println("连接失败并超时");
    //重新设置并再试一次，或者让它进入深度睡眠状态
    ESP.restart();
    delay(1000);
  }
  Serial.println("connected...^_^");
  yield();
}

void drawProgress(OLEDDisplay *display, int percentage, String label) {    //绘制进度
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 20, label);
  display->drawProgressBar(2, 38, 124, 10, percentage);
  display->display();
}

void updateData(OLEDDisplay *display) {  //天气更新
  HeFengClient.doUpdateCurr(&currentWeather, HEFENG_KEY, HEFENG_LOCATION);
  HeFengClient.doUpdateFore(foreWeather, HEFENG_KEY, HEFENG_LOCATION);
  readyForWeatherUpdate = false;
}

void updateDatas(OLEDDisplay *display) {  //首次天气更新
  drawProgress(display, 0, "Updating fansnumb...");
  HeFengClient.fans(&currentWeather, BILIBILIID);
  fans = String(currentWeather.follower);
  
  drawProgress(display, 33, "Updating weather...");
  HeFengClient.doUpdateCurr(&currentWeather, HEFENG_KEY, HEFENG_LOCATION);
  
  drawProgress(display, 66, "Updating forecasts...");
  HeFengClient.doUpdateFore(foreWeather, HEFENG_KEY, HEFENG_LOCATION);
  
  readyForWeatherUpdate = false;
  drawProgress(display, 100, "Done...");
  delay(200);
  
}

void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  //显示时间
  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[16];

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  String date = WDAY_NAMES[timeInfo->tm_wday];

  sprintf_P(buff, PSTR("%04d-%02d-%02d  %s"), timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday, WDAY_NAMES[timeInfo->tm_wday].c_str());
  display->drawString(64 + x, 20 + y, String(buff));
  display->setFont(ArialMT_Plain_24);

  sprintf_P(buff, PSTR("%02d:%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
  display->drawString(64 + x, 37 + y, String(buff));
  display->setTextAlignment(TEXT_ALIGN_LEFT);
}

void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  //显示天气
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 50 + y, currentWeather.cond_txt + "    |   Wind: " + currentWeather.wind_sc + "  ");

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  String temp = currentWeather.tmp + "°C" ;
  display->drawString(60 + x, 13 + y, temp);
  display->setFont(ArialMT_Plain_10);
  display->drawString(62 + x, 36 + y, currentWeather.fl + "°C | " + currentWeather.hum + "%");
  display->setFont(Meteocons_Plain_36);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(32 + x, 12 + y, currentWeather.iconMeteoCon);
}

void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  //天气预报
  drawForecastDetails(display, x, y, 0);
  drawForecastDetails(display, x + 44, y, 1);
  drawForecastDetails(display, x + 88, y, 2);
}

void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex) {  //天气预报

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y+14, foreWeather[dayIndex].datestr);
  display->setFont(Meteocons_Plain_21);
  display->drawString(x + 20, y + 26, foreWeather[dayIndex].iconMeteoCon);

  String temp = foreWeather[dayIndex].tmp_min + " | " + foreWeather[dayIndex].tmp_max;
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y + 46, temp);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
}

void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {   //绘图页眉覆盖
  now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[14];
  sprintf_P(buff, PSTR("%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min);

  display->setColor(WHITE);
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(6, 0, String(buff));
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  String temp = fans;
  display->drawString(122, 0, temp);
  display->drawHorizontalLine(0, 12, 128);
}

void setReadyForWeatherUpdate() {  //为天气更新做好准备
  Serial.println("Setting readyForUpdate to true");
  readyForWeatherUpdate = true;
}
