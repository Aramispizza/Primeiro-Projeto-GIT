#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define FIREBASE_HOST "url.firebaseio.com"
#define FIREBASE_AUTH "    "
#define WIFI_SSID "rede name"
#define WIFI_PASSWORD "12345678"
#define RST_PIN         5           // define o pino Reset
#define SS_PIN          15          // define o pino slave select

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
SoftwareSerial Central(D2, D3);     //
String a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6, b7, b8, b9, c1, c2, c3, c4, c5, c6, c7, c8, c9, d1, d2, d3, d4, d5;
int k, l, m, n, r;
int trav = 0; int trav2 = 0;
byte auda[2];

//byte de gravação//
byte NumeroGrav[16];
byte SenhaGrav[16];
byte NomeGrav[16];
byte CreditoGrav[16];
//byte de leitura
byte NumeroLei[16];
byte SenhaLei[16];
byte NomeLei[16];
byte CreditoLei[16];
//byte de apagar
byte NumeroApa[16];
//byte SenhaApa[16];
byte NomeApa[16];
byte CreditoApa[16];
//byte de operação
byte Numero[16];
byte Senha[16];
byte Nome[16];
byte Credito[16];


byte CREinser[16];

byte DataSen[16];
byte DataVal[16];
byte clear[16];
byte buffer[16];

int cont = 0;
int Validsen = 0;
int ValidVal = 0;
//*****************************************************************************************//
void setup() {
  pinMode(D3, OUTPUT);
  pinMode(D2, INPUT);


  Serial.begin(9600); // Initialize serial communications with the PC
  Central.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);// connect to wifi.
  Serial.print("conectando");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("conectado: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                           // Init MFRC522 card
  cardapag();
  mensageminicial();
}

//*****************************************************************************************//
void loop()
{

  if (Serial.available() > 0)
  {
    int algo = Serial.read();
    if ((algo == 49) && (trav == 0))
    {
      trav = 1;
      gravacao();
    }
    if ((algo == 50) && (trav == 0))
    {
      trav = 1;
      leitura();
    }
    if ((algo == 51) && (trav == 0))
    {
      trav = 1;
      Financeiro();
    }
    if ((algo == 52) && (trav == 0))
    {
      trav = 1;
      DeletCard();
    }
  }
}
/*
if(Serial.available()>0)
{
int algo= Serial.read();
Central.write(algo);
}*/

void mensageminicial()
{

  Serial.println("Digite 1 para GRAVACAO");
  Serial.println("Digite 2 para LEITURA");
  Serial.println("Digite 3 para OPERACAO");
  Serial.println("Digite 4 para DELETAR CARTAO");
  Serial.println();

}

void mensagem_inicial_cartao()
{
  Serial.println("Aproxime o seu cartao do leitor...");
}


