
#include <math.h>
//Truth table for MUX 0-1(S0=PWM2, S1=PWM3 & S2=PWM4)
//--S2--S1--S0
//  0   0   0---- READ CALIBRATION RESISTOR
//  0   0   1---- READ TEMPERATURE SENSOR
//  0   1   0---- READ WM1
//  0   1   1---- READ WM2
//  1   0   0---- READ WM3
//  1   0   1---- READ WM4
//  1   1   0---- READ WM5
//  1   1   1---- READ WM6
// SET PWM5 HIGH TO EXCITE SENSOR

//WHEN CURRENT FLOW REVERSED 
//Truth table for MUX 2-3(S0=PWM6, S1=PWM7 & S2=PWM8)
//--S2--S1--S0
//  0   0   0---- READ CALIBRATION RESISTOR
//  0   0   1---- READ TEMPERATURE SENSOR
//  0   1   0---- READ WM1
//  0   1   1---- READ WM2
//  1   0   0---- READ WM3
//  1   0   1---- READ WM4
//  1   1   0---- READ WM5
//  1   1   1---- READ WM6
// SET PWM9 HIGH TO EXCITE SENSOR 

#define num_of_read 1 //READ THE SENSOR NUM NUMBER OF TIMES..SHOULD THE SUCCESIVE READINGS VARY
const int Rx = 7500;  //fixed resistor attached in series to the sensor and ground...the same value repeated for all WM and Temp Sensor.
const long default_TempC=24;
const long open_resistance=-100; //check the open resistance value by replacing sensor with an open and replace the value here...this value might vary slightly with circuit components 
const long short_resistance=-20; // similarly check short resistance by shorting the sensor terminals and replace the value here.
const long short_CB=240;
const long open_CB=255;
int i;                            
int j=0;

int ARead_A0=0; //ANALOG READ CHANNEL FOR MUX 0-1
int ARead_A1=0; //ANALOG READ CHANNEL FOR MUX 0-1

int SupplyV10K=0;
int SenV10K=0;
int SupplyVTempC=0;
int SenVTempC=0;
int SupplyVWM1=0;
int SenVWM1=0;
int SupplyVWM2=0;
int SenVWM2=0;

int WM1_CB=0;
int WM2_CB=0;

void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  // initialize the digital pin as an output.
    pinMode(2, OUTPUT);  //used for S0 control of the MUX 0-1
    pinMode(3, OUTPUT);  //used for S1 control of the MUX 0-1
    pinMode(4, OUTPUT);  //used for S2 control of the MUX 0-1
    pinMode(5, OUTPUT);  //used Energize Sensor A path
    pinMode(6, OUTPUT);  //Enable disable MUX 0-1
      
    delay(100);   // time in milliseconds, wait 0.1 minute to make sure the OUTPUT is assigned
}

