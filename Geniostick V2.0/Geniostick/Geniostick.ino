
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
int rotaryPinsW[] = {4, 5, 6};
int rotaryPinsR[] = {7, 8, 9, 10};
int btnPinsW[] = {14, 15};
int btnPinsR[] = {18, 19, 20, 21};

Joystick_ joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
                   29, 1,                 // Button Count, Hat Switch Count
                   false, false, false,   // Np X, Y, Z Axis
                   false, false, false,   // No Rx, Ry, Rz
                   false, false,          // No rudder or throttle
                   false, false, false);  // No accelerator, brake, or steering,


int buttonStates[N_BOTOES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int buttonLastStates[N_BOTOES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {

  for (int i = 0 ; i < sizeof(rotaryPinsW); i++ )
  {
    pinMode(rotaryPinsW[i], OUTPUT);
  }
  for (int i = 0 ; i < sizeof(rotaryPinsR); i++ )
  {
    pinMode(rotaryPinsR[i], INPUT_PULLUP);
  }
  
  for (int i = 0 ; i < sizeof(btnPinsW); i++ )
  {
    pinMode(btnPinsW[i], OUTPUT);
  }
  for (int i = 0 ; i < sizeof(btnPinsR); i++ )
  {
    pinMode(btnPinsR[i], INPUT_PULLUP);
  }
  attachInterrupt(digitalPinToInterrupt(2), encoderLeft, FALLING );
  attachInterrupt(digitalPinToInterrupt(3), encoderRight, FALLING );
  joystick.begin();
}
void encoderLeft()
{
  //Código que aciona o joystick de acordo com o estado
  //setar variavel que será lida pelo loop
  //o loop volta a variavel para o estado zero
  buttonStates[BTN_ENCODER_LEFT] = 1;
}
void encoderRight()
{
  //Código que aciona o joystick de acordo com o estado
  //setar variavel que será lida pelo loop
  //o loop volta a variavel para o estado zero
  buttonStates[BTN_ENCODER_RIGHT] = 0;
}
void readRotary()
{
  //4 5 6 7
  // X
  // 8 9 10
  for ( int i = 0; i < sizeof(rotaryPinsW) ; i++ )
  {
    digitalWrite(rotaryPinsW[i], 0);
    //Escrever 0 na saída Write[i]
    for ( int j = 0; j < sizeof(rotaryPinsR) ; j++ )
    {
      //ler a entrada de read
      if ( digitalRead(rotaryPinsR[j]) < 1 )
      {
        rotaryValue = (i * sizeof(rotaryPinsR)) + j;
      }
    }
    digitalWrite(rotaryPinsW[i], 1);
  }
}
void readButtons()
{
  int pos = 0;
  int sizeR =  sizeof(btnPinsR);
  int sizeW = sizeof(btnPinsW);

  for ( int i = 0; i <  sizeW; i++ )
  {
    digitalWrite(btnPinsW[i], 0);
    //Escrever 0 na saída Write[i]
    for ( int j = 0; j < sizeR ; j++ )
    {
      pos = (i * sizeR) + j;
      //transforma o valor de 1 para 0 e 0 para 1
      buttonStates[pos] = 1 - digitalRead(btnPinsR[j]);
    }
    digitalWrite(btnPinsW[i], 1);
  }
  //seguindo o esquema elétrico do DPAD
  //se UP e DOWN foram pressionados então siginifica que o botão central foi acionado
  if ( buttonStates[DPAD_UP] > 0 && buttonStates[DPAD_DOWN] > 0 )
  {
    buttonStates[BTN_CENTER] = 1;
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
      //atribui o estado atual ao botão
      joystick.setButton(i, buttonStates[i]);
      //salva o estado atual como estado anterior
      buttonLastStates[i] != buttonStates[i];
    }
  }
  if ( buttonStates[BTN_CENTER] != buttonLastStates[BTN_CENTER])
  {
    //atribui o estado atual ao botão
    joystick.setButton(BTN_CENTER, buttonStates[BTN_CENTER]);
    //salva o estado atual como estado anterior
    buttonLastStates[BTN_CENTER] != buttonStates[BTN_CENTER];
  }
  //só le o DPAD se o botão central não estiver pressionado,
  // caso contrário todos seriam 1
  if (  buttonStates[BTN_CENTER] < 1 )
  {
    int hatValue = -1;
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
    joystick.setHatSwitch(0, hatValue);
  }
	setEncoderValue(BTN_ENCODER_LEFT);
	setEncoderValue(BTN_ENCODER_RIGHT);
}
void setEncoderValue(int indice)
{
	if( buttonStates[indice] != buttonLastStates[indice] )
  {
  	//a posição do botão é o numero dele mais o offset selecionado no rotary
  	joystick.setButton(indice+(2*rotaryValue), buttonStates[indice]);
  	buttonLastStates[indice] = buttonStates[indice];
  	//sempre zera o estado pois a interrupção só o coloca em 1. se já for zero, nada acontece
  	buttonStates[indice] = 0;
	}
}

void loop() {
  readRotary ();
  readButtons();
  pressButtons();
  delay(10);
}
