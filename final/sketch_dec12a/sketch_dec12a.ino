#include <WiFi.h>
#include<PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 

#define PORT 8888 
#define MAX_LEN 1024 
#define SDA 13                    //Define SDA pins
#define SCL 14                    //Define SCL pins
#define BUTTONW 19                 //Define buttons
#define BUTTONE 2
#define BUTTONS 4
#define BUTTONN 15
#define BUTTONQ 5


// this is setting the lcd variable to be manipulated
LiquidCrystal_I2C lcd(0x27, 16, 2);

// this is setting up the variables for the internet connection
const char *ssid_Router = "iPhone";
const char *password_Router = "12345678";
const char *mqtt_server = "34.136.238.197";
const char *mqtt_subscribe_topic = "transferMessage";
const char *mqtt_publish_topic = "transferMovement";

// This is setting up the variables
bool gameContinue = true;

WiFiClient espClient;
PubSubClient client(espClient);

int sockfd;

void callback(const char *topic, byte *payload, unsigned int length) {
  Serial.print("In here?");
  lcd.setCursor(0,1);            
  lcd.print("                ");
  String message = "";
  for(int i=0; i<length; i++){
    message += (char)payload[i];
  }
  Serial.print("Messge grabbed: ");
  Serial.println(message);
  if(length == 1) {
    gameContinue = false;
    lcd.setCursor(0,1);             // Move the cursor to row 0, column 0
    lcd.print(" -You Finished- ");
  } else {
    lcd.setCursor(0,1);             // Move the cursor to row 0, column 0
    lcd.print(message);
  }
}

// THIS IS THE FIRST THING THAT RUNS
void setup() {
  Serial.begin(115200);
  client.setServer(mqtt_server, 1883);
  setup_wifi();
  client.setCallback(callback);
  // sets up the button
  pinMode(BUTTONW, INPUT);
  pinMode(BUTTONE, INPUT);
  pinMode(BUTTONS, INPUT);
  pinMode(BUTTONN, INPUT);

  //Once the client has been set up with the broker, we are going to configure and light up the lcd; this will be a way for us to know when we are connected and ready to go
  Wire.begin(SDA, SCL);           // attach the IIC pin
  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();                     // LCD driver initialization
  lcd.backlight();                // Open the backlight
  lcd.setCursor(0,0);             // Move the cursor to row 0, column 0
  lcd.print("Game Starting...");     // The print content is displayed on the LCD
}

// connecting to wifi/the router that you are using
void setup_wifi() {
  Serial.println("Setup start");
  WiFi.begin(ssid_Router, password_Router);
  Serial.println(String("Connecting to ")+ssid_Router);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Setup End");
}

// called if the connection is not established/if it is running for the first time
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("transferMessage"); // Subscribe to the desired topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// this callback function runs automatically once a publisher publishes something to a topic that this client has already subscribed to

bool i2CAddrTest(uint8_t addr) {
  Wire.begin();
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}

// I think that this loop is the main function
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.print("Is this in here?");
  struct sockaddr_in servaddr;
  // Creating TCP socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0); 
  if (sockfd == -1) { 
      perror("Socket creation failed"); 
      exit(EXIT_FAILURE); 
  }
  Serial.print("past creating the socket");
  memset(&servaddr, 0, sizeof(servaddr)); 
  // Filling server information 
  servaddr.sin_family = AF_INET; 
  servaddr.sin_port = htons(PORT); 
  servaddr.sin_addr.s_addr = inet_addr("34.136.238.197"); // Change this to the server's IP 
  // Connect to the server 
  Serial.print("going to connect");
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
      Serial.print("Connection failed"); 
      exit(EXIT_FAILURE); 
  }  
  Serial.print("connected to server?");

  while(gameContinue){
    client.loop();
    getUserInput();
    delay(500);
  }

  close(sockfd); 
  delay(10000000);
}

void getUserInput() {
    if(digitalRead(BUTTONW) == HIGH){
      Serial.print("WE ARE IN WEST");
      lcd.setCursor(0,0);             // Move the cursor to row 0, column 0
      lcd.print(" You moved left ");
      write(sockfd, "a", 1); 
      //client.publish(mqtt_publish_topic, temp);
    } else if (digitalRead(BUTTONE) == HIGH){
      Serial.print("WE ARE IN EAST");
      lcd.setCursor(0,0);             // Move the cursor to row 0, column 0
      lcd.print("You moved right");
      write(sockfd, "d", 1);
      //client.publish(mqtt_publish_topic, temp);
    } else if (digitalRead(BUTTONS) == HIGH){
       Serial.print("WE ARE IN SOUTH");
      lcd.setCursor(0,0);             // Move the cursor to row 0, column 0
      lcd.print(" You moved down ");
      write(sockfd, "s", 1);
      //client.publish(mqtt_publish_topic, temp);
    } else if (digitalRead(BUTTONN) == HIGH){
      Serial.print("WE ARE IN NORTH");
      lcd.setCursor(0,0);             // Move the cursor to row 0, column 0
      lcd.print("  You moved up  ");
      write(sockfd, "w", 1);
      //client.publish(mqtt_publish_topic, temp);
    }
}
