#pragma once
#include <string>

struct State {
    const char *text; 

    // Pointers to other states for choices:
    State* choiceA = nullptr;  // If a dual-choice exists, this leads to the state for choice A
    State* choiceB = nullptr;  // If a dual-choice exists, this leads to the state for choice B

    State* nextState = nullptr;  // If no choices exist, this leads to the next state
    
    bool hasChoices = false;  // Determines whether the state has dual choices or not

    bool isIncrement = false;

    // Constructor for states with choices:
    State(const char *text, State* choiceA, State* choiceB)
        : text(text), choiceA(choiceA), choiceB(choiceB), hasChoices(true) {}

    // Constructor for states with no choices:
    State(const char *text, State* nextState)
        : text(text), nextState(nextState), hasChoices(false) {}

    // Constructor for states holding an increment value:
    State(const char *text, State* nextState, bool isIncrement)
        : text(text), nextState(nextState), hasChoices(false), isIncrement(isIncrement) {}


        
};

State* setupGame();
State* setupTrueEnd();