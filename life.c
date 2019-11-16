/************************************************************/
/*                   THE GAME OF LIFE                       */
/*************************************************************
*  This program contains:                                    *
*  1. life (regular rules - using adv toroidal wrapping)     *
*      a.) option to fill the board randomly                 *
*      b.) option to start from a known configuration        *
*          such as the glider gun                            *
*  2. Advanced life features are uploaded as life_extra.c    *
*     and implement Immigration Life and Color Cycle Life    *
**************************************************************
*  NOTE please compile using -w for nanosleep                *
*************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/ioctl.h>

#define ROWS 60
#define COLUMNS 80
#define DENSITY 5
#define COLOR_DENSITY 2
#define GENERATIONS 500
#define QUARTER 4
#define HALF 2

enum cell_state {dead = 0, alive = 1, set_alive = 3, hold = 4};
enum color {red, yellow, blue, mild_blue, normal};
typedef enum color color;
enum bool {false, true};
typedef enum bool bool; 
enum start_choice {known_start, random_start};
enum known_type {glider, small_explosion, explosion, ten_cell, 
                 light_spaceship, glider_gun};
typedef enum known_type known_type; 
typedef int cell;
typedef int state; 
typedef int choice;
struct timespec {
   time_t tv_sec;
   long tv_nsec;
};
typedef struct timespec timespec;

/* LIFE PRIMARY FUNCTIONS */
void life(choice start_state);
int sum_cells(cell board[][COLUMNS], cell row, cell col);
state next_cell_state(int sum, state current_state);
cell read(cell board[][COLUMNS], cell row, cell col); 
cell read_toroidal(cell board[][COLUMNS], cell row, cell col);
void gen_next_board(cell current_board[][COLUMNS], 
                   cell next_board[][COLUMNS]);
/* LIFE HELPER FUNCS */
bool iscopy(cell board1[][COLUMNS], cell board2[][COLUMNS]);
void print_board(cell board[][COLUMNS]);
void random_fill(cell board[][COLUMNS]);
void known_fill(cell board[][COLUMNS]); 
void set_known_board(cell board[][COLUMNS], int config);
void print_intro(void); 
void set_color(int color_choice); 
void clear_console(void);
int console_width(void);
int position_r(cell row);
int position_c(cell col); 
void position_text(int offset);
choice get_choice(void); 
int count_live_cells(cell board[][COLUMNS]);
/* KNOWN CONFIGURATION SETUP FUNCTIONS */
void set_glider(cell board[][COLUMNS]);
void set_small_explosion(cell board[][COLUMNS]);
void set_explosion(cell board[][COLUMNS]);
void set_ten_cell(cell board[][COLUMNS]);
void set_light_spaceship(cell board[][COLUMNS]);
void set_glider_gun(cell board[][COLUMNS]);
void block(cell board[][COLUMNS], cell row, cell col);
void eye(cell board[][COLUMNS], cell row, cell col);
void hook(cell board[][COLUMNS], cell row, cell col);
void cannon(cell board[][COLUMNS], cell row, cell col);

int main(void)
{
   choice start_state; 
   srand(time(NULL));
   print_intro();

   start_state = get_choice(); 
   life(start_state);
   return 0; 
}

/*****************************************/
/*     REGULAR VERSION LIFE FUNCTIONS    */
/*****************************************/
/* This version of life randomly fills   */
/* the board and follows standard rules. */
/* It uses toroidal wrapping. There is   */
/* an option to fill the board with a    */
/* known configuration (ie glider gun)   */
/*****************************************/
void life (choice start_state)
{
   int i = 0;
   cell boarda[ROWS][COLUMNS] = {0};
   cell boardb[ROWS][COLUMNS] = {0};
   timespec tim, tim2;
   tim.tv_sec = 0;
   tim.tv_nsec = 250000000;

   if (start_state == random_start){
      random_fill(boarda); 
   } else {
      known_fill(boarda); 
   }
   while (i++ < GENERATIONS){
      clear_console();
      print_board(boarda); 
      nanosleep(&tim, &tim2);
 
      gen_next_board(boarda, boardb);
      clear_console(); 
      print_board(boardb);
      nanosleep(&tim, &tim2);

      gen_next_board(boardb, boarda); 
      if(iscopy(boarda, boardb)){
         exit(0);
      } 
  }
}

bool iscopy(cell board1[][COLUMNS], cell board2[][COLUMNS])
{
   cell r, c;
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         if (board1[r][c] != board2[r][c]){
            return false; 
         }
      }
   }
   return true; 
}

void random_fill(cell board[][COLUMNS])
{
   int r, c;
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         if (rand() % DENSITY == 0){
            board[r][c] = alive; 
         }
      }
   }
}

