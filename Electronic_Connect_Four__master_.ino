
//Global variables and values to be changed by user if neccesary

//number of col or rows,
int numberOfCols=3;
int numberOfRows=4;

//array the size of number of cols or rows
int cursorColPos[3];
int cursorRowPos[4];

//place in top to down or left to right the arduino pins associated with each column and row
int colPins[]={2,3,4};
int rowPins[]={5,6,7,8};

//arrays for saving moves. All values should be ((((numberOfCols*NumberOfRows)-(numberOfCols))/2)round value up if non integer)+1)
int player1RowMoveList[6];
int player1ColMoveList[6];
int player2RowMoveList[6];
int player2ColMoveList[6];

//pins for input buttons
int left=A5;
int middle=A6;
int right=A7;

//analog pins for delay and undo
int delayPin=A0;
int undoPin=A1;

//pin for switching between player 1 and player 2
int player2=18;

//switch voltages for ignoring certain users inputs
int player1BottomVoltage=700;
int player2TopVoltage=700;
int player2BottomVoltage=200;

//pot voltages for determing first player to move and input delay.
int startingPlayerPotVoltage=775;
int userInputDelaySubtraction=400;

//
//Do not change variables below
//


//mutiplex delay should always be one.Needed for LEDs to blink properly in multiplexPos() function
int multiplexDelay=1;

//intiliazing as 0 but immediately changes based on potentiometer reading
int userInputDelay=0;

//counter for keeping track of number of moves. Initializes as 0.
int player1MoveCounter=0;
int player2MoveCounter=0;

//makes global variables for current player and current player save. Intializes as zero but changes based on potentiometer reading.
int currentPlayer=0;
int tempCurrentPlayerSave=0;

//variable for validating time delay
unsigned long time=millis();

