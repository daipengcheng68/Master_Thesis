#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define black 0
#define white 1

#define echopin 2
#define trigpin 3
long duration;
float distance;

//servo
int lf = 0;
int rf = 1;
int lb = 2;
int rb = 3;
int lift = 15;
static float pos_init_speed = 437.5, neg_init_speed = 212.5;
//int servostop = 325;
//int servomax = 550;
//int servomin = 100;
//cny position
int frr,fmr,fm,fml,fll;
int lff,lmf,lm,lmb,lbb;
int rff,rmf,rm,rmb,rbb;


int cross = 0;

int f_val, l_val, r_val;
int f_error = 0, s_error = 0;
      //2.75      0.0015
float kp=2.75, ki=0.001, kd=3;
float kps=2, kis=0.001, kds=3;
float pid_f_val=0, f_p = 0, f_i = 0, f_d = 0;
float pre_f_error=0, pre_f_i=0;
float pid_s_val=0, s_p = 0, s_i = 0, s_d = 0;
float pre_s_error=0, pre_s_i=0;

float lf_speed, rf_speed, lb_speed, rb_speed;

void fs_read(); 
void cal_f_error();
void cal_sl_error();
void cal_sr_error();
void cal_f_pid();
void cal_s_pid();
void servo_f_control();
void servo_sl_control();
void servo_sr_control();
void lturn();
void rturn();
void forward();
void backward();
void directright();
void directleft();
void nothing();
void liftup();
void liftdown();

void A2B();
void A2C();
void A2D();
void A2G();
void onestep();
void Lonestep();
void Ronestep();
void LT180();
void dis();
void checkpos();

int flag = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pwm.begin();
  pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates

  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  
  //f
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  //l
  pinMode(A5,INPUT);
  pinMode(A6,INPUT);
  pinMode(A7,INPUT);
  pinMode(A8,INPUT);
  pinMode(A9,INPUT);
  //r
  pinMode(A10,INPUT);
  pinMode(A11,INPUT);
  pinMode(A12,INPUT);
  pinMode(A13,INPUT);
  pinMode(A14,INPUT);

  Reset();
  nothing();
  delay(1000);  
}


void loop() {
//  Serial.print("O"); 
  
  if(Serial.available()){
    char c = Serial.read();
//    Serial.println(c);
    Reset();
    switch(c){
      case 'f':
        OneStep();  
        nothing();
        Serial.println("F");
        break;
      case 'l':
        LeftOneStep();
        directleft();
        delay(500);
        nothing();
        Serial.println("L");
        break;
      case 'r':
        RightOneStep();
        directright();
        delay(500);
        nothing();
        Serial.println("R");
        break;
      case 's':
        nothing();
        Serial.println("S");
        break;
      case 't':
        LT180();
        nothing();
        Serial.println("T");
        break;
      case 'u':
        liftup();
        Serial.println("U");
        break;
      case 'd':
        liftdown();
        Serial.println("D");
        break;
    }  
  }  

//  fs_read();
//  cal_f_error();
//  cal_f_pid();
//  servo_f_control();
//  liftup();
//  delay(500);
//  liftdown();
}

void Reset(){
  pre_f_error = 0;
  pre_s_error = 0;  
}
//
//int ToThePoint(){
//  if(f_val==0){
//    return 1;
//  }
//  return 0;
//}

void OneStep(){
  while(true){
    fs_read();
    cal_f_error();
    cal_f_pid();
    servo_f_control();
    if (f_val==0){
      forward();
      delay(500);
      while(true){
        fs_read();
        cal_f_error();
        cal_f_pid();
        servo_f_control();
        if (lm == 0 || rm == 0){
          break;
        }
      }
      break;
    }
  }
}
void LeftOneStep(){
  while(true){
    fs_read();
    cal_sl_error();
    cal_s_pid();
    servo_sl_control();
    if (l_val==0){
      break;
    }
  }
}
void RightOneStep(){
  while(true){
    fs_read();
    cal_sr_error();
    cal_s_pid();
    servo_sr_control();
    if (r_val==0){
      break;
    }
  }
}

