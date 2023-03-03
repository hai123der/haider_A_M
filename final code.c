#define LCD_RS_PIN PIN_B1
#define LCD_RW_PIN PIN_B2
#define LCD_ENABLE_PIN PIN_B3
#define LCD_DATA4 PIN_B4
#define LCD_DATA5 PIN_B5
#define LCD_DATA6 PIN_B6
#define LCD_DATA7 PIN_B7
#include <16f877a.h>
#device adc=10
#use delay (clock=4000000)
#FUSES NOWDT                 
#FUSES xt                   
#FUSES NOPROTECT             
#include <lcd.c>
float pwm=500;
unsigned int8 speed_;
char num='@';// char n[12]={'1' ,'2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#'}; 
int t=50,flag_key; //for key function
int start =0,seg=0,to_zero=0,no_time=0,with_time=0,change_speed=0; //for motor function
int8 check; // for ultra sonic
unsigned int16 i, distance; //ultra sonic
int16 t1; //for encoder 

# int_timer1     // example for timer1 as counter from external pulses
void t1_isr(){
t1++;
set_timer1(65535); // one puls it will be t1 increase
}


void  key(){ // in this func the code will inter in inf loop until user enter char
flag_key=0; 
while(!flag_key){
for(int i=0;i<4;i++){

if(i==0){
output_high(pin_d1);
    if( input(pin_d5)){num='1'; flag_key=1;delay_ms(t);}
else if( input(pin_d6)){num='2';flag_key=1;delay_ms(t);}
else if( input(pin_d7)) {num='3';flag_key=1;delay_ms(t);}
delay_ms(t);
output_low(pin_d1);
}

else if(i==1){
output_high(pin_d2);
 if(input(pin_d5)) {num='4';flag_key=1;delay_ms(t);}
else if( input(pin_d6)) {num='5';flag_key=1;delay_ms(t);}
else if( input(pin_d7)) {num='6';flag_key=1;delay_ms(t);}
delay_ms(t);
output_low(pin_d2);}

else if(i==2){
output_high(pin_d3);
 if(input(pin_d5)) {num= '7';flag_key=1;delay_ms(t);}
else if( input(pin_d6)) {num= '8';flag_key=1;delay_ms(t);}
else if( input(pin_d7)) {num= '9';flag_key=1;delay_ms(t);}
delay_ms(t);
output_low(pin_d3);}
else if(i==3){
output_high(pin_d4);
 if(input(pin_d5)) {num= '*';flag_key=1;delay_ms(t);}
else if( input(pin_d6)) {num= '0';flag_key=1;delay_ms(t);}
else if( input(pin_d7)) {num= '#';flag_key=1;delay_ms(t);}
delay_ms(t);
output_low(pin_d4);}
} }}


void ultra_sonick(){
#use fast_io(A)
  set_tris_a(0x02);                                   
  setup_timer_1 (T1_INTERNAL | T1_DIV_BY_2);       // Configure Timer1 module
  set_timer1(0);                                   // Reset Timer1
  lcd_putc('\f');                                  // LCD Clear
  lcd_gotoxy(4, 1);                                // Go to column 4 row 1
  lcd_putc("Distance:");
  while(TRUE){
    check = 0;
    i = 0;
    output_high(PIN_A0);
    delay_us(10);
    output_low(PIN_A0);
    set_timer1(0);                                 // Reset Timer1
    while(!input(PIN_A1) && (get_timer1() < 1000));
    if(get_timer1() > 990)
      check = 1;                                   // Timeout error
    set_timer1(0);                                 // Reset Timer1
    while(input(PIN_A1) && (i < 25000))
      i = get_timer1();                            // Store Timer1 value in i 
    if(i > 24990)                                  // Out of range error
      check = 2;
    if(check == 1){
      lcd_gotoxy(3, 2);                            // Go to column 3 row 2
      lcd_putc("  Time Out  ");
    }
    if(check == 2){
      lcd_gotoxy(3, 2);                            // Go to column 3 row 2
      lcd_putc("Out Of Range");
    }
    else{
      distance = i/28.5;                             // Calculate the distance
      lcd_gotoxy(3, 2);                            // Go to column 3 row 2
      lcd_putc("       cm   ");
      lcd_gotoxy(6, 2);                            // Go to column 6 row 2
      printf(lcd_putc,"%3Lu",distance+1);
    }
    delay_ms(100);
    
  }
}


