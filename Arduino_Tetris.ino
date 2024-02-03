#include <Wire.h>

//Pin di controllo e risposta
#define BUZZER 3
#define PUSH   4 //Pin che verrà utilizzato nella versione Pro
#define LED   13


//Definiamo i parametri del display
#define OLED_ADDRESS				0x3C //Indirizzo del display oled
#define OLED_COMMAND				0x80
#define OLED_DATA					0x40
#define OLED_DISPLAY_OFF			0xAE
#define OLED_DISPLAY_ON				0xAF
#define OLED_NORMAL_DISPLAY			0xA6
#define OLED_INVERSE_DISPLAY		0xA7
#define OLED_SET_BRIGHTNESS			0x81
#define OLED_SET_ADDRESSING			0x20
#define OLED_HORIZONTAL_ADDRESSING	0x00 
#define OLED_VERTICAL_ADDRESSING	0x01
#define OLED_PAGE_ADDRESSING		0x02 
#define OLED_SET_COLUMN				0x21
#define OLED_SET_PAGE				0x22

//------------------------------------------------------------------------------------
//Blocchi grafici del gioco
const bool  BlockI[4][4] = { { 0, 1, 0, 0 },{ 0, 1, 0, 0 },{ 0, 1, 0, 0 },{ 0, 1, 0, 0 }, };
const bool  BlockJ[4][4] = { { 0, 1, 0, 0 },{ 0, 1, 0, 0 },{ 1, 1, 0, 0 },{ 0, 0, 0, 0 }, };
const bool  BlockL[4][4] = { { 0, 1, 0, 0 },{ 0, 1, 0, 0 },{ 0, 1, 1, 0 },{ 0, 0, 0, 0 }, };
const bool  BlockO[4][4] = { { 0, 0, 0, 0 },{ 0, 1, 1, 0 },{ 0, 1, 1, 0 },{ 0, 0, 0, 0 }, };
const bool  BlockS[4][4] = { { 0, 0, 0, 0 },{ 0, 1, 1, 0 },{ 1, 1, 0, 0 },{ 0, 0, 0, 0 }, };
const bool  BlockT[4][4] = { { 0, 0, 0, 0 },{ 1, 1, 1, 0 },{ 0, 1, 0, 0 },{ 0, 0, 0, 0 }, };
const bool  BlockZ[4][4] = { { 0, 0, 0, 0 },{ 1, 1, 0, 0 },{ 0, 1, 1, 0 },{ 0, 0, 0, 0 }, };
//------------------------------------------------------------------------------------
// Blocco per il punteggio
const byte NumberFont[10][8] PROGMEM = {
	{ 0x00, 0x1c, 0x22, 0x26, 0x2a, 0x32, 0x22, 0x1c },
	{ 0x00, 0x1c, 0x08, 0x08, 0x08, 0x08, 0x0c, 0x08 },
	{ 0x00, 0x3e, 0x02, 0x04, 0x18, 0x20, 0x22, 0x1c },
	{ 0x00, 0x1c, 0x22, 0x20, 0x18, 0x20, 0x22, 0x1c },
	{ 0x00, 0x10, 0x10, 0x3e, 0x12, 0x14, 0x18, 0x10 },
	{ 0x00, 0x1c, 0x22, 0x20, 0x20, 0x1e, 0x02, 0x3e },
	{ 0x00, 0x1c, 0x22, 0x22, 0x1e, 0x02, 0x04, 0x18 },
	{ 0x00, 0x04, 0x04, 0x04, 0x08, 0x10, 0x20, 0x3e },
	{ 0x00, 0x1c, 0x22, 0x22, 0x1c, 0x22, 0x22, 0x1c },
	{ 0x00, 0x0c, 0x10, 0x20, 0x3c, 0x22, 0x22, 0x1c }
};

const byte GameOver[8][8] PROGMEM = {
	{ B00000000, B00111110, B01000001, B01000001, B01000000, B01000011, B01000001, B00111110 },
	{ B00000000, B00111110, B01000001, B01000001, B01111111, B01000001, B01000001, B01000001 },
	{ B00000000, B01000001, B01100011, B01010101, B01001001, B01000001, B01000001, B01000001 },
	{ B00000000, B01111111, B01000000, B01000000, B01111100, B01000000, B01000000, B01111111 },
	{
		B00000000,
		B00111110,
		B01000001,
		B01000001,
		B01000001,
		B01000001,
		B01000001,
		B00111110
	},
	{
		B00000000,
		B01000001,
		B01000001,
		B00100010,
		B00100010,
		B00010100,
		B00010100,
		B00001000
	},
	{ B00000000, B01111111, B01000000, B01000000, B01111100, B01000000, B01000000, B01111111 },
	{
		B00000000,
		B0111110,
		B01000001,
		B01000001,
		B01111110,
		B01000100,
		B01000010,
		B01000001
	}
};