void gravacao ()  //------------------------------------------------------------------------FUNÇÃO QUE GRAVA DADOS NO CARTÃO---------------------------------------------------------------
{
  zeradados();
  mensagem_inicial_cartao();
  r = 0;
  Central.write("gravação#");
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Look for new cards
  while ( ! mfrc522.PICC_IsNewCardPresent())
  {
    delay(100);
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("Card UID:"));    //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");      //IMPRIME NA SERIAL O ID DO CARD
    Serial.print(mfrc522.uid.uidByte[i], HEX);                    //IMPRIME NA SERIAL O ID DO CARD
  }
  Serial.print(F(" PICC type: "));   // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));                     //IMPRIME NA SERIAL O PICC TYPE DO CARD

  MFRC522::StatusCode status;
  byte SenGra[16];
  byte block;
  byte len;

  Serial.setTimeout(20000L) ;     // wait until 20 seconds for input from serial
  // Ask personal data: Family name
  Serial.println(F("INSIRA O NUMERO DO CARTAO, NO FIM DIGITE #"));
  len = Serial.readBytesUntil('#', (char *) NumeroGrav, 16) ; // read family name from serial
  for (byte i = len; i < 16; i++)
  {
    NumeroGrav[i] = 0x00;   // pad with spaces
  }

  Serial.write(NumeroGrav, 16);
  a1 = NumeroGrav[0];   a2 = NumeroGrav[1];  a3 = NumeroGrav[2];  a4 = NumeroGrav[3];  a5 = NumeroGrav[4];   a6 = NumeroGrav[5];   a7 = NumeroGrav[6]; a8 = NumeroGrav[7];
  a9 = NumeroGrav[8];   b1 = NumeroGrav[9];   b2 = NumeroGrav[10]; b3 = NumeroGrav[11]; b4 = NumeroGrav[12]; b5 = NumeroGrav[13];  b6 = NumeroGrav[14];  b7 = NumeroGrav[15];


  block = 28;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));

  // Write block
  status = mfrc522.MIFARE_Write(block, NumeroGrav, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  //---------------------------------------------------------------------------------------------------------
  // CRIA SENHA
  Central.write("senha#");
  Central.setTimeout(20000L) ;
  Central.flush();
  Serial.println(F("Cliente digitando a senha... #"));
  len = Central.readBytesUntil('#', (char *) SenhaGrav, 16);
  for (byte i = len; i < 16; i++) SenhaGrav[i] = 0x00;
  Serial.write(SenhaGrav, 16);

  // Serial.println(F("CRIE A SenhaGrav DO CARD, NO FIM DIGITE #"));
  // len = Serial.readBytesUntil('#', (char *) SenhaGrav, 16) ; // read first name from serial
  //for (byte i = len; i < 16; i++) Senha[i] =0x00;     // pad with spaces

  SenGra[0] = SenhaGrav[0];  SenGra[1] = SenhaGrav[1];  SenGra[2] = SenhaGrav[2];  SenGra[3] = SenhaGrav[3];  SenGra[4] = SenhaGrav[4];  SenGra[5] = SenhaGrav[5];  SenGra[6] = SenhaGrav[6];  SenGra[7] = SenhaGrav[7];
  SenGra[8] = SenhaGrav[8];  SenGra[9] = SenhaGrav[9];  SenGra[10] = SenhaGrav[10];  SenGra[11] = SenhaGrav[11];  SenGra[12] = SenhaGrav[12];  SenGra[13] = SenhaGrav[13];  SenGra[14] = SenhaGrav[14];  SenGra[15] = SenhaGrav[15];

  block = 32;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));
  // Write block
  status = mfrc522.MIFARE_Write(block, SenhaGrav, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  //------------------------------------------------------------------------------------
  Serial.println(F("INSIRA O SEU PRIMEIRO E ULTIMO NOME, NO FIM DIGITE #"));
  len = Serial.readBytesUntil('#', (char *) NomeGrav, 16) ; // read first name from serial
  for (byte i = len; i < 16; i++) NomeGrav[i] = 0x00;    // pad with spaces


  b8 = NomeGrav[0];  b9 = NomeGrav[1];  c1 = NomeGrav[2];   c2 = NomeGrav[3];   c3 = NomeGrav[4];   c4 = NomeGrav[5];   c5 = NomeGrav[6];   c6 = NomeGrav[7];
  c7 = NomeGrav[8];  c8 = NomeGrav[9];  c9 = NomeGrav[10];  d1 = NomeGrav[11];  d2 = NomeGrav[12];  d3 = NomeGrav[13];  d4 = NomeGrav[14];  d5 = NomeGrav[15];

  block = 29;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));
  // Write block
  status = mfrc522.MIFARE_Write(block, NomeGrav, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  //-----------------------------------------------------------
  // SALDO DO CARTÃO
  Serial.println(F("VALOR A SER INSERIDO NO CARTÃO, NO FIM DIGITE #"));
  len = Serial.readBytesUntil('#', (char *) CreditoGrav, 16) ; // read first name from serial
  for (byte i = len; i < 16; i++) CreditoGrav[i] = 0x00;    // pad with spaces

  block = 33;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));
  // Write block
  status = mfrc522.MIFARE_Write(block, CreditoGrav, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));
  k = 1000 * (CreditoGrav[0] - 48);
  l = 100 * (CreditoGrav[1] - 48);
  m = 10 * (CreditoGrav[2] - 48);
  n = CreditoGrav[3] - 48;
  r = k + l + m + n;
  //org();
  Firebase.set("Usuarios/Card=" + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + b1 + b2 + b3 + b4 + b5 + b6 + b7, SenhaGrav); // colocar o nome senha depois
  Firebase.setInt("Usuarios/Nome=" + b8 + b9 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + d1 + d2 + d3 + d4 + d5, r);

  Serial.println(" ");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  trav = 0;
  Central.flush();
  mensageminicial();
}





