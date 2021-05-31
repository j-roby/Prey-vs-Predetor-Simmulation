#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <err.h>
#include <omp.h>



typedef struct {
    int hunger;
    int reproduction;
    bool rest;
} Predator;

typedef struct {
    int hunger;
    int reproduction;
    bool rest;
} Prey;

typedef struct {
    bool grass;
    int timer;
    Predator *wolf[2];
    Prey *sheep[2];

} Field;


typedef enum{
    EAT,
    MATE,
    REST,
    RANDOM
} Tasks;

/* Setup has values to create a wolf/sheep  */
typedef struct {
    int sheepHunger;
    int wolfHunger;
    int sheepMate;
    int wolfMate;
    int sheepGain;
    int wolfGain;
    int grassTimer;
} Setup;

typedef struct {
    Field** field;
    int time;
                                            int wolves;
    int herd;
    int size;
    Setup setup;
    double startTime;
    double endTime;
    double totalSerial;
} Simulation;

/* If called measn the user did not put in the right arguments */
void usage(void) {
    extern char *__progname;
    fprintf(stderr, "usage:%s -w number of wolves -s number of sheep\n"
                    "             [-u energy gained by wolf] [-d energy gained by sheep]\n"
                    "             [-c wolf mating level] [-r] [-b sheep mating level]\n"
                    "             [-g grass timer] FieldSize   FileName\n", __progname);
    exit(1);
}

/* Debugging code- used to see whats in field */
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

/** 
 *  Prints out the fields with information on the
 *  wolves and herd position in a human redable format
 */
void print_world(Simulation *newWorld) {
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

/*
 * sets up wolrd with NULL value
 */
int setup_world(Simulation *newWorld, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
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

/* 
 * sets of field with NULL value
 */
void setup_field(Field **newWorld, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            newWorld[i][j].wolf[0] = NULL;
            newWorld[i][j].wolf[1] = NULL;
            newWorld[i][j].sheep[0] = NULL;
            newWorld[i][j].sheep[1] = NULL;
            newWorld[i][j].grass = true;
            newWorld[i][j].timer = 0;

        }
    }
}

/**
 * Gives wolf a task
 */
Tasks find_wolf_task(Simulation *newWorld, Predator *wolf) {
    if (wolf->rest == true) {
        wolf->rest = false;
        return REST;
    }
    if (wolf->hunger > newWorld->setup.wolfGain &&
          wolf->reproduction  > newWorld->setup.wolfMate) {
        return RANDOM;
    }

    if (wolf->hunger < wolf->reproduction) {
        return EAT;
    } else
        return MATE;

}

/**
 * finds sheep a task
 */