//Costanti variabili e strutture di controllo
//------------------------------------------------------------------------------------
#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_DOWN 3
#define KEY_ROTATE 4  

byte uiKeyLeft = 2;
byte uiKeyRight = 3;
byte uiKeyDown = 4;
byte uiKeyRotate = 5;

struct keyPress
{
	long left;
	long right;
	long down;
	long rotate;
};
struct PieceSpace
{
	byte umBlock[4][4];
	char Row;
	char Coloum;
};

byte pageArray[8] = { 0 };
byte scoreDisplayBuffer[8][6] = { { 0 },{ 0 } };
byte nextBlockBuffer[8][2] = { { 0 },{ 0 } };
bool optomizePageArray[8] = { 0 };
byte blockColoum[10] = { 0 };
byte tetrisScreen[14][25] = { { 1 } ,{ 1 } };
PieceSpace currentPiece = { 0 };
PieceSpace oldPiece = { 0 };
byte nextPiece = 0;
keyPress key = { 0 };
bool gameOver = false;
unsigned long moveTime = 0;
int pageStart = 0;
int pageEnd = 0;

int score = 0;
int acceleration = 0;
int level = 0;
int levellineCount = 0;
int dropDelay = 1000;

//------------------------------------------------------------------------------------
void OLEDCommand(byte command){
	Wire.beginTransmission(OLED_ADDRESS);
	Wire.write(OLED_COMMAND);
	Wire.write(command);
	Wire.endTransmission();
}
//------------------------------------------------------------------------------------
void OLEDData(byte data){
	Wire.beginTransmission(OLED_ADDRESS);
	Wire.write(OLED_DATA);
	Wire.write(data);
	Wire.endTransmission();
}

void setCursor(byte X, byte Y) {
  OLEDCommand(0x00 + (X & 0x0F));        //set column lower address
  OLEDCommand(0x10 + ((X >> 4) & 0x0F)); //set column higher address
  OLEDCommand(0xB0 + Y);                 //set page address

}

//------------------------------------------------------------------------------------
//Funzione di Setup
void setup(){
	Serial.begin(9600);
	while(!Serial) { ; }

	Wire.begin();
	Wire.setClock(400000);

	OLEDCommand(OLED_DISPLAY_OFF);
	delay(20);
	OLEDCommand(OLED_DISPLAY_ON);
	delay(20);
	OLEDCommand(OLED_NORMAL_DISPLAY);
	delay(20);
	OLEDCommand(0x8D);
	delay(20);
	OLEDCommand(0x14);
	delay(20);
	OLEDCommand(OLED_NORMAL_DISPLAY);

	fillTetrisScreen(0);

	randomSeed(analogRead(7));
	pinMode(PUSH, INPUT);
	pinMode(BUZZER, OUTPUT);
	pinMode(13, OUTPUT);
	digitalWrite(LED, HIGH);
	delay(100);
	digitalWrite(LED, LOW);
	delay(200);
	digitalWrite(LED, HIGH);
	delay(50);
	digitalWrite(LED, LOW);
}

void fillTetrisArray(byte value){
	for(char r = 0; r < 24; r++){
		for(char c = 0; c < 14; c++){
			tetrisScreen[c][r] = value;
		}
	}
	for(char r = 21; r < 24; r++) for(char c = 0; c < 14; c++) tetrisScreen[c][r] = 0; 
}