void DeletCard() //--------------------------------------------------------Função que apaga o card do sistema---------------------------------------------------------------------
{
  Validsen = 0;
  ValidVal = 0;
  Central.flush();
  zeradados();
  mensagem_inicial_cartao();
  FirebaseObject object = Firebase.get ("/");
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  byte block;
  byte len;
  MFRC522::StatusCode status;
  while ( ! mfrc522.PICC_IsNewCardPresent())
  {
    delay(100);
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //-------------------------------------------

  len = 18;
  
  //-------------------------------------------
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 28, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(28, NumeroApa, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  a1 = (NumeroApa[0]);  a2 = (NumeroApa[1]);  a3 = (NumeroApa[2]);  a4 = (NumeroApa[3]);  a5 = (NumeroApa[4]);  a6 = (NumeroApa[5]);
  a7 = (NumeroApa[6]);  a8 = (NumeroApa[7]);  a9 = (NumeroApa[8]);  b1 = (NumeroApa[9]);  b2 = (NumeroApa[10]);  b3 = (NumeroApa[11]);
  b4 = (NumeroApa[12]); b5 = (NumeroApa[13]); b6 = (NumeroApa[14]); b7 = (NumeroApa[15]);

  //------------------------------------------------------------------------------------------------------------------------------



  byte senha4[16];

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 32, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(32, senha4, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;

  }


  //-------------------------------------------------------------------------------------------------------------------

  
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 29, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(29, NomeApa, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  b8 = NomeApa[0];  b9 = NomeApa[1];  c1 = NomeApa[2];  c2 = NomeApa[3];  c3 = NomeApa[4];  c4 = NomeApa[5];  c5 = NomeApa[6];  c6 = NomeApa[7];
  c7 = NomeApa[8];  c8 = NomeApa[9];  c9 = NomeApa[10];  d1 = NomeApa[11];  d2 = NomeApa[12];  d3 = NomeApa[13];  d4 = NomeApa[14];  d5 = NomeApa[15];

  //------------------------------------------------------------------------------------------------------------------------------------



  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 33, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(33, CreditoApa, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }



  //------------------------------------------------------------------------------------------------

  Central.setTimeout(20000L);
  String senha = object.getString("Usuarios/Card=" + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + b1 + b2 + b3 + b4 + b5 + b6 + b7);
  int valor = object.getInt("Usuarios/Nome=" + b8 + b9 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + d1 + d2 + d3 + d4 + d5);
  senha.getBytes(DataSen, 7);

  k = (1000 * (CreditoApa[0] - 48));
  l = (100 * (CreditoApa[1] - 48));
  m = (10 * (CreditoApa[2] - 48));
  n = (CreditoApa[3] - 48);
  r = k+l+m+n;
  Serial.println(r);
  Serial.println(valor);
  Serial.write(senha4, 16);
  Serial.write(DataSen, 16);

  Central.write("deletsen#");

  Serial.println(F("Cliente digitando a senha... #"));
  Central.flush();
  byte Senha1[16];
  len = Central.readBytesUntil('#', (char *) Senha1, 6);
  for (byte i = len; i < 16; i++) Senha1[i] = 0x00;
  Serial.write(Senha1, 16);

  for (int i = 0; i < 6 ; i++)
  {
    if ((Senha1[i] == DataSen[i]) && (senha4[i] == DataSen[i]))
    {
      Validsen++;
    }
    else
    {
      Serial.println("Senha incorreta");
    }
    if (Validsen == 6)
    {
      Serial.println("Aguarde!");
    }
  }
  if (r == valor)
  {
    ValidVal++;
  }
  else
  {
    Serial.println("Erro no saldo");
    Serial.println(r);
    Serial.println(valor);
  }
  if ((ValidVal == 1) && (Validsen == 6))
  {
    //--------------------------------------------------------------------------------------------------
    Serial.setTimeout(20000L) ;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 28, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("PCD_Authenticate() success: "));

    status = mfrc522.MIFARE_Write(28, clear, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("MIFARE_Write() success: "));

    //--------------------------------------------------------------------------------------------------

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 29, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("PCD_Authenticate() success: "));

    status = mfrc522.MIFARE_Write(29, clear, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("MIFARE_Write() success: "));

    //--------------------------------------------------------------------------------------------------

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 32, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("PCD_Authenticate() success: "));

    status = mfrc522.MIFARE_Write(32, clear, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("MIFARE_Write() success: "));

    //--------------------------------------------------------------------------------------------------

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 33, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("PCD_Authenticate() success: "));

    status = mfrc522.MIFARE_Write(33, clear, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println(F("MIFARE_Write() success: "));

    Serial.println("Obrigado por utilizar nossos servicos");
    Central.write("deletok#");
    Firebase.remove("Usuarios/Card=" + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + b1 + b2 + b3 + b4 + b5 + b6 + b7);
    Firebase.remove("Usuarios/Nome=" + b8 + b9 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + d1 + d2 + d3 + d4 + d5);
  }



  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  trav = 0;
  Central.flush();
  mensageminicial();
}