void pot(){

float volt ;
long analog_input ;
setup_adc_ports(A_analog);
setup_adc(adc_clock_internal);
lcd_putc("\f");
lcd_gotoxy(1,1);
lcd_putc("volt= ");
while(TRUE)
{
set_adc_channel(2);
delay_us(10);
analog_input=read_adc();
volt=(analog_input*5)/1023;
lcd_gotoxy(1,2);
printf(lcd_putc,"%f",volt);
delay_ms(100);


}}


void lm35(){


char message1[] = "Temp =  00.0 C";
char message2[] =      "=  00.0 K";
signed int16 Kelvin, Celsius;
  setup_adc(ADC_CLOCK_INTERNAL);                 // ADC Module uses its internal oscillator
  setup_adc_ports(AN0);                          // Configure AN0 pin as analog
  set_adc_channel(3);                            // Select channel 3 (AN3)
  lcd_init();                                    // Initialize LCD module
  lcd_putc('\f');                                // Clear LCD
  while(TRUE){
    delay_ms(1000);
    Kelvin = read_adc() * 0.489;                 // Read analog voltage and convert it to Kelvin (0.489 = 500/1023)
    Celsius = Kelvin - 273;                      // Convert Kelvin to degree Celsius
    if(Celsius < 0){
      Celsius = abs(Celsius);                    // Absolute value
      message1[7] = '-';                         // Put minus "-" sign
    }
    else
      message1[7]  = ' ';                        // Put space 
    if (Celsius > 99)
      message1[7]  = 1 + 48;                     // Put 1 (of hundred)
    message1[8]  = (Celsius / 10) % 10  + 48;
    message1[9]  =  Celsius % 10  + 48;
    message1[12] = 223;                          // Degree symbol
    message2[2]  = (Kelvin / 100) % 10 + 48;
    message2[3]  = (Kelvin / 10) % 10 + 48;
    message2[4] = Kelvin % 10 + 48;
    lcd_gotoxy(1, 1);                            // Go to column 1 row 1
    printf(lcd_putc, message1);                  // Display message1
    lcd_gotoxy(6, 2);                            // Go to column 6 row 2
    printf(lcd_putc, message2);                  // Display message2
    
  
  }
  }
  
void dc(){
setup_adc(ADC_CLOCK_DIV_32);           // Set ADC conversion time to 32Tosc
  setup_adc_ports(AN0_AN1_AN3);          // Configure AN0,AN1 and AN3 as analog
  setup_ccp1(CCP_PWM);                   // Configure CCP1 as a PWM
  setup_ccp2(CCP_PWM);                   // Configure CCP2 as a PWM
  delay_ms(100);                         // Wait 100ms
  setup_timer_2(T2_DIV_BY_16, 255, 1);   // Set PWM frequency to 488Hz
  
  while(seg){
 if(!with_time){ 
lcd_putc("\f");
lcd_gotoxy(1,1);
lcd_putc("foe set speed ");
lcd_gotoxy(1,2);
lcd_putc("enter  0 ");   
if(input(pin_d6))
    {change_speed=1;}}
    
     output_high(pin_c0);
    output_low(pin_c3);
        output_high(pin_c4);
    output_low(pin_c5);// Read from AN1 and store in j
    set_pwm1_duty(pwm);
    set_pwm2_duty(pwm);
    delay_ms(50); // Wait 1ms
    output_high(pin_d4);
   
    
    if(!no_time){
 
    if(to_zero>0)
    {
   to_zero--;
   output_high(pin_e0);
delay_us(10);
output_low(pin_e0);
 delay_ms(600);
}
else{
output_high(pin_b0);
delay_ms(500);
output_low(pin_b0);
output_low(pin_c0);
output_low(pin_c3);
output_low(pin_c4);
output_low(pin_c5);
output_low(pin_d0);
delay_ms(1000);
output_high(pin_d0);
delay_ms(1000);
output_low(pin_d0);
  set_pwm1_duty(0);
    set_pwm2_duty(0);
seg=0;

}}
while (change_speed)
{
lcd_putc("\f");
lcd_gotoxy(1,1);
lcd_putc("* for up speed");
lcd_gotoxy(1,2);
lcd_putc("# for down speed ");
lcd_gotoxy(21,1);
lcd_putc("0 for start");
lcd_gotoxy(21,2);
printf(lcd_putc,"PWM = %f ",pwm);
output_low(pin_c0);
output_low(pin_c3);
output_low(pin_c4);
output_low(pin_c5);
key();
if (num=='*'){
pwm=pwm+1;
if(pwm>1000)pwm=1000;}

else if (num=='#'){
pwm=pwm-1;
if(pwm<0)pwm=0;}

else if (num=='0'){
change_speed=0;}

}

  }}