void fillTetrisScreen(byte value){
	for(int r = 1; r < 21; r++){
		for(int c = 2; c < 12; c++){
			tetrisScreen[c][r] = value;
		}
	}
}
void drawTetrisScreen(){
	for(byte r = 1; r < 21; r++){
		for(byte c = 2; c < 12; c++){
			if((tetrisScreen[c][r] == 2) | (tetrisScreen[c][r] == 3)){
				//Invia la linea
				for(byte i = 0; i < 10; i++){
					blockColoum[i] = tetrisScreen[i + 2][r];
					//Cacella il blocco
					if(tetrisScreen[i + 2][r] == 3) tetrisScreen[i + 2][r] = 0;
				}
				drawTetrisLine((r - 1) * 6);
				break; break;
			}
		}
	}
}
//------------------------------------------------------------------------------------
void drawTetrisLine(byte x){
	memset(optomizePageArray, 0, 8);
	memset(pageArray, 0, 8);
	x++;
	//Disegna il blocco [colonna 0]
	if(blockColoum[0] == 2 | blockColoum[0] == 1){
		pageArray[0] = pageArray[0] | B11111001;
		optomizePageArray[0] = 1;
	}
	//Cancella il blocco [colonna 0]
	if(blockColoum[0] == 3){
		pageArray[0] = pageArray[0] | B00000001;
		pageArray[0] = pageArray[0] & B00000111;
		optomizePageArray[0] = 1;
	}
  //Disegna il blocco [colonna 1]
	if(blockColoum[1] == 2 | blockColoum[1] == 1){
		pageArray[1] = pageArray[1] | B00111110;
		optomizePageArray[1] = 1;
	}
  //Cancella il blocco [colonna 1]
	if(blockColoum[1] == 3){
		pageArray[1] = pageArray[1] & B11000001;
		optomizePageArray[1] = 1;
	}
  //Disegna il blocco [colonna 2]
	if(blockColoum[2] == 2 | blockColoum[2] == 1){
		pageArray[1] = pageArray[1] | B10000000;
		optomizePageArray[1] = 1;
		pageArray[2] = pageArray[2] | B00001111;
		optomizePageArray[2] = 1;
	}
  //Cancella il blocco [colonna 2]
	if(blockColoum[2] == 3){
		pageArray[1] = pageArray[1] & B01111111;
		optomizePageArray[1] = 1;
		pageArray[2] = pageArray[2] & B11110000;
		optomizePageArray[2] = 1;
	}
  //Disegna il blocco [colonna 3]
	if(blockColoum[3] == 2 | blockColoum[3] == 1){
		pageArray[2] = pageArray[2] | B11100000;
		optomizePageArray[2] = 1;
		pageArray[3] = pageArray[3] | B00000011;
		optomizePageArray[3] = 1;
	}
  //Cancella il blocco [colonna 3]
	if(blockColoum[3] == 3){
		pageArray[2] = pageArray[2] & B00011111;
		optomizePageArray[2] = 1;
		pageArray[3] = pageArray[3] & B11111100;
		optomizePageArray[3] = 1;
	}
  //Disegna il blocco [colonna 4]
	if(blockColoum[4] == 2 | blockColoum[4] == 1){
		pageArray[3] = pageArray[3] | B11111000;
		optomizePageArray[3] = 1;
	}
  //Cancella il blocco [colonna 4]
	if(blockColoum[4] == 3){
		pageArray[3] = pageArray[3] & B00000111;
		optomizePageArray[3] = 1;
	}
  //Disegna il blocco [colonna 5]
	if(blockColoum[5] == 2 | blockColoum[5] == 1){
		pageArray[4] = pageArray[4] | B00111110;
		optomizePageArray[4] = 1;
	}
  //Cancella il blocco [colonna 5]
	if(blockColoum[5] == 3){
		pageArray[4] = pageArray[4] & B11000001;
		optomizePageArray[4] = 1;
	}
  //Disegna il blocco [colonna 6]
	if(blockColoum[6] == 2 | blockColoum[6] == 1){
		pageArray[4] = pageArray[4] | B10000000;
		optomizePageArray[4] = 1;
		pageArray[5] = pageArray[5] | B00001111;
		optomizePageArray[5] = 1;
	}
  //Cancella il blocco [colonna 6]
	if(blockColoum[6] == 3){
		pageArray[4] = pageArray[4] & B01111111;
		optomizePageArray[4] = 1;
		pageArray[5] = pageArray[5] & B11110000;
		optomizePageArray[5] = 1;
	}
  //Disegna il blocco [colonna 7]
	if(blockColoum[7] == 2 | blockColoum[7] == 1){
		pageArray[5] = pageArray[5] | B11100000;
		optomizePageArray[5] = 1;
		pageArray[6] = pageArray[6] | B00000011;
		optomizePageArray[6] = 1;
	}
  //Cancella il blocco [colonna 7]
	if(blockColoum[7] == 3){
		pageArray[5] = pageArray[5] & B00011111;
		optomizePageArray[5] = 1;
		pageArray[6] = pageArray[6] & B11111100;
		optomizePageArray[6] = 1;
	}
  //Disegna il blocco [colonna 8]
	if(blockColoum[8] == 2 | blockColoum[8] == 1){
		pageArray[6] = pageArray[6] | B11111000;
		optomizePageArray[6] = 1;
	}
  //Cancella il blocco [colonna 8]
	if(blockColoum[8] == 3){
		pageArray[6] = pageArray[6] & B00000111;
		optomizePageArray[6] = 1;
	}
  //Disegna il blocco [colonna 9]
	if(blockColoum[9] == 2 | blockColoum[9] == 1){
		pageArray[7] = pageArray[7] | B10111110;
		optomizePageArray[7] = 1;
	}
  //Cancella il blocco [colonna 9]
	if(blockColoum[9] == 3){
		pageArray[7] = pageArray[7] | B10000000;//create side wall
		pageArray[7] = pageArray[7] & B11000001;
		optomizePageArray[7] = 1;
	}

	for(int page = 0; page < 8; page++){
		if(optomizePageArray[page]){
			pageStart = page;
			break;
		}
	}
	for(int page = 7; page >= 0; page--){
		if(optomizePageArray[page]){
			pageEnd = page;
			break;
		}
	}
  //indirizzamento verticale
	OLEDCommand(OLED_SET_ADDRESSING);
	OLEDCommand(OLED_VERTICAL_ADDRESSING);
	OLEDCommand(OLED_SET_COLUMN);
	OLEDCommand(x);
	OLEDCommand(x + 4);
	OLEDCommand(OLED_SET_PAGE);
	OLEDCommand(pageStart);
	OLEDCommand(pageEnd);
  for(int c = 0; c <5; c++){
		for(int p = pageStart; p <= pageEnd; p++){
			OLEDData(pageArray[p]);
		}
	}
}
//------------------------------------------------------------------------------------
void loadPiece(byte peiceNumber, byte row, byte coloum, bool loadScreen){
	byte pieceRow = 0;
	byte pieceColoum = 0;
	byte c = 0;
	switch (peiceNumber){
	case 1: memcpy(currentPiece.umBlock, BlockI, 16);
          break;
	case 2: memcpy(currentPiece.umBlock, BlockJ, 16); 
          break;
	case 3: memcpy(currentPiece.umBlock, BlockL, 16); 
          break;
	case 4: memcpy(currentPiece.umBlock, BlockO, 16);
          break;
	case 5: memcpy(currentPiece.umBlock, BlockS, 16); 
          break;
	case 6: memcpy(currentPiece.umBlock, BlockT, 16); 
          break;
	case 7: memcpy(currentPiece.umBlock, BlockZ, 16); 
          break;
	}
	currentPiece.Row = row;
	currentPiece.Coloum = coloum;
	if(loadScreen){
		oldPiece = currentPiece;
		for(c = coloum; c < coloum + 4; c++){
			for(int r = row; r < row + 4; r++) {
				if(currentPiece.umBlock[pieceColoum][pieceRow]) tetrisScreen[c][r] = 2;
				pieceRow++;
			}
			pieceRow = 0;
			pieceColoum++;
		}
	}
}
//------------------------------------------------------------------------------------
void drawPiece(){
	char coloum;
	char row;
	byte pieceRow = 0;
	byte pieceColoum = 0;
	char c = 0;
	coloum = oldPiece.Coloum;
	row = oldPiece.Row;
	for(c = coloum; c < coloum + 4; c++){
		for(char r = row; r < row + 4; r++){
			if(oldPiece.umBlock[pieceColoum][pieceRow]) tetrisScreen[c][r] = 3;
			pieceRow++;
		}
		pieceRow = 0;
		pieceColoum++;
	}
	pieceRow = 0;
	pieceColoum = 0;
	c = 0;
	coloum = currentPiece.Coloum;
	row = currentPiece.Row;
	for(c = coloum; c < coloum + 4; c++){
		for(char r = row; r < row + 4; r++){
			if(currentPiece.umBlock[pieceColoum][pieceRow]) tetrisScreen[c][r] = 2;
			pieceRow++;
		}
		pieceRow = 0;
		pieceColoum++;
	}
}
//------------------------------------------------------------------------------------
void drawLandedPiece(){
	char coloum;
	char row;
	byte pieceRow = 0;
	byte pieceColoum = 0;
	char c = 0;
	coloum = currentPiece.Coloum;
	row = currentPiece.Row;
	for(c = coloum; c < coloum + 4; c++){
		for(int r = row; r < row + 4; r++){
			if(currentPiece.umBlock[pieceColoum][pieceRow]) tetrisScreen[c][r] = 1;
			pieceRow++;
		}
		pieceRow = 0;
		pieceColoum++;
	}
	processCompletedLines();
}
//------------------------------------------------------------------------------------
bool led = true;
//------------------------------------------------------------------------------------
void RotatePiece(){
	byte i, j;
	byte umFig[4][4] = { 0 };
	memcpy(oldPiece.umBlock, currentPiece.umBlock, 16);
	oldPiece.Row = currentPiece.Row;
	oldPiece.Coloum = currentPiece.Coloum;
	for(i = 0; i < 4; ++i)
		for(j = 0; j < 4; ++j)
			umFig[j][i] = currentPiece.umBlock[4 - i - 1][j];

	oldPiece = currentPiece;
	memcpy(currentPiece.umBlock, umFig, 16);
	if(checkColloision()) currentPiece = oldPiece;
	if(led){digitalWrite(LED, HIGH); led = false; 
  }
	delay(1);
	digitalWrite(LED, LOW);
	if(led == false){digitalWrite(LED, LOW); led = true; 
  }
}
//------------------------------------------------------------------------------------
bool movePieceDown(){
	bool pieceLanded = false;
	char rndPiece = 0;
	oldPiece = currentPiece;
	currentPiece.Row = currentPiece.Row - 1;
	//Verifica la collisione dei blocchi
	if(checkColloision()){
		currentPiece = oldPiece;
		drawLandedPiece();
		pieceLanded = true;
	}
	if(pieceLanded){
		loadPiece(nextPiece, 19, 4, false);
		acceleration = 0;
		if(checkColloision()) gameOver = true;
		else{
			loadPiece(nextPiece, 19, 4, true);
			acceleration = 0;
		}
		nextPiece = random(1, 7);
		setNextBlock(nextPiece);
	}
}
//------------------------------------------------------------------------------------
void movePieceLeft() {
	oldPiece = currentPiece;
	currentPiece.Coloum = currentPiece.Coloum - 1;
	if(checkColloision()) 	currentPiece = oldPiece;
}

