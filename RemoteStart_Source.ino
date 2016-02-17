char in[6]; //array for the data being read
char correct[5] = {'1','u','l', 'V',' '}; //array to compare read data to RFID sends the 4 digit code followed by a space
int unlock = 13; //assigns unlock pin
int door = 12; //assigns door pin
int sba = 11; //assigns side 1 of seatbelt 
int sbb = 9; //assigns side 2 of seatbelt
int lock = 10; //assigns lock
int acc = 8; //assigns accessories
int accin = 7; //assigns the accessories input
int brake = 6; //assigns the brake 
int clutch = 5; //assigns the clutch
int bypass = 4; //assigns the bypass
int on1 = 3; //assigns the on1
int start = 2; //assigns the starter
int sbo; //integer for seatbelt check
int trash; //creates an integer used to clear the serial buffer
unsigned long doortimei; //creates timer for the door opening
unsigned long doortimeo; //creates timer for the door opening on exit
unsigned long sbtime; //creates timer for the seatbelt being clicked
void setup()
{
    Serial1.begin(9600); //starts serial reading for the RFID
    Serial.begin(57600); //starts serial for sending information back to the USB Serial Monitor
    pinMode(door,INPUT_PULLUP); //sets the door pin to be an input (will never be used as an output)
    pinMode(accin,INPUT);//sets the accin pin to be an input (will never be used as an output)
    pinMode(brake,INPUT); //sets the brake pin to be an input (will never be used as an output)          
}
void loop()
{
    scanf();     //calls scan function
}
void scanf() //scan function
{
    for (int i = 0; i < 6; i++)
    {
      in[i] = NULL;
    }
  
    Serial.print(in); //prints the code currently stored by the arduino (should actually be blank and print nothing)
    Serial.print(" begin "); //prints the word begin for tracking purposes 
    Serial.print('\n'); //new line for ease of reading
    Serial1.readBytes(in, 5); // Read 5 bytes and fill the in array
    delay(2000); //delay aids in synchronizing the reader for inaccurate pulses
    Serial.print(in); //prints the received code for tracking
    if (strncmp(in, correct, 5)==0) //compares the first five bytes in the in array to the first five bytes in the correct array
        {
            Serial.print("rfid match"); //prints match if the code matched for tracking 
            Serial.print('\n'); //new line for ease of reading
             //erases the data in the in array, ensures that the old read won't cause an unlock
            unlockf(); //it was a match, call upon unlock function
            doortimei = millis();//starts a timer to check for the door being opened
            doorchki();//calls the function to check the door of the vehicle
        }
        else //code didn't match
            {
                Serial.print("rfid doesnt match"); //prints because the RFID did not match
                Serial.print('\n'); //new line for ease of reading
                
                for (int i = 0; i < 6; i++) //clear the buffer
                {
                  in[i] = NULL;
                }
       
                scanf();//restarts this function
            }
             
}
void unlockf() //unlock function
{ 
    pinMode(unlock,OUTPUT); //sets the unlock pin as an output
    digitalWrite(unlock,LOW);//sets the unlock pin to GND to activate the relay
    delay(1000);//leaves relay on for one second
    pinMode(unlock,INPUT);//sets unlock pin back to float, shutting relay
    Serial.print("car now unlocked");//prints the word unlock to show the car has been unlocked
    Serial.print('\n'); //new line for ease of reading
    return;//returns to the previous function after unlocking door
}
void doorchki() //door check function for getting in car
{
    if(millis()-doortimei <=30000) //checks to see if the it has been thirty seconds since the car unlocked
        {
            if (digitalRead(door)!=0) //has the door been opened, ground when closed
                {
                    Serial.print("door now open");//prints that the door has been opened
                    Serial.print('\n'); //new line for ease of reading
                    sbtime=millis(); //sets the timer for the seatbelt
                    Serial.print("Begin seatbelt check");//prints for tracking
                    Serial.print('\n'); //new line for ease of reading
                    sbchecki(); //runs the seatbelt check function
                }
                else //door has not been opened
                    {
                        doorchki(); //restart this check
                    }      
        }
        else //thirty seconds has passed without the door opening
            {
                Serial.print("door not opened"); //prints that the door was not opened
                Serial.print('\n'); //new line for ease of reading
                lockf(); //execute lock function
                scanf(); //returns to scan
            }
} 
void lockf()       //lock function
{          
  pinMode(lock,OUTPUT); //sets the lock pin as an output
  digitalWrite(lock,LOW);//sets the lock pin to GND to activate the relay
  delay(1000);//leaves relay on for one second
  pinMode(lock,INPUT);//sets lock pin back to float, shuts relay
  Serial.print("lock");//prints lock for tracking
  Serial.print('\n'); //new line for ease of reading
  trash = Serial1.available();//finds how many extra reads were done from the RFID and sets that equal to trash
  char trash2[trash];//creates an array (trash2) that will accommodate all of the bytes found in the trash check
  Serial1.readBytes(trash2,trash);//fills the trash array with all of the trash bytes, emptying the serial buffer, preventing those extra reads from causing an unlock
  scanf();//returns to the scan function after locking the doors
}

