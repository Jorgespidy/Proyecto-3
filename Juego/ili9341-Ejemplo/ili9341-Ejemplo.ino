//*********************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * IE3027: Electrónica Digital 2 - 2019
 */
//*********************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  
const int P1_UP = PA_3;
const int P1_DOWN = PA_2;
const int P2_UP = PF_4;
const int P2_DOWN = PF_0;
const uint8_t P1_H = 20;
int maxscore = 5;
int p2score = 0;
int p1score = 0;
const uint8_t p1x=235;
uint8_t p1y = 110;
const uint8_t p2x=15;
uint8_t p2y = 110;
uint16_t ballx = 50, bally = 80;
uint16_t b_dirx = 1, b_diry = 1;
boolean reset_game = true;
boolean game = false;
int sonido = 0;
//*********************************************
// Functions Prototypes
//*********************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
void showScore(void);
void gameOver (void);
void inicio (void);
//*********************************************
// Inicialización
//*********************************************
void setup() {
  pinMode(P1_UP, INPUT);
  pinMode(P1_DOWN, INPUT);
  pinMode(P2_UP, INPUT_PULLUP);
  pinMode(P2_DOWN, INPUT_PULLUP);
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  Serial2.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  //Serial.println("Inicio");
  inicio();

}
//*********************************************
// Loop Infinito
//*********************************************
void loop() {
static bool up = false;
static bool down = false;
static bool up2 = false;
static bool down2 = false;
up |= (digitalRead (P1_UP)==HIGH);
down |= (digitalRead (P1_DOWN)==HIGH);
up2 |= (digitalRead (P2_UP) == HIGH);
down2 |= (digitalRead (P2_DOWN) == HIGH);
if (reset_game)
{
  ballx = random(100,150);
  bally = random(20,30);
  do
  {
   b_dirx = random(-1, 1);
  }while(b_dirx==0);
  do
  {
  b_diry = random(-1,1);
  }while(b_diry==0);
  
  reset_game =false;
}

 
  if (game == false){
  // ----------------- parte del codigo que se encarga del movimiento de la bola ------------------
  uint8_t updateX = ballx + b_dirx; // dicta como se mueve la bola en el eje X
  uint8_t updateY = bally + b_diry; // dicta como se mueve la bola en el eje Y
  if(updateX == p2x) // detecta si hay una colision con el eje horizontal del lado del jugador 2 (lado izquierdo)
  {
    p1score++;
    reset_game = true;
    if(p1score == maxscore)
    {
      game = true;
      gameOver();
    }else
    {
      //  AQUI PODRIA SER EL SONIDO DE PERDER
      showScore();
    }
  }
  if(updateX == p1x + 10) // detecta una colision del lado derecho en la horizontal
  {
    p2score++;
    reset_game = true;
    if(p2score == maxscore)
    {
      game = true;
      gameOver();
    }else
    {
      // IGUAL A LO DE ARRIBA
      showScore();
    }
  }
  if (updateY ==10 || updateY==215) {       //limita los topes en el eje Y
    sonido = 3;
    Serial2.write (sonido);
    b_diry = -b_diry;
    updateY +=  b_diry; // este codigo evita que se vea que la pelota se aplasta contra la pared (hace un rebote visualmente correcto)
    
  }
  if(updateX == p2x+15 // hay que correr 15 pixeles la posicion del jugador 2 en X porque la imagen de la raqueta es muy ancha
  && updateY >= p2y
  && updateY <= p2y+55)
  {
    sonido = 4; //sonido de topar con la raqueta del enemigo
    Serial2.write (sonido);
    b_dirx = -b_dirx;
    updateX += 2 * b_dirx; // tambien hace un rebote visualmente correcto en el eje X (contra las raquetas)
  }
  if(updateX == p1x
  && updateY >= p1y
  && updateY <= p1y +55)
  {
    sonido = 4; //sonido de topar con la  raqueta del jugador
    Serial2.write (sonido);
    b_dirx = -b_dirx;
    updateX +=2 * b_dirx;
  }
  
  LCD_Bitmap(updateX, updateY, 8, 8, ball); // imprime el recorrido de la pelota
  H_line(updateX, updateY+8, 8, 0x00); // lineas que borran el rastro que deja la pelota
  H_line(updateX, updateY, 8, 0x00);
  V_line(updateX, updateY, 8, 0x00);
  V_line(updateX+8, updateY, 8, 0x00);
  
  ballx=updateX;
  bally=updateY;


  }


    if (game == false){
    // ---------------- movimiento de las raquetas -----------------
    if(up2){
      H_line(p2x, p2y+50, P1_H, 0x00); // subir player 2
      H_line(p2x, p2y+49, P1_H, 0x00);
      p2y-=2;
       
    }
    if(down2){
      H_line (p2x, p2y-1, P1_H, 0x00);// borra estela al bajar player 2
      p2y+=2;
    }
    up2 = down2 = false;
    if(p2y <15) p2y = 15;
    if(p2y + P1_H > 195) p2y = 195 - P1_H; // topes con las paredes verticales para raqueta 1
    
    
    if (up) {
      H_line(p1x, p1y+50, P1_H, 0x00); // borra rastro al subir player 1
      H_line(p1x, p1y+49, P1_H, 0x00);
      p1y -= 2;
    }
    if(down){
      H_line(p1x, p1y, P1_H, 0x00); // borra rastro al bajar player 1
      H_line(p1x, p1y-1, P1_H, 0x00); // se hicieron 2 lineas porque al jugador se el aumentan 2 lineas cada vez
      p1y += 2;     //aumento de 2 lineas cada vez que se presiona el boton
    }
    up = down = false;
    if(p1y <15) p1y = 15;
    if(p1y + P1_H >195) p1y = 195 - P1_H;  // topes en la vertical para raqueta 2
    
    
   

LCD_Bitmap(p2x, p2y, P1_H, 50, raqueta2); // asi se mueven las raquetas
LCD_Bitmap(p1x, p1y, P1_H, 50, raqueta1);



    }

}