void onestep(){
    switch (cross){
    case 0:
      fs_read();
      cal_f_error();
      cal_f_pid();
      servo_f_control();
      if (f_val == 0){
        cross ++;
        
      }break;
    case 1:
      forward();
      delay(500);
      nothing();
//      delay(5);
      break;
    }
}

void Lonestep(){
  switch (cross){
    case 0:
      fs_read();
      cal_sl_error();
      cal_s_pid();
      servo_sl_control();
      if (l_val == 0){
        cross ++;
        directleft();
        delay (500);
      }
    case 1:
      nothing();
      break;
  }
}
void Ronestep(){
  switch (cross){
    case 0:
      fs_read();
      cal_sr_error();
      cal_s_pid();
      servo_sr_control();
      if (r_val == 0){
        cross ++;
        directright();
        delay (500);
      }
    case 1:
      nothing();
      break;
  }
}

void LT180(){
  lturn();
  delay(3500);
  fs_read();
  while (fm!=0){
    lturn();
    fs_read();
  }
}

void fs_read(){

  if (analogRead(A0)<250){
    frr = 0;
  }else{
    frr=1;
  }
  if (analogRead(A1)<250){
    fmr = 0;
  }else {
    fmr = 1;
  }
  if (analogRead(A2)<250){
    fm  = 0;
  }else{
    fm  = 1;
  }
  if (analogRead(A3)<250){
    fml = 0;
  }else{
    fml = 1;
  }
  if (analogRead(A4)<250){
    fll = 0;
  }else{
    fll = 1;
  }
//  Serial.print("frr= ");
//  Serial.print(frr);
//  Serial.print("\t");
//  Serial.print("fmr= ");
//  Serial.print(fmr);
//  Serial.print("\t");
//  Serial.print("fm= ");
//  Serial.print(fm);
//  Serial.print("\t");
//  Serial.print("fml= ");
//  Serial.print(fml);
//  Serial.print("\t");
//  Serial.print("fll= ");
//  Serial.print(fll);
//  Serial.print("\t ");

  if (analogRead(A10)<250){
    lff = 0;
  }else{
    lff = 1;
  }
  if (analogRead(A11)<250){
    lmf = 0;
  }else{
    lmf = 1;
  }
  if (analogRead(A12)<250){
    lm  = 0;
  }else{
    lm  = 1;
  }
  if (analogRead(A13)<250){
    lmb = 0;
  }else{
    lmb = 1;
  }
  if (analogRead(A14)<250){
    lbb = 0;
  }else{
    lbb = 1;
  }
//  Serial.print("lff= ");
//  Serial.print(lff);
//  Serial.print("\t");
//  Serial.print("lmf= ");
//  Serial.print(lmf);
//  Serial.print("\t");
//  Serial.print("lm= ");
//  Serial.print(lm);
//  Serial.print("\t");
//  Serial.print("lmb= ");
//  Serial.print(lmb);
//  Serial.print("\t");
//  Serial.print("lbb= ");
//  Serial.print(lbb);
//  Serial.print("\t");  

  if (analogRead(A5)<250){
    rff = 0;
  }else{
    rff = 1;
  }
  if (analogRead(A6)<250){
    rmf = 0;
  }else{
    rmf = 1;
  }
  if (analogRead(A7)<250){
    rm  = 0;
  }else{
    rm  = 1;
  }
  if (analogRead(A8)<250){
    rmb = 0;
  }else{
    rmb = 1;
  }
  if (analogRead(A9)<250){
    rbb = 0;
  }else{
    rbb = 1;
  }
//  Serial.print("rff= ");
//  Serial.print(rff);
//  Serial.print("\t");
//  Serial.print("rmf= ");
//  Serial.print(rmf);
//  Serial.print("\t");
//  Serial.print("rm= ");
//  Serial.print(rm);
//  Serial.print("\t");
//  Serial.print("rmb= ");
//  Serial.print(rmb);
//  Serial.print("\t");
//  Serial.print("rbb= ");
//  Serial.print(rbb);
//  Serial.println("");  
}

