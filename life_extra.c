/*************************************************************
*  This program contains 2 advanced versions of LIFE         *
*  2. Immigration life (color war)                           *
*      This version of life begins with live cells split     *
*      between two colors randomly. The color of the next    *
*      cell is determined by the majority color of           *
*      its parents. Over time one color may dominate         *
*      -to run this version, input 0 at the start.           *
*      -this version is achieved using a 2D array of structs *
*      -functions for the advanced version appear            *
*       at the end of the file and all begin with "im_"      *
*  3. Advanced Color life (life cycle)                       *
*      This version shows the lifespan of the cell through   *
*      color.                                                *
*                 Birth = cyan (light blue)                  *
*                 Child = green                              *
*                 Adult = yellow                             *
*      -to run this version, input 1 at the start            *
*  NOTE please compile using -w to for nanosleep             *
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

enum cell_state {dead = 0, alive = 1, set_alive = 3, hold = 4};
enum color {red, yellow, blue, mild_blue, normal, cyan, 
            green, magenta};
typedef enum color color;
enum bool {false, true};
typedef enum bool bool; 
enum start_choice {immigration_life, adv_life}; 
typedef int cell;
typedef int state; 
typedef int choice;
struct _square {
   state cell_state;
   int cell_color;
};
typedef struct _square square;
struct _square_sums {
   int sum_cell;
   int sum_color;
};
typedef struct _square_sums square_sums; 
struct timespec {
   time_t tv_sec;
   long tv_nsec;
};
typedef struct timespec timespec;

/* IMMIGRATION LIFE FUNCS */
void immigration(void);
void im_init_board(square board[][COLUMNS]);
void im_random_fill(square board[][COLUMNS], choice version);
void im_print_board(square board[][COLUMNS], choice version);
square im_read_toroidal(square board[][COLUMNS], cell row, cell col);
int im_sum_state(square board[][COLUMNS], cell row, cell col);
int im_sum_color_red(square board[][COLUMNS], cell row, cell col);
int im_sum_color_yellow(square board[][COLUMNS], cell row, cell col);
void im_gen_next_board(square current[][COLUMNS], square next[][COLUMNS]);
int im_next_cell_color(int sum_yellow, int sum_red);
void im_switchpointers(square **p1, square **p2);
bool im_iscopy(square board1[][COLUMNS], square board2[][COLUMNS]);
int cnt_yellow(square board[][COLUMNS]);
int cnt_red(square board[][COLUMNS]);
/* COLOR LIFE FUNCS */
void advanced_life();
void im_next_colorstate_board(square current[][COLUMNS], square next[][COLUMNS]);
/* HELPER FUNCTIONS */
void print_intro(void); 
void set_color(int color_choice); 
void clear_console(void);
int console_width(void); 
void position_text(int offset);
choice get_choice(void); 

int main(void)
{
   choice start_state; 
   srand(time(NULL));
   print_intro();

   start_state = get_choice(); 

   if (start_state == immigration_life){
      immigration();
   } else {
      advanced_life();
   }

   return 0; 
}

/****************************************************/
/*     IMMIGRATION (Advanced version) FUNCTIONS     */
/****************************************************/
/* Divides the board's live cells into two colors   */
/* The color of each newly born cells is determined */ 
/* by the majority color of the parent cells.       */
/* Over time, one color may come to dominate.       */
/* The color and sum values for each are stored     */ 
/* in a structure called 'square'                   */
/****************************************************/
void immigration(void)
{
   int i = 0;
   choice version = immigration_life; 
   square boarda[ROWS][COLUMNS];
   square boardb[ROWS][COLUMNS];
   timespec tim, tim2;
   tim.tv_sec = 0;
   tim.tv_nsec = 250000000;
   
   im_init_board(boarda);
   im_init_board(boardb);
   im_random_fill(boarda, version);

   while (i++ < GENERATIONS){
      clear_console();
      im_print_board(boarda, version);
      nanosleep(&tim, &tim2);
      im_gen_next_board(boarda, boardb);

      clear_console();
      im_print_board(boardb, version);
      nanosleep(&tim, &tim2);
      im_gen_next_board(boardb, boarda);

      if (im_iscopy(boarda,boardb)){
         exit(0); 
      }
   }
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

void im_gen_next_board(square current[][COLUMNS], square next[][COLUMNS])
{
   /* Generates the next board based on the previous board */
   int r, c;
   int sum_state, sum_color_red, sum_color_yellow; 
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         sum_state = im_sum_state(current, r, c);
         sum_color_red = im_sum_color_red(current, r, c);
         sum_color_yellow = im_sum_color_yellow(current, r, c);
         /* set next cell state based on previous cell state */
         next[r][c].cell_state = next_cell_state(sum_state, 
                         current[r][c].cell_state);
         /* set next cell color to previous cell color */
         next[r][c].cell_color = current[r][c].cell_color;
         /* if cell dies set color to death color */
         if(next[r][c].cell_state == dead){   
            next[r][c].cell_color = mild_blue;
         } 
         /* if cell is born set color based on surrounding live cells */     
         if(current[r][c].cell_state == dead && next[r][c].cell_state == alive){
            next[r][c].cell_color = im_next_cell_color(sum_color_yellow, sum_color_red);
         }
      }
   }
}