void showScore(void){
  
  String jugador1 = "P1 :";
  LCD_Print(jugador1, 270, 50, 2, 0xFFFF, 0x00);
  LCD_Print(String(p1score), 270, 80, 1, 0xFFFF, 0x00);

  String jugador2 = "P2 :";
  LCD_Print(jugador2, 270, 120, 2, 0xFFFF, 0x00);
  LCD_Print(String(p2score), 270, 150, 1, 0xFFFF, 0x00);
}

void gameOver(void){
  reset_game = false;
  sonido = 1;
  Serial2.write (sonido);
  LCD_Clear(0x0);
  delay(200);
  if (p1score > p2score){
    String ganador1 = "GANO P1!";
    LCD_Print(String(ganador1), 90, 100, 2, 0xFFFF, 0x00);
    delay(200);
    LCD_Print(String(ganador1), 100, 110, 2, 0xFFFF, 0x00);
    delay(200);
    LCD_Print(String(ganador1), 110, 120, 2, 0xFFFF, 0x00);
    delay(2000);
    p1score = 0;
    p2score = 0;
    inicio();
  }
  else{
    String ganador2 = "GANO P2!";
    LCD_Print(String(ganador2), 90, 100, 2, 0xFFFF, 0x00);
    delay(200);
    LCD_Print(String(ganador2), 100, 110, 2, 0xFFFF, 0x00);
    delay(200);
    LCD_Print(String(ganador2), 110, 120, 2, 0xFFFF, 0x00);
    delay(2000);
    p1score = 0;
    p2score = 0;
    inicio();
  }
  
}

  void inicio (void) {
    sonido = 2;
    Serial2.write (sonido);
  LCD_Init();
  LCD_Clear(0x00);
  
  FillRect(0, 0, 320, 240, 0x0);
  String text1 = "A";
  LCD_Print(text1, 120, 100, 2, 0xF800, 0x0);
  delay(200);
  String text2 = "T";
  LCD_Print(text2, 140, 90, 2, 0xF800, 0x0);
  delay(200);
  String text3 = "A";
  LCD_Print(text3, 160, 110, 2, 0xF800, 0x0);
  delay(200);
  String text4 = "R";
  LCD_Print(text4, 180, 90, 2, 0xF800, 0x0);
  delay(200);
  String text5 = "I";
  LCD_Print(text5, 200, 110, 2, 0xF800, 0x0);
  delay(600);
  String text6 = "PONG!";
  LCD_Print(text6, 130, 100, 2, 0xFFE0, 0x0);
  String text7 = "Press any button..";
  sonido = 2;
  Serial2.write(sonido);
  LCD_Print(text7, 20, 180, 2, 0xFFFF, 0x0);
  while(digitalRead(P1_UP)==HIGH
  && digitalRead(P1_DOWN)==HIGH
  && digitalRead(P2_UP)==HIGH
  && digitalRead(P2_DOWN)==HIGH)
  
  {
    delay(250);
  }
  
  LCD_Clear(0x001F);
  delay(200);
  LCD_Clear(0xFFE0);
  delay(200);
  LCD_Clear(0xF800);
  delay(200);
  LCD_Clear(0x0);
  Rect(10,10,250,220,0xffff);
  p1score = 0;
  p2score = 0;
  ballx = random(100, 150);
  bally = random(100,120);
  
  game = false;
  
  }


//*********************************************
// Función para inicializar LCD
//*********************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //**************
  // Secuencia de Inicialización
  //**************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //**************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //**************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //**************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //**************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //**************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //**************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //**************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //**************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //**************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //**************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //**************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //**************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //**************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//*********************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//*********************************************
// Función para enviar datos a la LCD - parámetro (dato)
//*********************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//*********************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//*********************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//*********************************************
// Función para borrar la pantalla - parámetros (color)
//*********************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//*********************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//********************************************* 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//********************************************* 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//*********************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//*********************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//*********************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//*********************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}
//*********************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//*********************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//*********************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//*********************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//*********************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
  for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width -1 - offset)*2;
      k = k+width*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k - 2;
     } 
  }
  }else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
  }
    
    
    }
  digitalWrite(LCD_CS, HIGH);
}