void cal_f_error(){
  if (frr==1 && fmr==1 && fm==1 && fml==1 && fll==1){
    f_val = 11111;
  }else if (frr==1 && fmr==1 && fm==0 && fml==1 && fll==1){
    f_val = 11011;
  }else if (frr==1 && fmr==0 && fm==0 && fml==1 && fll==1){
    f_val = 10011;
  }else if (frr==1 && fmr==0 && fm==1 && fml==1 && fll==1){
    f_val = 10111;
  }else if (frr==0 && fmr==1 && fm==1 && fml==1 && fll==1){
    f_val = 1111;
  }else if (frr==1 && fmr==1 && fm==0 && fml==0 && fll==1){
    f_val = 11001;
  }else if (frr==1 && fmr==1 && fm==1 && fml==0 && fll==1){
    f_val = 11101;
  }else if (frr==1 && fmr==1 && fm==1 && fml==1 && fll==0){
    f_val = 11110;
  }else if (frr==0 && fmr==0 && fm==0 && fml==0 && fll==0 || 
            frr==0 && fmr==0 && fm==0 || 
            fm==0 && fml==0 && fll==0){
    f_val = 0;
  }

  switch (f_val){
    case 11111:
      if (f_error<0){
        f_error = -7;
      }else if (f_error>0){
        f_error =  7;
      }break;
    case 11011:
      if (f_error<0){
        f_error = -5;
      }else if (f_error>0){
        f_error =  5;
      }else{
        f_error = 0;
      }break;
    case 10011: f_error =  5; break;
    case 10111: f_error =  9; break;
//    case 1111:  f_error =  9; break;
    case 11001: f_error = -5; break;
    case 11101: f_error = -9; break;
//    case 11110: f_error = -9; break;
    case 0:     f_error =  pre_f_error; break;      
  }
//  Serial.print("f_error= ");
//  Serial.print(f_error);
//  Serial.print("\t");
}

void cal_sl_error(){ //lff,lmf,lm,lmb,lbb
  if (lff==1 && lmf==1 && lm==1 && lmb==1 && lbb==1){
    l_val = 11111;
  }else if (lff==1 && lmf==1 && lm==0 && lmb==1 && lbb==1){
    l_val = 11011;
  }else if (lff==1 && lmf==0 && lm==0 && lmb==1 && lbb==1){
    l_val = 10011;
  }else if (lff==1 && lmf==0 && lm==1 && lmb==1 && lbb==1){
    l_val = 10111;
  }else if (lff==0 && lmf==1 && lm==1 && lmb==1 && lbb==1){
    l_val = 1111;
  }else if (lff==1 && lmf==1 && lm==0 && lmb==0 && lbb==1){
    l_val = 11001;
  }else if (lff==1 && lmf==1 && lm==1 && lmb==0 && lbb==1){
    l_val = 11101;
  }else if (lff==1 && lmf==1 && lm==1 && lmb==1 && lbb==0){
    l_val = 11110;
  }else if (lff==0 && lmf==0 && lm==0 && lmb==0 && lbb==0 ||
            lff==0 && lmf==0 && lm==0 ||
            lm==0 && lmb==0 && lbb==0){
    l_val = 0;
  }

  switch (l_val){
    case 11111:
      if (s_error<0){
        s_error = -7;
      }else if (s_error>0){
        s_error =  7;
      }break;
    case 11011:
      if (s_error<0){
        s_error = -5;
      }else if (s_error>0){
        s_error =  5;
      }else{
        s_error = 0;
      }break;
    case 10011: s_error = -5; break;
    case 10111: s_error = -9; break;
//    case 1111:  s_error = -9; break;
    case 11001: s_error =  5; break;
    case 11101: s_error =  9; break;
//    case 11110: s_error =  9; break;
    case 0:     s_error =  pre_s_error; break;      
  }
}

