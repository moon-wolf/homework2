#include<MsTimer2.h>
#include<Arduino.h>
#include<ros.h>
#include<std_msgs/Int16.h>
ros::NodeHandle nh;

volatile boolean tim1msF = false;
volatile double valR=0, valL=0;
volatile uint8_t prevR=0, prevL=0;
double distance=0, radian=0, degree=0, pulse=0, pulse1=0, degMotor=0, degTag=0;
int pin1=21, pin2=2, pin3=20, pin4=19;
const int pin5=11, pin6=12, pin7=6, pin8=7;// pin5,6 = right　pin6,7 = left
int count_time=0, judge1=0, judge2=0;

void updateEncoderR(){
  uint8_t a = digitalRead(pin1);
  uint8_t b = digitalRead(pin2);
  uint8_t ab = (a << 1) | b;
  uint8_t encR  = (prevR << 2) | ab;
  if(encR == 0b1101 || encR == 0b0100 || encR == 0b0010 || encR == 0b1011){
    valR ++;
  }else if(encR == 0b1110 || encR == 0b0111 || encR == 0b0001 || encR == 0b1000) {
    valR --;
  }
  prevR = ab;
}

void updateEncoderL(){
  uint8_t c = digitalRead(pin3);
  uint8_t d = digitalRead(pin4);
  uint8_t cd = (c << 1) | d;
  uint8_t encL  = (prevL << 2) | cd;
  if(encL == 0b1101 || encL == 0b0100 || encL == 0b0010 || encL == 0b1011){
    valL ++;
  }else if(encL == 0b1110 || encL == 0b0111 || encL == 0b0001 || encL == 0b1000) {
    valL --;
  }
  prevL = cd;
}

void timer1mS(){
  tim1msF = true;
}
int Timer(int n){
  MsTimer2::set(1, timer1mS);
  MsTimer2::start();
  while(count_time <= n){
    if(tim1msF){
      count_time = count_time + 1;
      tim1msF = false;
    }
  }
  MsTimer2::stop();
  tim1msF = false;
  count_time = 0;
  return 0;
}

void turn(int degTag){
  Timer(2000);
  valR = 0;
  valL = 0;
  pulse = 0;
  pulse1 = 0;
//機体の旋回角度degTagが正の値の時、右旋回 (pulseが正、pulse1が負)
  if(degTag >= 0) {
    degTag=degTag;
    degMotor = 4.3 * degTag + 8.0;
    pulse = degMotor / 2.7;
    pulse1 = -pulse;
 
  while(degTag >= 0){                                //機体の旋回角度degTagが正の値の時、右旋回 (pulseが正、pulse1が負)
    nh.spinOnce();
    if(valR >= pulse1 -10 && valR <= pulse1 +10){    //pulse-5 <= valR <= pulse+5で停止
      analogWrite(pin5,0);
      analogWrite(pin6,0);
      judge1 = 1;
    }
    else if(valR < pulse1 -10){                   //valRがpulse1 -10を超えると、正転で戻る
      analogWrite(pin5,170);
      analogWrite(pin6,0);
    }
    else if(valR <= pulse1 +30){                   //valRがpulse1 +30を超えると、逆転の速度を落とす
      analogWrite(pin5,0);
      analogWrite(pin6,170);
    }
    else{
      analogWrite(pin5,0);                         //右クローラを逆転
      analogWrite(pin6,180);
    }
    if(valL >= pulse -5 && valL <= pulse +5){      //pulse -5 <= valL <= pulse +5の時、停止
      analogWrite(pin7,0);
      analogWrite(pin8,0);
      judge2 = 1;
    }
    else if(valL > pulse +10){                    //valLがpulse +10を超えると、逆転で戻る
      analogWrite(pin7,0);
      analogWrite(pin8,170);
    }
    else if(valL >= pulse -30){                    //valLがpulse -30を超えると、正転の速度を落とす
      analogWrite(pin7,170);
      analogWrite(pin8,0);
    } 
    else{
      analogWrite(pin7,180);                       //左クローラを正転
      analogWrite(pin8,0);
    }
    if(judge1 == 1 && judge2 == 1){
      judge1 = 0;
      judge2 = 0;
      valR = 0;
      valL = 0;
      pulse = 0;
      pulse1 = 0;
      break;
    }
  }
  }
  
 //機体の旋回角度degTagが負の値の時、左旋回 (pulseが正、pulse1が負)   
  if(degTag < 0){
    degMotor = 4.3 * degTag - 8.0;
    pulse = degMotor / 2.7;
    pulse1 = -pulse;
  while(1){                               //機体の旋回角度degTagが負の値の時、左旋回 (pulseが負、pulse1が正)   
    nh.spinOnce();
    if(valR >= pulse1 -10 && valR <= pulse1 +10){    //pulse1 -5 <= valR <= pulse1 +5の時、停止
      analogWrite(pin5,0);
      analogWrite(pin6,0);
      judge1 = 1;
    }
    else if(valR > pulse1 +10){                   //valRがpulse1 +10を超えると、逆転で戻る
      analogWrite(pin5,0);
      analogWrite(pin6,170);
    }
    else if(valR >= pulse1 -30){                   //valRがpulse1 -30を超えると、正転の速度を落とす
      analogWrite(pin5,170);
      analogWrite(pin6,0);
    } 
    else{
      analogWrite(pin5,180);                       //右クローラを正転
      analogWrite(pin6,0);
    }
    if( valL >= pulse -10 && valL <= pulse +10){     //pulse -5 <= valL <= pulse +5で停止
      analogWrite(pin7,0);
      analogWrite(pin8,0);
      judge2 = 1;
    }
    else if(valL < pulse -10){                    //valLがpulse -10を超えると、正転で戻る
      analogWrite(pin7,170);
      analogWrite(pin8,0);
    }
    else if(valL <= pulse +30){                    //valLがpulse +30を超えると、逆転の速度を落とす
      analogWrite(pin7,0);
      analogWrite(pin8,170);
    }
    else{
      analogWrite(pin7,0);                         //左クローラを逆転
      analogWrite(pin8,180);
    }
    if(judge1 == 1 && judge2 == 1){
      judge1 = 0;
      judge2 = 0;
      valR = 0;
      valL = 0;
      pulse = 0;
      pulse1 = 0;
      break;
    }
  }
  }
}

void order1(const std_msgs::Int16 &message){
  const int degree = message.data;
  turn(degree);
}
ros::Subscriber<std_msgs::Int16> Destination("/control", &order1);

void setup(){
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(pin4, INPUT);
  pinMode(pin5,OUTPUT); 
  pinMode(pin6,OUTPUT); 
  pinMode(pin7,OUTPUT); 
  pinMode(pin8,OUTPUT); 
  attachInterrupt(0, updateEncoderR, CHANGE);
  attachInterrupt(2, updateEncoderR, CHANGE);
  attachInterrupt(4, updateEncoderL, CHANGE);
  attachInterrupt(3, updateEncoderL, CHANGE);
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, HIGH);
  nh.initNode();
  nh.subscribe(Destination);
}

void loop(){
  nh.spinOnce();
  Timer(1);
}
