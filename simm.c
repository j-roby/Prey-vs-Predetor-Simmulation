#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define MAXWOLFHUNGER 50

#define MAXWOLFMATE 50
#define MAXSHEEPHUNGER 50
#define MAXSHEEPMATE 50
typedef struct {
    int hunger;
    int reproduction;
    bool rest;
} Predetor;

typedef struct {
    int hunger;
    int reproduction;
    bool rest;
} Prey;

typedef struct {
    bool grass;
    int timer;
    Predetor *wolf[2];
    Prey *sheep[2];    

} Field;

/*typedef struct {
    
     

} Container;
*/
typedef enum{
    EAT,
    MATE,
    REST, 
    RANDOM
} Tasks;

typedef struct {
    Field** field;
    int time;
    int wolves;
    int sheeps;
    int size;
} Simmulation;

void print_field(Field **newWorld, int size) {
    printf("|");
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            
            printf("%c",(newWorld[i][j].wolf[0] != NULL)? 'w':' ');
            printf("%c|",(newWorld[i][j].wolf[1] != NULL)? 'w':' ');
 

        }
        printf("\n|");
        for (int j = 0; j < size; j++) {
            
            printf("%c",(newWorld[i][j].sheep[0] != NULL)? 's':' ');
            printf("%c|",(newWorld[i][j].sheep[1] != NULL)? 's':' ');
        }
        printf("\n|");
        for (int j = 0; j < size*3; j++) {
            printf("_");
        }
        printf("\n|");
    }
    printf("\n\n");
}


void print_world(Simmulation *newWorld) {
    printf("|");
    for (int i = 0; i < newWorld->size; i++) {
        for (int j = 0; j < newWorld->size; j++) {
            
            printf("%c",(newWorld->field[i][j].wolf[0] != NULL)? 'w':' ');
            printf("%c|",(newWorld->field[i][j].wolf[1] != NULL)? 'w':' ');
 

        }
        printf("\n|");
        for (int j = 0; j < newWorld->size; j++) {
            
            printf("%c",(newWorld->field[i][j].sheep[0] != NULL)? 's':' ');
            printf("%c|",(newWorld->field[i][j].sheep[1] != NULL)? 's':' ');
        }
        printf("\n|");
        for (int j = 0; j < newWorld->size*3; j++) {
            printf("_");
        }
        printf("\n|");
    }
    printf("\n\n");
}

int setup_world(Simmulation *newWorld, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            //newWorld->field[i][j].wolf[0] = malloc(sizeof(Predetor));
            //newWorld->field[i][j].wolf[1] = malloc(sizeof(Predetor));
            //newWorld->field[i][j].sheep[0] = malloc(sizeof(Prey));
            //newWorld->field[i][j].sheep[1] = malloc(sizeof(Prey));
            newWorld->field[i][j].wolf[0] = NULL;
            newWorld->field[i][j].wolf[1] = NULL;
            newWorld->field[i][j].sheep[0] = NULL;
            newWorld->field[i][j].sheep[1] = NULL;
            newWorld->field[i][j].grass = true;
            newWorld->field[i][j].timer = 0;

        }
    }
    return 0;
}
void setup_field(Field **newWorld, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            //newWorld[i][j].wolf[0] = malloc(sizeof(Predetor));
            //newWorld[i][j].wolf[1] = malloc(sizeof(Predetor));
            //newWorld[i][j].sheep[0] = malloc(sizeof(Prey));
            //newWorld[i][j].sheep[1] = malloc(sizeof(Prey));
            newWorld[i][j].wolf[0] = NULL;
            newWorld[i][j].wolf[1] = NULL;
            newWorld[i][j].sheep[0] = NULL;
            newWorld[i][j].sheep[1] = NULL;
            newWorld[i][j].grass = true;
            newWorld[i][j].timer = 0;

        }
    }
}


Tasks find_task(Predetor* wolf) {


    return MATE;

}

int is_wolf_active(Field newField) {
    if(newField.wolf[0] != NULL) {  
        return 0;
    } else if (newField.wolf[1] != NULL) {
        return 1;
    }
    return -1;

} 

