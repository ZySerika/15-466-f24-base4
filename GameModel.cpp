#include "GameModel.hpp"

State* setupGame() {
    // Final state (no choices, leads to no other state):
    State* end = new State("END", nullptr);

    State* s26 = new State("“But you wouldn't say a 'hello' to me?”", end);

    State* s25 = new State("“my longing for past colleagues. Everything—“ ", s26);

    State* s24 = new State("“I have briefed to you all of my life… my family, my job, children…", s25);

    State* s23 = new State(" Then you hear the same voice murmuring,", s24);

    State* s22 = new State("She slowly put away her phone, looking out of the window.", s23);

    State* s21 = new State("You neared the destination.  ", s22);

    State* dec4 = new State("You stayed silent.", s21, false);

    State* inc4 = new State("You couldn't.", s21, true);

    State* choiceState4 = new State("(Press A) Say something. (Press D) Do nothing.", inc4, dec4);

    State* s20 = new State("It is where you two first met. ", choiceState4);

    State* s19 = new State("You find yourself driving across a golden shade under the sun.", s20);

    State* dec3 = new State("You stayed silent.", s19, false);

    State* inc3 = new State("You couldn't.", s19, true);

    State* choiceState3 = new State("(Press A) Say something. (Press D) Do nothing.", inc3, dec3);

    State* s18 = new State("In a foreign dialect you couldn't conceive of.", choiceState3);

    State* s17 = new State("Then she talked to her boss. ", s18);

    State* dec2 = new State("You stayed silent.", s17, false);

    State* inc2 = new State("You couldn't.", s17, true);

    State* choiceState2 = new State("(Press A) Say something. (Press D) Do nothing.", inc2, dec2);

    State* s16 = new State("A daughter making excuses to skip piano class.", choiceState2);

    State* s15 = new State("A son on the sports team. ", s16);

    State* s14 = new State("She talked with someone resembling her husband on the other side.", s15);

    State* s13 = new State("She picked up her phone. ", s14);

    State* dec1 = new State("You stayed silent.", s13, false);

    State* inc1 = new State("You couldn't.", s13, true);

    // A state with dual choices:
    State* choiceState = new State("(Press A) Say something. (Press D) Do nothing.", inc1, dec1);

    State* s12 = new State("She is not your ex-wife. She is not your past time lover.", choiceState);

    State* s11 = new State("It is your past time lover.", s12);

    State* s10 = new State("It is not your ex-wife. It is something more distant.", s11);

    State* s9 = new State("It took no time for you to realize she's frighteningly familiar.", s10);

    State* c2 = new State("You stop by. A lady stepped in, resembling your age.", s9);

    State* endm = new State("END", nullptr);

    State* end1 = new State("You arrive at the airport. No one is there, and no one should be--another day in life.", endm);

    State* c1 = new State("You drive away. You don't want to be seen.", end1);

    State* s8 = new State("Someone waved at some distance. (Press A) Drive away. (Press D) Stop by.", c1, c2);

    State* s7 = new State("The acquaintances whom you would once look down upon. Your ex-wife and your daughter—-", s8);

    State* s6 = new State("You are on your way to the airport. This way you avoid the awkward encounters. ", s7);

    State* s5 = new State("You ran out of talents—taxi driving seemed like the only thing you could do in this small city.", s6);

    State* s4 = new State("In marriage. In intimate relationships. ", s5);

    State* s3 = new State("It's been years since your failure. In start-up endeavors.", s4);

    State* s2 = new State("Memories stay jagged. The past remains out of reach.", s3);

    State* start = new State("You are in your thirties. (press W to continue)", s2);

    

    return start;
}

State* setupTrueEnd() {
    State* end3 = new State("END. Thanks for playing.", nullptr);
    State* end2 = new State("“I know.” She smiled warmly.", end3);
    State* end1 = new State("“Uh, Hey.” You said.", end2);

    return end1;
}