void segment(){
set_tris_a(0x00);
lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("set the time  ");
lcd_gotoxy(1,2);
lcd_putc("input * for up ");
lcd_gotoxy(21,1);
lcd_putc("input # for down");
lcd_gotoxy(21,2);
lcd_putc("input 0 for start");

output_low(pin_a1);
output_low(pin_a2);
output_low(pin_d0);

while(!start){
key();
if (num=='*'){
output_high(pin_a5);
delay_us(10);
output_low(pin_a5);
delay_us(10);
to_zero++;}

else if (num=='#'){
output_high(pin_e0);
delay_us(10);
output_low(pin_e0);
delay_us(10);
to_zero--;}

else if (num=='0'){
lcd_putc("\f");
lcd_gotoxy(1,1);
lcd_putc("set the speed  ");
delay_ms(500);
change_speed=1;
while (change_speed)
{
lcd_putc("\f");
lcd_gotoxy(1,1);
lcd_putc("* for up speed");
lcd_gotoxy(1,2);
lcd_putc("# for down speed ");
lcd_gotoxy(21,1);
lcd_putc("0 for start");
lcd_gotoxy(21,2);
printf(lcd_putc,"PWM = %f ",pwm);
output_low(pin_c0);
output_low(pin_c3);
output_low(pin_c4);
output_low(pin_c5);
key();
if (num=='*'){
pwm=pwm+1;
if(pwm>1000)pwm=1000;}

else if (num=='#'){
pwm=pwm-1;
if(pwm<0)pwm=0;}

else if (num=='0'){
change_speed=0;}

}
lcd_putc("\f");
lcd_gotoxy(1,1);
seg=1;
start=1;}


else{
lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("incorrect ");
lcd_gotoxy(1,2);
lcd_putc("input ");}
}
}



void stepper_move(short direction_){
  if(direction_){ 
   output_c(0b00011000);
   delay_ms(speed_);
   output_c(0b00010001);
   delay_ms(speed_);
   output_c(0b00100001);
   delay_ms(speed_);
   output_c(0b00101000);
   delay_ms(speed_);
  }
  else{
   output_c(0b00010001);
   delay_ms(speed_);
   output_c(0b00011000);
   delay_ms(speed_);
   output_c(0b00101000);
   delay_ms(speed_);
   output_c(0b00100001);
   delay_ms(speed_);
  }
}


void stepper(){
  set_tris_c(0x00);
  setup_adc(ADC_CLOCK_DIV_32);      // Set ADC conversion time to 32Tosc
  setup_adc_ports(A_analog);             // Configure AN0 as analog  
  set_adc_channel(2);               // Select channel 0 input
  
  delay_ms(100);                    // Wait 100ms
  while(TRUE){
   output_high(pin_c1);
   output_high(pin_c2);
   output_high(pin_d0);
   key();
  if(num=='1')
  {speed_ = read_adc();
    if(speed_ < 2) speed_ = 2;
    stepper_move(0);}
    else if(num=='2')
   { speed_ = read_adc();
    if(speed_ < 2) speed_ = 2;
    stepper_move(1);}
    else{
    lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("incorrect ");
} 
   }}

void encoder(){
t1=0;
setup_timer_1(t1_external|t1_div_by_4);
set_timer1(65535);
enable_interrupts(global); 
enable_interrupts(int_timer1);
while(TRUE){
lcd_gotoxy(1,1);
lcd_putc("\f");
printf(lcd_putc,"counter = %lu",t1);
delay_ms(100);

}}


void leds(){
int C1=1,con=1;
set_tris_e(0x00);
while(TRUE){
if(C1<9 & con){
C1++; output_high(pin_e2); 
if (C1>=9)con=0;}
else if (C1>=9 & !con )
{C1++; output_low(pin_e2);
if (C1<18){con=1; C1=1;}}

output_high(pin_e1);
delay_ms(500);
output_low(pin_e1);
delay_ms(500);

  
}}

