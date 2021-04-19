
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 49 //reset
#define SS_PIN 53 //SDA

const int rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en , d4, d5, d6, d7);
MFRC522 mfrc522(SS_PIN, RST_PIN);

MFRC522::Uid uids_masters[10];
int uid_m_size = 0;

MFRC522::Uid uids_user[10];
int uid_u_size = 0;

const byte rows = 4;
const byte cols = 4;
char hexaKeys[rows][cols] = 
{
   {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'*','0','#','D'}
};

byte rowPins[rows] = {22,23,24,25};
byte colsPins[cols] = {26,27,28,29};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colsPins, rows, cols);

void setup() 
{
  //init RFID
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  while(!Serial);
  
  Serial.println(F("RFID reading UID"));
  //init LCD
  lcd.begin(16, 2);
  lcd.print("Bonjour ;)");
  delay(2000);
}

void loop() 
{
  char str_uid[32] = "";
  lcd.clear();
  if(uid_m_size == 0)
  {
    lcd.print("Register master");
    lcd.setCursor(0,1);
    lcd.print("key please");
  }
  if(mfrc522.PICC_IsNewCardPresent())
  {
    if(mfrc522.PICC_ReadCardSerial())
    {
      //print_detected_uid(mfrc522);
      array_to_string(mfrc522.uid.uidByte, 4 , str_uid);

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Key detected");
      
      lcd.setCursor(0,1);
      lcd.print(str_uid);

      delay(2000);
      
      if(uid_m_size == 0)
      {
        uids_masters[0] = mfrc522.uid;
        uid_m_size++;
        lcd.clear();
        lcd.print("Master Key");
        lcd.setCursor(0,1);
        lcd.print("resgistered");
        delay(2000);
      }
      bool master = false;
      for(unsigned int i = 0; i < uid_m_size; i++)
      {
        if(compare_uids(uids_masters[i], mfrc522.uid))
        {
          display_menu();
          master = true;
          break;
        }
      }
      if(!master)
      {
        bool access = false;
        for(unsigned int i = 0; i < uid_u_size; i++)
        {
          if(compare_uids(uids_user[i], mfrc522.uid))
          {
            access = true;
            break;
          }
        }
        lcd.clear();
        lcd.setCursor(0,0);
        
        if(access)
            lcd.print("Access accorded");
        else
            lcd.print("Access blocked");
            
        delay(1000);
      }
      mfrc522.PICC_HaltA(); //close para el badge
    }
  }
}
void display_menu()
{
  unsigned int menu_size = 4;
  unsigned int menu_page = 0;
  bool loop_menu = true;
  while(loop_menu)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Menu: A:<= B:=>");
    print_Menu_Page(menu_page);
    
    char customKey = customKeypad.getKey();
    if(customKey == 'B')
    {
      menu_page++;
      if(menu_page == menu_size)
        menu_page = 0;
    }
    else if(customKey == 'A')
    {
      menu_page--;
      if(menu_page == -1)
        menu_page = menu_size-1;
    }
    else if(customKey == 'C')
    {
      switch(menu_page)
      {
        case 1:
          add_user_uid();
        break;
        case 2:
          delete_key();
        break;
        case 3:
          loop_menu = false;
        break;
        default:
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Access accorded");
          delay(1000);
          loop_menu = false;
        break;
      }
    }
    delay(100);
  }
}
void add_user_uid()
{
  MFRC522 s = mfrc522;
  while(true)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Waiting for key");
    if(mfrc522.PICC_IsNewCardPresent())
    {
      if(mfrc522.PICC_ReadCardSerial())
      {
        uids_user[uid_u_size++] = mfrc522.uid;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("new key saved");
        delay(2000);
        break;
      }
    }
    delay(500);
  }
  mfrc522.PICC_HaltA();
  mfrc522 = s;
}
void delete_key()
{
  MFRC522 s = mfrc522;
  bool d_loop = true;
  while(d_loop)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Waiting for key");
    if(mfrc522.PICC_IsNewCardPresent())
    {
      if(mfrc522.PICC_ReadCardSerial())
      {
        for(unsigned int i = 0; i < uid_m_size; i++)
        {
          if(compare_uids(mfrc522.uid, uids_masters[i]))
          {
            for(unsigned int j = i; j < uid_m_size-1; j++)
            {
              uids_masters[j] = uids_masters[j+1];
            }
            uid_m_size--;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("key removed");
            delay(2000);
            d_loop = false;
            break;
          }
        }
        for(unsigned int i = 0; i < uid_u_size; i++)
        {
          if(compare_uids(mfrc522.uid, uids_user[i]))
          {
            for(unsigned int j = i; j < uid_u_size-1; j++)
            {
              uids_user[j] = uids_user[j+1];
            }
            uid_u_size--;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("key removed");
            delay(2000);
            d_loop = false;
            break;
          }
        }
      }
    }
    delay(500);
  }
  mfrc522.PICC_HaltA();
  mfrc522 = s;
}
void print_Menu_Page(unsigned int i)
{
  lcd.setCursor(0,1); 
  switch(i)
  {
    case 1:
      lcd.print("2.Add Key");
      lcd.setCursor(12,1);
      lcd.print("=> C");
    break;
    case 2:
      lcd.print("3.Delete Key");
      lcd.setCursor(12,1);
      lcd.print("=> C");
    break;
    case 3:
      lcd.print("4.Close Menu");
      lcd.setCursor(12,1);
      lcd.print("=> C");
    break;
    default:
      lcd.print("1.Access");
      lcd.setCursor(12,1);
      lcd.print("=> C");
    break;
  }
}
bool compare_uids(MFRC522::Uid A, MFRC522::Uid B)
{
  if(A.size != B.size)
    return false;
    
  for(unsigned int i = 0; i < A.size; i++)
  {
    if(A.uidByte[i] != B.uidByte[i])
      return false;
  }
  return true;
}
void array_to_string(byte array[], unsigned int len, char buffer[])
{
  byte nib1, nib2;
  for(unsigned int i = 0; i < len; i++)
  {
    nib1 = (array[i] >> 4) & 0x0F;
    nib2 = (array[i] >> 0) & 0x0F;
    buffer[i*2+0] = nib1 < 0xA ? '0'+nib1 : 'A'+nib1 -0xA;
    buffer[i*2+1] = nib2 < 0xA ? '0'+nib2 : 'A'+nib2 -0xA;
  }
}
void print_detected_uid(MFRC522 mfrc522)
{
  Serial.print("Tag UID:");
  for(byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10?" 0":" ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  Serial.println();
}
void print_master_uids()
{
  for(int i = 0; i < uid_m_size; i++)
  {
    Serial.print("Uids[");
    Serial.print(i);
    Serial.print("]: ");
    for(byte j = 0; j < 10; j++)
    {
      Serial.print(uids_masters[i].uidByte[j] < 0x10?" 0":" ");
      Serial.print(uids_masters[i].uidByte[j], HEX);
    }
    Serial.println();
  }
}
