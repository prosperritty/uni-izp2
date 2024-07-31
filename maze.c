/*
IZP projekt №2
VUT FIT
**************
30.11.2023
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//Defines of directions
//Explained in function isborder
#define LEFT 0
#define RIGHT 1
#define UPDOWN 2
#define SIDES 3
//Defines for test results
#define TEST_VALID 0
#define TEST_INVALID 1
//Some ASCII table defines
#define ASCII_SPACE 32
#define ASCII_LINEFEED 10

typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;
 
//Constructor of Map and array of cells
Map* Ctor(FILE* file){
  
  //Allocate whole structure
  Map *map;
  map = malloc(sizeof(Map));
  fscanf(file, "%i", &map->rows);
  fscanf(file, "%i", &map->cols);
  fgetc(file);

  //Allocate arrray of cell values
  map->cells = malloc(sizeof(unsigned char)*map->rows*map->cols);

  return map;
}

//Destructor of Map and array of cells
void Dtor(Map *map){
    free(map->cells);
    free(map);
}

//Fills an array of cells in MAP from txt file
void cells_filling(Map *map, FILE* file){
  char temp;
  for(int r = 0; r<map->rows; r++){
    for(int c = 0; c<map->cols; c++){
      
      //Skip all "white chars"
      while(true){
        temp = fgetc(file);
        if(temp != ASCII_LINEFEED && temp!=ASCII_SPACE){
          map->cells[r*map->cols+c] = temp;
          break;
        }
      }

    }
  }
}

//Function that will find out if there is a border by some project specifications(bin value)
bool isborder(Map *map, int r, int c, int border){

  char binExpression[SIDES+1] = "";

  //The coordintes of cell i have in arguments of this function can start with 1
  //So i need to decrease the value by 1, so i can have the right one value from array
  int valueExpression = map->cells[c-1+(r-1)*map->cols] - '0';
  
  //Cycle converts char to binary
  for(int i = 0; i<SIDES; i++){
    if(valueExpression % 2 == 0) binExpression[2-i] = '0';
    else binExpression[2-i] = '1';
    valueExpression = valueExpression/2;
  }

  binExpression[3] = '\0';

  //BIN VALUES RELATION
  //UPDOWN - RIGHT - LEFT
  if(binExpression[2-border] == '1') return true;    //there is a border
  else return false;                                 //there isn't
}

//Help function that will find out if cell have upper or lower border
//TRUE->Upper FALSE->Lower
bool upper_border(int r, int c){
  if((r+c)%2==0) return true;
  else return false;
}

//Help function, that will make moves in maze solving
void change_pos(int *r, int *c, int border){
  if(border==RIGHT) *c = *c + 1;
  else if(border==LEFT) *c = *c - 1;
  else if(border == UPDOWN && upper_border(*r, *c)) *r = *r - 1;
  else if(border == UPDOWN && !upper_border(*r, *c)) *r = *r + 1;
}

//Function that find out if there is any problem with txt file
int is_valid(char *fname){
  FILE* file = fopen(fname, "r");
  if(file == NULL) return TEST_INVALID;
  Map *map = Ctor(file);

  if(map == NULL || map->cells == NULL) return TEST_INVALID;
  if(map->cols<=0 || map->rows<=0) return TEST_INVALID;

  char temp;
  //Cycle checks if all cells have a value assigned
  for(int r = 0; r<map->rows; r++){

    for(int c = 0; c<map->cols; c++){
      
      //Skip all white spaces
      while(true){
        temp = fgetc(file);
        if(temp != ASCII_LINEFEED && temp!=ASCII_SPACE){
          map->cells[r*map->cols+c] = temp;
          break;
        }
      }
      if(map->cells[r*map->cols+c]<'0' || map->cells[r*map->cols+c]>'7') return TEST_INVALID;
      if(feof(file)) return TEST_INVALID;
    }

  }

  //Cycle checks if there is any char besides SPACE and LINE FEED after cell values
  while (feof(file) == 0){
    int temp = fgetc(file);
    if(temp == -1) break;
    if(temp != ASCII_SPACE && temp != ASCII_LINEFEED) return TEST_INVALID;
  }


  //Cycle, that checks if all borders are equal
  for(int r = 1;r<=map->rows;r++){

    for(int c = 1; c<=map->cols;c++){
      //bool variables for checking on boards for current position and requesting position
      bool initialCheck, nextCheck;
      
      initialCheck = isborder(map, r, c, RIGHT);
      //Control RIGHT-LEFT borders
      change_pos(&r, &c, RIGHT);
      if(c!=map->cols+1){
        nextCheck = isborder(map, r, c, LEFT);
        if(initialCheck != nextCheck) return TEST_INVALID;
      }
      change_pos(&r, &c, LEFT);

      //Control UPDOWN borders
      initialCheck = isborder(map, r, c, UPDOWN);
      change_pos(&r, &c, UPDOWN);
      if(r!=map->rows+1 && r!=0){
        nextCheck = isborder(map, r, c, UPDOWN);
        if(initialCheck != nextCheck) return TEST_INVALID;
      }
      change_pos(&r, &c, UPDOWN);
    }
  }
  
  fclose(file);
  Dtor(map);
  return TEST_VALID;
}

//Function, that will check if entry point is appropriate
bool correct_enter(Map *map, int r, int c){
  if(r<=0 || c<=0 || r>map->rows || c>map->cols) return TEST_INVALID;
  
  bool isCorner;
  if((r==1 && c==1) || (r==1 && c==map->cols) || (r==map->rows && c==1) || (r==map->rows && c==map->cols)) isCorner = true;

  if(isCorner != true){
    //Enter must be from edges
    if(r>1 && r<map->rows){
      if(c!=1 && c!=map->cols) return TEST_INVALID;
    }
    else if(c>1 && c<map->cols){
      if(r!=1 && r!=map->rows) return TEST_INVALID;
    }

    //There must be passable border to enter
    if((c==1) && isborder(map, r, c, LEFT)) return TEST_INVALID;
    else if((c==map->cols) && isborder(map, r, c, RIGHT)) return TEST_INVALID;
    //There is no enter from bottom or above if there isnt border by structure
    else if((r == 1 && !upper_border(r, c)) || (r == map->rows && upper_border(r, c))) return TEST_INVALID;
    //There is no enter from bottom or above if there isnt passable border
    else if((r==1 || r==map->rows) && isborder(map, r, c, UPDOWN)) return TEST_INVALID;
  }

  //In case entry is from "corner" create Counter
  //If this counter hits 2 we can not go through -> not correct enter
  else if(isCorner == true){
    int borderCounter = 0;

    if((c==1) && isborder(map, r, c, LEFT)) borderCounter = borderCounter + 1;
    else if((c==map->cols) && isborder(map, r, c, RIGHT)) borderCounter = borderCounter + 1;

    if((r == 1 && !upper_border(r, c)) || (r == map->rows && upper_border(r, c))) borderCounter = borderCounter + 1;
    else if((r==1 || r==map->rows) && isborder(map, r, c, UPDOWN)) borderCounter = borderCounter + 1;

    if(borderCounter == 2) return TEST_INVALID;
    
  }

  return TEST_VALID;
}

//Finction that will find first border to go
int start_border(Map *map, int r, int c, int leftright){             /* RULES *right=1* *left=2* RULES */
  if(leftright == 1){
    //Left-enter on odd row
    if(c==1 && r%2==1 && !isborder(map, r, c, RIGHT)) return RIGHT;
    else if(c==1 && r%2==1 && !isborder(map, r, c, UPDOWN)) return UPDOWN;
    //Left-enter on even row *CORNER ENTER EXCEPTION*
    if(c==1 && r%2==0 && !isborder(map, r, c, UPDOWN) && r!=map->rows) return UPDOWN;
    else if(c==1 && r%2==0 && !isborder(map, r, c, RIGHT)) return RIGHT;
    //Right-enter if cell have upper border *CORNER ENTER EXCEPTION*
    if(c==map->cols && upper_border(r, c) && !isborder(map, r, c, UPDOWN) && r!=1) return UPDOWN;
    else if(c==map->cols && upper_border(r, c) && !isborder(map, r, c, LEFT)) return LEFT;
    //Right-enter if cell have lower border
    if(c==map->cols && !upper_border(r, c) && !isborder(map, r, c, LEFT)) return LEFT;
    else if(c==map->cols && !upper_border(r, c) && !isborder(map, r, c, UPDOWN)) return UPDOWN;
    //Upper-enter
    if(r==1 && !isborder(map, r, c, LEFT)) return LEFT;
    else if(r==1 && !isborder(map, r, c, RIGHT)) return RIGHT;
    //Down-enter
    if(r==map->cols && !isborder(map, r, c, RIGHT)) return RIGHT;
    else if(r==map->cols && !isborder(map, r, c, LEFT)) return LEFT;
  }
  if(leftright == 2){
    //Left-enter on odd row *CORNER ENTER EXCEPTION*
    if(c==1 && r%2==1 && !isborder(map, r, c, UPDOWN) && r!=1) return UPDOWN;
    else if(c==1 && r%2==1 && !isborder(map, r, c, RIGHT)) return RIGHT;
    //Left-enter on even row
    if(c==1 && r%2==0 && !isborder(map, r, c, RIGHT)) return RIGHT;
    else if(c==1 && r%2==0 && !isborder(map, r, c, UPDOWN)) return UPDOWN;
    //Right-enter if cell have upper border
    if(c==map->cols && upper_border(r, c) && !isborder(map, r, c, LEFT)) return LEFT;
    else if(c==map->cols && upper_border(r, c) && !isborder(map, r, c, UPDOWN)) return UPDOWN;
    //Right-enter if cell have lower border  *CORNER ENTER EXCEPTION*
    if(c==map->cols && !upper_border(r, c) && !isborder(map, r, c, UPDOWN) && r!=map->rows) return UPDOWN;
    else if(c==map->cols && !upper_border(r, c) && !isborder(map, r, c, LEFT)) return LEFT;
    //Upper-enter
    if(r==1 && !isborder(map, r, c, RIGHT)) return RIGHT;
    else if(r==1 && !isborder(map, r, c, LEFT)) return LEFT;
    //Down-enter
    if(r==map->cols && !isborder(map, r, c, LEFT)) return LEFT;
    else if(r==map->cols && !isborder(map, r, c, RIGHT)) return RIGHT;
  }

  //DEAD END
  return -1;
}