void setup() {

//LED setup
  
//make arduino row pins outputs and initialize them as low
for (int i=0; i <numberOfRows;i++){
  pinMode(rowPins[i],OUTPUT);
  digitalWrite(rowPins[i],LOW);
  }

//make arduino col pins outputs and initialize them as low
 for (int i=0; i<numberOfCols;i++){  
  pinMode(colPins[i],OUTPUT);
  digitalWrite(colPins[i],LOW);  
  }0;

pinMode(player2,OUTPUT);

//Controls setup  
  
//initializing the array move lists as -1 since 0 index corresponds to first row and column
for (int i=0; i<(numberOfCols*numberOfRows);i++){
  player1RowMoveList[i]=-1;
  player1ColMoveList[i]=-1;
  player2RowMoveList[i]=-1;
  player2ColMoveList[i]=-1;
  }

pinMode(delayPin,INPUT);
pinMode(undoPin,INPUT);

//setting input buttons as arduino inputs
pinMode(left,INPUT);
pinMode(right,INPUT);
pinMode(middle,INPUT);

//setting them low as default
digitalWrite(left,LOW);
digitalWrite(right,LOW);
digitalWrite(middle,LOW);

//start the cursor at position 0,0 (first row,first column);
cursorColPos[0]=1;
cursorRowPos[0]=1;
  
//End of controls setupMovesList[
  
  
Serial.begin(9600);
if (currentPlayer==1){
changeToPlayer1();
}
else if (currentPlayer==2){
changeToPlayer2();
}

//determines starting player based on potentiometer position
if (analogRead(delayPin)>startingPlayerPotVoltage){
currentPlayer=1;
tempCurrentPlayerSave=1;
}
else{
  currentPlayer=2;
tempCurrentPlayerSave=2;
  }

//code that continously loops
while(1){

  //setting input delay for both players based on single potentiometer reading
  userInputDelay=analogRead(delayPin)-userInputDelaySubtraction;


//Code for continously multiplexing both players saved moves

  //saving if player 1 or player 2 has cursor control
  tempCurrentPlayerSave=currentPlayer;

  //blinking player 1 first, then 2 since this pattern allows least maxiumum amount of time a single player is not blinked
  changeToPlayer1();
  blinkPlayer1();
  changeToPlayer2();
  blinkPlayer2();

  
  //returning cursor control to correct player
  if (tempCurrentPlayerSave==1){
    changeToPlayer1();
    }
    else if (tempCurrentPlayerSave==2){
      changeToPlayer2();0;

      }

//continually multiplex the cursor position
multiplexPos(cursorColIndex(),cursorRowIndex());


//Do not allow player 1 to move unless it is his move and the delay time has passed
if (((millis()-time)>userInputDelay)&&(currentPlayer==1)){
  time=millis();

  //comparison stops player2 from inputting moves during player1 turn

  // player 1 undos player 2 move if there is a move that can be undone
  if ((analogRead(undoPin)>player1BottomVoltage)&&(player2MoveCounter>0)){
    undoPlayer2Move();
    }

    //player 1 moves cursor right
    else if (analogRead(right)>player1BottomVoltage){
      moveRight();
      }

      //player 1 moves cursor left
      else if (analogRead(left)>player1BottomVoltage){
        moveLeft();
        }
        
        //if move below cursor has not happened
        else if ((analogRead(middle)>player1BottomVoltage) && (checkIfMoveHappened(cursorRowIndex()+1,cursorColIndex())==false)){

          //update player 1 move list and move counter
          player1RowMoveList[player1MoveCounter]=cursorRowIndex()+1;
          player1ColMoveList[player1MoveCounter]=cursorColIndex();
          player1MoveCounter=player1MoveCounter+1; //if move is already present in move list

          //blink player 1 move list and player 2 
          blinkAllButCursor();

          // if the move has space in column to fall down, drop the move automatically
          dropPlayer1Move();

          //player 1 turn is over and change to player 2 turn
          changeToPlayer2();
          
          }
          }


if (((millis()-time)>userInputDelay)&&(currentPlayer==2)){
  time=millis();

    
// move cursor based on corresponding inputs
if ((analogRead(undoPin)<player2TopVoltage)&&(analogRead(undoPin)>player2BottomVoltage)&&(player1MoveCounter>0)){
  undoPlayer1Move();
  }
  else if ((analogRead(right)<player2TopVoltage)&&(analogRead(right)>player2BottomVoltage)){
  moveRight();
  }
  else if ((analogRead(left)<player2TopVoltage)&&(analogRead(left)>player2BottomVoltage)){
  moveLeft();
  }

  //for player 2, add position selected to move list and increase the amount of moves saved by 1
  else if ((analogRead(middle)<player2TopVoltage) && (analogRead(middle)>player2BottomVoltage) && (checkIfMoveHappened(cursorRowIndex()+1,cursorColIndex())==false)){
  player2RowMoveList[player2MoveCounter]=cursorRowIndex()+1;
  player2ColMoveList[player2MoveCounter]=cursorColIndex();
  
  player2MoveCounter=player2MoveCounter+1; //if move is already present in move list
  blinkAllButCursor();

  //animation to show drop
  dropPlayer2Move();
  
  changeToPlayer1();
  }
  
  }

}
}
  
//start of functions

void undoPlayer1Move(){
  player1RowMoveList[player1MoveCounter]=-1;
  player1ColMoveList[player1MoveCounter]=-1;  
  
  player1MoveCounter=player1MoveCounter-1; 
  changeToPlayer1();
}
    

void undoPlayer2Move(){
  player2RowMoveList[player2MoveCounter]=-1;
  player2ColMoveList[player2MoveCounter]=-1;  
  
  player2MoveCounter=player2MoveCounter-1; 
  changeToPlayer2();
}
    
void blinkAllButCursor(){
  blinkPlayer1();
  blinkPlayer2();
  }

void dropPlayer1Move(){
  for (int i = 0; i<numberOfRows-2;i++){
  if ((checkIfMoveHappened(player1RowMoveList[player1MoveCounter-1]+1,player1ColMoveList[player1MoveCounter-1])==false))
  player1RowMoveList[player1MoveCounter-1]=player1RowMoveList[player1MoveCounter-1]+1;

  blinkAllButCursor();
  }
}