void loop()
{   
   while(j==0)
    {
    
    ARead_A0=0;
    ARead_A1=0;
    for (i=0; i<num_of_read; i++)   //the num_of_read initialized above, controls the number of read successive read loops that is avereraged. 
      {
    //**********LETS READ THE 10K CALIBRATION RESISTOR ON CHANNEL 0**************
    // first take reading through path A
    
    digitalWrite(6, LOW);   //Enable MUX 0-1 
    digitalWrite(2, LOW);   //s0=s1=s2=0                 
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    delay(10); //Make sure the MUX channels for sensor on Channel 0 is ready for activation
    // sensor on channel 0 is the 10 k calibration resistor 
    digitalWrite(5, HIGH);   //energize the sensor through path A
    delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
    ARead_A0=analogRead(A0);   //read the supply voltage past the Diode and Switching transistor
    ARead_A1=analogRead(A1);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
    digitalWrite(5, LOW);      //set the excitation voltage to OFF/LOW
    SupplyV10K=ARead_A0;
    SenV10K=ARead_A1;
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    double K101=(SupplyV10K-SenV10K);
    double K102=K101/SenV10K;
    double K103=Rx*K102;
    double K104=K103-140.00;
    double Calib_Resistance=K104;
    //double Calib_Resistance=double((((Rx)*(SupplyV10K-SenV10K)/SenV10K))-140.00);
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    
    //**********LETS READ THE TEMPERATURE SENSOR ON CHANNEL 1**************
    // first take reading through path A
    ARead_A0=0;
    ARead_A1=0;
    digitalWrite(2, HIGH);   //s0=1; s1=s2=0                 
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    delay(10); //Make sure the MUX channels for sensor on Channel 0 is ready for activation
    // sensor on channel 1 is the Temperature Sensor 
    digitalWrite(5, HIGH);   //energize the sensor through path A
    delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
    ARead_A0=analogRead(A0);   //read the supply voltage past the Diode and Switching transistor
    ARead_A1=analogRead(A1);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
    digitalWrite(5, LOW);      //set the excitation voltage to OFF/LOW
    SupplyVTempC=ARead_A0;
    SenVTempC=ARead_A1;
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    //SupplyVTempC=(SupplyVTempC + ARead_A2)/(2 * num_of_read);
    //SenVTempC=(SenVTempC + ARead_A3)/(2 * num_of_read);
    double T1=(SupplyVTempC-SenVTempC);
    double T2=T1/SenVTempC;
    double T3=Rx*T2;
    double T4=T3-140.00;
    //double TempC_Resistance=T4;
    int TempC_Resistance=long((((Rx)*(SupplyVTempC-SenVTempC)/SenVTempC))-140);
    //Convert the resistance value of 10K temp sensor to actual Temperature value in Degree C 
    //double TempC=(-23.89*(log(TempC_Resistance)))+246.00;
    int TempC=(0.0593*(TempC_Resistance/1000)*(TempC_Resistance/1000))-(3.2809*(TempC_Resistance/1000))+51.839;
    if (TempC_Resistance<0)
    {
    Serial.print("Temperature Sensor absent or open circuit assuming: \n");  
    TempC=default_TempC;
    }
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    
//**********LETS READ THE WM1 SENSOR ON CHANNEL 2**************
    // first take reading through path A
    ARead_A0=0;
    ARead_A1=0;

    digitalWrite(2, LOW);   //s0=0;s1=1;s2=0                 
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    delay(10); //Make sure the MUX channels for sensor on Channel 2 is ready for excitation
    // sensor on channel 2 is the WM1 
    digitalWrite(5, HIGH);   //energize the sensor through path A
    delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
    ARead_A0=analogRead(A0);   //read the supply voltage past the Diode and Switching transistor
    ARead_A1=analogRead(A1);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
    digitalWrite(5, LOW);      //set the excitation voltage to OFF/LOW
    SupplyVWM1=ARead_A0;
    SenVWM1=ARead_A1;
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    //SupplyVWM1=(SupplyVWM1 + ARead_A2)/(2 * num_of_read);
    //SenVWM1=(SenVWM1 + ARead_A3)/(2 * num_of_read);
    double C1=(SupplyVWM1-SenVWM1);
    double C2=C1/SenVWM1;
    double C3=Rx*C2;
    double C4=C3-140.00;
    double WM1_Resistance=C4;
    delay(100); //0.1 second wait before moving to next channel or switching MUX

//**********LETS READ THE WM2 SENSOR ON CHANNEL 3**************
    // first take reading through path A
    ARead_A0=0;
    ARead_A1=0;

    digitalWrite(2, HIGH);   //s0=1;s1=1;s2=0                 
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    delay(10); //Make sure the MUX channels for sensor on Channel 2 is ready for excitation
    // sensor on channel 3 is the WM2 
    digitalWrite(5, HIGH);   //energize the sensor through path A
    delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
    ARead_A0=analogRead(A0);   //read the supply voltage past the Diode and Switching transistor
    ARead_A1=analogRead(A1);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
    digitalWrite(5, LOW);      //set the excitation voltage to OFF/LOW
    SupplyVWM2=ARead_A0;
    SenVWM2=ARead_A1;
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    digitalWrite(18, HIGH);   //Disable MUX 0-1 pair to prevent ground leakage on corresponding channels on path B 
    //SupplyVWM2=(SupplyVWM2 + ARead_A2)/(2 * num_of_read);
    //SenVWM2=(SenVWM2 + ARead_A3)/(2 * num_of_read);
    double D1=(SupplyVWM2-SenVWM2);
    double D2=C1/SenVWM2;
    double D3=Rx*D2;
    double D4=D3-140.00;
    //double WM2_Resistance=D4;
    int WM2_Resistance=((((Rx)*(SupplyVWM2-SenVWM2)/SenVWM2))-140);
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    Serial.print("**********************END READING BLOCK******************************************\n");
//*****************CONVERSION OF RESISTANCE TO ACTUAL VALUE AND CHECK FOR DEVIATION FROM CALIBRATION************************************
//Check for deviation of Calibration resistance 
    double calib_check=0.0;
    if(Calib_Resistance <=0)
    {
      calib_check=1;
    Serial.print(" Calibration resistor absent or not working assigning calib as= ");        // contd.....ideal temperature and current flow
    Serial.print(calib_check);
    Serial.print("\n");
    }
    if(Calib_Resistance >=0)
    {calib_check=Calib_Resistance/10075.00;   // when a 10K resistor is read the resistance value return will be between 9900- 10100 which is +/- 1% of 10K...in this case the 10K was read as 10075 under contd...
    Serial.print("\n");
    Serial.print("Calibration check= ");     // contd.....ideal temperature and current flow
    Serial.print(calib_check);
    Serial.print("\n");
    }   
    //convert WM1 Reading to Centibars or KiloPascal
    // The trailing 00 for numbers are very critical in the formula below eg: 1 is written as 1.00, else code messes math due to int vs double for numbers
    if (WM1_Resistance>550.00) 
    {
      if(WM1_Resistance>8000.00)
      {
      WM1_CB=-2.246-5.239*(WM1_Resistance/1000.00)*(1+.018*(TempC-24.00))-.06756*(WM1_Resistance/1000.00)*(WM1_Resistance/1000.00)*((1.00+0.018*(TempC-24.00))*(1.00+0.018*(TempC-24.00))); 
      Serial.print("Entered WM1 >8000 Loop \n");
      } 
     if (WM1_Resistance>1000.00) 
      {
        if (WM1_Resistance<8000)
        {
     WM1_CB=(-3.213*(WM1_Resistance/1000.00)-4.093)/(1-0.009733*(WM1_Resistance/1000.00)-0.01205*(TempC)) ;
     Serial.print("Entered WM1 >1000 Loop \n");
        }
      }
      if (WM1_Resistance<1000.00)
        {
        if (WM1_Resistance>550.00)
        {
        WM1_CB=-20.00*((WM1_Resistance/1000.00)*(1.00+0.018*(TempC-24.00))-0.55);
        Serial.print("Entered WM1>550 Loop \n");
        }
        }
     }
    if(WM1_Resistance<550.00)
      {
        if(WM1_Resistance>300.00)
        {
        WM1_CB=0.00;
        Serial.print("Entered 550<WM1>0 Loop \n");
        }
        if(WM1_Resistance<300.00)
        {
          if(WM1_Resistance>=short_resistance)
          {   
          WM1_CB=short_CB; //240 is a fault code for sensor terminal short
          Serial.print("Entered Sensor Short Loop WM1 \n");
          }
         
        }
        if(WM1_Resistance<=open_resistance)
        {
        WM1_CB=open_CB; //255 is a fault code for open circuit or sensor not present 
        Serial.print("Entered Open or Fault Loop for WM1 \n");
        }
      }

     //convert WM2 Reading to Centibars or KiloPascal
    if (WM2_Resistance>550.00) 
    {
      if(WM2_Resistance>8000.00)
      {
      WM2_CB=-2.246-5.239*(WM2_Resistance/1000.00)*(1.00+.018*(TempC-24.00))-.06756*(WM2_Resistance/1000.00)*(WM2_Resistance/1000.00)*((1.00+0.018*(TempC-24.00))*(1.00+0.018*(TempC-24.00))); 
      Serial.print("Entered WM2 >8000 Loop\n");
      } 
     if (WM2_Resistance>1000.00) 
       {
        if (WM2_Resistance<8000)
        {
      WM2_CB=(-3.213*(WM2_Resistance/1000.00)-4.093)/(1.00-0.009733*(WM2_Resistance/1000.00)-0.01205*(TempC)) ;
      Serial.print("Entered WM2 >1000 Loop \n");
        }
       }
      if (WM2_Resistance<1000.00)
        {
        if (WM2_Resistance>550.00)
        {
        WM2_CB=-20.00*((WM2_Resistance/1000.00)*(1.00+0.018*(TempC-24.00))-0.55);
        Serial.print("Entered WM2>550 Loop \n");
        }
        }
     }
    if(WM2_Resistance<550)
      {
        if(WM2_Resistance>300)
        {
        WM2_CB=0.00;
        Serial.print("Entered 550<WM2>0 Loop \n");
        }
        if(WM2_Resistance<300)
          {
          if(WM2_Resistance>=short_resistance)
          {   
          WM2_CB=short_CB; //240 is a fault code for sensor terminal short
          Serial.print("Entered Sensor Short Loop WM2 \n");
          }
          }
        if(WM2_Resistance<=open_resistance)
        {
        WM2_CB=open_CB; //255 is a fault code for open circuit or sensor not present 
        Serial.print("Entered Open or Fault Loop for WM2 \n");
        }
      }
    //Apply Calibration Correction
    if(calib_check>1)
    {
      if(WM1_Resistance>300)
      {
      WM1_CB=WM1_CB-((calib_check-1.00)*WM1_CB);
      Serial.print("Entered -Calib Loop for WM1 \n");
      }
      if(WM2_Resistance>300)
      {
      WM2_CB=WM2_CB-((calib_check-1.00)*WM2_CB);
      Serial.print("Entered -Calib Loop for WM2 \n");
      }
    }
    if(calib_check<1)
    {
      if(WM1_Resistance>300)
      {
      WM1_CB=WM1_CB+((1.00-calib_check)*WM1_CB);
      Serial.print("Entered +Calib Loop for WM1 \n");
      }
      if(WM2_Resistance>300)
      {
      WM2_CB=WM2_CB+((1.00-calib_check)*WM2_CB);
      Serial.print("Entered +Calib Loop for WM2 \n");
      }   
    }
    
    Serial.print("Temperature(C)= ");
    Serial.print(abs(TempC));
    Serial.print("\n");
    Serial.print("WM1 Resistance(Ohms)= ");
    Serial.print(WM1_Resistance);
    Serial.print("\n");
    Serial.print("WM2 Resistance(Ohms)= ");
    Serial.print(WM2_Resistance);
    Serial.print("\n");
    Serial.print("WM1(CB)= ");
    Serial.print(abs(WM1_CB));
    Serial.print("\n");
    Serial.print("WM2(CB)= ");
    Serial.print(abs(WM2_CB));
    Serial.print("\n\n"); 

//****************END CONVERSION BLOCK********************************************************

    
    }
    delay(5000);
    //j=1;
   }
}    
