#include <Wire.h>
#include "MAX30105.h"
#include <math.h>
#include <ArduinoQueue.h>

MAX30105 particleSensor;
void setup(){
  Serial.begin(115200);
  // Initialize sensor
  if (particleSensor.begin() == false){
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 2; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 8192; //Options: 2048, 4096, 8192, 16384
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}
const int ws = 10;
const int ws_1 = 60;
const float ratio = 0.4;
float gaussianWindow[9] = {0};
float gaussianKernel[9] = {0.000133830625,0.00443186162,0.0539911274,0.241971446,0.398943469,0.241971446,0.0539911274,0.00443186162,0.000133830625};
float daiqiudao[3] = {0};
float fuzhijiance[ws_1] = {0};
float window[2*ws+1] = {0};
int blinkCounter = 0;

void loop()
{
  uint32_t Red = particleSensor.getIR();
  uint32_t IR = particleSensor.getRed();
  Serial.print(IR);
  Serial.print(',');
  float tmp = float(IR);
  for(int i=0; i<8; i++){
    gaussianWindow[i] = gaussianWindow[i+1];
  }
  gaussianWindow[8] = (float) tmp;
  float sum = 0;
  for(int i=0;i<9;i++){
    sum += gaussianWindow[i]*gaussianKernel[i];//半窗宽为4，σ为1的的高斯滤波
  }
  Serial.print(sum);
  Serial.print(',');
  for(int i=0; i<2; i++){
    daiqiudao[i] = daiqiudao[i+1]; //将高斯滤波结果放入daiqiudao
  }
  daiqiudao[2] = sum;
  float daoshu = (daiqiudao[2]-daiqiudao[0])/2; //求一阶导
  Serial.print(daoshu);
  Serial.print(',');
  for(int i=0;i<ws*2;i++){
    window[i] = window[i+1];
  }
  window[ws*2] = daoshu; //将一阶导放入window
  for(int i=0;i<ws_1-1;i++){
    fuzhijiance[i]=fuzhijiance[i+1];
  }
  fuzhijiance[ws_1-1] = daoshu;//将一阶导放入fuzhijiance中
  //准备修改的部分，核心眨眼检测
  int blink_ = 0;
  if(window[ws]<=window[ws-1]&&window[ws]<=window[ws+1]){
    int pre = 0;
    int post = ws*2;
    for(int i= ws;i<2*ws-1;i++){
      if(window[i]>window[i+1]){
        post = i;
        break;
      }
    }
    for(int i= ws;i>0;i--){
      if(window[i]>window[i-1]){
        pre = i;
        break;
      }
    }
    float a = window[pre];
    float b = window[ws];
    float c = window[post];
    int min_ = 0;
    int max_ = 0;
    for(int i=0;i<ws_1;i++){
      if(fuzhijiance[i]<min_){
        min_ = fuzhijiance[i];
      }
      else if(fuzhijiance[i]>max_){
        max_ = fuzhijiance[i];
      }
    }
    if(c>a&&c>0&&b<0&&(max_-min_)*ratio<=(c-b)&&max_-min_>50){
      blink_ = 20;
    }
  }//极小值判定
  Serial.print(blinkCounter);
  Serial.print(',');
  Serial.print(0);
  Serial.print(',');
  Serial.println(blink_);
}