int is_sheep_active(Field newField) {
    if(newField.sheep[0] != NULL) {  
        return 0;
    } else if (newField.sheep[1] != NULL) {
        return 1;
    }
    return -1;

} 


int is_wolf_empty(Field newField) {
    if(newField.wolf[0] == NULL) {  
        return 0;
    } else if (newField.wolf[1] == NULL) {
        return 1;
    }
    return -1;

} 
void wolf_eat(Simmulation *newWorld, Field **nextField, int i, int j, int wolfPos) {
    int min =newWorld->size*newWorld->size;
    int moveX= min;
    int moveY = min;
    int newI = i;
    int newJ = j;

    
    int distance[newWorld->size][newWorld->size];
    for (int x = 0; x < newWorld->size; x++) {
        for (int y = 0; y< newWorld->size; y++) {
            if (newWorld->field[x][y].sheep[0] != NULL ||
                newWorld->field[x][y].sheep[1] != NULL) {
                distance[x][y] = abs(i-x)+abs(j-y);
                if (distance[x][y] == 0) {
                    min =0;
                    moveX = x;
                    moveY = y;
                }else if (distance[x][y] < min) {
                    min = distance[x][y];
                    moveX = x;
                    moveY = y;
                }                    
                    //move coordinates
                    //if (new next Field has room)
            } else {
                distance[x][y] = newWorld->size * newWorld->size;    
            }
        }  
    }
    if (min == newWorld->size *newWorld->size)
        return;
        if (min == 0) {
            if (nextField[moveX][moveY].sheep[0] != NULL) {
                nextField[moveX][moveY].wolf[wolfPos]->hunger = MAXWOLFHUNGER;
                nextField[moveX][moveY].sheep[0] = NULL;
                newWorld->field[moveX][moveY].sheep[0] = NULL; 
                newWorld->sheeps = newWorld->sheeps-1;
                    
            }else if (nextField[moveX][moveY].sheep[1] != NULL) {
                nextField[moveX][moveY].wolf[wolfPos]->hunger = MAXWOLFHUNGER;
                nextField[moveX][moveY].sheep[1] = NULL;
                newWorld->field[moveX][moveY].sheep[1] = NULL; 
                newWorld->sheeps = newWorld->sheeps-1;
                 
            }
            return;
        }
        int pos;
        if ((moveX - i) > 0) {
            newI = i+1;
            if ( (pos = is_wolf_empty((nextField[newI][newJ]))) != -1) {
                nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predetor));
                    
                nextField[newI][newJ].wolf[pos]->hunger = 
                    nextField[i][j].wolf[wolfPos]->hunger;
                    
                nextField[newI][newJ].wolf[pos]->reproduction = 
                    nextField[i][j].wolf[wolfPos]->reproduction;
                nextField[newI][newJ].wolf[pos]->rest = 
                    nextField[i][j].wolf[wolfPos]->rest;
                //free(nextField[i][j].wolf[wolfPos]);
                //free(newWorld->field[i][j].wolf[wolfPos]);
                nextField[i][j].wolf[wolfPos] = NULL;
                newWorld->field[i][j].wolf[wolfPos] = NULL;
                return;

            }
        } else if((moveX-i) < 0) {
            newI = i-1;
            if ( (pos = is_wolf_empty((nextField[newI][newJ]))) != -1) {
                nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predetor));
                    
                nextField[newI][newJ].wolf[pos]->hunger = 
                    nextField[i][j].wolf[wolfPos]->hunger;
                    
                nextField[newI][newJ].wolf[pos]->reproduction = 
                    nextField[i][j].wolf[wolfPos]->reproduction;
                nextField[newI][newJ].wolf[pos]->rest = 
                    nextField[i][j].wolf[wolfPos]->rest;
                //free(nextField[i][j].wolf[wolfPos]);
                //free(newWorld->field[i][j].wolf[wolfPos]);
                nextField[i][j].wolf[wolfPos] = NULL;
                newWorld->field[i][j].wolf[wolfPos] = NULL;
                return;

            }
        }
        newI = i;
        if ((moveY-j) > 0) {
            newJ = j +1;
            if ((pos = is_wolf_empty(nextField[newI][newJ])) != -1 ) {
                nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predetor));
                nextField[newI][newJ].wolf[pos]->hunger = 
                    nextField[i][j].wolf[wolfPos]->hunger;
                nextField[newI][newJ].wolf[pos]->reproduction = 
                    nextField[i][j].wolf[wolfPos]->reproduction;
                nextField[newI][newJ].wolf[pos]->rest = 
                    nextField[i][j].wolf[wolfPos]->rest;
                //free(nextField[i][j].wolf[wolfPos]);
                //free(newWorld->field[i][j].wolf[wolfPos]);
                nextField[i][j].wolf[wolfPos] = NULL;
                newWorld->field[i][j].wolf[wolfPos] = NULL;      
                return;
            }
        }else if ((moveY-j) < 0) {
            newJ = j-1;
             if ((pos = is_wolf_empty(nextField[newI][newJ])) != -1 ) {
                nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predetor));
                nextField[newI][newJ].wolf[pos]->hunger = 
                    nextField[i][j].wolf[wolfPos]->hunger;
                nextField[newI][newJ].wolf[pos]->reproduction = 
                    nextField[i][j].wolf[wolfPos]->reproduction;
                nextField[newI][newJ].wolf[pos]->rest = 
                    nextField[i][j].wolf[wolfPos]->rest;
                //free(nextField[i][j].wolf[wolfPos]);
                //free(newWorld->field[i][j].wolf[wolfPos]);
                nextField[i][j].wolf[wolfPos] = NULL;
                newWorld->field[i][j].wolf[wolfPos] = NULL;
               return;
            }
        }
    return;
}