void movePieceRight() {
	oldPiece = currentPiece;
	currentPiece.Coloum = currentPiece.Coloum + 1;
	if(checkColloision()) 	currentPiece = oldPiece;
}
//------------------------------------------------------------------------------------
bool checkColloision(){
	byte pieceRow = 0;
	byte pieceColoum = 0;
	char c = 0;
	char coloum = currentPiece.Coloum;
	char row = currentPiece.Row;
	for(c = coloum; c < coloum + 4; c++){
		for(char r = row; r < row + 4; r++){
			if(currentPiece.umBlock[pieceColoum][pieceRow]){
				if(tetrisScreen[c][r] == 1) return true;
			}
			pieceRow++;
		}
		pieceRow = 0;
		pieceColoum++;
	}
	return false;
}
//------------------------------------------------------------------------------------
void processCompletedLines(){
	char rowCheck = 0;
	char coloumCheck = 0;
	bool fullLine = false;
	bool noLine = true;
	char linesProcessed = 0;
	char clearedLines = 0;
	char topRow = 0;
	char bottomRow = 0;
	char currentRow = 0;
	int amountScored = 0;
	if(currentPiece.Row < 1)bottomRow = 1;
	else bottomRow = currentPiece.Row;
	for(int rowCheck = bottomRow; rowCheck < currentPiece.Row + 4; rowCheck++){
		bool fullLine = true;
		for(coloumCheck = 2; coloumCheck < 12; coloumCheck++) {
			if(tetrisScreen[coloumCheck][rowCheck] == 0) { fullLine = false; break; }
		}
		if(fullLine){ 
			for(char c = 2; c < 12; c++) tetrisScreen[c][rowCheck] = 3;
			bottomRow = rowCheck + 1;
			linesProcessed++;
			delay(77); 
		}
		drawTetrisScreen();
	}
	if(linesProcessed){
		clearedLines = linesProcessed;
		while(clearedLines){
      //Suona quando processa la linea
      tone(BUZZER, 1000, 40);
      delay(50);
      tone(BUZZER, 2000, 60);
      delay(50);
      tone(BUZZER, 500, 80);
      delay(50);
			for(currentRow = 1; currentRow < 20; currentRow++){
				noLine = true;
				for(char c = 2; c < 12; c++){
					if(tetrisScreen[c][currentRow])  noLine = false;
				}
				if(noLine){
					for(int r = currentRow + 1; r < 20; r++){
						for(char c = 2; c < 12; c++){
							if(tetrisScreen[c][r]) tetrisScreen[c][r - 1] = 2;
							else tetrisScreen[c][r - 1] = 3;
						}
					}
				}
			}
			for(char r = 1; r < 24; r++){
				for(char c = 2; c < 12; c++){
					if(tetrisScreen[c][r] == 2)tetrisScreen[c][r] = 1;
				}
			}
			clearedLines--;
			drawTetrisScreen();
		}
	}
	//Punteggio
	switch (linesProcessed){
	  case 1:amountScored = 40 * (level + 1);
      break;
	  case 2:amountScored = 100 * (level + 1);
      break;
	  case 3:amountScored = 300 * (level + 1);
      break;
	  case 4:amountScored = 1200 * (level + 1);
		       OLEDCommand(OLED_INVERSE_DISPLAY);
		       delay(100);
		      OLEDCommand(OLED_NORMAL_DISPLAY);
		  break;
	}

	for(long s = score; s < score + amountScored; s = s + (1 * (level+1))) setScore(s, false);
	score = score + amountScored;
	setScore(score, false);
	levellineCount = levellineCount + linesProcessed;
	if(levellineCount > 10) {
		level++;
		levellineCount = 0;
		OLEDCommand(OLED_INVERSE_DISPLAY);
		delay(100);
		OLEDCommand(OLED_NORMAL_DISPLAY);
		delay(100);
		OLEDCommand(OLED_INVERSE_DISPLAY);
		delay(100);
		OLEDCommand(OLED_NORMAL_DISPLAY);
	}
	for(char r = bottomRow; r <= topRow; r++){
		for(char c = 2; c < 12; c++) {
			if(tetrisScreen[c][r]) tetrisScreen[c][r] = 1;
		}
	}
}
//------------------------------------------------------------------------------------
void tetrisScreenToSerial(){
	for(int r = 0; r < 24; r++){
		for(int c = 0; c < 14; c++){
			Serial.print(tetrisScreen[c][r], DEC);
		}
		Serial.println();
	}
	Serial.println();
}
long keytimer = 0;
bool processKey = true;
int Debounce = 0;
//------------------------------------------------------------------------------------
//Funzione per il processamento dei comandi da joystick
bool processKeys(){
	char uiKeyCode = 0;
	bool keypressed = true;
	int leftRight = 300 - acceleration;
	int rotate = 700;
	int down = 110 - acceleration;
	int Val_X_J = analogRead(A0);
	int Val_Y_J = analogRead(A1);
	int Val_B_J = digitalRead(3);
	if(Val_X_J <= 200){
		Debounce++;
		if(Debounce > 10){
			if(processKey){
				uiKeyCode = KEY_LEFT;
				key.left = millis();
        tone(BUZZER, 500, 10);
        delay(20);
			}
			if(millis() < key.left + leftRight) processKey = false;
			else{
				processKey = true;
				acceleration = acceleration + 70;
				if(acceleration > leftRight) acceleration = leftRight;
			}
		}
	}
	else if(Val_X_J >= 800){
		Debounce++;
		if(Debounce > 10){
			if(processKey) {
				uiKeyCode = KEY_RIGHT;
				key.right = millis();
        tone(BUZZER, 500, 10);
        delay(20);
			}
			if(millis() < key.right + leftRight) processKey = false;
			else{
				processKey = true;
				acceleration = acceleration + 70;
				if(acceleration > leftRight) acceleration = leftRight;
			}
		}
	}
	else if(Val_Y_J >= 800){
		Debounce++;
		if(Debounce > 10){
			if(processKey){
				uiKeyCode = KEY_DOWN;
				key.down = millis();
        tone(BUZZER, 700, 30);
        delay(20);
			}
			if(millis() < key.down + down) processKey = false;
			else{
				processKey = true;
				acceleration = acceleration + 40;
				if(acceleration > down) acceleration = down;
			}
		}
	}
	else if(Val_Y_J <= 200){
		Debounce++;
		if(Debounce > 10){
			if(processKey){
				uiKeyCode = KEY_ROTATE;
				key.rotate = millis();
        tone(BUZZER, 500, 10);
        delay(20);
			}
			if(millis() < key.rotate + rotate) processKey = false;
			else processKey = true;
		}
	}
	else{
		acceleration = 0; processKey = true; Debounce = 0;
	}
	switch (uiKeyCode){
	   case KEY_LEFT:movePieceLeft();
       break;
	   case KEY_RIGHT:movePieceRight();
       break;
	   case KEY_DOWN:movePieceDown();
       break;
	   case KEY_ROTATE:RotatePiece();
       break;
	   default:keypressed = false;
       break;
	}
	if(keypressed){
		drawPiece();
		drawTetrisScreen();
	}
}

