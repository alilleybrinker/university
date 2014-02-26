/**
 * Copyright 2014 Andrew Brinker
 */

#include "./Grammar.h"
#include <set>
#include <map>
#include <string>
#include <fstream>
#include <cstdio>

#define DELIM   "$"
#define EPSILON "e"


/**
 * Print the elemenets of a set of character
 * @param first -> The first being added to
 * @param input -> The set to be printed.
 */
void print_add(char first, std::set<char> input) {
  printf("%c -> ", first);
  for (auto item : input) {
    printf("%c ", item);
  }
  printf("\n");
}


/**
 * Print the character being added to the given LHS
 * @param input -> The set to be printed.
 */
void print_add_char(char first, char input) {
  printf("%c -> %c\n", first, input);
}


/**
 * Load the contents of the given grammar into the class
 * @param  file_name -> name of the file being loaded
 * @return           -> exit code
 */
Grammar::Grammar(std::string file_name) {
  std::ifstream input_file(file_name);
  // The terminals section
  for (std::string line; getline(input_file, line);) {
    if (line == DELIM) break;
    _terminals.insert(line[0]);
  }
  // The productions section
  std::string first_line;
  getline(input_file, first_line);
  _follow[first_line[0]].insert(DELIM[0]);
  _non_terminals.insert(first_line[0]);
  _productions.insert(first_line);
  for (std::string line; getline(input_file, line);) {
    if (line == "$") break;
    _non_terminals.insert(line[0]);
    _productions.insert(line);
  }
}


/**
 * Finds the first and follow sets for the grammar
 * @return exit code
 */
bool Grammar::parse() {
  bool failed = findFirst();
  if (failed) return true;
  failed = findFollow();
  return failed;
}


/**
 * Returns the first sets for the grammar
 * @return the first sets
 */
std::map<char, std::set<char>> Grammar::first() const {
  return _first;
}


/**
 * Returns the follow sets for the grammar
 * @return the follow sets
 */
std::map<char, std::set<char>> Grammar::follow() const {
  return _follow;
}


/**
 * Finds the first set for the grammar
 * @return exit code
 */
bool Grammar::findFirst() {
  firstForTerminals();
  firstForEpsilonProductions();
  firstForNonterminals();
  return false;
}


/**
 * Does nothing right now
 * @return exit code
 */
bool Grammar::findFollow() {
  bool changed;
  do {
    changed = false;
    for (auto production : _productions) {
      size_t i = 0;
      char lhs = production[0];
      std::string rhs = production.substr(3);
      while (i < rhs.length()) {
        // Looping through each symbol.
        bool is_non_terminal = isNonTerminal(rhs[i]);
        if (is_non_terminal && i < rhs.length() - 1) {
          std::set<char> next_first = _first[rhs[i + 1]];
          bool has_epsilon = next_first.erase(EPSILON[0]);
          addSetToFollow(rhs[i], next_first, &changed);
          if (has_epsilon) {
            addSetToFollow(rhs[i], _follow[lhs], &changed);
          }
        } else if (is_non_terminal && i == rhs.length() - 1) {
          addSetToFollow(rhs[i], _follow[lhs], &changed);
        }
        ++i;
      }
    }
  } while(changed);
  return false;
}


/**
 * Applies the first rule for finding the first sets
 */
void Grammar::firstForTerminals() {
  for (auto terminal : _terminals) {
    _first[terminal].insert(terminal);
  }
}


/**
 * Applies the second rule for finding the first sets
 */
void Grammar::firstForEpsilonProductions() {
  for (auto production : _productions) {
    if (production.substr(3) == EPSILON) {
      _first[production[0]].insert(EPSILON[0]);
    }
  }
}


/**
 * Applies the third rule for finding the first set
 */
void Grammar::firstForNonterminals() {
  bool changed;
  do {
    changed = false;
    for (auto production : _productions) {
      size_t i = 0;
      char lhs = production[0];
      std::string rhs = production.substr(3);
      while (i < rhs.length()) {
        std::set<char> first = _first[rhs[i]];
        if (hasEpsilon(first)) {
          first.erase(first.find(EPSILON[0]));
          addSetToFirst(lhs, first, &changed);
          ++i;
        } else {
          addSetToFirst(lhs, first, &changed);
          break;
        }
        if (i >= rhs.length()) {
          addCharToFirst(lhs, EPSILON[0], &changed);
        }
        ++i;
      }
    }
  } while (changed);
}


/**
 * Add the given first set to the first of the given nonterminal
 * @param nonterminal -> The symbol whose first is being added to
 * @param first       -> The symbols being added
 * @param changed     -> A flag to see if anything actually changed
 */
void Grammar::addSetToFirst(char nonterminal,
                            std::set<char> first,
                            bool *changed) {
  for (auto symbol : first) {
    auto result = _first[nonterminal].insert(symbol);
    if (result.second) *changed = true;
  }
}


/**
 * Adds the given symbol to the first of the given nonterminal
 * @param nonterminal -> The symbol whose first is being added to
 * @param symbol      -> The symbol being added
 * @param changed     -> A flag to see if anything actually changed
 */
void Grammar::addCharToFirst(char nonterminal, char symbol, bool *changed) {
  auto result = _first[nonterminal].insert(symbol);
  if (result.second) *changed = true;
}


/**
 * Add the given follow set to the first of the given nonterminal
 * @param nonterminal -> The symbol whose follow is being added to
 * @param follow       -> The symbols being added
 * @param changed     -> A flag to see if anything actually changed
 */
void Grammar::addSetToFollow(char nonterminal,
                            std::set<char> follow,
                            bool *changed) {
  for (auto symbol : follow) {
    auto result = _follow[nonterminal].insert(symbol);
    if (result.second) *changed = true;
  }
}


/**
 * Checks whether epsilon is present in the given set.
 * @param  first -> The set being checked for epsilon
 * @return       -> The result of the test
 */
bool Grammar::hasEpsilon(std::set<char> first) {
  return first.find(EPSILON[0]) != first.end();
}


/**
 * Checks whether the given symbol is a non_terminal
 * @param  symbol -> The symbol being checked
 * @return        TRUE if a terminal, FALSE otherwise
 */
bool Grammar::isNonTerminal(char symbol) {
  return _non_terminals.find(symbol) != _non_terminals.end();
}