void lockr()       //lock function
{          
  pinMode(lock,OUTPUT); //sets the lock pin as an output
  digitalWrite(lock,LOW);//sets the lock pin to GND to activate the relay
  delay(1000);//leaves relay on for one second
  pinMode(lock,INPUT);//sets lock pin back to float, shuts relay
  Serial.print("lock");//prints lock for tracking
  Serial.print('\n'); //new line for ease of reading
  trash = Serial1.available();//finds how many extra reads were done from the RFID and sets that equal to trash
  char trash2[trash];//creates an array (trash2) that will accommodate all of the bytes found in the trash check
  Serial1.readBytes(trash2,trash);//fills the trash array with all of the trash bytes, emptying the serial buffer, preventing those extra reads from causing an unlock
  runf();
}

void sbchecki() //seatbelt check getting into car function
{
    Serial.print('\n');//creates a new line for ease of reading
    if(millis()-sbtime <=30000) //checks to see if it has been thirty seconds since the door was opened
        {
            pinMode(sba,OUTPUT); //sets one side of seatbelt to output
            digitalWrite(sba,LOW); //sets that side to GND
            pinMode(sbb,INPUT_PULLUP); //sets other side of seatbelt as an input with pullup resistor
            if(digitalRead(sbb)==0) //checks to see if the seatbelt wires are connected
                {
                    Serial.print("seatbelt connected");//prints that the SB is connected for tracking
                    Serial.print('\n'); //new line for ease of reading
                    pinMode(sba,INPUT); //stops GND on seatbelt wire
                    startf(); //calls upon start function
                }
                else //seatbelt has not been connected
                    {
                        sbchecki(); //continue checking
                    }
        }   
            else //thirty seconds has passed without the seatbelt being clicked
                {
                    Serial.print("seatbelt has not been buckled");//print for tracking
                    Serial.print('\n'); //new line for ease of reading
                    lockf(); //call upon the lock function
                }
}
void startf() //start function
{
    Serial.print("start"); //prints start for tracking
    Serial.print('\n'); //new line for ease of reading
    if(digitalRead(accin)!=1) //is the car off
        {
            if(digitalRead(brake)==HIGH) //is the ebrake on
                {
                    pinMode(clutch,OUTPUT); //set the clutch bridge pin to output
                    digitalWrite(clutch,LOW); //activate the relay for clutch bridge
                    pinMode(bypass,OUTPUT); //set the bypass pin to output    
                    digitalWrite(bypass,LOW);//output GND to the bypass
                    pinMode(acc,OUTPUT);//set the acc pin to output
                    digitalWrite(acc,LOW);//activates relay to turn on accessories
                    pinMode(on1,OUTPUT);//set the on1 pin to output
                    digitalWrite(on1,LOW);//activates the relay for on1
                    pinMode(start,OUTPUT);//sets the starter pin to output
                    digitalWrite(start,LOW);//activates the starter relay
                    delay(1500);//delay to pulse the starter
                    pinMode(start,INPUT);//sets the start relay to float (stops powering starter)
                    pinMode(clutch,INPUT);//sets the clutch relay to float (stops connecting clutch wires)
                    delay(20000);//waits 20 seconds
                    lockr(); //locks doors
                        
                }
                else //brake is off (aka driving)
                    {
                        Serial.print("brake off");    //prints brake off for tracking  
                        Serial.print('\n'); //new line for ease of reading  
                        lockf();  //reutns to lock function
                        scanf(); //returns to scan function
                    }
        }
            else//car is on
                {
                    Serial.print("Car on");//prints car on for tracking
                    Serial.print('\n'); //new line for ease of reading
                    caronf();//calls the car already running function
                }        
}
void caronf() //function for when the car is already on
{
    if(digitalRead(accin)!=1)//is the car off?
        {
            Serial.print("car has been shut off"); //tracking
            Serial.print('\n'); //new line for ease of reading
            lockf(); //lock door
            scanf();//got to scan
        }
            else //car is on
            {
                caronf();    //restart this function
            }
}
void runf() //run function
{
    Serial.print("run");//prints run for tracking
    Serial.print('\n'); //new line for ease of reading
      if(digitalRead(brake)==1)//if the ebrake becomes active
        {
            Serial.print("brake on");    //prints brake on for tracking
            Serial.print('\n'); //new line for ease of reading
            if(sbchecko()==1)//if the seatbelt is disconnected
                {
                    Serial.print("seatbelt off");//prints seatbelt off for tracking
                    Serial.print('\n'); //new line for ease of reading
                    shutf();//runs the shut function
                }
                else //the seatbelt is still connected
                    {
                        Serial.print("seatbelt on"); //prints seatbelt on for tracking
                        Serial.print('\n'); //new line for ease of reading
                        runf();//keep running this loop
                    }
        }
            else//the ebrake is not active
                {
                    Serial.print("ebrake off");    //prints ebrake off for tracking
                    Serial.print('\n'); //new line for ease of reading
                    runf();//keep running this loop
                }
}
int sbchecko() //seatbelt check function for getting out of car
{
    pinMode(sba,OUTPUT); //sets one side of seatbelt to output
    digitalWrite(sba,LOW); //sets that side to GND
    pinMode(sbb,INPUT_PULLUP); //sets other side of seatbelt as an input
    sbo = digitalRead(sbb); // sets sbo to the value of sbb (0 if the seatbelt is)
    return sbo; //this function will output an integer equal to 0 if the seatbelt is connected or 1 if it isn't 
}
void shutf() //shut function
{
    pinMode(on1,INPUT);//restores the on1 to float, shutting relay
    pinMode(acc,INPUT);//restores the acc to float, shutting relay
    pinMode(bypass,INPUT);//restores bypass to float, disabling it
    unlockf();//calls the unlock function
    doortimeo = millis();//starts a timer to check for the door being opened
    doorchko();//runs the exit door check
}
void doorchko()
{
    if(millis()-doortimeo <=20000) //checks to see if the it has been twenty seconds since the car shut
        {
            if (digitalRead(door)!=0) //has the door been opened, ground when closed
                {
                    Serial.print("door opened");//tracking
                    Serial.print('\n'); //new line for ease of reading
                    lockf();//locks the doors
                    scanf();//return to scan function
                 }
                else //door has not been opened
                    {
                        Serial.print("door not opened");//tracking
                        Serial.print('\n'); //new line for ease of reading
                        doorchko(); //restart this check
                    }  
        }
        else
            {
                Serial.print("20 seconds has passed, locking doors");//tracking
                Serial.print('\n'); //new line for ease of reading
                lockf(); //locks the doors    
                scanf();//return to scan function
            }
}