void wolf_mate(Simmulation *newWorld,Field **nextField,int i,int j, int wolfPos) {
    int min =newWorld->size*newWorld->size;
    int moveX= min;
    int moveY = min;
    int newI = i;
    int newJ = j;
    int matePos;
    if (nextField[i][j].wolf[0] != NULL
        && nextField[i][j].wolf[1] != NULL) {
        printf("%d %d \n", i, j);
        if (i+1 < newWorld->size) {
            if ((matePos = is_wolf_empty(nextField[i+1][j])) != -1) {
                printf("here %d\n", matePos);
                Predetor *wolf = malloc(sizeof(Predetor));
                nextField[i][j].wolf[0]->hunger = nextField[i][j].wolf[0]->hunger /2;
                nextField[i][j].wolf[1]->hunger = nextField[i][j].wolf[1]->hunger /2;
                nextField[i][j].wolf[0]->rest = true;
                nextField[i][j].wolf[1]->rest = true;
                wolf->hunger = MAXWOLFHUNGER; 
                wolf->reproduction = MAXWOLFMATE; 
                wolf->rest = false; 
                nextField[i+1][j].wolf[matePos] = wolf;
                return;
            }
        }
        if (i-1 >= 0) {
            if ((matePos = is_wolf_empty(nextField[i-1][j])) != -1) {
                printf("here %d\n", matePos);
                Predetor *wolf = malloc(sizeof(Predetor));
                nextField[i][j].wolf[0]->hunger = nextField[i][j].wolf[0]->hunger /2;
                nextField[i][j].wolf[1]->hunger = nextField[i][j].wolf[1]->hunger /2;
                nextField[i][j].wolf[0]->rest = true;
                nextField[i][j].wolf[1]->rest = true;
                wolf->hunger = MAXWOLFHUNGER; 
                wolf->reproduction = MAXWOLFMATE; 
                wolf->rest = false; 
                nextField[i-1][j].wolf[matePos] = wolf;

                return;
            }
        }
        if (j+1 < newWorld->size) {
            if ((matePos = is_wolf_empty(nextField[i][j+1])) != -1) {
                printf("here %d\n", matePos);
                Predetor *wolf = malloc(sizeof(Predetor));
                nextField[i][j].wolf[0]->hunger = nextField[i][j].wolf[0]->hunger /2;
                nextField[i][j].wolf[1]->hunger = nextField[i][j].wolf[1]->hunger /2;
                nextField[i][j].wolf[0]->rest = true;
                nextField[i][j].wolf[1]->rest = true;
                wolf->hunger = MAXWOLFHUNGER; 
                wolf->reproduction = MAXWOLFMATE; 
                wolf->rest = false; 
                nextField[i][j+1].wolf[matePos] = wolf;
                
                return;
            }
        }
        if (j-1 >= 0) {
            if ((matePos = is_wolf_empty(nextField[i][j-1])) != -1) {
                printf("here %d\n", matePos);
                Predetor *wolf = malloc(sizeof(Predetor));
                nextField[i][j].wolf[0]->hunger = nextField[i][j].wolf[0]->hunger /2;
                nextField[i][j].wolf[1]->hunger = nextField[i][j].wolf[1]->hunger /2;
                nextField[i][j].wolf[0]->rest = true;
                nextField[i][j].wolf[1]->rest = true;
                wolf->hunger = MAXWOLFHUNGER; 
                wolf->reproduction = MAXWOLFMATE; 
                wolf->rest = false; 
                nextField[i][j-1].wolf[matePos] = wolf;
                return;

            }
        }

        return;
        
    }
    
    int distance[newWorld->size][newWorld->size];
    for (int x = 0; x < newWorld->size; x++) {
        for (int y = 0; y< newWorld->size; y++) {
            if((x == i) && (y == j) ) {
                distance[x][y] = newWorld->size * newWorld->size;

            } else if (newWorld->field[x][y].wolf[0] != NULL ||
                newWorld->field[x][y].wolf[1] != NULL) {
                distance[x][y] = abs(i-x)+abs(j-y);
                if (distance[x][y] == 0) {
                    min =0;
                    moveX = x;
                    moveY = y;
                }else if (distance[x][y] < min) {
                    min = distance[x][y];
                    moveX = x;
                    moveY = y;
                }                    
                    //move coordinates
                    //if (new next Field has room)
            } else {
                distance[x][y] = newWorld->size * newWorld->size;    
            }
        }  
    }
    if (min == newWorld->size *newWorld->size)
        return;
    int pos;
    if ((moveX - i) > 0) {
        newI = i+1;
        if ( (pos = is_wolf_empty((nextField[newI][newJ]))) != -1) {
            nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predetor));
                    
            nextField[newI][newJ].wolf[pos]->hunger = 
                nextField[i][j].wolf[wolfPos]->hunger;
                    
            nextField[newI][newJ].wolf[pos]->reproduction = 
                nextField[i][j].wolf[wolfPos]->reproduction;
            nextField[newI][newJ].wolf[pos]->rest = 
                nextField[i][j].wolf[wolfPos]->rest;
                //free(nextField[i][j].wolf[wolfPos]);
                //free(newWorld->field[i][j].wolf[wolfPos]);
            nextField[i][j].wolf[wolfPos] = NULL;
            newWorld->field[i][j].wolf[wolfPos] = NULL;
            return;

        }
    } else if((moveX-i) < 0) {
        newI = i-1;
        if ( (pos = is_wolf_empty((nextField[newI][newJ]))) != -1) {
            nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predetor));
                    
            nextField[newI][newJ].wolf[pos]->hunger = 
                nextField[i][j].wolf[wolfPos]->hunger;
                    
            nextField[newI][newJ].wolf[pos]->reproduction = 
                nextField[i][j].wolf[wolfPos]->reproduction;
            nextField[newI][newJ].wolf[pos]->rest = 
                nextField[i][j].wolf[wolfPos]->rest;
                //free(nextField[i][j].wolf[wolfPos]);
                //free(newWorld->field[i][j].wolf[wolfPos]);
            nextField[i][j].wolf[wolfPos] = NULL;
            newWorld->field[i][j].wolf[wolfPos] = NULL;
            return;

        }
    }
    newI = i;
    if ((moveY-j) > 0) {
        newJ = j +1;
        if ((pos = is_wolf_empty(nextField[newI][newJ])) != -1 ) {
            nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predetor));
            nextField[newI][newJ].wolf[pos]->hunger = 
                nextField[i][j].wolf[wolfPos]->hunger;
            nextField[newI][newJ].wolf[pos]->reproduction = 
                nextField[i][j].wolf[wolfPos]->reproduction;
            nextField[newI][newJ].wolf[pos]->rest = 
                nextField[i][j].wolf[wolfPos]->rest;
                //free(nextField[i][j].wolf[wolfPos]);
                //free(newWorld->field[i][j].wolf[wolfPos]);
            nextField[i][j].wolf[wolfPos] = NULL;
            newWorld->field[i][j].wolf[wolfPos] = NULL;      
            return;
        }
    } else if ((moveY-j) < 0) {
        newJ = j-1;
        if ((pos = is_wolf_empty(nextField[newI][newJ])) != -1 ) {
        nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predetor));
            nextField[newI][newJ].wolf[pos]->hunger = 
                nextField[i][j].wolf[wolfPos]->hunger;
            nextField[newI][newJ].wolf[pos]->reproduction = 
                nextField[i][j].wolf[wolfPos]->reproduction;
            nextField[newI][newJ].wolf[pos]->rest = 
                nextField[i][j].wolf[wolfPos]->rest;
                //free(nextField[i][j].wolf[wolfPos]);
                //free(newWorld->field[i][j].wolf[wolfPos]);
            nextField[i][j].wolf[wolfPos] = NULL;
            newWorld->field[i][j].wolf[wolfPos] = NULL;
            return;
        }
    }
    return;
}