int im_next_cell_color(int sum_yellow, int sum_red)
{
   /* The next cell color is determined by the majority color */
   if (sum_yellow > sum_red){
      return yellow;
   }
   return red; 
}

int im_sum_state(square board[][COLUMNS], cell row, cell col)
{
   /* counts number of live cells within a given cell's 8 neighbors */
   /*                   s1, s2, s3                                    */
   /*                   s4, x , s4,                                   */
   /*                   s5, s6, s7                                    */
   int r, c, sum = 0;
   square temp;  
   for (r = row-1; r <= row+1; r++){
      for (c = col-1; c <= col+1; c++){
         temp = im_read_toroidal(board, r, c);
         if (temp.cell_state == alive){
            sum++;
         }
      }
   }   
   return sum; 
}

int im_sum_color_yellow(square board[][COLUMNS], cell row, cell col)
{
   /* counts number of yellow cells within a given cell's 8 neighbors */
   int r, c, sum = 0;
   square temp;  
   for (r = row-1; r <= row+1; r++){
      for (c = col-1; c <= col+1; c++){
         temp = im_read_toroidal(board, r, c);
         if (temp.cell_color == yellow){
            sum++;
         }
      }
   }   
   return sum; 
}

int im_sum_color_red(square board[][COLUMNS], cell row, cell col)
{
   /* counts number of red cells within a given cell's 8 neighbors */
   int r, c, sum = 0;
   square temp;  
   for (r = row-1; r <= row+1; r++){
      for (c = col-1; c <= col+1; c++){
         temp = im_read_toroidal(board, r, c);
         if (temp.cell_color == red){
            sum++;
         }
      }
   }   
   return sum; 
}

square im_read_toroidal(square board[][COLUMNS], cell row, cell col)
{
  /* wraps around the board Left-Right, Top-Bottom */
  /* returns the correct struct square             */
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

void im_init_board(square board[][COLUMNS])
{
   int i, j;
   for (i = 0; i < ROWS; i++){
      for (j = 0; j < COLUMNS; j++){
         board[i][j].cell_state = dead;
         board[i][j].cell_color = mild_blue; 
      }
   }
}

void im_random_fill(square board[][COLUMNS], choice version)
{
   int r, c;
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         if (rand() % DENSITY == 0){
            board[r][c].cell_state = alive;
            if (version == immigration_life){ 
               if (rand() % COLOR_DENSITY == 0){
                  board[r][c].cell_color = red;
               } else {
                  board[r][c].cell_color = yellow;
               }
            } else {
               board[r][c].cell_color = cyan;
            }
         } 
      }
   }
}

void im_print_board(square board[][COLUMNS], choice version)
{
   int r, c;
   char cell_block = '#'; 
   int sum_red = 0, sum_yellow = 0;
   printf("\n");
   position_text(COLUMNS/2);
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         set_color(board[r][c].cell_color);
         printf("%c", cell_block);
         if (c == COLUMNS - 1){
            printf("\n");
            position_text(COLUMNS/2);           
         }
      }
   }
   if (version == immigration_life){
      sum_red = cnt_red(board);
      sum_yellow = cnt_yellow(board);
      set_color(red);    printf("RED %d ", sum_red);
      set_color(yellow); printf("YELLOW %d \n", sum_yellow);
      set_color(normal);
   }
}

int cnt_yellow(square board[][COLUMNS])
{
   /* counts total number of yellow squares on board */
   int r, c, sum = 0;
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         if (board[r][c].cell_color == yellow){
            sum++;
         }
      }
   }
   return sum; 
}

int cnt_red(square board[][COLUMNS])
{
   /* counts total number of red squares on board */
   int r, c, sum = 0;
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         if (board[r][c].cell_color == red){
            sum++;
         }
      }
   }
   return sum; 
}

