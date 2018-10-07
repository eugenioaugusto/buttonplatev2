
/*geniostick version 0.2
*

*/

#include <Joystick.h>

#define N_BOTOES 11

#define BTN_1 0
#define BTN_2 1
#define BTN_3 2
#define BTN_4 3
#define DPAD_UP 4
#define DPAD_RIGHT 5
#define DPAD_DOWN 6
#define DPAD_LEFT 7
#define BTN_CENTER 8
#define BTN_ENCODER_LEFT 9
#define BTN_ENCODER_RIGHT 10

int rotaryValue = 0;
//Pinos de escrita para o rotary

Joystick_ joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
                   29, 1,                 // Button Count, Hat Switch Count
                   false, false, false,   // Np X, Y, Z Axis
                   false, false, false,   // No Rx, Ry, Rz
                   false, false,          // No rudder or throttle
                   false, false, false);  // No accelerator, brake, or steering,


int buttonStates[N_BOTOES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int buttonLastStates[N_BOTOES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  //Serial.begin(9600);
  //entradas do encoder
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  //portas do Rotary
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  //portas dos botoes
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(2), encoderLeft, FALLING );
  attachInterrupt(digitalPinToInterrupt(3), encoderRight, FALLING );
  joystick.begin();
}
void encoderLeft()
{
  //CÃ³digo que aciona o joystick de acordo com o estado
  setEncoderValue(BTN_ENCODER_LEFT);
}
void encoderRight()
{
  //CÃ³digo que aciona o joystick de acordo com o estado
  setEncoderValue(BTN_ENCODER_RIGHT);
}
void setEncoderValue(int indice)
{
 //a posiÃ§Ã£o do botÃ£o Ã© o numero dele mais o offset selecionado no rotary
 if( buttonStates[indice] == 0 )
  {
  buttonStates[indice] = 100;
  }
}
void readRotary()
{
  //4 5 6 7
  // X
  // 8 9 10
  int rotaryPinsW[] = {4, 5, 6};
  int rotaryPinsR[] = {7, 8, 9, 10};
  int newRotary = 0;
  for ( int i = 0; i < 3 ; i++ )
  {
    digitalWrite(rotaryPinsW[i], 0);
    //Escrever 0 na saÃ­da Write[i]
    for ( int j = 0; j < 4 ; j++ )
    {
      //ler a entrada de read
      if ( digitalRead(rotaryPinsR[j]) < 1 )
      {
        newRotary = ( i * 4) + j;
        if(newRotary != rotaryValue)
        {
          //Serial.print(i);
        //Serial.print(" * ");
        //Serial.print(4);
        //Serial.print(" + ");
        //Serial.print(j);
        //Serial.print(" = ");
        //Serial.println(newRotary);  
        rotaryValue = newRotary;
        }
        
        
      }
    }
    digitalWrite(rotaryPinsW[i], 1);
  }
}
void readButtons()
{
  int pos = 0;
  int btnPinsW[] = {14, 15};
  int btnPinsR[] = {18, 19, 20, 21};

  for ( int i = 0; i <  2; i++ )
  {
    digitalWrite(btnPinsW[i], 0);
    //Escrever 0 na saÃ­da Write[i]
    for ( int j = 0; j < 4 ; j++ )
    {
      pos = (i * 4) + j;
      //transforma o valor de 1 para 0 e 0 para 1
      buttonStates[pos] = 1 - digitalRead(btnPinsR[j]);
    }
    digitalWrite(btnPinsW[i], 1);
  }
  //seguindo o esquema elétrico do DPAD
  //se UP e DOWN foram pressionados entÃ£o siginifica que o botÃ£o central foi acionado
  if ( buttonStates[DPAD_UP] > 0 && buttonStates[DPAD_DOWN] > 0 )
  {
    buttonStates[BTN_CENTER] = 1;
    buttonStates[DPAD_UP] = 0;
    buttonStates[DPAD_DOWN] = 0;
    buttonStates[DPAD_RIGHT] = 0;
    buttonStates[DPAD_LEFT] = 0;
  }
  else
  {
    buttonStates[BTN_CENTER] = 0;
  }
}
void pressButtons()
{
  //le os botoes normais 0 - 3
  for ( int i = 0; i <= BTN_4; i++)
  {
    if ( buttonStates[i] != buttonLastStates[i])
    {
      //atribui o estado atual ao botÃ£o
      joystick.setButton(i, buttonStates[i]);
      //salva o estado atual como estado anterior
      buttonLastStates[i] = buttonStates[i];
    }
  }
  if ( buttonStates[BTN_CENTER] != buttonLastStates[BTN_CENTER])
  {
    //atribui o estado atual ao botÃ£o
    joystick.setButton(BTN_CENTER - 4, buttonStates[BTN_CENTER]);
    //salva o estado atual como estado anterior
    buttonLastStates[BTN_CENTER] = buttonStates[BTN_CENTER];
  }
  //sÃ³ le o DPAD se o botÃ£o central nÃ£o estiver pressionado,
  // caso contrÃ¡rio todos seriam 1
    int hatValue = -1;
  if (  buttonStates[BTN_CENTER] < 1 )
  {
    if ( buttonStates[DPAD_UP] != buttonLastStates[DPAD_UP] )
    {
      hatValue = 0;
    }
    else if ( buttonStates[DPAD_RIGHT] != buttonLastStates[DPAD_RIGHT] )
    {
      hatValue = 90;
    }
    else if ( buttonStates[DPAD_DOWN] != buttonLastStates[DPAD_DOWN] )
    {
      hatValue = 180;
    }
    else if ( buttonStates[DPAD_LEFT] != buttonLastStates[DPAD_LEFT] )
    {
      hatValue = 270;
    }
  }
    joystick.setHatSwitch(0, hatValue);
  
}

void testaValorEncoder(int indice)
{
  int indiceBtn = indice - 4;
  if( buttonStates[indice] > 0 )
  {
    if(buttonLastStates[indice] == 0)
    {
       //Serial.print("recebeu entrada de encoder para o indice: ");
        //Serial.print(indiceBtn);
        //Serial.print("  Rotari valor: ");
        //Serial.print(rotaryValue);
        //Serial.print(" Acionando botÃ£o: ");
        //Serial.println(indiceBtn+(2*rotaryValue));
        joystick.setButton(indiceBtn+(2*rotaryValue), 1);
        buttonLastStates[indice] = buttonStates[indice];
        buttonStates[indice]--;
    }
    else
    {
      buttonStates[indice]--;
      if( buttonStates[indice] == 0 )
      {
        buttonLastStates[indice] = buttonStates[indice];
        joystick.setButton(indiceBtn+(2*rotaryValue), 0);
        //Serial.print("Liberou ");
        //Serial.println(indiceBtn);
      }
      //else
      //{
        //Serial.print("Incide [");
        //Serial.print(indice);
        //Serial.print("Diminuiu para [");
        //Serial.print(buttonStates[indice]);
        //Serial.println("]");
      //}
    }
  }
}
void loop() {
  readRotary ();
  readButtons();
  pressButtons();
  testaValorEncoder(BTN_ENCODER_LEFT);
  testaValorEncoder(BTN_ENCODER_RIGHT);
  delay(10);
}
