/**
 * Copyright 2014 Andrew Brinker, Anthony Sterrett
 */

#include "./Utilities.h"

#include <sys/stat.h>
#include <cmath>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>

#define PATH_SEP      "/"
#define EXT_SEP       "."
#define COMMENT_SEP   "!"
#define ASM_FILE_EXT  ".s"
#define OBJ_FILE_EXT  ".o"
#define ASM_MODE       0
#define VM_MODE        1

/**
 * Check whether the given file name is a valid assembly source file.
 *
 * The only standard for validity is that it ends with a ".s" extension. That is
 * what this function checks for.
 *
 * @param  file_name -> The name of the assembly source file being checked.
 * @return whether the name is valid or not.
 */
bool isFileNameValid(const std::string file_name, const int mode) {
  size_t pos = file_name.find_last_of(EXT_SEP);
  std::string extension = "";
  if (pos != std::string::npos) {
    extension = file_name.substr(pos);
  }
  switch (mode) {
    case ASM_MODE:
      return extension == ASM_FILE_EXT;
    case VM_MODE:
      return extension == OBJ_FILE_EXT;
    default:
      return false;
  }
}


/**
 * Check whether the given file name exists.
 * @param  file_name -> The name of the file being checked.
 * @return whether the file exists.
 */
bool doesFileExist(std::string file_name) {
  struct stat buffer;
  return (stat (file_name.c_str(), &buffer) == 0);
}


/**
 * Remove the file extension from the given file name
 * @param  file_name -> The file name being stripped.
 * @return the stripped file name.
 */
std::string stripExtension(std::string file_name) {
  size_t pos = file_name.find_last_of(EXT_SEP);
  if (pos != std::string::npos) {
    return file_name.substr(0, pos);
  }
  return file_name;
}


/**
 * Remove the path from the given file name
 * @param  file_name -> The file name being stripped.
 * @return the stripped file name.
 */
std::string stripPath(std::string file_name) {
  size_t pos = file_name.find_last_of(PATH_SEP);
  if (pos != std::string::npos) {
    return file_name.substr(pos + 1, std::string::npos);
  }
  return file_name;
}


/**
 * Pad the given line with the fill character up to the given size
 * @param line        -> The line to be padded
 * @param fill        -> The filler character to use
 * @param target_size -> The target size
 */
void pad(std::string *line, const char fill, size_t target_size) {
  size_t length = line->length();
  if (length >= target_size) return;
  while (length < target_size) {
    line->push_back(fill);
    ++length;
  }
}


/**
 * Convert a given integer into its signed binary string representation
 * @param  original -> The integer to be converted
 * @param  bits     -> The number of bits to use (including a bit for the sign)
 * @return either the converted string or an empty string
 */
std::string toSignedBinaryString(const int original, const int bits) {
  // Check the sanity of the inputs.
  const float exponent = pow(2, bits - 1);
  if (original < -exponent || exponent - 1 < original || bits > 64) return "";
  // Do the conversion.
  std::string converted = "";
  bool first = false;
  for (int i = bits - 1; i >= 0; --i) {
    if ((original & (1ULL << i)) != 0) {
      if (!first) first = true;
      converted += '1';
    } else if (first) {
      converted += '0';
    }
  }
  // Pad the front by appending to the end of the string and then rotating the
  // string appropriately.
  const size_t size = converted.size();
  for (size_t i = 0; i < bits - size; ++i) converted.push_back('0');
  std::rotate(converted.begin(), converted.begin() + size, converted.end());
  return converted;
}


/**
 * Convert a given integer into its unsigned binary string representation.
 * @param  original -> The integer to be converted
 * @param  bits     -> The number of bits to use
 * @return either the converted string, or an empty string
 */
std::string toUnsignedBinaryString(const int original, const int bits) {
  std::string s = toSignedBinaryString(original, bits + 1);
  if (s[0] != '0') return "";
  return s.substr(1);
}


/**
 * Convert the given binary string into a decimal string of the same number
 * @param  original -> The string to be converted
 * @return the converted string
 */
int binaryStringToDecimal(const std::string original) {
  int value = 0;
  const int size = original.size();
  for (int i = 0; i < size; ++i) {
    if (original[size - 1 - i] == '1') {
      value += pow(2, i);
    }
  }
  return value;
}


/**
 * Remove any comments from the given line of code
 * @param  line -> The line to be stripped.
 * @return the stripped line.
 */
std::string stripComments(std::string line) {
  size_t pos = line.find_last_of(COMMENT_SEP);
  if (pos != std::string::npos) {
    return line.substr(0, pos);
  }
  return line;
}


/**
 * Remove whitespace from the end of a string
 * @param  line -> The string to be stripped
 * @return the stripped string
 */
std::string stripEndingWhitespace(std::string line) {
  int i = line.length();
  do {
    --i;
  } while (isspace(line[i]));
  return line.substr(0, i + 1);
}


/**
 * Split the given string into a vector of its space-delimited parts
 * @param line -> The line to be split.
 * @return the split line.
 */
std::vector<std::string> split(std::string line) {
  std::vector<std::string> tokens;
  std::istringstream stream(line);
  std::copy(std::istream_iterator<std::string>(stream),
            std::istream_iterator<std::string>(),
            std::back_inserter<std::vector<std::string>>(tokens));  // NOLINT
  return tokens;
}


/**
 * Convert the given integer into a hexadecimal string representation
 * @param  n    -> The integer being converted
 * @param  size -> The size of the output string
 * @return the hexadecimal string representation of the integer
 */
std::string hex(uintmax_t n, uintmax_t size) {
  std::string binstr;
  do {
    binstr.push_back(((n & 0xf) == 0x0) ? '0' :
                      ((n & 0xf) == 0x1) ? '1' :
                      ((n & 0xf) == 0x2) ? '2' :
                      ((n & 0xf) == 0x3) ? '3' :
                      ((n & 0xf) == 0x4) ? '4' :
                      ((n & 0xf) == 0x5) ? '5' :
                      ((n & 0xf) == 0x6) ? '6' :
                      ((n & 0xf) == 0x7) ? '7' :
                      ((n & 0xf) == 0x8) ? '8' :
                      ((n & 0xf) == 0x9) ? '9' :
                      ((n & 0xf) == 0xa) ? 'a' :
                      ((n & 0xf) == 0xb) ? 'b' :
                      ((n & 0xf) == 0xc) ? 'c' :
                      ((n & 0xf) == 0xd) ? 'd' :
                      ((n & 0xf) == 0xe) ? 'e' : 'f');
  } while (n >>= 4);
  while (binstr.size() < size) {
    binstr.push_back('0');
  }
  std::reverse(binstr.begin(), binstr.end());
  return binstr;
}


/**
 * Check to see if the given string has the second string as a suffix.
 * @param str -> The string
 * @param suf -> The suffix
 * @return whether suf is a suffix of str
 */
bool hasSuffix(std::string str, std::string suf) {
  std::string::iterator it = str.begin();
  while (static_cast<unsigned>(str.end() - it) > suf.size()) {
    ++it;
  }
  return std::equal(it, str.end(), suf.begin());
}
