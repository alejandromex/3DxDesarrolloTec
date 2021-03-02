#include <Arduino.h>
#include <math.h>

#define num_of_read 1 //READ THE SENSOR NUM NUMBER OF TIMES..SHOULD THE SUCCESIVE READINGS VARY
const int Rx = 7400;  //fixed resistor attached in series to the sensor and ground...the same value repeated for all WM and Temp Sensor.
const long default_TempC=24;
const long open_resistance=35000; //check the open resistance value by replacing sensor with an open and replace the value here...this value might vary slightly with circuit components 
const long short_resistance=200; // similarly check short resistance by shorting the sensor terminals and replace the value here.
const long short_CB=240, open_CB=255 ;

int i, j=0;                            
int SupplyV=3.9; // Cambio de voltaje a 3.3 - - - Assuming 5V output, this can be measured and replaced with an exact value if required
int WM1_CB=0;

double SenV10K=0, SenVTempC=0, SenVWM1=0, ARead_13=0 ;

#define constant 3.9;
float readVoltage()
{
  float ADCvoltage=(float)analogRead(13)/4095*constant;
  return ADCvoltage*constant;

}

void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  // initialize the digital pin as an output.
    pinMode(26, OUTPUT);  //used for S0 control of the MUX 0-1
    pinMode(25, OUTPUT);  //used for S1 control of the MUX 0-1
    pinMode(33, OUTPUT);  //used for S2 control of the MUX 0-1
    pinMode(12, OUTPUT);  //Sensor Vs or GND
    pinMode(32, OUTPUT);  //Enable disable MUX 0-1
    pinMode(14, OUTPUT);  //Sensor Vs or GND
    
    delay(100);   // time in milliseconds, wait 0.1 minute to make sure the OUTPUT is assigned
}