cell read(cell board[][COLUMNS], cell row, cell col)
{
   /* returns zeros for anything off the board */
   /* toroidal version below is used instead   */
   if (row < 0 || row > ROWS-1){
      return dead;  
   }
   if (col < 0 || col > COLUMNS-1){
      return dead; 
   } 
   return board[row][col];   
}

cell read_toroidal(cell board[][COLUMNS], cell row, cell col)
{
  /* wraps around the board Left-Right, Top-Bottom */
  int r = row % ROWS;
  int c = col % COLUMNS;
  if (row < 0){
     r = ROWS - 1;  
  }
  if (col < 0){
     c = COLUMNS - 1; 
  }
  return board[r][c];
} 

int sum_cells(cell board[][COLUMNS], cell row, cell col)
{
   /* counts the total number of live cells */
   int r, c, sum = 0; 
   for (r = row-1; r <= row+1; r++){
      for (c = col-1; c <= col+1; c++){
         if (read_toroidal(board, r, c)== alive){
            sum++;
         }
      }
   }   
   return sum; 
}

state next_cell_state(int sum, state current_state)
{
   if (sum == set_alive) {
      return alive;
   }
   if (sum == hold) {
      return current_state;
   }
   return dead; 
}

void gen_next_board(cell current_board[][COLUMNS], 
                   cell next_board[][COLUMNS])
{
   int r, c, sum = 0; 
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         sum = sum_cells(current_board, r, c);
         next_board[r][c] = next_cell_state(sum, current_board[r][c]); 
      }
   }
}

void print_board(cell board[][COLUMNS])
{
   int r, c;
   static int cnt = 0; 
   char cell_block = '#'; 
   printf("\n");
   position_text(COLUMNS/2);
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         if (board[r][c] == alive){
            set_color(yellow);
         } else {
            set_color(mild_blue);
         }
         printf("%c", cell_block);
         if (c == COLUMNS - 1){
            printf("\n");
            position_text(COLUMNS/2);
         }
      }
   }
   set_color(normal);
   printf("\nLIVE CELLS: %d\n", count_live_cells(board)); 
   printf("GENERATION: %d\n", cnt++);
}

void position_text(int offset)
{
   int i, con_w = console_width(); 
   for (i = 0; i < (con_w/2-offset); i++){
      printf(" ");
   }
}

void known_fill(cell board[][COLUMNS])
{
   choice config; 

   set_color(normal); 
   printf("\n");
   position_text(COLUMNS/2);
   printf("    Enter number for "); 
   set_color(red);
   printf("KNOWN"); 
   set_color(normal);
   printf(" configuration: \n"); 
   set_color(blue);
   position_text(COLUMNS/2);
   printf("        GLIDER ----------------- 0: \n");
   set_color(red);
   position_text(COLUMNS/2);
   printf("        SMALL EXPLODER --------- 1: \n");
   set_color(blue);
   position_text(COLUMNS/2);
   printf("        EXPLODER --------------- 2: \n");
   set_color(red);
   position_text(COLUMNS/2);
   printf("        TEN CELL ROW ----------- 3: \n");
   set_color(blue);
   position_text(COLUMNS/2);
   printf("        LIGHTWEIGHT SPACESHIP -- 4: \n");
   set_color(yellow);
   position_text(COLUMNS/2);
   printf("        GOSPER GLIDER GUN ------ 5: \n");
   while(!scanf("%d", &config)){
      printf("***ERROR: invalid input***");
   } 
   set_known_board(board, config); 
}

void set_known_board(cell board[][COLUMNS], int config)
{
   /* sets the board into a known configuration */
   switch(config){
   case glider:
      set_glider(board); 
      break;
   case small_explosion:
      set_small_explosion(board);
      break;
   case explosion:
      set_explosion(board);
      break;
   case ten_cell:
      set_ten_cell(board);
      break;
   case light_spaceship:
      set_light_spaceship(board);
      break;
   case glider_gun:
      set_glider_gun(board);
      break;
   default:
      printf("\nNO SELECTION: RANDOM BOARD\n");
      random_fill(board); 
   }   
}

void set_color(int color_choice)
{
   switch(color_choice){
   case blue:
      printf("\033[1;34m");
      break;
   case mild_blue:
      printf("\033[0;34m");
      break;
   case red:
      printf("\033[1;31m");
      break;
   case yellow:
      printf("\033[1;33m");
      break;
   default:
      printf("\033[0m");
   }
}

void clear_console(void)
{
   if (!system("clear")){
      printf(" ");
   } 
}

int console_width(void)
{
   struct winsize w;
   ioctl(0, TIOCGWINSZ, &w);
   return w.ws_col;
}

int position_r(cell row)
{
   return row + ROWS/HALF;
}

int position_c(cell col)
{
   return col + COLUMNS/QUARTER;
}

