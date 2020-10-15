#include <StringSplitter.h>

//--------------------------------------------------------------- This is the firmware for the cabinets locks 
//---------------------------- it uses a uart link between the locks and is written with the goal that its semi infinitely scaleable by limiting the need for every lock to have its own id
String command;
boolean command_started = false;
boolean command_ready = false;
char receivedChar;
boolean newData = false;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    //Serial.println("<Arduino is ready>");
}

void loop() {
    recvOneChar();
    getCommand();
    showNewData();
    
}

void recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        newData = true;
    }
}
//------------------------------------------------ Stiching togater the command from uart since you need to send uart data char by char
void getCommand() {
  if (newData == true && command_started == true && receivedChar != '>') {
      command += receivedChar;
    }
    
  if (newData == true && receivedChar == '<') {
      //Serial.println("command started");
      command_started = true;
    }
    
    if (newData == true && receivedChar == '>') {
      //Serial.println("command ended");
      command_started = false;
      command_ready = true;
      
    }
      //----------------------------------------- Command handling 
      //------- the command from should be a string split by ':' the command is structured like this target/status:lockId
      
    if (command_ready == true) {
      String  commandPart1;
      String  commandPart2;
       //Serial.println(command);
       //---------------------- splitting the command into pices couse whytf would arduC have an inbuilt split str function 
       StringSplitter *splitter = new StringSplitter(command, ':', 3);
       int itemCount = splitter->getItemCount();
       commandPart1 = splitter->getItemAtIndex(0);
       commandPart2 = splitter->getItemAtIndex(1);
       //--- checking if the this lock should unlock else it should pass the command along
       if (commandPart1 != commandPart2 && commandPart2 != "open") {
          //Serial.println("passing on to next lock");
          int increment = (commandPart2.toInt()) + 1;
          int id = commandPart1.toInt();
          //-------------- serial write only wants chars or ints 
          //Serial.print("< %i8 : %i8 >", id, increment); 
          
          Serial.print("<");
          Serial.print(id);
          Serial.print(":");
          Serial.print(increment);
          Serial.print(">");
          
        }
        //----- unlock the lock and pass on unlocked messange
        else{
          //Serial.println("hey thats me");
          digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(1000);                       // wait for a second
          digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
          delay(1000); 
        }

       
       command_ready = false;
       command = "";
      }

  }


  //-------- function that pulls in new characters and verifies that they arent old 
void showNewData() {
    if (newData == true) {
      Serial.println(receivedChar);
        newData = false;
    }
}