bool processNewGameKey()
{
	//int Val_X_J = analogRead(A0);
	int Val_Y_J = analogRead(A1);
	if(Val_Y_J < 100){
		Debounce++;
		if(Debounce > 10){
			Debounce = 0;
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------
//Funzione scrittura punteggio
void setScore(long score, bool blank){
	long ones = (score % 10);
	long tens = ((score / 10) % 10);
	long hundreds = ((score / 100) % 10);
	long thousands = ((score / 1000) % 10);
	long tenthousands = ((score / 10000) % 10);
	long hunderedthousands = ((score / 100000) % 10);
	byte font = 0;
	char bytes_out[8];
	memset(scoreDisplayBuffer, 0, sizeof scoreDisplayBuffer);
	//Sesta cifra del punteggio
	for(int v = 0; v<8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[hunderedthousands][v]);
	//Scrive il buffer del conteggio
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][0] = scoreDisplayBuffer[i][0] | bytes_out[i] >> 1;
	}
  //Quinta cifra del punteggio
	for(int v = 0; v<8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[tenthousands][v]);
  //Scrive il buffer del conteggio
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][0] = scoreDisplayBuffer[i][0] | (bytes_out[i] << 6);
	}
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][1] = scoreDisplayBuffer[i][1] | bytes_out[i] >> 1;
	}
  //Quarta cifra del punteggio
	for(int v = 0; v<8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[thousands][v]);
	//Scrive il buffer del conteggio
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][1] = scoreDisplayBuffer[i][1] | (bytes_out[i] << 6);
	}
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][2] = scoreDisplayBuffer[i][2] | bytes_out[i] >> 1;
	}
  //Terza cifra del punteggio
	for(int v = 0; v<8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[hundreds][v]);
	//Scrive il buffer del conteggio
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][2] = scoreDisplayBuffer[i][2] | (bytes_out[i] << 6);
	}
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][3] = scoreDisplayBuffer[i][3] | bytes_out[i] >> 1;
	}
  //Seconda cifra del punteggio
	for(int v = 0; v<8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[tens][v]);
	//Scrive il buffer del conteggio
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][3] = scoreDisplayBuffer[i][3] | (bytes_out[i] << 6);
	}
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][4] = scoreDisplayBuffer[i][4] | bytes_out[i] >> 1;
	}
  //Prima cifra del punteggio
	for(int v = 0; v<8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[ones][v]);
	//Scrive il buffer del conteggio
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][4] = scoreDisplayBuffer[i][4] | (bytes_out[i] << 6);
	}
	for(int i = 0; i < 8; i++){
		scoreDisplayBuffer[i][5] = scoreDisplayBuffer[i][5] | bytes_out[i] >> 1;
	}
	//Imposta la modalità di indirizzamento verticale e la colonna - fine inizio pagina
	OLEDCommand(OLED_SET_ADDRESSING);
	OLEDCommand(OLED_VERTICAL_ADDRESSING);
	OLEDCommand(OLED_SET_COLUMN);
	OLEDCommand(120);
	OLEDCommand(127); 
	OLEDCommand(OLED_SET_PAGE);
	OLEDCommand(0);
	OLEDCommand(5);
	for(int p = 0; p < 8; p++){
		for(int c = 0; c <6; c++){
			if(blank) OLEDData(0);
			else OLEDData(scoreDisplayBuffer[p][c]);
		}
	}
}
//------------------------------------------------------------------------------------
void setNextBlock(byte peiceNumber){
	memset(nextBlockBuffer, 0, sizeof nextBlockBuffer); //clear buffer
	switch(peiceNumber){
	case 1:
		//************L Pezzo - 1 *************
		for(int k = 2; k < 6; k++){
			nextBlockBuffer[k][0] = B01110111;
			nextBlockBuffer[k][1] = B01110111;
		}
		break;
	case 2:
		//************J Pezzo - 2 *************
		for(int k = 0; k < 3; k++){
			nextBlockBuffer[k][0] = B01110000;
			nextBlockBuffer[k][1] = B01110111;
		}
		for(int k = 4; k < 7; k++){
			nextBlockBuffer[k][0] = B01110000;
		}
		break;
	case 3:
		//************L Pezzo - 3 *************
		for(int k = 0; k < 3; k++){
			nextBlockBuffer[k][0] = B01110000;
		}
		for(int k = 4; k < 7; k++){
			nextBlockBuffer[k][0] = B01110000;
			nextBlockBuffer[k][1] = B01110111;
		}
		break;
	case 4:
		//************O Pezzo - 4 *************
		for(int k = 0; k < 3; k++){
			nextBlockBuffer[k][0] = B01110000;
			nextBlockBuffer[k][1] = B00000111;
		}
		for(int k = 4; k < 7; k++){
			nextBlockBuffer[k][0] = B01110000;
			nextBlockBuffer[k][1] = B00000111;
		}
		break;
	case 5:
		//************S Pezzo - 5 *************
		for(int k = 0; k < 3; k++){
			nextBlockBuffer[k][0] = B01110000;
			nextBlockBuffer[k][1] = B00000111;
		}
		for(int k = 4; k < 7; k++){
			nextBlockBuffer[k][0] = B00000000;
			nextBlockBuffer[k][1] = B11101110;
		}
		break;
	case 6:

		//************T Pezzo - 6 *************
		for(int k = 0; k < 3; k++){
			nextBlockBuffer[k][0] = B01110000;
			nextBlockBuffer[k][1] = B01110111;
		}
		for(int k = 4; k < 7; k++){
			nextBlockBuffer[k][0] = B00000000;
			nextBlockBuffer[k][1] = B00001110;
		}
		break;
	case 7:
		//************Z Pezzo - 7 *************
		for(int k = 0; k < 3; k++){
			nextBlockBuffer[k][0] = B01110000;
			nextBlockBuffer[k][1] = B00000111;
		}
		for(int k = 4; k < 7; k++){
			nextBlockBuffer[k][0] = B11101110;
			nextBlockBuffer[k][1] = B00000000;
		}
		break;
	}
	//Imposta la modalità di indirizzamento verticale e la colonna - fine inizio pagina
	OLEDCommand(OLED_SET_ADDRESSING);
	OLEDCommand(OLED_VERTICAL_ADDRESSING);
	OLEDCommand(OLED_SET_COLUMN);
	OLEDCommand(120);
	OLEDCommand(127);
	OLEDCommand(OLED_SET_PAGE);
	OLEDCommand(6);
	OLEDCommand(7);
	for(int p = 0; p < 8; p++){
		for(int c = 0; c <2; c++){
			OLEDData(nextBlockBuffer[p][c]);
		}
	}
}
//------------------------------------------------------------------------------------
void drawBottom(){
	//Imposta la modalità di indirizzamento verticale e la colonna - fine inizio pagina
	OLEDCommand(OLED_SET_ADDRESSING);
	OLEDCommand(OLED_VERTICAL_ADDRESSING);
	OLEDCommand(OLED_SET_COLUMN);
	OLEDCommand(0);
	OLEDCommand(0);

	OLEDCommand(OLED_SET_PAGE);
	OLEDCommand(0);
	OLEDCommand(7);
	for(int c = 0; c <8; c++){
		OLEDData(255);
	}
}
//------------------------------------------------------------------------------------
void drawSides(){
	//Imposta la modalità di indirizzamento verticale e la colonna - fine inizio pagina
	OLEDCommand(OLED_SET_ADDRESSING);
	OLEDCommand(OLED_VERTICAL_ADDRESSING);
	OLEDCommand(OLED_SET_COLUMN);
	OLEDCommand(0);
	OLEDCommand(127);
	OLEDCommand(OLED_SET_PAGE);
	OLEDCommand(0);
	OLEDCommand(7);
	for(int r = 0; r < 128; r++) {
		for (int c = 0; c < 8; c++){
			if(c == 0) OLEDData(1);
			else if(c == 7) OLEDData(128);
			else OLEDData(0);
		}
	}
}

