#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
// CONFIGURACION DE DISPOSITIVOS
#define WIRE_BUS 13
#define WIRE_CHILLER_BUS 12
// RELAY DE FERMENTADORES Y CHILLER
#define RELAY_FERM_1 8
#define RELAY_FERM_2 9
#define RELAY_FERM_3 10
#define RELAY_FERM_4 11
#define RELAY_CHILLER_C 3
#define RELAY_CHILLER_H A0
#define RELAY_PUMP 2
// POSICIONES DE GUARDADO EEPROM
#define EEPROM_TEMP_1 0
#define EEPROM_TEMP_2 4
#define EEPROM_TEMP_3 8
#define EEPROM_TEMP_4 12
#define EEPROM_TEMP_HYSTERESIS 16
#define EEPROM_TEMP_HYSTERESIS_CHILLER 20
#define EEPROM_TEMP_CHILLER_C 24

LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire wireSensors(WIRE_BUS);
OneWire wireChiller(WIRE_CHILLER_BUS);
DallasTemperature Sensors(&wireSensors);
DallasTemperature SensorChiller(&wireChiller);
// VARIABLES GLOBALES FUNCION FLANCO SUBIDA DE  DEBOTONES
#define BTN_MENU 3
#define BTN_NEXT 0
#define BTN_UP 2
#define BTN_DOWN 1
uint8_t button[4]{
    4,
    5,
    6,
    7};
uint8_t button_state[4];
uint8_t RelayPump = RELAY_PUMP,
        RelayValve_1 = RELAY_FERM_1,
        RelayValve_2 = RELAY_FERM_2,
        RelayValve_3 = RELAY_FERM_3,
        RelayValve_4 = RELAY_FERM_4,
        RelayChiller_C = RELAY_CHILLER_C,
        RelayChiller_H = RELAY_CHILLER_H;
uint8_t btn_push(uint8_t btn)
{
  uint8_t newVal = digitalRead(button[btn]);
  uint8_t result = button_state[btn] != newVal && newVal == 1;
  button_state[btn] = newVal;
  return result;
};
// VARIABLES GLOBALES Y FUNCIONDES DE SENSORES DE TEMPERATURA
#define DEFAULT_TEMP 15.0
#define DEFAULT_TEMP_CHILLER_c 5.0
#define MAX_TEMP_TANKS 25.0
#define MIN_TEMP_TANKS .1
#define MAX_TEMP_CHILLER 10
#define MIN_TEMP_CHILLER -4.9
float TF_1, TF_2, TF_3, TF_4, TC_C, TC_H;
float hysteresis,
    hysteresisChiller,
    DefaultHysteresis,
    DefaultHysteresisC;
float TempFerm_1,
    TempFerm_2,
    TempFerm_3,
    TempFerm_4,
    TempChiller_C,
    DefaultTemp_1 = DEFAULT_TEMP,
    DefaultTemp_2 = DEFAULT_TEMP,
    DefaultTemp_3 = DEFAULT_TEMP,
    DefaultTemp_4 = DEFAULT_TEMP,
    DefaultTempChiller = DEFAULT_TEMP_CHILLER_c;