void dropPlayer2Move(){
  for (int i = 0; i<numberOfRows-2;i++){0;

  if ((checkIfMoveHappened(player2RowMoveList[player2MoveCounter-1]+1,player2ColMoveList[player2MoveCounter-1])==false))
  player2RowMoveList[player2MoveCounter-1]=player2RowMoveList[player2MoveCounter-1]+1;

  blinkAllButCursor();
  }
  
  }

void blinkPlayer1(){
    for (int i = 0;i<(player1MoveCounter);i++){
    multiplexPos(player1ColMoveList[i],player1RowMoveList[i]);
    }
    
  
  }

void blinkPlayer2(){
    for (int i = 0;i<(player2MoveCounter);i++){
   multiplexPos(player2ColMoveList[i],player2RowMoveList[i]);
    }
  }
void changeToPlayer1(){
  currentPlayer=1;
  digitalWrite(player2,LOW);0;

  }

void changeToPlayer2(){
  currentPlayer=2;
  digitalWrite(player2,HIGH);
  }

bool checkIfMoveHappened(int rowPos,int colPos){
  for (int i=0;i<(player1MoveCounter);i++){
    if ((rowPos==player1RowMoveList[i])&&(colPos==player1ColMoveList[i])){
    return true;
    }
  }
  for (int i=0;i<(player2MoveCounter);i++){
    if ((rowPos==player2RowMoveList[i])&&(colPos==player2ColMoveList[i])){
    return true;
    }
  }
    return false;
  }

//returns the index of the row that cursor is in
int cursorRowIndex(){
  for (int i=0;i<numberOfRows;i++){
    if (cursorRowPos[i]==1){
      return i;
      }
      }
    return 0;
  }

//returns the index of the col that the curosr is in
int cursorColIndex(){
  for(int i=0;i<numberOfCols;i++){
    if (cursorColPos[i]==1){
      return i;
      }
      }
      return 0;
  }



//moves cursor left  
void moveLeft(){

  //does not move cursor if already in leftmost column
  for (int i=1;i<numberOfCols;i++){
    if (cursorColPos[i]==1){
      cursorColPos[i]=0;
      cursorColPos[i-1]=1;
      }
    }
    }

//moves cursor right
void moveRight(){

  //does not move cursor if already in rightmost column
  for (int i=(numberOfCols-2);i>=0;i--){
    if (cursorColPos[i]==1){
      cursorColPos[i]=0;
      cursorColPos[i+1]=1;
      }
    }
    }

    //move cursors down
void moveDown(){
  
  //does not move cursor down already in bottom row
  for (int i=(numberOfRows-2);i>=0;i--){
    if (cursorRowPos[i]==1){
      cursorRowPos[i]=0;
      cursorRowPos[i+1]=1;
      }
    }
    }

//moves cursor up
void moveUp(){

  //does not move cursor up if already in top row
    for (int i=1;i<numberOfRows;i++){
    if (cursorRowPos[i]==1){
      cursorRowPos[i]=0;
      cursorRowPos[i-1]=1;
      }
    }
    }


//mutiplexing funciton
void multiplexPos(int colPinsIndex,int rowPinsIndex){
  for (int i=0;i<numberOfRows;i++){

    //set all grounds pin high unless pin corresponds to LED being mutiplexed
    if (i==rowPinsIndex){
      digitalWrite(rowPins[i],HIGH);
      }
    }
    //set postive pin high that corresponds to LED being mutiplexed
    digitalWrite(colPins[colPinsIndex],HIGH);
  
  //delay that prevent all LED turning on in mutiplexed column
  delay(multiplexDelay);

  //set positive pin low that corresponds to LED being mutiplexed
    digitalWrite(colPins[colPinsIndex],LOW);

  //set all ground pins low unless pin corresponds to LED being mutiplexed.
  for (int i=0; i<numberOfRows;i++){
    if (i==rowPinsIndex){
      digitalWrite(rowPins[i],LOW);
      }
    }
  }