void leitura()   //-------------------------------------------------------------FUNÇÃO QUE LÊ AS INFORMAÇÕES DO CARTÃO---------------------------------------------------------------
{
  Validsen = 0;
  ValidVal = 0;
  zeradados();
  mensagem_inicial_cartao();
  
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  byte block;
  byte len;
  MFRC522::StatusCode status;
  Central.write("Leitura#");
  while ( ! mfrc522.PICC_IsNewCardPresent())
  {
    delay(100);
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //-------------------------------------------

  Serial.print(F("N.CARTAO: "));
  block = 28;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 28, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    void setup();
    return;
  }

  status = mfrc522.MIFARE_Read(block, NumeroLei, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    void setup(); 
    return;
  }

  //MOSTRA NA SERIAL O NumeroLei DO CARTÃO
  for (uint8_t i = 0; i < 16; i++)
  {
    if (NumeroLei[i] != 32)
    {
      Serial.write(NumeroLei[i]);
    }
  }
  Serial.println(" ");

  a1 = (NumeroLei[0]);  a2 = (NumeroLei[1]);  a3 = (NumeroLei[2]);  a4 = (NumeroLei[3]);  a5 = (NumeroLei[4]);  a6 = (NumeroLei[5]);
  a7 = (NumeroLei[6]);  a8 = (NumeroLei[7]);  a9 = (NumeroLei[8]);  b1 = (NumeroLei[9]);  b2 = (NumeroLei[10]);  b3 = (NumeroLei[11]); ////verificar
  b4 = (NumeroLei[12]); b5 = (NumeroLei[13]); b6 = (NumeroLei[14]); b7 = (NumeroLei[15]);

  //----------------------------------------------------------------------------------------------
  Serial.print(F("NOME: "));
  block = 29;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 29, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    void setup();
    return;
  }
  status = mfrc522.MIFARE_Read(block, NomeLei, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    void setup(); 
    return;
  }

  //MOSTRA NA SERIAL O NOME DO USUARIO
  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(NomeLei[i] );
  }
  Serial.println(" ");

  b8 = NomeLei[0];  b9 = NomeLei[1];  c1 = NomeLei[2];  c2 = NomeLei[3];  c3 = NomeLei[4];  c4 = NomeLei[5];  c5 = NomeLei[6];  c6 = NomeLei[7];
  c7 = NomeLei[8];  c8 = NomeLei[9];  c9 = NomeLei[10];  d1 = NomeLei[11];  d2 = NomeLei[12];  d3 = NomeLei[13];  d4 = NomeLei[14];  d5 = NomeLei[15];


  Serial.print(F("SENHA: "));
  block = 32;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 32, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    void setup();
    return;
  }

  status = mfrc522.MIFARE_Read(block, SenhaLei, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    void setup();
    return;
  }


  //MOSTRA NA SERIAL A SENHA DO USUARIO//TEMPORARIA
  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(SenhaLei[i] );
  }
  Serial.println(" ");
  //-------------------------------------------------------------
  Serial.print(F("SALDO: "));
  block = 33;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 33, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
     void setup();
    return; 
  }

  status = mfrc522.MIFARE_Read(block, CreditoLei, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    void setup();
    return;
  }

  //MOSTRA NA SERIAL O SALDO DO CARTÃO
  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(CreditoLei[i] );
  }
  Serial.println(" ");


  //------------------------------------------------------


  Central.write(CreditoLei, 16);
  Serial.println(F("\n**End Reading**\n"));

  FirebaseObject object = Firebase.get ("/");
  String senha = object.getString("Usuarios/Card=" + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + b1 + b2 + b3 + b4 + b5 + b6 + b7);
  int valor = Firebase.getInt("Usuarios/Nome=" + b8 + b9 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + d1 + d2 + d3 + d4 + d5);
  senha.getBytes(DataSen, 16);
  Serial.println(valor);
  Serial.write(DataSen, 16);

  k = 1000 * (CreditoLei[0] - 48);
  l = 100 * (CreditoLei[1] - 48);
  m = 10 * (CreditoLei[2] - 48);
  n = CreditoLei[3] - 48;
  r = k + l + m + n;

  for (int i = 0; i < 6; i++)
  {
    if (SenhaLei[i] == DataSen[i])
    {
      Validsen++;
    }
    else
    {
      Serial.println("Senha incorreta");
    }
    if (Validsen == 6)
    {
      Serial.println("Transação autorizada");
    }
  }

  if (valor == r)
  {
    ValidVal++;
  }
  else
  {
    Serial.println("Erro no saldo");
  }
  if (ValidVal == 1)
  {
    Serial.println("Transação autorizada");
  }



  Serial.println(ValidVal);
  Serial.print(("Senha do servidor:"));
  Serial.println(senha);
  Serial.print(("Saldo do servidor:"));
  Serial.println(valor);
  //}
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  trav = 0;
 Central.flush();
  mensageminicial();
}