void solve_maze(Map *map, int r, int c, int border, int leftright){    /* RULES *right=1* *left=2* RULES */
  //Work with first cell
  printf("%i,%i\n", r, c);
  change_pos(&r, &c, border);

  //Variable, that saves broder value of previous move relatively to current one
  int entryBorder;

  while(r>0 && r<=map->rows && c>0 && c<=map->cols){
    printf("%i,%i\n", r, c);

    //Change border value "where" to "from"
    if(border != UPDOWN){
      entryBorder = border == LEFT ? RIGHT : LEFT;
    }
    else {
      entryBorder = UPDOWN;
    }

    //Sequence counterclock-wise check
    //Works for righthand rule if there is border up and for lefthand rule if there is border down
    //Starts from checking border with value +1
    if((leftright==1 && upper_border(r, c)) || (leftright==2 && !upper_border(r, c))){
      for(int i = 1;i<=SIDES;i++){
        if(!isborder(map, r, c, ((entryBorder+i+SIDES)%SIDES))){
          border = (entryBorder+i+SIDES)%SIDES;
          break;
        }
      }
    }

    //Sequence clockwise check
    //Works for righthand rule if there is border down and for lefthand rule if there is border up
    //Starts from checking border with value -1
    else if((leftright==1 && !upper_border(r, c)) || (leftright==2 && upper_border(r, c))){
      for(int i = 1;i<=SIDES;i++){
        if(!isborder(map, r, c, ((entryBorder-i+SIDES)%SIDES))){
          border = (entryBorder-i+SIDES)%SIDES;
          break;
        }
      }
    }

    //Change the position
    change_pos(&r, &c, border);
  }
}