DeviceAddress Tank_1, Tank_2, Tank_3, Tank_4, ChillerC;
// ESTADO DE TANQUES
bool Tank_1_Status = true;
bool Tank_2_Status = true;
bool Tank_3_Status = true;
bool Tank_4_Status = true;
// DISPLAY DE TEMPERATURA TANQUES ** HOME **
void displayTemp()
{
  Sensors.requestTemperatures();
  Sensors.setResolution(Tank_1, 9);
  Sensors.setResolution(Tank_2, 9);
  Sensors.setResolution(Tank_3, 9);
  Sensors.setResolution(Tank_4, 9);
  TF_1 = Sensors.getTempC(Tank_1);
  TF_2 = Sensors.getTempC(Tank_2);
  TF_3 = Sensors.getTempC(Tank_3);
  TF_4 = Sensors.getTempC(Tank_4);
  // DISPLAY TF_1 TF_2

  lcd.setCursor(0, 0);
  lcd.print("T1:");
  lcd.setCursor(3, 0);
  if (Tank_1_Status == true)
  {

    lcd.print(TF_1, 1);
    lcd.setCursor(7, 0);
    lcd.print((char)223);
  }
  else
  {
    lcd.print("OFF ");
  }
  lcd.setCursor(0, 1);
  lcd.print("T2:");
  lcd.setCursor(3, 1);
  if (Tank_2_Status == true)
  {

    lcd.print(TF_2, 1);
    lcd.setCursor(7, 1);
    lcd.print((char)223);
  }
  else
  {
    lcd.print("OFF ");
  }
  // DISPLAY TF_3 TF_4
  lcd.setCursor(8, 0);
  lcd.print("T3:");
  lcd.setCursor(11, 0);
  if (Tank_3_Status == true)
  {

    lcd.print(TF_3, 1);
    lcd.setCursor(15, 0);
    lcd.print((char)223);
  }
  else
  {
    lcd.print("OFF ");
  }
  lcd.setCursor(8, 1);
  lcd.print("T4: ");
  lcd.setCursor(11, 1);
  if (Tank_4_Status == true)
  {
    lcd.print(TF_4, 1);
    lcd.setCursor(15, 1);
    lcd.print((char)223);
  }
  else
  {
    lcd.print("OFF ");
  }
}
// CHEQUEO DE SENSORES DE TANQUES
void checkSensors()
{
  if (!wireSensors.search(Tank_1))
  {
    Tank_1_Status = false;
    lcd.setCursor(2, 0);
    lcd.print("FERMENTER 1");
    lcd.setCursor(2, 1);
    lcd.print("DISCONNECTED");
    delay(500);
    lcd.clear();
  }
  else
  {
    lcd.setCursor(2, 0);
    lcd.print("FERMENTER 1");
    lcd.setCursor(2, 1);
    lcd.print("CONNECTED");
    delay(500);
    lcd.clear();
  }
  if (!wireSensors.search(Tank_2))
  {
    Tank_2_Status = false;
    lcd.setCursor(2, 0);
    lcd.print("FERMENTER 2");
    lcd.setCursor(2, 1);
    lcd.print("DISCONNECTED");
    delay(500);
    lcd.clear();
  }
  else
  {
    lcd.setCursor(2, 0);
    lcd.print("FERMENTER 2");
    lcd.setCursor(2, 1);
    lcd.print("CONNECTED");
    delay(500);
    lcd.clear();
  }
  if (!wireSensors.search(Tank_3))
  {
    Tank_3_Status = false;
    lcd.setCursor(2, 0);
    lcd.print("FERMENTER 3");
    lcd.setCursor(2, 1);
    lcd.print("DISCONNECTED");
    delay(500);
    lcd.clear();
  }
  else
  {
    lcd.setCursor(2, 0);
    lcd.print("FERMENTER 3");
    lcd.setCursor(2, 1);
    lcd.print("CONNECTED");
    delay(500);
    lcd.clear();
  }
  if (!wireSensors.search(Tank_4))
  {
    Tank_4_Status = false;
    lcd.setCursor(2, 0);
    lcd.print("FERMENTER 4");
    lcd.setCursor(2, 1);
    lcd.print("DISCONNECTED");
    delay(500);
    lcd.clear();
  }
  else
  {
    lcd.setCursor(2, 0);
    lcd.print("FERMENTER 4");
    lcd.setCursor(2, 1);
    lcd.print("CONNECTED");
    delay(500);
    lcd.clear();
  }
}
// VAIRABLES DE CONTROL GENERALES
uint8_t StateOption = 0;
uint8_t StateTank = 0;
uint8_t StateChiller = 0;
boolean SubMenu = false;
boolean OnOffPump = false;