Tasks find_sheep_task(Simulation *newWorld, Prey *sheep) {
    if (sheep->rest == true) {
        sheep->rest = false;
        return REST;
    }
    if (sheep->hunger > newWorld->setup.sheepGain &&
            sheep->reproduction > newWorld->setup.sheepMate ) {
        return RANDOM;
    if (sheep->hunger < sheep->reproduction) {
        return EAT;
    } else
        return MATE;

}

/**
 * checks if this fields has any active wolf on it
 */
int is_wolf_active(Field newField) {
    if(newField.wolf[0] != NULL) {
        return 0;
    } else if (newField.wolf[1] != NULL) {
        return 1;
    }
    return -1;

}

/**
 * checks if this fields has any active sheep on it
 */
int is_sheep_active(Field newField) {
    if(newField.sheep[0] != NULL) {
        return 0;
    } else if (newField.sheep[1] != NULL) {
        return 1;
    }
    return -1;

}

/**
 * checks if this fields is empty
 */
int is_wolf_empty(Field newField) {
    if(newField.wolf[0] == NULL) {
        return 0;
    } else if (newField.wolf[1] == NULL) {
        return 1;
    }
    return -1;

}

/**
 * checks if this fields is empty
 */
int is_sheep_empty(Field newField) {
    if(newField.sheep[0] == NULL) {
        return 0;
    } else if (newField.sheep[1] == NULL) {
        return 1;
    }
    return -1;

}
/**
 * generates random value
 */
int random_value(int upperLim, int lowerLim) {

    return (rand() % (upperLim - lowerLim +1)) + lowerLim;

}

/**
 * makes wolf move in a random direction
 */
void wolf_random(Simulation *newWorld,Field **nextField, int i, int j, int wolfPos) {
    int upper = 4;
    int lower = 1;
    int newI = i;
    int newJ = j;
    bool dir = false;
    int pos;
    int noValue = 0;
    while(1) {
        noValue++;
        if (noValue > 20) {
            return;
        }
        /* Creates random rumber from 4 to 1
        *  assings it a direction and moves if legal
        */
        newI = i;
        newJ = j;
        dir = false;
        int randNo = (rand() % (upper - lower +1)) + lower;
        switch(randNo) {

            case 1:
                if (i+1 < newWorld->size) {
                    dir = true;
                    newI+= 1;
                }
                break;
            case 2:
                 if (i-1 >= 0) {
                    dir = true;
                    newI-= 1;
                }
                break;
            case 3:
                 if (j+1 < newWorld->size) {
                    dir = true;
                    newJ+= 1;
                }
                break;
            case 4:
                if (j-1 >= 0) {
                    dir = true;
                    newJ-= 1;
                }
                break;
        }
       if (dir == true) {
           /* if there is an empty spot move wolf */
            if ( (pos = is_wolf_empty((nextField[newI][newJ]))) != -1) {
#pragma omp critical
    {
                double time = omp_get_wtime();

                nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predator));
                nextField[newI][newJ].wolf[pos]->hunger =
                        newWorld->field[i][j].wolf[wolfPos]->hunger;
                nextField[newI][newJ].wolf[pos]->reproduction =
                        newWorld->field[i][j].wolf[wolfPos]->reproduction;
                nextField[newI][newJ].wolf[pos]->rest =
                        newWorld->field[i][j].wolf[wolfPos]->rest;
                nextField[i][j].wolf[wolfPos] = NULL;
                newWorld->field[i][j].wolf[wolfPos] = NULL;
                time = omp_get_wtime() - time;
                newWorld->totalSerial += time;


    }
                return;
            }
        }
    }
}