void loop()
{   
    while(j==0)
    {
    
    ARead_13=0;
        
    for (i=0; i<num_of_read; i++)   //the num_of_read initialized above, controls the number of read successive read loops that is averaged. 
      {
		//**********LETS READ THE 10K CALIBRATION RESISTOR ON CHANNEL 0**************
		// first take reading through path A
		Serial.println((String)ARead_13 + "Lectura analago 13 UP");

		digitalWrite(32, LOW);   //Enable MUX 0-1
		
		digitalWrite(26, LOW);   //s0=s1=s2=0                 
		digitalWrite(25, LOW);
		digitalWrite(33, LOW);
		delay(10); //Make sure the MUX channels for sensor on Channel 0 is ready for activation
		// sensor on channel 0 is the 10 k calibration resistor 
		
		digitalWrite(14, LOW); //Set pin 14 as gnd
		digitalWrite(12, HIGH);   //Set pin 27 as Vs
		
		delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
		ARead_13+=analogRead(13);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
		digitalWrite(12, LOW);      //set the excitation voltage to OFF/LOW
		delay(100); //0.1 second wait before moving to next channel or switching MUX
	 		Serial.println((String)ARead_13 + "Lectura analago 13 UP");

		//swap polarity

		digitalWrite(14, HIGH); //Set pin 11 as Vs
		delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
		ARead_13+=analogRead(13);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
		digitalWrite(14, LOW);      //set the excitation voltage to OFF/LOW
				Serial.println((String)ARead_13 + "Lectura analago 13 UP");

	  }
	SenV10K=((ARead_13/4095)*SupplyV) / (2 * num_of_read); //get the average of the readings and convert to volts
	double Calib_Resistance = (Rx*(SupplyV-SenV10K)/SenV10K)-140; //do the voltage divider math, using the Rx variable representing the known resistor 
	
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    
    //**********LETS READ THE TEMPERATURE SENSOR ON CHANNEL 1**************
    // first take reading through path A

    ARead_13=0;
	
	for (i=0; i<num_of_read; i++)   //the num_of_read initialized above, controls the number of read successive read loops that is averaged. 
	{

		digitalWrite(26, HIGH);   //s0=1; s1=s2=0                 
		digitalWrite(25, LOW);
		digitalWrite(33, LOW);
		delay(10); //Make sure the MUX channels for sensor on Channel 0 is ready for activation
		// sensor on channel 1 is the Temperature Sensor 
		
		digitalWrite(12, HIGH);   //Set pin 5 as Vs
		
		delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
		ARead_13+=analogRead(13);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
		
    
    digitalWrite(12, LOW);      //set the excitation voltage to OFF/LOW
		delay(100); //0.1 second wait before moving to next channel or switching MUX
		
		// Now lets swap polarity

		digitalWrite(14, HIGH); //Set pin 11 as Vs

		delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
		ARead_13+=analogRead(13);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
		digitalWrite(14, LOW);      //set the excitation voltage to OFF/LOW

	}
	Serial.println((String)ARead_13 + "Lectura analago 13");
	SenVTempC=((ARead_13/4095)*SupplyV) / (2 * num_of_read); //get the average of the readings and convert to volts
	// double TempC_Resistance = (Rx*(SupplyV-SenVTempC)/SenVTempC)-140; //do the voltage divider math, using the Rx variable representing the known resistor
	double TempC_Resistance = (Rx*(SupplyV-SenVTempC)/SenVTempC)-140; //do the voltage divider math, using the Rx variable representing the known resistor
	
	Serial.println(ARead_13/4095);

	Serial.println((String)SenVTempC + " - SenVTempC");

	Serial.println((String)TempC_Resistance + " - TempC_Resistance"); 

    //Convert the resistance value of 10K temp sensor to actual Temperature value in Degree C 
    double TempC=(-23.89*(log(TempC_Resistance)))+246.00;

	Serial.println("====================");


		Serial.println(log(TempC_Resistance));


	Serial.println("====================");

    if (TempC_Resistance<0)
    {
		Serial.print("Temperature Sensor absent or open circuit assuming: \n");  
		TempC=default_TempC;
    }
    delay(100); //0.1 second wait before moving to next channel or switching MUX
    
//**********LETS READ THE WM1 SENSOR ON CHANNEL 2**************
    // first take reading through path A
    ARead_13=0;
	
	for (i=0; i<num_of_read; i++)   //the num_of_read initialized above, controls the number of read successive read loops that is averaged. 
	{
		digitalWrite(26, LOW);   //s0=0;s1=1;s2=0                 
		digitalWrite(25, HIGH);
		digitalWrite(33, LOW);
		delay(10); //Make sure the MUX channels for sensor on Channel 2 is ready for excitation
		// sensor on channel 2 is the WM1 

		digitalWrite(12, HIGH);   //Set pin 5 as Vs
		
		delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
		ARead_13+=analogRead(13);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
		digitalWrite(12, LOW);      //set the excitation voltage to OFF/LOW

		delay(100); //0.1 second wait before moving to next channel or switching MUX
		
		// Now lets swap polarity

		digitalWrite(14, HIGH); //Set pin 11 as Vs
		
		delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
		ARead_13+=analogRead(13);   // read the sensor voltage right after the WM sensor and right before the series resistor Rx-7870
		digitalWrite(14, LOW);      //set the excitation voltage to OFF/LOW
	
	}
	
	SenVWM1=((ARead_13/4095)*SupplyV) / (2 * num_of_read); //get the average of the readings and convert to volts
	double WM1_Resistance = (Rx*(SupplyV-SenVWM1)/SenVWM1); //do the voltage divider math, using the Rx variable representing the known resistor
	
    delay(100); //0.1 second wait before moving to next channel or switching MUX

//**********LETS READ THE WM2 SENSOR ON CHANNEL 3**************
    // first take reading through path A
    ARead_13=0;

	for (i=0; i<num_of_read; i++)   //the num_of_read initialized above, controls the number of read successive read loops that is averaged. 
	{

		digitalWrite(26, HIGH);   //s0=1;s1=1;s2=0                 
		digitalWrite(25, HIGH);
		digitalWrite(33, LOW);
		delay(10); //Make sure the MUX channels for sensor on Channel 2 is ready for excitation
		// sensor on channel 3 is the WM2 

		digitalWrite(12, HIGH);   //Set pin 5 as Vs
		
		delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
		ARead_13+=analogRead(13);   // read the sensor voltage right after the sensor and right before the series resistor Rx-7870
		digitalWrite(12, LOW);      //set the excitation voltage to OFF/LOW
		delay(100); //0.1 second wait before moving to next channel or switching MUX
		
		// Now lets swap polarity

		digitalWrite(14, HIGH); //Set pin 11 as Vs
		
		delay(0.09); //wait 90 micro seconds and take sensor read...do not exceed 100uS
		ARead_13+=analogRead(13);   // read the sensor voltage right after the WM sensor and right before the series resistor Rx-7870
		digitalWrite(14, LOW);      //set the excitation voltage to OFF/LOW
	}

	digitalWrite(32, HIGH);   //Disable MUX 0-1 pair
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
    {
		calib_check=Calib_Resistance/10075.00;   // when a 10K resistor is read the resistance value return will be between 9900- 10100 which is +/- 1% of 10K...in this case the 10K was read as 10075 under contd...
		Serial.print("\n");
		Serial.print("Calibration check= ");     // contd.....ideal temperature and current flow
		Serial.print(calib_check);
		Serial.print("\n");
    }   
    //convert WM1 Reading to Centibars or KiloPascal
	if (WM1_Resistance>550.00) {
		
		if (WM1_Resistance>8000.00) {
			
			WM1_CB=-2.246-5.239*(WM1_Resistance/1000.00)*(1+.018*(TempC-24.00))-.06756*(WM1_Resistance/1000.00)*(WM1_Resistance/1000.00)*((1.00+0.018*(TempC-24.00))*(1.00+0.018*(TempC-24.00))); 
			Serial.print("Entered WM1 >8000 Loop \n");
			
		} else if (WM1_Resistance>1000.00) {

			WM1_CB=(-3.213*(WM1_Resistance/1000.00)-4.093)/(1-0.009733*(WM1_Resistance/1000.00)-0.01205*(TempC)) ;
			Serial.print("Entered WM1 >1000 Loop \n");

		} else {

			WM1_CB=-20.00*((WM1_Resistance/1000.00)*(1.00+0.018*(TempC-24.00))-0.55);
			Serial.print("Entered WM1>550 Loop \n");
		}
		
	} else {

		if(WM1_Resistance>300.00)  {
			WM1_CB=0.00;
			Serial.print("Entered 550<WM1>0 Loop \n");
		}
		
		if(WM1_Resistance<300.00 && WM1_Resistance>=short_resistance) {
			
			WM1_CB=short_CB; //240 is a fault code for sensor terminal short
			Serial.print("Entered Sensor Short Loop WM1 \n");
		}
	}
	
	if(WM1_Resistance>=open_resistance) {
		
		WM1_CB=open_CB; //255 is a fault code for open circuit or sensor not present 
		Serial.print("Entered Open or Fault Loop for WM1 \n");
	}

	
    //Apply Calibration Correction
    if(calib_check>1)
    {
      if(WM1_Resistance>300)
      {
        WM1_CB=WM1_CB-((calib_check-1.00)*WM1_CB);
        Serial.print("Entered -Calib Loop for WM1 \n");
      }		
    }
    else
    {

      if(WM1_Resistance>300)
      {
        WM1_CB=WM1_CB+((1.00-calib_check)*WM1_CB);
        Serial.print("Entered +Calib Loop for WM1 \n");
      }
    }
    
    Serial.print("Temperature(C)= ");
    Serial.print(abs(TempC));
    Serial.print("\n");
    Serial.print("WM1 Resistance(Ohms)= ");
    Serial.print(WM1_Resistance);
    Serial.print("\n");
    Serial.print("WM1(CB)= ");
    Serial.print(abs(WM1_CB));
    Serial.print("\n");
    

//****************END CONVERSION BLOCK********************************************************

    delay(5000);
    //j=1;
   }
}    