void play_field(Simmulation *newWorld,Field **nextField, int i, int j) {
   
    int task;
    int wolfPos; 
    if ((wolfPos = is_wolf_active(newWorld->field[i][j])) != -1) {
        task = find_task(newWorld->field[i][j].wolf[wolfPos]);
        if (task == EAT) {
            wolf_eat(newWorld, nextField, i, j, wolfPos);
            
        } else if (task == MATE) {

            wolf_mate(newWorld, nextField, i, j, wolfPos);

        } else if (task == REST) {

        } else if (task == RANDOM) {


        }
        

    } 
    
  
}

int run_simmulation(Simmulation *newWorld) {
    /* memory problems could arise make sure to free both worlds before saving */
    Field** nextField;
    nextField = malloc(sizeof(Field*) * newWorld->size);
    for (int i = 0; i < newWorld->size; i++) {
        nextField[i] = malloc(sizeof(Field) * newWorld->size);    
    }
    int error = setup_world(newWorld, newWorld->size);
    setup_field(nextField, newWorld->size);
  
    Prey sheep1;
    sheep1.hunger = 12;
    newWorld->field[2][2].sheep[0]= &sheep1;
    Prey sheep2;
    sheep2.hunger = 12;
    newWorld->field[3][2].sheep[0]= &sheep2;
    Predetor wolf;
    wolf.hunger = 12;
    newWorld->field[0][0].wolf[0]= &wolf; 
    Predetor wolf2;
    wolf2.hunger = 12;
    newWorld->field[4][4].wolf[1]= &wolf2;
    newWorld->sheeps = 2;
    for (int i = 0; i < newWorld->size; i++) {
        for (int j = 0; j < newWorld->size; j++) {
            if (newWorld->field[i][j].wolf[0] != NULL) {
                Predetor* wolf = malloc(sizeof(Predetor));
                nextField[i][j].wolf[0] = wolf;
                nextField[i][j].wolf[0]->hunger = newWorld->field[i][j].wolf[0]->hunger;
                nextField[i][j].wolf[0]->reproduction = newWorld->field[i][j].wolf[0]->reproduction;
                nextField[i][j].wolf[0]->rest = newWorld->field[i][j].wolf[0]->rest;  
            }
            
           if (newWorld->field[i][j].sheep[0] != NULL) {
                Prey* sheep = malloc(sizeof(Prey));
                nextField[i][j].sheep[0] = sheep;
                nextField[i][j].sheep[0]->hunger = newWorld->field[i][j].sheep[0]->hunger;
                nextField[i][j].sheep[0]->reproduction = newWorld->field[i][j].sheep[0]->reproduction;
                nextField[i][j].sheep[0]->rest = newWorld->field[i][j].sheep[0]->rest;  
            }
            if (newWorld->field[i][j].wolf[1] != NULL) {
                Predetor* wolf = malloc(sizeof(Predetor));
                nextField[i][j].wolf[1] = wolf;
                nextField[i][j].wolf[1]->hunger = newWorld->field[i][j].wolf[1]->hunger;
                nextField[i][j].wolf[1]->reproduction = newWorld->field[i][j].wolf[1]->reproduction;
                nextField[i][j].wolf[1]->rest = newWorld->field[i][j].wolf[1]->rest;  
            }
            if (newWorld->field[i][j].sheep[1] != NULL) {
                Prey* sheep = malloc(sizeof(Prey));
                nextField[i][j].sheep[1] = sheep;
                nextField[i][j].sheep[1]->hunger = newWorld->field[i][j].sheep[1]->hunger;
                nextField[i][j].sheep[1]->reproduction = newWorld->field[i][j].sheep[1]->reproduction;
                nextField[i][j].sheep[1]->rest = newWorld->field[i][j].sheep[1]->rest;  
            } 
            nextField[i][j].grass = newWorld->field[i][j].grass;
            nextField[i][j].timer = newWorld->field[i][j].timer;
            

        }
    }

    nextField[0][0].wolf[0]->hunger = 15;
    printf("new %d old %d \n", newWorld->field[0][0].wolf[0]->hunger, nextField[0][0].wolf[0]->hunger);
    int wolfPos;
    int sheepPos;
    int count = 0;
    print_world(newWorld);
    
    while(1) {
        newWorld->time++;
        printf("i\n");
        for (int i = 0; i < newWorld->size; i++) {
            for (int j = 0; j < newWorld->size; j++) {
                if (newWorld->field[i][j].wolf[0] != NULL ||
                       newWorld->field[i][j].wolf[1] != NULL  ) {
                    play_field(newWorld, nextField, i, j);
                }
            }
        }
        for (int i = 0; i < newWorld->size; i++) {
            for (int j = 0; j < newWorld->size; j++) {
                if ((wolfPos = is_wolf_active(nextField[i][j])) != -1 ) {
                    newWorld->field[i][j].wolf[wolfPos] = malloc(sizeof(Predetor));
                    newWorld->field[i][j].wolf[wolfPos]->hunger = nextField[i][j].wolf[wolfPos]->hunger; 
                    newWorld->field[i][j].wolf[wolfPos]->reproduction = nextField[i][j].wolf[wolfPos]->reproduction; 
                    newWorld->field[i][j].wolf[wolfPos]->rest = nextField[i][j].wolf[wolfPos]->rest;
                }

                if ((sheepPos = is_sheep_active(nextField[i][j])) != -1) {
                    newWorld->field[i][j].sheep[sheepPos] = malloc(sizeof(Prey));
                    newWorld->field[i][j].sheep[sheepPos]->hunger = nextField[i][j].sheep[sheepPos]->hunger; 
                    newWorld->field[i][j].sheep[sheepPos]->reproduction = nextField[i][j].sheep[sheepPos]->reproduction; 
                    newWorld->field[i][j].sheep[sheepPos]->rest = nextField[i][j].sheep[sheepPos]->rest;

                }

            }
        }    
        if (newWorld->field[3][4].wolf[1] != NULL) {
            printf("yes\n");
        }
        print_world(newWorld);
        count++;
        if (newWorld->sheeps == 0 || count > 20) 
            exit(1);

    }
    return error;
}

int main(int argc, char** argv) {
    
    int size = 5;
    Simmulation newWorld;
    Field** newFields;
    newFields = malloc(sizeof(Field*) * size);
    for (int i = 0; i < size; i++) {
        newFields[i] = malloc(sizeof(Field) * size);    
    }
    newWorld.field = newFields;
    newWorld.size = size;
    int exit = run_simmulation(&newWorld);
    
    for (int i = 0; i < size; i++) {
        free(newFields[i]);
    }
    free(newFields);
    return exit;
}