void Financeiro()//------------------------------------------------------------------------FUNÇÃO QUE FAZ OPERAÇÃO FINANCEIRA----------------------------------------
{
  
//
  cont = 0;
  Validsen = 0;
  ValidVal = 0;
  zeradados();
  mensagem_inicial_cartao();
  FirebaseObject object = Firebase.get ("/");
  Central.write("aproxcard#");
  
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  byte block;
  byte len;
  MFRC522::StatusCode status;
  while ( ! mfrc522.PICC_IsNewCardPresent())
  {
    delay(100);
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.println(F("**Cartao Detectado:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //-------------------------------------------------
  block = 28;
  len = 18;

  //-------------------------------------------
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 28, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mensageminicial();
    return;
  }

  status = mfrc522.MIFARE_Read(block, Numero, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mensageminicial();
    return;
  }

  a1 = (Numero[0]);  a2 = (Numero[1]);  a3 = (Numero[2]);  a4 = (Numero[3]);  a5 = (Numero[4]);  a6 = (Numero[5]);
  a7 = (Numero[6]);  a8 = (Numero[7]);  a9 = (Numero[8]);  b1 = (Numero[9]);  b2 = (Numero[10]);  b3 = (Numero[11]);
  b4 = (Numero[12]); b5 = (Numero[13]); b6 = (Numero[14]); b7 = (Numero[15]);



  //-------------------------------------------------------------------------------------------------------------------

  block = 32;
  byte senhaop[16];
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 32, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mensageminicial();
    return;
  }

  status = mfrc522.MIFARE_Read(block, senhaop, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mensageminicial();
    return;
  }

  
    Serial.write(senhaop,16);
  
  Serial.println(" ");
  //------------------------------------------------------------------------------------------------------------------------------------

  block = 29;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 29, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mensageminicial();
    return;
  }
  status = mfrc522.MIFARE_Read(block, Nome, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mensageminicial();
    return;
  }

  b8 = Nome[0];  b9 = Nome[1];  c1 = Nome[2];  c2 = Nome[3];  c3 = Nome[4];  c4 = Nome[5];  c5 = Nome[6];  c6 = Nome[7];
  c7 = Nome[8];  c8 = Nome[9];  c9 = Nome[10];  d1 = Nome[11];  d2 = Nome[12];  d3 = Nome[13];  d4 = Nome[14];  d5 = Nome[15];

  //------------------------------------------------------------------------------------------------------------------------------

  block = 33;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 33, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mensageminicial();
    return;
  }

  status = mfrc522.MIFARE_Read(block, Credito, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mensageminicial();
    return;
  }


  String senha = object.getString("Usuarios/Card=" + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + b1 + b2 + b3 + b4 + b5 + b6 + b7); //Lê o enderenço no banco de dados
  senha.getBytes(DataSen, 16);                                                                       //transforma string em bytes para comparar com a senha digitada
  int valor = object.getInt("Usuarios/Nome=" + b8 + b9 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + d1 + d2 + d3 + d4 + d5);
  Central.setTimeout(20000L) ;
  Serial.setTimeout(20000L) ;
  Serial.write(DataSen, 16);
  if ((DataSen[0] == 0x00) && (DataSen[1] == 0x00) && (DataSen[2] == 0x00))
  {
    Serial.print("cartão invalido");
  }
  else
  {
    Serial.println("Para Compra digite 1");
    Serial.println("Para Recarga digite 2");
    len = Serial.readBytesUntil('#', (char *) auda, 2) ; // read family name from serial
    
    Central.setTimeout(20000L);
    
    if (auda[0] == 49)
    {
      Central.write("Compra#");
      byte senha12[16];
      Serial.println(F("Cliente digitando a senha... #"));
      Central.flush();
      len = Central.readBytesUntil('#', (char *) senha12, 7);
      for (byte i = len; i < 16; i++) senha12[i] = 0x00;
      Serial.write(senha12, 16);

      for (byte a = 0; a < 6; a++)
      {
        if ((senha12[a]==DataSen[a])&& (senha12[a]==senhaop[a]))  
        {
          cont++;
        }
        else
        {
          Serial.println(F("Senha incorreta"));
          Serial.print(cont);
          mensageminicial();
        }
      }
      if (cont == 6)
      {
        Serial.println(F("Senha correta"));
        Serial.println(F("digite o valor da compra, no final digite#"));
        len = Serial.readBytesUntil('#', (char *) CREinser, 16) ; // read family name from serial
        for (byte i = 4; i < 16; i++)
        {
          CREinser[i] = 0x00;   // pad with spaces
        }
        org();
        Serial.print("valor Debitado:");
        Serial.println(r);
        r = valor - r;
        result();
        for (byte i = 4; i < 16; i++) Credito[i] = 0x00;

        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 33, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
          Serial.print(F("PCD_Authenticate() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
        }
        else Serial.println(F("PCD_Authenticate() success: "));
        // Write block
        status = mfrc522.MIFARE_Write(33, Credito, 16);
        if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Write() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
        }
        else Serial.println(F("MIFARE_Write() success: "));
        Central.write(CREinser,16);
        Firebase.setInt("Usuarios/Nome=" + b8 + b9 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + d1 + d2 + d3 + d4 + d5, r);
      }
    }

    
    if (auda[0] == 50)
    {
      
      byte senha23[16];
      Central.write("recarga#");

      Serial.println(F("Cliente digitando a senha... #"));
      Central.flush();
      len = Central.readBytesUntil('#', (char *) senha23, 7);
      
      Serial.write(senha23, 16);
      
      for (int a = 0; a < 6; a++)
      {
        if ((senha23[a]==senhaop[a])&&(senha23[a]==DataSen[a])) 
        {
          cont++;
        }
        else
        {
          Serial.println(F("Senha incorreta"));
          Serial.print(cont);
          mensageminicial();
        }
      }
      if (cont == 6)
      {
        Serial.println(F("Senha correta"));
        Serial.println(F("digite o valor da Recarga, no final digite#"));
        len = Serial.readBytesUntil('#', (char *) CREinser, 16) ; // read family name from serial
        for (byte i = 4; i < 16; i++)
        {
          CREinser[i] = 0x00;   // pad with spaces
        }
        org();
        Serial.print("valor inserido:");
        Serial.println(r);
        r = r + valor;
        result();
        for (byte i = 4; i < 16; i++) Credito[i] = 0x00;

        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 33, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
          Serial.print(F("PCD_Authenticate() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          mensageminicial();
          return;
        }
        else Serial.println(F("PCD_Authenticate() success: "));
        // Write block
        status = mfrc522.MIFARE_Write(33, Credito, 16);
        if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Write() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          mensageminicial();
          return;
        }
        else Serial.println(F("MIFARE_Write() success: "));

        Central.write(CREinser,16);
        Firebase.setInt("Usuarios/Nome=" + b8 + b9 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + d1 + d2 + d3 + d4 + d5, r);
      }
    }


  }
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  trav = 0;
  cont = 0;
  mensageminicial();
}














































































