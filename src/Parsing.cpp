#include "Parsing.h"

void ExpStatement::_compile(const char* code, const char** rest,
                            TokenMap parent_scope) {
  // The string ";}\n" list the delimiters I want for my programming language.
  // Feel free to change it to something more adequate for your own language.
  expr.compile(code, parent_scope, ";}\n", &code);

  // Skip the delimiter character:
  if (*code && *code != '}') ++code;

  if (rest) *rest = code;
}

packToken ExpStatement::_exec(TokenMap scope) const {
  return expr.eval(scope);
}

Statement* BlockStatement::buildStatement(const char** source, TokenMap scope) {
  const char* code = *source;

  // If it is a block statement:
  if (*code == '{') {
    Statement* stmt = new BlockStatement();
    stmt->compile(code, source, scope);
    return stmt;
  }

  // Parse the first word of the text:
  std::string name = rpnBuilder::parseVar(code);

  // Check if it is a reserved word:
  statementMap_t& stmt_map = statementMap();
  auto it = stmt_map.find(name);
  if (it != stmt_map.end()) {
    // If it is parse it and return:
    Statement* stmt = it->second();
    stmt->compile(code+name.size(), source, scope);
    return stmt;
  }

  // Return a normal statement:
  return new ExpStatement(code, source, scope);
}

void BlockStatement::cleanList(codeBlock_t* list) {
  for(auto stmt : *list) {
    delete stmt;
  }

  list->clear();
}

BlockStatement::BlockStatement(const BlockStatement& other) {
  for(const Statement* stmt : other.list) {
    list.push_back(stmt->clone());
  }
}

BlockStatement& BlockStatement::operator=(const BlockStatement& other) {
  cleanList(&list);
  for(const Statement* stmt : other.list) {
    list.push_back(stmt->clone());
  }
  return *this;
}

BlockStatement::~BlockStatement() {
  cleanList(&list);
}

void BlockStatement::_compile(const char* code, const char** rest,
                              TokenMap parent_scope) {
  // Make sure the list is empty:
  cleanList(&list);

  while (isspace(*code)) ++code;

  if (*code == '{') {

    // Find the next non-blank character:
    ++code;
    while (isspace(*code)) ++code;

    // Parse each statement of the block:
    while (*code && *code != '}') {
      // Ignore empty statements:
      if (strchr(";\n", *code)) {
        ++code;
      } else {
        list.push_back(buildStatement(&code, parent_scope));
      }

      // Discard blank spaces:
      while (isspace(*code)) ++code;
    }

    if (*code == '}') {
      ++code;
    } else {
      throw syntax_error("Missing a '}' somewhere on the code!");
    }
  } else {
    list.push_back(buildStatement(&code, parent_scope));
  }

  if (rest) *rest = code;
}

packToken BlockStatement::_exec(TokenMap scope) const {
  // Returned value:
  packToken rv;
  for(const auto stmt : list) {
    // In a more complete implementation, `rv` should
    // be checked for "return" or "throw" behaviors.
    rv = stmt->exec(scope);
  }

  return rv;
}