void wolf_eat(Simulation *newWorld, Field **nextField, int i, int j, int wolfPos) {
    int min =newWorld->size*newWorld->size;
 int moveX= min;
    int moveY = min;
    int newI = i;
    int newJ = j;

    /* creates distance matix */
    int distance[newWorld->size][newWorld->size];
    for (int x = 0; x < newWorld->size; x++) {
        for (int y = 0; y< newWorld->size; y++) {

    bool sheepTrue = (newWorld->field[x][y].sheep[0] != NULL ||
                newWorld->field[x][y].sheep[1] != NULL) ;

            if (sheepTrue) {



            distance[x][y] = abs(i-x)+abs(j-y);
                if (distance[x][y] == 0) {
                    min =0;
                    moveX = x;
                    moveY = y;
                } else if (distance[x][y] < min) {
                    min = distance[x][y];
                    moveX = x;
                    moveY = y;
                }
            } else {
                distance[x][y] = newWorld->size * newWorld->size;
            }
        }
    }

    if (min == newWorld->size *newWorld->size)
        return;
    /* if sheep in same field eat sheep regain energy */
    if (min == 0) {
#pragma omp critical
    {
        double time = omp_get_wtime();

        if (nextField[moveX][moveY].sheep[0] != NULL) {
            nextField[moveX][moveY].wolf[wolfPos]->hunger += newWorld->setup.wolfGain;
            nextField[moveX][moveY].sheep[0] = NULL;
            newWorld->field[moveX][moveY].sheep[0] = NULL;
            newWorld->herd--;

        }else if (nextField[moveX][moveY].sheep[1] != NULL) {
            nextField[moveX][moveY].wolf[wolfPos]->hunger += newWorld->setup.wolfGain;
            nextField[moveX][moveY].sheep[1] = NULL;
            newWorld->field[moveX][moveY].sheep[1] = NULL;
            newWorld->herd--;

        }
        time = omp_get_wtime() - time;
        newWorld->totalSerial += time;
    }
        return;
    }
    /* move to sheep */
    int pos;
    if ((moveX - i) > 0) {
        newI = i+1;
        if ( (pos = is_wolf_empty((nextField[newI][newJ]))) != -1) {
#pragma omp critical
    {
                double time = omp_get_wtime();

                nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predator));

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
                time = omp_get_wtime() - time;
                newWorld->totalSerial += time;
     }
            return;

        }
    } else if((moveX-i) < 0) {
        newI = i-1;
        if ( (pos = is_wolf_empty((nextField[newI][newJ]))) != -1) {
#pragma omp critical
    {
                double time = omp_get_wtime();

                nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predator));

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
                time = omp_get_wtime() - time;
                newWorld->totalSerial += time;


    }
            return;

        }
    }
    newI = i;
    if ((moveY-j) > 0) {
        newJ = j +1;
        if ((pos = is_wolf_empty(nextField[newI][newJ])) != -1 ) {
#pragma omp critical
    {
            double time = omp_get_wtime();
            nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predator));
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
            time = omp_get_wtime() - time;
            newWorld->totalSerial += time;
    }
            return;
        }
    } else if ((moveY-j) < 0) {
        newJ = j-1;
         if ((pos = is_wolf_empty(nextField[newI][newJ])) != -1 ) {
#pragma omp critical
    {
            double time = omp_get_wtime();
            nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predator));
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
            time = omp_get_wtime() - time;
            newWorld->totalSerial += time;




    }
           return;
        }
    }

    return;
}

/**
 * clone a wolf with 0.05 probability
 * works similar to wolf_random
 */
void wolf_mate(Simulation *newWorld,Field **nextField,int i,int j, int wolfPos) {

    double prob = (double)rand()/RAND_MAX;
    int newI = i;
    int newJ = j;
    int upper = 5;
    int lower = 1;
    bool dir = false;
    int pos;
    int noValue = 0;
    //change 0.05 to whatever probabilty you want
    if (prob <= 0.05) {
        while(1) {
            noValue++;
            if (noValue > 20) {
            return;
            }
            newI = i;
            newJ = j;
            dir = false;
            int randNo = (rand() % (upper - lower +1)) + lower;
            switch(randNo) {

                case 1:
                    if (i+1 < newWorld->size) {
                        dir = true;
                        newI+= 1;
                    }
                    break;
                case 2:
                     if (i-1 >= 0) {
                        dir = true;
                        newI-= 1;
                    }
                    break;
                case 3:
                     if (j+1 < newWorld->size) {
                        dir = true;
                        newJ+= 1;
                    }
                    break;
                case 4:
                    if (j-1 >= 0) {
                        dir = true;
                        newJ-= 1;
                    }
                    break;
                case 5:
                    dir = true;
                    break;
             }

            if (dir == true) {
               if ( (pos = is_wolf_empty((nextField[newI][newJ]))) != -1) {
#pragma omp critical
    {
                    double time = omp_get_wtime();

                    newWorld->wolves++;
                    nextField[i][j].wolf[wolfPos]->hunger =
                        nextField[i][j].wolf[wolfPos]->hunger /2;
                    nextField[i][j].wolf[wolfPos]->reproduction =
                        newWorld->setup.wolfMate;
                    nextField[i][j].wolf[wolfPos]->rest = true;
                    nextField[newI][newJ].wolf[pos] = malloc(sizeof(Predator));
                    nextField[newI][newJ].wolf[pos]->hunger =
                        random_value(newWorld->setup.wolfGain*2,
                        newWorld->setup.wolfGain /2);
                    nextField[newI][newJ].wolf[pos]->reproduction =
                        newWorld->setup.wolfMate;
                    nextField[newI][newJ].wolf[pos]->rest = false;
                    time = omp_get_wtime() - time;
                    newWorld->totalSerial += time;


    }
                    return;
                }


            }

        }

    } else {
        wolf_random(newWorld, nextField,i,j,wolfPos);
    }


}

