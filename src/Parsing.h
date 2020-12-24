#pragma once

#ifndef PARSING_H_O
#define PARSING_H_O

#include <string>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include "cparse/shunting-yard.h"
#include "cparse/shunting-yard-exceptions.h"

class Statement {
 protected:
  virtual void _compile(const char* code, const char** rest,
                        TokenMap parent_scope) = 0;
  virtual packToken _exec(TokenMap scope) const = 0;

 public:
  virtual ~Statement() {}
  void compile(const char* code, const char** rest = 0,
               TokenMap parent_scope = &TokenMap::empty) {
    return _compile(code, rest, parent_scope);
  }

  packToken exec(TokenMap scope) const { return _exec(scope); }

  // This will allow each statement to be copied:
  virtual Statement* clone() const = 0;
};

class ExpStatement : public Statement {
  calculator expr;

 private:
  void _compile(const char* code, const char** rest, TokenMap parent_scope);
  packToken _exec(TokenMap scope) const;

 public:
  ExpStatement() {}
  ExpStatement(const char* code, const char** rest = 0,
               TokenMap parent_scope = &TokenMap::empty) {
    _compile(code, rest, parent_scope);
  }
  virtual Statement* clone() const { return new ExpStatement(*this); }
};

class BlockStatement : public Statement {
 public:
  typedef std::map<std::string, Statement* (*)()> statementMap_t;

  // Associate each type of statement with a keyword:
  static statementMap_t& statementMap() {
    static statementMap_t map;
    return map;
  }

  // Use this to register new statements on statementsMap.
  template<typename T>
  static Statement* factory() { return new T(); }

 private:
  typedef std::vector<Statement*> codeBlock_t;
  codeBlock_t list;

 private:
  void cleanList(codeBlock_t* list);

 private:
  void _compile(const char* code, const char** rest, TokenMap parent_scope);
  packToken _exec(TokenMap scope) const;

  Statement* buildStatement(const char** source, TokenMap scope);

 public:
  BlockStatement() {}

  // Implement The Big 3, for safely copying:
  BlockStatement(const BlockStatement& other);
  ~BlockStatement();
  BlockStatement& operator=(const BlockStatement& other);

  virtual Statement* clone() const {
    return new BlockStatement(*this);
  }
};
#endif
