#include "lexer/lexer.h"
#include "lexer/token.h"
#include <ctype.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token token_definitions[] = {
    {IDENTIFIER, "[a-zA-Z_][a-zA-Z0-9_]*"}, // Match valid identifiers
    {CONSTANT, "[0-9]+"},                   // Match numeric constants
    {INT_KW,
     "(^|[^a-zA-Z0-9_])int([^a-zA-Z0-9_]|$)"}, // Match "int" as a whole word
    {VOID_KW,
     "(^|[^a-zA-Z0-9_])void([^a-zA-Z0-9_]|$)"}, // Match "void" as a whole word
    {RET_KW, "(^|[^a-zA-Z0-9_])return([^a-zA-Z0-9_]|$)"}, // Match "return" as a
                                                          // whole word
    {OPEN_PAREN, "("},                                    // Match literal '('
    {CLOSE_PAREN, ")"},                                   // Match literal ')'
    {OPEN_BRACE, "{"},                                    // Match literal '{'
    {CLOSE_BRACE, "}"},                                   // Match literal '}'
    {SEMICOLON, ";"},                                     // Match literal ';'
};

#define TOKEN_DEFINITIONS_COUNT                                                \
  sizeof(token_definitions) / sizeof(token_definitions[0])
char *ltrim(char *str) {
  while (isspace((unsigned char)*str))
    str++;
  return str;
}

TokenInstance *create_token_instance(TokenName token_name, const char *value) {
  TokenInstance *instance = malloc(sizeof(TokenInstance));
  if (instance == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  memcpy(instance, &(TokenInstance){.token_name = token_name, .value = value},
         sizeof(TokenInstance));
  return instance;
}

int32_t main(int32_t argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  const char *filename = argv[1];
  FILE *file_ptr = fopen(filename, "r");
  if (file_ptr == NULL) {
    fprintf(stderr, "Error opening file");
    return 1;
  }

  // TODO: use dynamic array instead
  TokenInstance *token_instances[1024];
  int token_instances_idx = 0;

  char *buffer = NULL;
  ssize_t nread;
  size_t len = 0;

  while ((nread = getline(&buffer, &len, file_ptr)) != -1) {
    regmatch_t match;
    char *buffer_ptr = buffer;

    while (*buffer_ptr != '\0') {
      buffer_ptr = ltrim(buffer_ptr);
      if (*buffer_ptr == '\0') {
        break;
      }
      TokenName token_name;
      match.rm_so = INT_MAX;
      match.rm_eo = INT_MAX;

      for (int i = 0; i < TOKEN_DEFINITIONS_COUNT; ++i) {
        regex_t compiled_pattern;
        if (regcomp(&compiled_pattern, token_definitions[i].regex, 0) != 0) {
          fprintf(stderr, "Failed to compile regex: %s\n",
                  token_definitions[i].regex);
          return 1;
        }
        regmatch_t match_arr[1];
        if (regexec(&compiled_pattern, buffer_ptr, 1, match_arr, 0) == 0 &&
            match_arr[0].rm_so < match.rm_so) {
          match = match_arr[0];
          token_name = token_definitions[i].token_name;
        }
      }
      if (match.rm_so == INT_MAX) {
        fprintf(stderr, "Unrecognized token, last token: %s\n",
                token_instances[token_instances_idx - 1]->value);
        return 1;
      }
      int token_idx = match.rm_so;
      int token_byte_len = match.rm_eo - match.rm_so;
      char *value = malloc(token_byte_len + 1);
      strncpy(value, buffer_ptr, token_byte_len);
      value[token_byte_len] = '\0';
      token_instances[token_instances_idx++] =
          create_token_instance(token_name, value);
      buffer_ptr += token_byte_len;
    }
  }

  for (int i = 0; i < token_instances_idx; ++i) {
    TokenInstance *instance = token_instances[i];
    printf("Token: %d, Value: %s\n", instance->token_name, instance->value);
  }

  fclose(file_ptr);

  return 0;
}
