#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "equations.h"  

/* Loads all functions */
int loadEquations(Equation equations[], int maxEquations){
    FILE *fp;
    int count;
    char line[MAX_EQUATION_LEN];

    fp = fopen(EQUATIONS_FILE, "r");
    if (!fp){
        return 0;
    }
    count = 0;
    while (count < maxEquations && fgets(line, sizeof(line), fp)){
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') continue;
        strncpy(equations[count].equation, line, MAX_EQUATION_LEN -1);
        equations[count].equation[MAX_EQUATION_LEN - 1] = '\0';
        count++;
    }
    fclose(fp);
    return count;
}

void displayAllEquations(void){
    Equation equations[MAX_EQUATIONS];
    int count;
    int i;
    printf("Testing \n");
    count = loadEquations(equations, MAX_EQUATIONS);
    printf("Total equations loaded: %d\n", count);

    printf("%-6s %-20s\n", "No.", "Equation");
    printf("%-6s %-20s\n", "---", "--------");
    for (i = 0; i < count; i++){
        printf("%-6d %-20s\n", i + 1, equations[i].equation);
    }
}
void getEquation(char *result){
    Equation equations[MAX_EQUATIONS];
    int count;
    int rand_index;

    count = loadEquations(equations, MAX_EQUATIONS);
    printf("Total equations loaded: %d\n", count);

    if (count == 0) {
        printf("No equations found!\n");
        result[0] = '\0';  
        return;
    }

    rand_index = rand() % count;

    strncpy(result, equations[rand_index].equation, MAX_EQUATION_LEN - 1);
    result[MAX_EQUATION_LEN - 1] = '\0';
}