int main(int argc, char** argv){

  //HELP
  if(argc==2){
    char *helptext = ""
    "*********************|HELP|*********************\n"
    "You can use this program with arguments like:\n\n"
    "./maze \"--test\" \"file.txt\" \nThis launch will control, if maze is valid or not \n\n"
    "./maze \"--rpath\" \"R\" \"C\" \"file.txt\" \nThis launch will solve maze with right hand rule\n\n"
    "./maze \"--lpath\" \"R\" \"C\" \"file.txt\" \nThis launch will solve maze with left hand rule\n\n"
    "Where R is row and C is column of start point\n"
    "*********************|HELP|*********************\n";
    if((strcmp(argv[1], "--help") == 0)) {
      printf("%s", helptext);
    }
    else{
      fprintf(stderr, "Bad argument.\n");
      return -1;
    }
  }

  //TEST
  else if(argc==3){
    if((strcmp(argv[1], "--test") == 0)){
      if(is_valid(argv[2]) == TEST_VALID) printf("Valid\n");
      else printf("Invalid\n");
    }
    else{
      fprintf(stderr, "Bad argument.\n");
      return -1;
    }
  }

  //SOLVE MAZE
  else if(argc == 5){
    //Read the arguments
    //Start point
    int startRow = atoi(argv[2]);
    int startColumn = atoi(argv[3]);

    //Hand rule
    int handRule;
    if(strcmp(argv[1], "--rpath") == 0) handRule = 1;
    else if(strcmp(argv[1], "--lpath") == 0) handRule = 2;
    else{
      fprintf(stderr, "Bad arguments\n");
      return -1;
    }
    //Firstly checks file
    if(is_valid(argv[4]) == TEST_VALID){
      FILE *file = fopen(argv[4], "r");
      Map *map = Ctor(file);
      cells_filling(map, file);
      
      //Secondly checks start point
      if(correct_enter(map, startRow, startColumn)==TEST_VALID){
        int firstBorder = start_border(map, startRow, startColumn, handRule);
        if(firstBorder==-1) printf("%i,%i\n", startRow, startColumn); //DEAD END CHECK
        else solve_maze(map, startRow, startColumn, firstBorder, handRule);
        //File close and destruction
        fclose(file);
        Dtor(map);
      }
      //Start point isn't ok
      else{
        fprintf(stderr, "Invalid\n");
        fclose(file);
        Dtor(map);
        return -1;
      }
    }
    //File isn't ok
    else{
      fprintf(stderr, "Invalid\n");
      return -1;
    }
  }
  //Arguments aren't ok
  else{
    fprintf(stderr, "Bad arguments\n");
    return -1;
  }

  return 0;
}