void cal_sr_error(){//rff,rmf,rm,rmb,rbb
  if (rff==1 && rmf==1 && rm==1 && rmb==1 && rbb==1){
    r_val = 11111;
  }else if (rff==1 && rmf==1 && rm==0 && rmb==1 && rbb==1){
    r_val = 11011;
  }else if (rff==1 && rmf==0 && rm==0 && rmb==1 && rbb==1){
    r_val = 10011;
  }else if (rff==1 && rmf==0 && rm==1 && rmb==1 && rbb==1){
    r_val = 10111;
  }else if (rff==0 && rmf==1 && rm==1 && rmb==1 && rbb==1){
    r_val = 1111;
  }else if (rff==1 && rmf==1 && rm==0 && rmb==0 && rbb==1){
    r_val = 11001;
  }else if (rff==1 && rmf==1 && rm==1 && rmb==0 && rbb==1){
    r_val = 11101;
  }else if (rff==1 && rmf==1 && rm==1 && rmb==1 && rbb==0){
    r_val = 11110;
  }else if (rff==0 && rmf==0 && rm==0 && rmb==0 && rbb==0 ||
            rff==0 && rmf==0 && rm==0 ||
            rm==0 && rmb==0 && rbb==0){
    r_val = 0;
  }

  switch (r_val){
    case 11111:
      if (s_error<0){
        s_error = -7;
      }else if (s_error>0){
        s_error =  7;
      }break;
    case 11011:
      if (s_error<0){
        s_error = -5;
      }else if (s_error>0){
        s_error =  5;
      }else{
        s_error = 0;
      }break;
    case 10011: s_error = -5; break;
    case 10111: s_error = -9; break;
//    case 1111:  s_error = -9; break;
    case 11001: s_error =  5; break;
    case 11101: s_error =  9; break;
//    case 11110: s_error =  9; break;
    case 0:     s_error =  pre_s_error; break;      
  }
}
void cal_f_pid(){
  f_p = f_error;
  f_i = f_error + pre_f_i;
  f_d = f_error - pre_f_error;

  pid_f_val = (kp * f_p)+(ki * f_i)+(kd * f_d);
  pid_f_val = constrain(pid_f_val, -123, 123);

  pre_f_error = f_error;
  pre_f_i = f_i;

//  Serial.print("pid_f_val= ");
//  Serial.print(pid_f_val);
//  Serial.print("\t");
}
void cal_s_pid(){
  s_p = s_error;
  s_i = s_error + pre_s_i;
  s_d = s_error - pre_s_error;

  pid_s_val = (kps * s_p)+(kis * s_i)+(kds * s_d);
  pid_s_val = constrain(pid_s_val, -225, 225);

  pre_s_error = s_error;
  pre_s_i = s_i;

}