/**
 * send sheep off in random direction
 * same as wolf random
 */
void sheep_random(Simulation *newWorld,Field **nextField, int i, int j, int sheepPos) {
    int upper = 4;
    int lower = 1;
    int newI = i;
    int newJ = j;
    bool dir = false;
    int pos;
    int noValue = 0;
    while(1) {
        noValue++;
        if (noValue > 20) {
            return;
        }
        newI = i;
        newJ = j;
        dir = false;
        int randNo = (rand() % (upper - lower +1)) + lower;
        switch(randNo) {

            case 1:
                if (i+1 < newWorld->size) {
                    dir = true;
                    newI+= 1;
                }
                break;
            case 2:
                 if (i-1 >= 0) {
                    dir = true;
                    newI-= 1;
                }
                break;
            case 3:
                 if (j+1 < newWorld->size) {
                    dir = true;
                    newJ+= 1;
                }
                break;
            case 4:
                if (j-1 >= 0) {
                    dir = true;
                    newJ-= 1;
                }
                break;
        }

        if (dir == true) {
           if ( (pos = is_sheep_empty((nextField[newI][newJ]))) != -1) {
#pragma omp critical
    {
                double time = omp_get_wtime();
                nextField[newI][newJ].sheep[pos] = malloc(sizeof(Prey));
                nextField[newI][newJ].sheep[pos]->hunger =
                        newWorld->field[i][j].sheep[sheepPos]->hunger;
                nextField[newI][newJ].sheep[pos]->reproduction =
                        newWorld->field[i][j].sheep[sheepPos]->reproduction;
                nextField[newI][newJ].sheep[pos]->rest =
                        newWorld->field[i][j].sheep[sheepPos]->rest;
                nextField[i][j].sheep[sheepPos] = NULL;
                newWorld->field[i][j].sheep[sheepPos] = NULL;
                time = omp_get_wtime() - time;
                newWorld->totalSerial += time;


    }
                return;

            }


        }
    }
}

/**
 * clone sheep
 * similar to wolf_mate
 */
void sheep_mate(Simulation *newWorld,Field **nextField,int i,int j, int sheepPos) {

    double prob = (double)rand()/RAND_MAX;
    int newI = i;
    int newJ = j;
    int upper = 5;
    int lower = 1;
    bool dir = false;
    int pos;
    int noValue = 0;
    if (prob <= 0.04) {
        while(1) {
           noValue++;
            if (noValue > 20) {
            return;
            }

            newI = i;
            newJ = j;
            dir = false;
            int randNo = (rand() % (upper - lower +1)) + lower;
            switch(randNo) {

                case 1:
                    if (i+1 < newWorld->size) {
                        dir = true;
                        newI+= 1;
                    }
                    break;
                case 2:
                     if (i-1 >= 0) {
                        dir = true;
                        newI-= 1;
                    }
                    break;
                case 3:
                     if (j+1 < newWorld->size) {
                        dir = true;
                        newJ+= 1;
                    }
                    break;
                case 4:
                    if (j-1 >= 0) {
                        dir = true;
                        newJ-= 1;
                    }
                    break;
                case 5:
                    dir = true;
                    break;
             }

            if (dir == true) {
                if ( (pos = is_sheep_empty((nextField[newI][newJ]))) != -1) {
#pragma omp critical
    {
                    double time = omp_get_wtime();
                    newWorld->herd++;
                    nextField[i][j].sheep[sheepPos]->hunger =
                        nextField[i][j].sheep[sheepPos]->hunger /2;
                    nextField[i][j].sheep[sheepPos]->reproduction =
                        newWorld->setup.sheepMate;
                    nextField[i][j].sheep[sheepPos]->rest = true;
                    nextField[newI][newJ].sheep[pos] = malloc(sizeof(Prey));
                    nextField[newI][newJ].sheep[pos]->hunger =
                        random_value(newWorld->setup.sheepGain*2,
                        newWorld->setup.sheepGain /2);
                    nextField[newI][newJ].sheep[pos]->reproduction =
                        newWorld->setup.sheepMate;
                    nextField[newI][newJ].sheep[pos]->rest = false;
                    time = omp_get_wtime() - time;
                    newWorld->totalSerial += time;

    }
                    return;
                }


            }

        }




    } else {
        sheep_random(newWorld, nextField,i,j,sheepPos);
    }


}

