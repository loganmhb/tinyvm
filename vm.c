/*
   a little vm that computes the factorial of 10
   by Logan Buckley
   adapted and expanded from the example at https://github.com/felixangell/mac.   
*/



#include <stdio.h>
#include <stdbool.h>

#define STACK_SIZE 256

typedef enum {
    PSH, POP, ADD, SUB, MUL, DIV,
    DUP, GLD, GPT, MOV, LOG, SET,
    IF, JMP, HLT
} InstructionSet;

typedef enum {
    A, B, C, D, E, F,
    NUM_OF_REGISTERS
} Registers;

const int program[] = {
    // Start with ten
    PSH, 10, //0
    // Keep track of how many multiplications to do
    SET, A, 0,//2
    // otherwise, duplicate the number and subtract one from the new dup
    DUP,    //5
    PSH, 1, //6
    SUB,    //8
    // Increment the val in reg A
    GPT, A,//9
    PSH, 1,//11
    ADD, // 13
    GLD, A,//14
    // if we've subtracted to zero, move on to multiplying
    DUP, //16
    GLD, B, //17
    IF, B, 0, 25,//19
    JMP, 5, //23 otherwise loop back to duping
    // Multiply top two frames:
    POP, // 25 throw away the 0
    GPT, A, //26
    PSH, 1, //28
    SUB, //29
    GLD, A, //30
    // Decrement counter:
    // If counter's zero, end
    IF, A, 0, 40, //34
    // Otherwise, log a frame
    MUL, //38
    JMP, 26, //39
    POP, // 41
    HLT // 42
};

int ip = 0;
bool running = true;
int stack[STACK_SIZE];
int sp = -1;
int registers[NUM_OF_REGISTERS];

int fetch() {
    return program[ip];
}

int pop_val() {
    return stack[sp--];
}

void push_val(int val) {
    stack[++sp] = val;
}

void eval(int instr) {
    switch (instr) {
    case HLT: {
        printf("Halting.\n");
        running = false;
        break;
    }
    case PSH: {
        int val = program[++ip];
        printf("Pushing val %d\n", val);
        push_val(val);
        break;
    }
    case POP: {
        printf("popped %d\n", pop_val());
        break;
    }
    case ADD: {
        int a = pop_val();
        int b = pop_val();
        int result = b + a;
        printf("Adding %d and %d and pushing %d\n", a, b, result);
        push_val(result);
        break;
    }
    case SUB: {
        int a = pop_val();
        int b = pop_val();
        int result = b - a;
        printf("Subtracting %d from %d and pushing %d\n", a, b, result);
        push_val(result);
        break;
    }
    case MUL: {
        int a = pop_val();
        int b = pop_val();
        int result = b * a;
        printf("Multiplying %d and %d and pushing %d\n", a, b, result);
        push_val(result);
        break;
    }
    case DUP: {
        int val = pop_val();
        printf("Duplicating %d\n", val);
        push_val(val);
        push_val(val);
        break;
    }
    case GLD: {
        int reg = program[++ip];
        int val = pop_val();
        printf("Loading %d into reg %d\n", val, reg);
        registers[reg] = val;
        break;
    }
    case GPT: {
        int reg = program[++ip];
        int val = registers[reg];
        printf("Taking %d from %d\n", val, reg);
        push_val(val);
        break;
    }
    case SET: {
        int reg = program[++ip];
        int val = program[++ip];
        printf("Setting reg %d to %d\n", reg, val);
        registers[reg] = val;
        break;
    }
    case JMP: {
        int dest = program[++ip];
        printf("Jumping from %d to %d\n", ip-1, dest);
        ip = dest-1; // ip will be incremented before used again
        break;
    }
    case IF: {
        int start = ip;
        int reg = program[++ip];
        int val = program[++ip];
        int dest = program[++ip];
        if (registers[reg] == val) {
            printf("Jumping from %d to %d\n",start, dest);
            ip = dest-1; // ip will be incremented again
        }
        break;
    }
    case MOV: {
        int val = registers[program[++ip]];
        int dest = program[++ip];
        registers[dest] = val;
        break;
    }
    case LOG: {
        int reg = program[++ip];
        int val = registers[reg];
        printf("Log: %d\n", val);
        break;
    }
    default:
        printf("An error occured with opcode %d.\n", instr);
        running = false;
        break;
    }
}

int main() {
    printf("Program: \n{ ");
    for (int i = 0; i < 42; i++)
        printf("%d, ", program[i]);
    printf("}\n");
    while (running) {
        eval(fetch());
        ip++;
    }   
}