void result() //FUNÇÃO QUE ORGANIZA OS VALORES SOMADOS OU SUBITRAIDOS NO BYTE PARA QUE POSSAM SER GRAVADOS NO CARTÃO 
               //POIS NÃO É POSSIVEL GRAVAR VALORES INTEIROS NO CARTÃO POIS PARA SER LIDOS FICAM MAIS DIFICEIS
{
  if (r > 9999)
  {
    Serial.print("Limite excedido");
    Serial.print(r - 9999);
    return;
  }
  if (r < 0)
  {
    Serial.print("Sem Saldo");
    Serial.print(r);
    return;
  }
  if ((r > 999) && (r < 9999))
  {
    Credito[0] = (r / 1000);
    Credito[1] = ((r - (Credito[0] * 1000)) / 100);
    Credito[2] = ((r - (Credito[0] * 1000) - (Credito[1] * 100)) / 10);
    Credito[3] = (r - (Credito[0] * 1000) - (Credito[1] * 100) - (Credito[2] * 10));

    Credito[0] = Credito[0] + 48;
    Credito[1] = Credito[1] + 48;
    Credito[2] = Credito[2] + 48;
    Credito[3] = Credito[3] + 48;
    return;
  }
  else
  {
    if ((r > 99) && (r < 1000))
    {
      Credito[0] = 48;
      Credito[1] = (r / 100);
      Credito[2] = ((r - (Credito[1] * 100)) / 10);
      Credito[3] = (r - (Credito[1] * 100) - (Credito[2] * 10));

      Credito[1] = Credito[1] + 48;
      Credito[2] = Credito[2] + 48;
      Credito[3] = Credito[3] + 48;
      return;
    }
    else
    {
      if ((r > 9) && (r < 100))
      {
        Credito[0] = 48;
        Credito[1] = 48;
        Credito[2] = (r / 10);
        Credito[3] = (r - (Credito[2] * 10));

        Credito[2] = Credito[2] + 48;
        Credito[3] = Credito[3] + 48;
        return;
      }
      else
      {
        if ((r >= 0) && (r <= 9))
        {
          Credito[0] = 48;
          Credito[1] = 48;
          Credito[2] = 48;
          Credito[3] = r + 48;
          
          return;
        }
      }
    }
  }
}