/**
 * if grass avialable eat if not sheep_random
 */
void sheep_eat(Simulation *newWorld, Field **nextField, int i, int j, int sheepPos) {

    if (newWorld->field[i][j].grass == true) {
        newWorld->field[i][j].grass = false;
        nextField[i][j].sheep[sheepPos]->hunger += newWorld->setup.wolfGain;
        return;
   }


    sheep_random(newWorld, nextField, i, j, sheepPos);

}

/**
 * give sheep a movement and reduce hunger and reproduction levels
 */
void sheep_action(Simulation *newWorld,Field **nextField, int i, int j, int sheepPos) {

    int task;
    task = find_sheep_task(newWorld,newWorld->field[i][j].sheep[sheepPos]);
    if (newWorld->field[i][j].sheep[sheepPos]->hunger <= 0) {
        nextField[i][j].sheep[sheepPos] = NULL;
#pragma omp critical
    {
    {
        double time = omp_get_wtime();
        newWorld->herd--;
        time = omp_get_wtime() - time;
        newWorld->totalSerial += time;
    }
        newWorld->field[i][j].sheep[sheepPos] = NULL;
        return;
    }
    if (newWorld->field[i][j].sheep[sheepPos]->reproduction > 0) {
        newWorld->field[i][j].sheep[sheepPos]->reproduction -= 1;
    }

    newWorld->field[i][j].sheep[sheepPos]->hunger -= newWorld->setup.sheepGain/4;
    nextField[i][j].sheep[sheepPos]->hunger -= newWorld->setup.sheepGain/4;
    /* do nothing */


    if (task == EAT) {
        sheep_eat(newWorld, nextField, i, j, sheepPos);

    } else if (task == MATE) {

        sheep_mate(newWorld, nextField, i, j, sheepPos);

    } else if (task == REST) {
        nextField[i][j].sheep[sheepPos]->rest = false;

    } else if (task == RANDOM) {

        sheep_random(newWorld, nextField, i, j, sheepPos);
    }

}

/**
 * give wolf a movement and reduce hunger and reproduction levels
*/
void wolf_action(Simulation *newWorld,Field **nextField, int i, int j, int wolfPos) {

    int task;
    task = find_wolf_task(newWorld, newWorld->field[i][j].wolf[wolfPos]);
    if (newWorld->field[i][j].wolf[wolfPos]->hunger <= 0) {
        nextField[i][j].wolf[wolfPos] = NULL;
#pragma omp critical
    {
        double time = omp_get_wtime();
        newWorld->wolves--;
        time = omp_get_wtime() - time;
       newWorld->totalSerial += time;

    }
        newWorld->field[i][j].wolf[wolfPos] = NULL;
        return;
    }
    if (newWorld->field[i][j].wolf[wolfPos]->reproduction > 0) {
        newWorld->field[i][j].wolf[wolfPos]->reproduction -= 1;
    }
    newWorld->field[i][j].wolf[wolfPos]->hunger -= newWorld->setup.wolfGain/4;
    nextField[i][j].wolf[wolfPos]->hunger -= newWorld->setup.wolfGain/4;

           /* do nothing */

   if (task == EAT) {
        wolf_eat(newWorld, nextField, i, j, wolfPos);

    } else if (task == MATE) {

        wolf_mate(newWorld, nextField, i, j, wolfPos);

    } else if (task == REST) {
        nextField[i][j].wolf[wolfPos]->rest = false;
    } else if (task == RANDOM) {

        wolf_random(newWorld, nextField, i, j, wolfPos);
    }

}

/** 
 * fill newWorld with nextField
 */
void fill_board(Simulation *newWorld, Field **nextField) {
   for (int i = 0; i < newWorld->size; i++) {
        for (int j = 0; j < newWorld->size; j++) {

            if (newWorld->field[i][j].grass == false &&
                    newWorld->field[i][j].timer > newWorld->setup.grassTimer) {
                newWorld->field[i][j].grass = true;

            }
            if (newWorld->field[i][j].timer > newWorld->setup.grassTimer)
                newWorld->field[i][j].timer = 0;

            if (nextField[i][j].wolf[0] != NULL) {

                newWorld->field[i][j].wolf[0] = malloc(sizeof(Predator));
                newWorld->field[i][j].wolf[0]->hunger = nextField[i][j].wolf[0]->hunger;
                newWorld->field[i][j].wolf[0]->reproduction = nextField[i][j].wolf[0]->reproduction;
                newWorld->field[i][j].wolf[0]->rest = nextField[i][j].wolf[0]->rest;
            }
            if (nextField[i][j].wolf[1] != NULL) {

                newWorld->field[i][j].wolf[1] = malloc(sizeof(Predator));
                newWorld->field[i][j].wolf[1]->hunger = nextField[i][j].wolf[1]->hunger;
                newWorld->field[i][j].wolf[1]->reproduction = nextField[i][j].wolf[1]->reproduction;
                newWorld->field[i][j].wolf[1]->rest = nextField[i][j].wolf[1]->rest;
            }
            if (nextField[i][j].sheep[0] != NULL) {

                newWorld->field[i][j].sheep[0] = malloc(sizeof(Prey));
                newWorld->field[i][j].sheep[0]->hunger = nextField[i][j].sheep[0]->hunger;
                newWorld->field[i][j].sheep[0]->reproduction = nextField[i][j].sheep[0]->reproduction;
                newWorld->field[i][j].sheep[0]->rest = nextField[i][j].sheep[0]->rest;

                newWorld->field[i][j].sheep[0] = malloc(sizeof(Prey));
                newWorld->field[i][j].sheep[0]->hunger = nextField[i][j].sheep[0]->hunger;
                newWorld->field[i][j].sheep[0]->reproduction = nextField[i][j].sheep[0]->reproduction;
                newWorld->field[i][j].sheep[0]->rest = nextField[i][j].sheep[0]->rest;
            }
            if (nextField[i][j].sheep[1] != NULL) {

                newWorld->field[i][j].sheep[1] = malloc(sizeof(Prey));
                newWorld->field[i][j].sheep[1]->hunger = nextField[i][j].sheep[1]->hunger;
                newWorld->field[i][j].sheep[1]->reproduction = nextField[i][j].sheep[1]->reproduction;
                newWorld->field[i][j].sheep[1]->rest = nextField[i][j].sheep[1]->rest;
            }
        }
    }

}

/**
 * run main simulation
 */
int run_simulation(Simulation *newWorld, const char* file) {
    Field** nextField;
    int randI;
    int randJ;
    int pos;
    FILE *fptr = fopen(file, "w");
    char sendFile[20];
    double start;
    double end;
    if (fptr == NULL) {
        errx(1, "error opening file");
    }
    sprintf(sendFile,"time wolves sheep\n");
    fputs(sendFile, fptr);

    nextField = malloc(sizeof(Field*) * newWorld->size);
    for (int i = 0; i < newWorld->size; i++) {
        nextField[i] = malloc(sizeof(Field) * newWorld->size);
    }
    int error = setup_world(newWorld, newWorld->size);
    setup_field(nextField, newWorld->size);
    //creates inital sheep
    for (int i = 0; i < newWorld->herd; i++) {
        randJ = (rand() % (newWorld->size ));
        randI = (rand() % (newWorld->size ));
        if((pos = is_sheep_empty(newWorld->field[randI][randJ])) != -1) {
            newWorld->field[randI][randJ].sheep[pos]= malloc(sizeof(Prey));
            newWorld->field[randI][randJ].sheep[pos]->hunger = random_value(newWorld->setup.sheepGain*2 , newWorld->setup.sheepGain/2);
            newWorld->field[randI][randJ].sheep[pos]->reproduction = newWorld->setup.sheepMate;
            newWorld->field[randI][randJ].sheep[pos]->rest = false;
        } else {
            i--;
        }

    }
    //creates inital wolf
    for (int i = 0; i < newWorld->wolves; i++) {
        randI = (rand() % (newWorld->size ));
        randJ = (rand() % (newWorld->size ));
        if((pos = is_wolf_empty(newWorld->field[randI][randJ])) != -1) {
            newWorld->field[randI][randJ].wolf[pos]= malloc(sizeof(Predator));
            newWorld->field[randI][randJ].wolf[pos]->hunger = random_value(newWorld->setup.wolfGain*2 , newWorld->setup.wolfGain/2);
            newWorld->field[randI][randJ].wolf[pos]->reproduction = newWorld->setup.wolfMate;
            newWorld->field[randI][randJ].wolf[pos]->rest = false;
        } else {
            i--;
        }
    }
    //fill nextField with newWorld
    for(int i = 0; i < newWorld->size; i++) {
        for(int j = 0; j < newWorld->size; j++) {
            if (newWorld->field[i][j].wolf[0] != NULL) {
                nextField[i][j].wolf[0] = malloc(sizeof(Predator));
                nextField[i][j].wolf[0]->hunger = newWorld->field[i][j].wolf[0]->hunger;
                nextField[i][j].wolf[0]->reproduction = newWorld->field[i][j].wolf[0]->reproduction;
                nextField[i][j].wolf[0]->rest = newWorld->field[i][j].wolf[0]->rest;
            }

           if (newWorld->field[i][j].sheep[0] != NULL) {
                nextField[i][j].sheep[0] = malloc(sizeof(Predator));
                nextField[i][j].sheep[0]->hunger = newWorld->field[i][j].sheep[0]->hunger;
                nextField[i][j].sheep[0]->reproduction = newWorld->field[i][j].sheep[0]->reproduction;
                nextField[i][j].sheep[0]->rest = newWorld->field[i][j].sheep[0]->rest;
            }
            if (newWorld->field[i][j].wolf[1] != NULL) {

                nextField[i][j].wolf[1] = malloc(sizeof(Predator));
                nextField[i][j].wolf[1]->hunger = newWorld->field[i][j].wolf[1]->hunger;
                nextField[i][j].sheep[1]->reproduction = newWorld->field[i][j].sheep[1]->reproduction;
                nextField[i][j].sheep[1]->rest = newWorld->field[i][j].sheep[1]->rest;
            }
            nextField[i][j].grass = newWorld->field[i][j].grass;
            nextField[i][j].timer = newWorld->field[i][j].timer;

        }
    }

    int count = 0;
    print_world(newWorld);
    //main loop
    newWorld->startTime = omp_get_wtime() - newWorld->startTime;
    newWorld->totalSerial = 0;
    start = omp_get_wtime();
    while(1) {
        newWorld->time++;
#pragma omp parallel for
        for (int i = 0; i < newWorld->size; i++) {
            for (int j = 0; j < newWorld->size; j++) {
                newWorld->field[i][j].timer++;
                if (newWorld->field[i][j].wolf[0] != NULL)
                    wolf_action(newWorld, nextField, i, j, 0);
                if (newWorld->field[i][j].wolf[1] != NULL )
                    wolf_action(newWorld, nextField, i, j, 1);

                if (newWorld->field[i][j].sheep[0] != NULL)
                    sheep_action(newWorld, nextField, i, j, 0);

                if (newWorld->field[i][j].sheep[1] != NULL )
                    sheep_action(newWorld, nextField, i, j, 1);


            }
        }
        newWorld->endTime = omp_get_wtime();
        sprintf(sendFile," %d    %d     %d   \n",newWorld->time, newWorld->wolves, newWorld->herd);
        fputs(sendFile, fptr);
        fill_board(newWorld, nextField);
        print_world(newWorld);
        count++;
        printf("%d %d \n",newWorld->herd , newWorld->wolves);
        if ((newWorld->herd <= 0) || (newWorld->wolves <= 0)) {
            fclose(fptr);
            break;
        }
    }
    end = omp_get_wtime();
    newWorld->endTime = omp_get_wtime() - newWorld->endTime;
    printf("Total time spent on process: %f \n", (end-start)+ newWorld->startTime+newWorld->endTime+newWorld->totalSerial);
    printf("Time spent in parralel:      %f \n", end-start);
    printf("Time spent in serial:        %f \n", newWorld->startTime+newWorld->endTime+newWorld->totalSerial);
    return error;
}

int main(int argc, char** argv) {
    int opt;
    Simulation newWorld;
    newWorld.startTime = omp_get_wtime();
    newWorld.setup.wolfGain = 20;
    newWorld.setup.sheepGain = 4;
    newWorld.setup.wolfMate = 12;
    newWorld.setup.sheepMate = 12;
    newWorld.setup.grassTimer = 30;
    newWorld.wolves = 0;
    newWorld.herd = 0;
    bool random = true;
    while ((opt = getopt(argc, argv, "u:d:c:b:g:w:s:r  ")) != -1) {
        switch (opt) {
        case 'u':
            newWorld.setup.wolfGain = atoi(optarg);

            break;
        case 'd':
            newWorld.setup.sheepGain = atoi(optarg);
            break;
        case 'c':
            newWorld.setup.wolfMate = atoi(optarg);
            break;
        case 'b':
            newWorld.setup.sheepMate = atoi(optarg);
            break;
        case 'g':
            newWorld.setup.grassTimer = atoi(optarg);
            break;
        case 'r':
            // r toggles randomness
            random = false;
            break;
        case 'w':
            newWorld.wolves = atoi(optarg);
            break;
        case 's':
            newWorld.herd = atoi(optarg);
            break;
        default:
            errx(1, "%c is not a valid character\n", opt);
            usage();

        }
    }
    if (newWorld.wolves == 0 || newWorld.herd == 0) {
        fprintf(stderr," starting wolf and sheep numbers too small\n");
        usage();
    }

    if (random == true)
        srand(time(0));
    argc -= optind;
    argv += optind;
    if (argc < 2)
        errx(1," Need a field size and file name");
    int size = atoi(argv[0]);
    const char *file = argv[1];
    if (newWorld.wolves+newWorld.herd > size*size*4)
        errx(1, "too many animals in the field");
    Field** newFields;
    newFields = malloc(sizeof(Field*) * size);
    for (int i = 0; i < size; i++) {
        newFields[i] = malloc(sizeof(Field) * size);
    }
    newWorld.field = newFields;
    newWorld.size = size;
    int exit = run_simulation(&newWorld, file);

    for (int i = 0; i < size; i++) {
        free(newFields[i]);
    }
    free(newFields);
    return exit;
}