void uart(){
#use rs232(uart1, baud = 9600)                // Initialize UART module
 
  putc(13);                                   // Go to first column
  printf("Hi ");                              // UART write
  delay_ms(2000);                             // Wait 5 seconds
  putc(13);                                   // Go to first column
  putc(10);                                   // Start a new line
  printf("PIC16F877A UART example");          // UART Write
  delay_ms(2000); 
  putc(13);
  putc(10); 
  printf("I hope this board  benefit you"); 
  
  }
  
void main(){

lcd_init();
set_tris_d(0x00);
set_tris_c(0x00);
set_tris_e(0x00);
set_tris_b(0x01);
set_tris_a(0x0f);
home:
output_high(pin_b0);
delay_ms(500);
output_low(pin_b0);
output_low(pin_b1);
output_low(pin_b2);
output_low(pin_b3);
output_low(pin_b4);
output_low(pin_b5);
output_low(pin_b6);
output_low(pin_b7);
output_low(pin_c0);
output_low(pin_c1);
output_low(pin_c2);
output_low(pin_c3);
output_low(pin_c4);
output_low(pin_c5);
output_low(pin_d1);
output_low(pin_d2);
output_low(pin_d3);
output_low(pin_d4);
output_low(pin_d5);
output_low(pin_d6);
output_low(pin_d7);
output_low(pin_e0);
output_low(pin_e1);
output_low(pin_e2);
output_high(pin_d0);
delay_ms(5);
output_low(pin_d0);
enable_interrupts(global);
enable_interrupts(int_ext);
ext_int_edge(l_to_h);

lcd_gotoxy(1,1);
lcd_putc("enter a number to ");
lcd_gotoxy(1,2);
lcd_putc("choose a func ");
delay_ms(1000);
lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("(1) motors ");
lcd_gotoxy(1,2);
lcd_putc("(2) sensors");
lcd_gotoxy(21,1);
lcd_putc("(3) encoder");
lcd_gotoxy(21,2);
lcd_putc("(4) leds   (5) uart");
delay_ms(500);


key();


if(num=='1'){  //motors 
 output_low(pin_c0);
    output_low(pin_c3);
        output_low(pin_c4);
    output_low(pin_c5);
     output_high(pin_e1);
lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("(1) DC motor ");
lcd_gotoxy(1,2);
lcd_putc("(2) Stepper ");

delay_ms(100);
key();
 
if(num=='1')  
{

lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc(" Do you want time ? \n ");
lcd_gotoxy(21,1);
lcd_putc("1) yes");
lcd_gotoxy(21,2);
lcd_putc("2) no");
key();
if(num=='1')
{with_time=1;segment();
dc();}
else if(num=='2')
{
lcd_gotoxy(1,1);
lcd_putc("\f");
no_time=1; seg=1;
dc();}
else 
{
lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("incorrect ");
lcd_gotoxy(1,2);
lcd_putc("input ");}
}

else if(num=='2'){ //stepper

output_high(pin_d0);
lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("1) first direction");
lcd_gotoxy(21,1);
lcd_putc("2) second direction");
stepper();
}
}


else if(num=='2'){  //sensors

lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("(1)  ultra sonick ");
delay_ms(10);
lcd_gotoxy(1,2);
lcd_putc("(2)lm35");
delay_ms(10);
lcd_gotoxy(21,1);
lcd_putc("(3) pot");
delay_ms(10);
key();
if(num=='1') // ultra sonick
ultra_sonick();
else if(num=='2') // LM35
lm35();
else if(num=='3') // pot
pot();
else {
lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("incorrect ");
}}

else if(num=='3') // encoder
{ lcd_gotoxy(1,1); lcd_putc("\f");  encoder(); }

else if(num=='4') // leds example
{ lcd_gotoxy(1,1);lcd_putc("\f"); leds();}

else if(num=='5') // uart  example 
{ lcd_gotoxy(1,1);lcd_putc("\f");uart();}

else {
lcd_gotoxy(1,1);
lcd_putc("\f");
lcd_putc("incorrect ");
lcd_gotoxy(1,2);
lcd_putc("input ");
delay_ms(100);}


}
