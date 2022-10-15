// Copyright 2022, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Hannah Bast <bast@cs.uni-freiburg.de>
//         Johannes Kalmbach <kalmbach@cs.uni-freiburg.de>
//

#ifndef NERDLEBENCHMARK_H_
#define NERDLEBENCHMARK_H_

#include <vector>
#include <string>

// Correct: The correct character is in the correct position (aka "Green")
// WrongPosition: A character appears in the expression, but NOT at this
//                  position (aka "Magenta")
// Wrong: This character does not appear in the expression at all (aka "Black")
// For details and cornercases read the specifications on the Wiki carefully.
enum class NerdleStatus {
  Correct, WrongPosition, Wrong
};

// A character of an expression, and its status. The character will always be
// one of "0123456789+-*/="
struct CharacterAndStatus {
  char character_;
  NerdleStatus status_;
};

// The following "using <name> = <type>" declarations give a simpler name to a
// type. You can for example write NerdleGameState or
// std::vector<std::vector<CharacterAndStatus> completely interchangeably.
//

// On row in the game of Nerdle. It consists of the expression that was guessed,
// and the corresponding `NerdleStatus`es.
using NerdleStatusRow = std::vector<CharacterAndStatus>;

// The complete state of a single game of Nerdle. If it is empty, then the game
// has just started, and nothing has been guessed yet. Otherwise the last guess
// and the corresponding status/hints will be in the last entry of the vector.
using NerdleGameState = std::vector<NerdleStatusRow>;

// Your solver class must inherit from this base class. It needs only one
// `override` function: nextGuess.
class NerdleSolverBase {
 public:
  virtual ~NerdleSolverBase() = default;
  // Given a NerdleGameState (the previous guesses and the corresponding
  // hints), compute the next guess. The result must have the correct
  // length (corresponding to the benchmark that the solver is used for
  // and must be a valid Nerdle expressions, according to the rules
  // on the Wiki, otherwise the benchmark will crash.
  virtual std::string nextGuess(const NerdleGameState& gameState) = 0;
};

// Run the complete benchmark for a given solver and the given lenght.
// length must be one of 8, 9, 10, 11.
void runNerdleBenchmark(NerdleSolverBase* solver, int length);

// Run a custom benchmark using expressions that you specify manually.
// This can be used to debug your solver because you then know the correct
// answer in a game of Nerdle.
// The expressions all must have the correct length and must be valid
// expressions according to the rules on the wiki, otherwise the
// benchmark will crash with an exception.
void runNerdleBenchmark(NerdleSolverBase* solver, int length,
                        const std::vector<std::string>& expressions);


#endif  // NERDLEBENCHMARK_H_