byte ByteMirror(byte b)
{
	return ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16; 
}

//------------------------------------------------------------------------------------
//Funzione Loop principale
void loop(){
	gameOver = false;
	score = 0;
	fillTetrisArray(1);
	fillTetrisScreen(2);
	drawTetrisScreen();
	delay(200);
	fillTetrisScreen(3);
	drawTetrisScreen();
	delay(200);
	drawSides();
	drawBottom();
	tetrisScreenToSerial();
	OLEDCommand(OLED_INVERSE_DISPLAY);
	delay(200);
	OLEDCommand(OLED_NORMAL_DISPLAY);
	loadPiece(random(1, 7), 20, 5, true);
	drawTetrisScreen();
	nextPiece = random(1, 7);
	setNextBlock(nextPiece);
	setScore(0, false);
	delay(300);
	setScore(0, true);
	delay(300);
	setScore(0, false);
	byte rnd = 0;

	//gameOver = true;


	while(!gameOver){
		movePieceDown();
		drawPiece();
		drawTetrisScreen();
		moveTime = millis();
		while(millis() - moveTime<(dropDelay - (level * 50))) processKeys();
	}

	delay(300);

	Serial.println("Game Over");
	OLEDCommand(OLED_SET_ADDRESSING);
	OLEDCommand(OLED_VERTICAL_ADDRESSING);
	for(int i=0;i<4;++i) {
		OLEDCommand(OLED_SET_COLUMN);
		OLEDCommand(70);
		OLEDCommand(79);
		OLEDCommand(OLED_SET_PAGE);
		OLEDCommand((byte)i+2);
		OLEDCommand((byte)i+2);
		for (int y=7;y>=0;y--) 
		{
			OLEDData(ByteMirror(pgm_read_byte(&GameOver[i][y])));
		}
	}
	for(int i=0;i<4;++i) {
		OLEDCommand(OLED_SET_COLUMN);
		OLEDCommand(60);
		OLEDCommand(69);
		OLEDCommand(OLED_SET_PAGE);
		OLEDCommand((byte)i+2);
		OLEDCommand((byte)i+2);
		for (int y=7;y>=0;y--) 
		{
			OLEDData(ByteMirror(pgm_read_byte(&GameOver[i+4][y])));
		}
	}

	while(!processNewGameKey());

	Serial.println("Resume");
}