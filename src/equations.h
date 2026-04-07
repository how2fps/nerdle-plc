#ifndef EQUATIONS_H
#define EQUATIONS_H

#define MAX_EQUATION_LEN 64
#define MAX_EQUATIONS    1024
#define EQUATIONS_FILE   "../equations.txt"

typedef struct {
    char equation[MAX_EQUATION_LEN];
} Equation;

/* Equation functions */
int loadEquations(Equation equations[], int maxEquations);
void displayAllEquations(void);
void getEquation(char *result);

#endif