void servo_f_control(){
  lf_speed = pos_init_speed + pid_f_val;
  rf_speed = neg_init_speed + pid_f_val;
  lb_speed = pos_init_speed + pid_f_val;
  rb_speed = neg_init_speed + pid_f_val;
  lf_speed = constrain(lf_speed, 100, 550);
  rf_speed = constrain(rf_speed, 100, 550);
  lb_speed = constrain(lb_speed, 100, 550);
  rb_speed = constrain(rb_speed, 100, 550);
  pwm.setPWM(lf, 0, lf_speed);
  pwm.setPWM(rf, 0, rf_speed);
  pwm.setPWM(lb, 0, lb_speed);
  pwm.setPWM(rb, 0, rb_speed);

//  Serial.print("lf_speed= ");
//  Serial.print(lf_speed);
//  Serial.print("\t");
//  Serial.print("rf_speed= ");
//  Serial.print(rf_speed);
//  Serial.println();
  
}
void servo_sl_control(){
  lf_speed = neg_init_speed - pid_s_val - 25;
  rf_speed = neg_init_speed + pid_s_val;
  lb_speed = pos_init_speed - pid_s_val - 20;
  rb_speed = pos_init_speed + pid_s_val;
  lf_speed = constrain(lf_speed, 100, 550);
  rf_speed = constrain(rf_speed, 100, 550);
  lb_speed = constrain(lb_speed, 100, 550);
  rb_speed = constrain(rb_speed, 100, 550);
  pwm.setPWM(lf, 0, lf_speed);
  pwm.setPWM(rf, 0, rf_speed);
  pwm.setPWM(lb, 0, lb_speed);
  pwm.setPWM(rb, 0, rb_speed);
}
void servo_sr_control(){
  lf_speed = pos_init_speed + pid_s_val;
  rf_speed = pos_init_speed + pid_s_val ;
  lb_speed = neg_init_speed + pid_s_val;
  rb_speed = neg_init_speed + pid_s_val ;
  lf_speed = constrain(lf_speed, 100, 550);
  rf_speed = constrain(rf_speed, 100, 550);
  lb_speed = constrain(lb_speed, 100, 550);
  rb_speed = constrain(rb_speed, 100, 550);
  pwm.setPWM(lf, 0, lf_speed);
  pwm.setPWM(rf, 0, rf_speed);
  pwm.setPWM(lb, 0, lb_speed);
  pwm.setPWM(rb, 0, rb_speed);
}

void rturn(){
  pwm.setPWM(lf, 0, 437.5);
  pwm.setPWM(rf, 0, 437.5);
  pwm.setPWM(lb, 0, 437.5);
  pwm.setPWM(rb, 0, 437.5);
}
void lturn(){
  pwm.setPWM(lf, 0, 212.5);
  pwm.setPWM(rf, 0, 212.5);
  pwm.setPWM(rb, 0, 212.5);
  pwm.setPWM(lb, 0, 212.5);
}
void forward(){  
  pwm.setPWM(lf, 0, 437.5);
  pwm.setPWM(rf, 0, 212.5);
  pwm.setPWM(rb, 0, 212.5);
  pwm.setPWM(lb, 0, 437.5);
}
void backward(){  
  pwm.setPWM(lf, 0, 212.5);
  pwm.setPWM(rf, 0, 437.5);
  pwm.setPWM(rb, 0, 437.5);
  pwm.setPWM(lb, 0, 212.5);
}
void directleft(){
  pwm.setPWM(lf, 0, 212.5);
  pwm.setPWM(rf, 0, 212.5);
  pwm.setPWM(rb, 0, 437.5);
  pwm.setPWM(lb, 0, 437.5);
}
void directright(){
  pwm.setPWM(lf, 0, 437.5);
  pwm.setPWM(rf, 0, 437.5);
  pwm.setPWM(rb, 0, 212.5);
  pwm.setPWM(lb, 0, 212.5);
}
void nothing(){
  pwm.setPWM(lf, 0, 325);
  pwm.setPWM(rf, 0, 325);
  pwm.setPWM(rb, 0, 325);
  pwm.setPWM(lb, 0, 325);
}
void liftdown(){
  dis();
  while (true){
    pwm.setPWM(lift, 0, 437.5);
    delay(700);
    dis();
    if (distance < 5.5 ){
      break;
    }
  }
  pwm.setPWM(lift, 0, 437.5);
  delay(2300);
  pwm.setPWM(lift, 0, 325);
}
void liftup(){
  dis();
  while (true){ 
    pwm.setPWM(lift, 0, 212.5);
    delay(700);
    dis();
    if (distance > 6.3){
      break;
    }
}
  pwm.setPWM(lift, 0, 212.5);
  delay(2300);
  pwm.setPWM(lift, 0, 325);
}
void dis(){
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  duration = pulseIn(echopin, HIGH);
  distance = duration * 0.034 /  2;
  Serial.println(distance);
}