void im_switchpointers(square **p1, square **p2)
{
   square *tmp;
   tmp = *p1;
   *p1 = *p2;
   *p2 = tmp;
}

bool im_iscopy(square board1[][COLUMNS], square board2[][COLUMNS])
{
   int r, c;
   for (r = 0; r < ROWS; r++){
      for(c = 0; c < COLUMNS; c++){
         if (board1[r][c].cell_state != board2[r][c].cell_state){
            return false;
         } 
      }
   }
   return true; 
}

/************************************************/
/*    ADVANCED COLOR-STATE LIFE FUNCTIONS       */
/************************************************/
/* This version of life uses color to show the  */
/* lifespan of the cells:                       */
/*             birth = cyan (light blue)        */
/*             child = green                    */
/*             adult = yellow                   */
/************************************************/
void advanced_life(void)
{
   int i = 0;
   choice version = adv_life; 
   square boarda[ROWS][COLUMNS];
   square boardb[ROWS][COLUMNS];
   timespec tim, tim2;
   tim.tv_sec = 0;
   tim.tv_nsec = 250000000;
   
   im_init_board(boarda);
   im_init_board(boardb);
   im_random_fill(boarda, adv_life);

   while (i++ < GENERATIONS){
      clear_console();
      im_print_board(boarda, adv_life);
      nanosleep(&tim, &tim2);
      im_next_colorstate_board(boarda, boardb);

      clear_console();
      im_print_board(boardb, adv_life);
      nanosleep(&tim, &tim2);
      im_next_colorstate_board(boardb, boarda);

      if (im_iscopy(boarda,boardb)){
         exit(0); 
      }
   }
}

void im_next_colorstate_board(square current[][COLUMNS], square next[][COLUMNS])
{
   /* Generates the next board based on the previous board */
   int r, c;
   int sum_state; 
   for (r = 0; r < ROWS; r++){
      for (c = 0; c < COLUMNS; c++){
         sum_state = im_sum_state(current, r, c);
         /* set next cell state based on previous cell state */
         next[r][c].cell_state = next_cell_state(sum_state, 
                         current[r][c].cell_state);
         /* if cell is born set color to cyan */     
         if(current[r][c].cell_state == dead && next[r][c].cell_state == alive){
            next[r][c].cell_color = cyan;
         }
         /* if continues living after birth, set to green */
         if(current[r][c].cell_color == cyan && next[r][c].cell_state == alive){
            next[r][c].cell_color = green;
         }
         /* if green and continues living, set color to yellow */
         if(current[r][c].cell_color == green && next[r][c].cell_state == alive){
            next[r][c].cell_color = yellow;
         }
         /* if yellow and continues living, keep same color */
         if(current[r][c].cell_color == yellow && next[r][c].cell_state == alive){
            next[r][c].cell_color = yellow;
         } 
         /* if cell dies set color to death color */
         if(next[r][c].cell_state == dead){   
            next[r][c].cell_color = mild_blue;
         } 
      }
   }
}

/*************************************************/
/*             HELPER FUNCTIONS                  */
/*************************************************/
void position_text(int offset)
{
   int i, con_w = console_width(); 
   for (i = 0; i < (con_w/2-offset); i++){
      printf(" ");
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
   case cyan:
      printf("\033[1;36m");
      break;
   case green:
      printf("\033[1;32m");
      break;
   case magenta:
      printf("\033[1;35m");
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

void print_intro(void)
{
   clear_console();
   printf("\n\n");
   position_text(COLUMNS/2);
   printf("            THE GAME OF");
   set_color(blue);
   printf(" LIFE\n");
   set_color(red);
   position_text(COLUMNS/2);
   printf("           cellular automaton\n\n");
}

choice get_choice(void)
{
   choice input;    

   set_color(normal);
   printf("\n");
   position_text(COLUMNS/2);
   printf("    For ");
   set_color(yellow);
   printf("IMMIGRATION LIFE");
   set_color(normal);
   printf("    enter ");
   set_color(yellow);
   printf("0: ");

   set_color(normal);
   printf("\n");
   position_text(COLUMNS/2);
   printf("    For ");
   set_color(cyan);
   printf("ADVANCED COLOR LIFE");
   set_color(normal);
   printf(" enter ");
   set_color(cyan);
   printf("1: ");

   while(!scanf("%d", &input)){
      printf("***ERROR: invalid input***");
   }   
   printf("\n");
   return input; 
}