unsigned long monitorTemp;
unsigned long initMonitor;
unsigned long initMenu;
unsigned long menuTemp;
// VARIABLES STRING
String OptionMenu[] = {"FERMENTER", "  CHILLER  ", "  SYSTEM  "};
String TitleMenu[] = {"TANK 1", "TANK 2", "TANK 3", "TANK 4", "FLUID", "HEAT", "HYST", "EXIT"};
// FUNCIONES DE GRAFICADO LCD
String PrintTitleMenu(String Option, uint8_t Position)
{
  if (menuTemp - initMenu >= 1000)
  {
    initMenu = millis();
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("CONFIG");
    lcd.setCursor(Position, 1);
    lcd.print(Option);
  }
  return Option;
}
String PrintOptionMenu(String Title, uint8_t Column0)
{
  lcd.clear();
  lcd.setCursor(Column0, 0);
  lcd.print(Title);
  return Title;
}
void SetTank_1()
{
  if (Tank_1_Status == true)
  {
    lcd.setCursor(7, 0);
    lcd.print("Set:");
    lcd.setCursor(11, 0);
    lcd.print(EEPROM.get(EEPROM_TEMP_1, TempFerm_1), 1);
    lcd.setCursor(15, 0);
    lcd.print((char)223);
    lcd.setCursor(0, 1);
    lcd.print(DefaultTemp_1, 1);
    lcd.setCursor(4, 1);
    lcd.print((char)223);
    lcd.setCursor(7, 1);
    lcd.print("PRESS +/-");
    if (btn_push(BTN_UP) && DefaultTemp_1 < MAX_TEMP_TANKS)
    {
      DefaultTemp_1 += .1;
    }
    else if (btn_push(BTN_DOWN) && DefaultTemp_1 >= MIN_TEMP_TANKS)
    {
      DefaultTemp_1 -= .1;
    }
    if (btn_push(BTN_MENU))
    {
      TempFerm_1 = EEPROM.put(EEPROM_TEMP_1, DefaultTemp_1);
    }
  }
  else
  {
    lcd.setCursor(2, 1);
    lcd.print("DISCONNECTED");
  }
}
void SetTank_2()
{
  if (Tank_2_Status == true)
  {
    lcd.setCursor(7, 0);
    lcd.print("Set:");
    lcd.setCursor(11, 0);
    lcd.print(EEPROM.get(EEPROM_TEMP_2, TempFerm_2), 1);
    lcd.setCursor(15, 0);
    lcd.print((char)223);
    lcd.setCursor(0, 1);
    lcd.print(DefaultTemp_2, 1);
    lcd.setCursor(4, 1);
    lcd.print((char)223);
    lcd.setCursor(7, 1);
    lcd.print("PRESS +/-");
    if (btn_push(BTN_UP) && DefaultTemp_2 < MAX_TEMP_TANKS)
    {
      DefaultTemp_2 += .1;
    }
    else if (btn_push(BTN_DOWN) && DefaultTemp_2 >= MIN_TEMP_TANKS)
    {
      DefaultTemp_2 -= .1;
    }
    if (btn_push(BTN_MENU))
    {
      TempFerm_2 = EEPROM.put(EEPROM_TEMP_2, DefaultTemp_2);
    }
  }
  else
  {
    lcd.setCursor(2, 1);
    lcd.print("DISCONNECTED");
  }
}
void SetTank_3()
{

  if (Tank_3_Status == true)
  {
    lcd.setCursor(7, 0);
    lcd.print("Set:");
    lcd.setCursor(11, 0);
    lcd.print(EEPROM.get(EEPROM_TEMP_3, TempFerm_3), 1);
    lcd.setCursor(15, 0);
    lcd.print((char)223);
    lcd.setCursor(0, 1);
    lcd.print(DefaultTemp_3, 1);
    lcd.setCursor(4, 1);
    lcd.print((char)223);
    lcd.setCursor(7, 1);
    lcd.print("PRESS +/-");
    if (btn_push(BTN_UP) && DefaultTemp_3 < MAX_TEMP_TANKS)
    {
      DefaultTemp_3 += .1;
    }
    else if (btn_push(BTN_DOWN) && DefaultTemp_3 >= MIN_TEMP_TANKS)
    {
      DefaultTemp_3 -= .1;
    }
    if (btn_push(BTN_MENU))
    {
      TempFerm_3 = EEPROM.put(EEPROM_TEMP_3, DefaultTemp_3);
    }
  }
  else
  {
    lcd.setCursor(2, 1);
    lcd.print("DISCONNECTED");
  }
}
void SetTank_4()
{
  if (Tank_4_Status == true)
  {
    lcd.setCursor(7, 0);
    lcd.print("Set:");
    lcd.setCursor(11, 0);
    lcd.print(EEPROM.get(EEPROM_TEMP_4, TempFerm_4), 1);
    lcd.setCursor(15, 0);
    lcd.print((char)223);
    lcd.setCursor(0, 1);
    lcd.print(DefaultTemp_4, 1);
    lcd.setCursor(4, 1);
    lcd.print((char)223);
    lcd.setCursor(7, 1);
    lcd.print("PRESS +/-");
    if (btn_push(BTN_UP) && DefaultTemp_4 < MAX_TEMP_TANKS)
    {
      DefaultTemp_4 += .1;
    }
    else if (btn_push(BTN_DOWN) && DefaultTemp_4 >= MIN_TEMP_TANKS)
    {
      DefaultTemp_4 -= .1;
    }
    if (btn_push(BTN_MENU))
    {
      TempFerm_4 = EEPROM.put(EEPROM_TEMP_4, DefaultTemp_4);
    }
  }
  else
  {
    lcd.setCursor(2, 1);
    lcd.print("DISCONNECTED");
  }
}
void SetChiller()
{
  lcd.setCursor(7, 0);
  lcd.print("Set:");
  lcd.setCursor(11, 0);
  lcd.print(EEPROM.get(EEPROM_TEMP_CHILLER_C, TempChiller_C), 1);
  lcd.setCursor(15, 0);
  lcd.print((char)223);
  lcd.setCursor(0, 1);
  lcd.print(DefaultTempChiller);
  lcd.setCursor(4, 1);
  lcd.print((char)223);
  lcd.setCursor(7, 1);
  lcd.print("PRESS +/-");
  if (btn_push(BTN_UP) && DefaultTempChiller < MAX_TEMP_CHILLER)
  {
    DefaultTempChiller += .1;
  }
  else if (btn_push(BTN_DOWN) && DefaultTempChiller >= MIN_TEMP_CHILLER)
  {
    DefaultTempChiller -= .1;
  }
  if (btn_push(BTN_MENU))
  {
    TempChiller_C = EEPROM.put(EEPROM_TEMP_CHILLER_C, DefaultTempChiller);
  }
}
void On_Off_Tanks()
{
  float OffFerm_1 = TempFerm_1 - hysteresis,
        OffFerm_2 = TempFerm_2 - hysteresis,
        OffFerm_3 = TempFerm_3 - hysteresis,
        OffFerm_4 = TempFerm_4 - hysteresis,
        OffChillerC = TempChiller_C - hysteresisChiller;
  if ((TF_1 < OffFerm_1 && TF_2 < OffFerm_2 && TF_3 < OffFerm_3 && TF_4 < OffFerm_4) || TC_C > OffChillerC)
  {
    OnOffPump = false;
    RelayPump = LOW;
  }
  else
  {
    OnOffPump = true;
    RelayPump = HIGH;
  }
  boolean onValve_1 = (TF_1 > OffFerm_1) ? true : false;
  onValve_1 &&Tank_1_Status ? RelayValve_1 = HIGH : RelayValve_1 = LOW;
  boolean onValve_2 = (TF_2 > OffFerm_2) ? true : false;
  onValve_2 &&Tank_2_Status ? RelayValve_2 = HIGH : RelayValve_2 = LOW;
  boolean onValve_3 = (TF_3 > OffFerm_3) ? true : false;
  onValve_3 &&Tank_3_Status ? RelayValve_3 = HIGH : RelayValve_3 = LOW;
  boolean OnValve_4 = (TF_4 > OffFerm_4) ? true : false;
  OnValve_4 &&Tank_4_Status ? RelayValve_4 = HIGH : RelayValve_4 = LOW;
}
void SetHysteresis()
{
  lcd.setCursor(7, 0);
  lcd.print("Set:");
  lcd.setCursor(11, 0);
  lcd.print(EEPROM.get(EEPROM_TEMP_HYSTERESIS, hysteresis), 1);
  lcd.setCursor(14, 0);
  lcd.print((char)223);
  lcd.setCursor(0, 1);
  lcd.print(DefaultHysteresis, 1);
  lcd.setCursor(4, 1);
  lcd.print((char)223);
  lcd.setCursor(7, 1);
  lcd.print("PRESS +/-");
  if (DefaultHysteresis < 5.0 && btn_push(BTN_UP))
  {
    DefaultHysteresis += .5;
  }
  else if (DefaultHysteresis > 0.0 && btn_push(BTN_DOWN))
  {
    DefaultHysteresis -= .5;
  }
  else if (btn_push(BTN_MENU))
  {
    hysteresis = EEPROM.put(EEPROM_TEMP_HYSTERESIS, DefaultHysteresis);
  }
}
void SetHysteresisChiller()
{
  lcd.setCursor(7, 0);
  lcd.print("Set:");
  lcd.setCursor(11, 0);
  lcd.print(EEPROM.get(EEPROM_TEMP_HYSTERESIS_CHILLER, hysteresisChiller), 1);
  lcd.setCursor(14, 0);
  lcd.print((char)223);
  lcd.setCursor(0, 1);
  lcd.print(DefaultHysteresisC, 1);
  lcd.setCursor(4, 1);
  lcd.print((char)223);
  lcd.setCursor(7, 1);
  lcd.print("PRESS +/-");
  if (DefaultHysteresisC < 5.0 && btn_push(BTN_UP))
  {
    DefaultHysteresisC += .5;
  }
  else if (DefaultHysteresisC > 0.0 && btn_push(BTN_DOWN))
  {
    DefaultHysteresisC -= .5;
  }
  else if (btn_push(BTN_MENU))
  {
    hysteresisChiller = EEPROM.put(EEPROM_TEMP_HYSTERESIS_CHILLER, DefaultHysteresisC);
  }
}
void TankOptions()
{
  switch (StateTank)
  {
  case 0:
    PrintTitleMenu(OptionMenu[0], 4);

    if (btn_push(BTN_NEXT))
    {
      StateTank++;
      SubMenu = true;
      PrintOptionMenu(TitleMenu[0], 0);
    }

    break;
  case 1:
    SetTank_1();
    if (btn_push(BTN_NEXT))
    {
      StateTank++;
      PrintOptionMenu(TitleMenu[1], 0);
    }

    break;
  case 2:
    SetTank_2();
    if (btn_push(BTN_NEXT))
    {
      StateTank++;
      PrintOptionMenu(TitleMenu[2], 0);
    }

    break;
  case 3:
    SetTank_3();

    if (btn_push(BTN_NEXT))
    {

      StateTank++;
      PrintOptionMenu(TitleMenu[3], 0);
    }

    break;
  case 4:
    SetTank_4();
    if (btn_push(BTN_NEXT))
    {
      StateTank++;
      PrintOptionMenu(TitleMenu[6], 0);
    }
    break;
  case 5:
    SetHysteresis();
    if (btn_push(BTN_NEXT))
    {
      StateTank++;
      PrintOptionMenu(TitleMenu[7], 6);
      lcd.setCursor(1, 1);
      lcd.print("PRESS BTN MENU");
    }
    break;
  case 6:
    if (btn_push(BTN_NEXT))
    {
      StateTank = 1;
      PrintOptionMenu(TitleMenu[0], 0);
    }
    else if (btn_push(BTN_MENU))
    {
      StateTank = 0;
      StateOption = 0;
      SubMenu = false;
      lcd.clear();
    }
    break;

  default:
    break;
  }
}
void ChillerOptions()
{
  switch (StateChiller)
  {
  case 0:
    PrintTitleMenu(OptionMenu[1], 3);

    if (btn_push(BTN_NEXT))
    {
      StateChiller++;
      SubMenu = true;
      PrintOptionMenu(TitleMenu[4], 0);
    }

    break;
  case 1:
    SetChiller();
    if (btn_push(BTN_NEXT))
    {
      PrintOptionMenu(TitleMenu[6], 0);
      StateChiller++;
    }
    break;
  case 2:
    SetHysteresisChiller();
    if (btn_push(BTN_NEXT))
    {
      StateChiller++;
      PrintOptionMenu(TitleMenu[7], 6);
      lcd.setCursor(1, 1);
      lcd.print("PRESS BTN MENU");
    }
    break;
  case 3:
    if (btn_push(BTN_NEXT))
    {
      StateChiller = 1;
      PrintOptionMenu(TitleMenu[4], 0);
    }
    else if (btn_push(BTN_MENU))
    {
      StateChiller = 0;
      StateOption = 0;
      SubMenu = false;
      lcd.clear();
    }

    break;
  default:
    break;
  }
}

