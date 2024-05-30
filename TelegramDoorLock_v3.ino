#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Servo.h>

#define WIFI_SSID "Write your Wifi SSID Here"
#define WIFI_PASSWORD "Write youter Wifi Password Here"
#define BOT_TOKEN "write your token here"

const unsigned long BOT_MTBS = 1000;
const unsigned long LED_INTERVAL = 5000;
const unsigned long LED_DURATION = 100;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;
unsigned long led_lasttime = 0;
unsigned long servo_lasttime = 0;
int ledStatus = 0;
int servoMoving = 0;

Servo myservo;
const int servoPin = D3; // This is servo pin connect to Wemos module
const int ledPin = D4;
const int buttonPin = D1; // Connect push button to this pin and ground pin

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "Bukaaa")
    {
      digitalWrite(ledPin, LOW);
      ledStatus = 1;
      bot.sendMessage(chat_id, "[<--->]", "");
      servoMoving = 1;
      myservo.write(180);  // Move the servo to 180 degrees
      digitalWrite(ledPin, HIGH);
    }

    if (text == "Kunci")
    {
      digitalWrite(ledPin, LOW);
      ledStatus = 0;
      bot.sendMessage(chat_id, "[>-<]", "");
      servoMoving = 1;
      myservo.write(0);  // Move the servo to 0 degrees
      digitalWrite(ledPin, HIGH);
    }

    if (text == "Status")
    {
      if (ledStatus)
      {
        bot.sendMessage(chat_id, "[<->]", "");
      }
      else
      {
        bot.sendMessage(chat_id, "[><]", "");
      }
    }
  }
  

  servoMoving = 0;
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  delay(10);
  digitalWrite(ledPin, HIGH);

  configTime(0, 0, "pool.ntp.org");
  secured_client.setTrustAnchors(&cert);
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  myservo.attach(servoPin);
  myservo.write(0);
  delay(500);
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }

  if (millis() - led_lasttime >= LED_INTERVAL && !servoMoving)
  {
    digitalWrite(ledPin, LOW);
    delay(LED_DURATION);
    digitalWrite(ledPin, HIGH);
    led_lasttime = millis();
  }

  if (digitalRead(buttonPin) == LOW) // This button use to change the wifi connection if the fisrt wifi ssid not worked
  {
    Serial.println("Button D1 pressed, changing WiFi...");
    const char *newSSID = "Alternative Wifi SSID";
    const char *newPassword = "Alternative Wifi paswword";

    WiFi.begin(newSSID, newPassword);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
    }
    Serial.println("Connected to new WiFi.");
  }

  if (millis() - led_lasttime >= LED_INTERVAL && !servoMoving)
  {
    digitalWrite(ledPin, LOW);
    delay(LED_DURATION);
    digitalWrite(ledPin, HIGH);

    led_lasttime = millis();
}
}