int org () {

  if ((CREinser[0] < 48) && (CREinser[1] < 48) && (CREinser[2] < 48) && (CREinser[3] < 48))
  {
    k = 0;
    l = 0;
    m = 0;
    n = 0;
  }
  if ((CREinser[0] >= 48) && (CREinser[0] < 58) && (CREinser[1] < 48) && (CREinser[2] < 48) && (CREinser[3] < 48))
  {
    k = 0;
    l = 0;
    m = 0;
    n = CREinser[0] - 48;
  }
  if ((CREinser[0] >= 48) && (CREinser[0] < 58) && (CREinser[1] >= 48) && (CREinser[1] < 58) && (CREinser[2] < 48) && (CREinser[3] < 48))
  {
    k = 0;
    l = 0;
    m = (CREinser[0] - 48) * 10;
    n = CREinser[1] - 48;
  }
  if ((CREinser[0] >= 48) && (CREinser[0] < 58) && (CREinser[1] >= 48) && (CREinser[1] < 58) && (CREinser[2] >= 48) && (CREinser[2] < 58) && (CREinser[3] < 48))
  {
    k = 0;
    l = (CREinser[0] - 48) * 100;
    m = (CREinser[1] - 48) * 10;
    n = CREinser[2] - 48;
  }
  if ((CREinser[0] >= 48) && (CREinser[0] < 58) && (CREinser[1] >= 48) && (CREinser[1] < 58) && (CREinser[2] >= 48) && (CREinser[2] < 58) && (CREinser[3] >= 48) && (CREinser[3] < 58))
  {
    k = (CREinser[0] - 48) * 1000;
    l = (CREinser[1] - 48) * 100;
    m = (CREinser[2] - 48) * 10;
    n = CREinser[3] - 48;
  }
  r = k + l + m + n;
  return r;
}