void print_intro(void)
{
   clear_console();
   printf("\n\n");
   position_text(COLUMNS/2);
   printf("             THE GAME OF");
   set_color(blue);
   printf(" LIFE\n");
   set_color(red);
   position_text(COLUMNS/2);
   printf("            cellular automaton\n\n");
}

choice get_choice(void)
{
   choice input;    

   set_color(normal);
   printf("\n");
   position_text(COLUMNS/2);
   printf("    For ");
   set_color(red);
   printf("KNOWN");
   set_color(normal);
   printf(" start         enter ");
   set_color(red);
   printf("0: ");

   set_color(normal);
   printf("\n");
   position_text(COLUMNS/2);
   printf("    For "); 
   set_color(blue); 
   printf("RANDOM"); 
   set_color(normal);
   printf(" start        enter");
   set_color(blue);
   printf(" 1: "); 

   while(!scanf("%d", &input)){
      printf("***ERROR: invalid input***");
   }   
   printf("\n");
   return input; 
}

int count_live_cells(cell board[][COLUMNS])
{
   /* counts total live cells on the board */
   int cnt = 0, r, c; 
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         if (board[r][c] == 1){
            cnt++;
         } 
      }
   }
   return cnt; 
}

/*************************************************/
/*      KNOWN CONFIGURATION SETUP FUNCTIONS      */
/*************************************************/
void set_glider(cell board[][COLUMNS])
{
   board[position_r(0)][position_c(1)] = 1;
   board[position_r(1)][position_c(2)] = 1;
   board[position_r(2)][position_c(0)] = 1;
   board[position_r(2)][position_c(1)] = 1;
   board[position_r(2)][position_c(2)] = 1;
}

void set_small_explosion(cell board[][COLUMNS])
{
   board[position_r(0)][position_c(1)] = 1; 
   board[position_r(1)][position_c(1)] = 1;
   board[position_r(1)][position_c(0)] = 1;
   board[position_r(1)][position_c(2)] = 1;
   board[position_r(2)][position_c(2)] = 1;
   board[position_r(2)][position_c(0)] = 1;
   board[position_r(3)][position_c(1)] = 1;
}

void set_explosion(cell board[][COLUMNS])
{
   int i;
   for (i = 0; i<= 4; i++){
      board[position_r(i)][position_c(0)] = 1;
      board[position_r(i)][position_c(4)] = 1;
   }
   board[position_r(0)][position_c(2)] = 1;
   board[position_r(4)][position_c(2)] = 1;
}

void set_ten_cell(cell board[][COLUMNS])
{
   int i;
   for (i = 0; i < 10; i++){
      board[position_r(0)][position_c(i)] = 1; 
   }
}

void set_light_spaceship(cell board[][COLUMNS])
{
   int i;
   for (i = 1; i < 5; i++){
      board[position_r(0)][position_c(i)] = 1;
   }
   board[position_r(1)][position_c(4)] = 1;
   board[position_r(2)][position_c(4)] = 1;
   board[position_r(1)][position_c(0)] = 1;
   board[position_r(3)][position_c(0)] = 1;
   board[position_r(3)][position_c(3)] = 1;
}

void set_glider_gun(cell board[][COLUMNS])
{
  block(board, position_r(2), position_c(0));
  block(board, position_r(0), position_c(34)); 
  eye(board, position_r(2), position_c(8));
  eye(board, position_r(0), position_c(22)); 
  hook(board, position_r(4), position_c(16)); 
  hook(board, position_r(7), position_c(35));
  cannon(board, position_r(12), position_c(24));
}

void block(cell board[][COLUMNS], cell row, cell col)
{
   /*forms block from starting row moving down and right*/
   board[row][col] = 1;
   board[row][col+1] = 1;
   board[row+1][col] = 1;
   board[row+1][col+1] = 1;
}

void eye(cell board[][COLUMNS], cell row, cell col)
{
   /*forms eye shape starting from top left*/
   board[row+1][col] = 1;
   board[row+2][col] = 1;
   board[row][col+1] = 1;
   board[row+2][col+1] = 1;
   board[row][col+2] = 1;
   board[row+1][col+2] = 1; 
}

void hook(cell board[][COLUMNS], cell row, cell col)
{
   /*hook shape starting from top left*/
   int i;
   for (i = 0; i < 3; i++){
      board[i + row][col] = 1;
   }
   board[row][col+1] = 1;
   board[row+1][col+2] = 1; 
}

void cannon(cell board[][COLUMNS], cell row, cell col)
{
   /*cannon shape starting from top left*/
   int i;
   for (i = 0; i < 3; i++){
      board[row][i+col] = 1;
   }
   board[row+1][col] = 1;
   board[row+2][col+1] = 1;
}