void setup()
{

  // INICIANDO PUERTO SERIAL
  Serial.begin(9600);

  // iniciando dispay lcd
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("TEMPERATURE");
  lcd.setCursor(4, 1);
  lcd.print("CONTROL");
  delay(2000);
  lcd.clear();

  checkSensors();
  // configuracion de pines de botones
  pinMode(button[BTN_MENU], INPUT_PULLUP);
  pinMode(button[BTN_NEXT], INPUT_PULLUP);
  pinMode(button[BTN_UP], INPUT_PULLUP);
  pinMode(button[BTN_DOWN], INPUT_PULLUP);
  button_state[0] = HIGH;
  button_state[1] = HIGH;
  button_state[2] = HIGH;
  button_state[3] = HIGH;
  // configuracion de los ralay
  pinMode(RELAY_PUMP, OUTPUT);
  pinMode(RELAY_FERM_1, OUTPUT);
  pinMode(RELAY_FERM_2, OUTPUT);
  pinMode(RELAY_FERM_3, OUTPUT);
  pinMode(RELAY_FERM_4, OUTPUT);
  pinMode(RELAY_CHILLER_C, OUTPUT);
  pinMode(RELAY_CHILLER_H, OUTPUT);
  RelayChiller_C = LOW;
  RelayChiller_H = LOW;
  RelayPump = LOW;
  RelayValve_1 = LOW;
  RelayValve_2 = LOW;
  RelayValve_3 = LOW;
  RelayValve_4 = LOW;
  initMonitor = millis();
  initMenu = millis();
}

void loop()
{
  menuTemp = millis();
  monitorTemp = millis();

  if (btn_push(BTN_MENU) && SubMenu == false)
  {
    StateOption++;
  }
  if (btn_push(BTN_MENU) && SubMenu == true)
  {
    StateOption = StateOption;
  }

  if (StateOption == 0)
  {

    displayTemp();
  }
  else if (StateOption == 1)
  {

    TankOptions();
  }
  else if (StateOption == 2)
  {
    StateOption = 2;
    ChillerOptions();
  }
  else if (StateOption >= 3)
  {
    StateOption = 0;
  }

  if (monitorTemp - initMonitor >= 250)
  {
    initMonitor = millis();
    On_Off_Tanks();
  }
}