void zeradados()
{
  a1 = "0";  a2 = "0";  a3 = "0";  a4 = "0";  a5 = "0";  a6 = "0";  a7 = "0";  a8 = "0";
  a9 = "0";  b1 = "0";  b2 = "0";  b3 = "0";  b4 = "0";  b5 = "0";  b6 = "0";  b7 = "0";
  b8 = "0";  b9 = "0";  c1 = "0";  c2 = "0";  c3 = "0";  c4 = "0";  c5 = "0";  c6 = "0";
  c7 = "0";  c8 = "0";  c9 = "0";  d1 = "0";  d2 = "0";  d3 = "0";  d4 = "0";  d5 = "0";
  for(byte i=0;i<16;i++)
  {
  NumeroGrav[i]=0x00;
 SenhaGrav[i]=0x00;
 NomeGrav[i]=0x00;
 CreditoGrav[i]=0x00;

 NumeroLei[i]=0x00;
 SenhaLei[i]=0x00;
 NomeLei[i]=0x00;
 CreditoLei[i]=0x00;

 NumeroApa[i]=0x00;
 //SenhaApa[i]=0x00;
 NomeApa[i]=0x00;
 CreditoApa[i]=0x00;
 Numero[i]=0x00;
 Senha[i]=0x00;
 Nome[i]=0x00;
 Credito[i]=0x00; 
  } 
    return;
}

void cardapag()
{
  clear[0] = 0x00;  clear[1] = 0x00;   clear[2] = 0x00;   clear[3] = 0x00;   clear[4] = 0x00;   clear[5] = 0x00;   clear[6] = 0x00;   clear[7] = 0x00;  clear[8] = 0x00;
  clear[9] = 0x00;  clear[10] = 0x00;  clear[11] = 0x00;  clear[12] = 0x00;  clear[13] = 0x00;  clear[14] = 0x00;  clear[15] = 0x00;  clear[16] = 0x00;
  return;
}
