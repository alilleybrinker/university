/**
 * Copyright 2014 Andrew Brinker, Anthony Sterrett
 */

#include <cstdint>
#include <vector>
#include <list>
#include <string>

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&);


class Assembler {
 public:
  typedef std::list<std::string> ASMSource;
  typedef std::list<uint16_t> ObjectSource;

  Assembler();
  std::string parse(std::string);

 private:
  void checkFileName(std::string);

  void reportError(std::exception &);

  DISALLOW_COPY_AND_ASSIGN(Assembler);
};

#endif  // ASSEMBLER_H
