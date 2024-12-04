typedef enum {
  INVALID_TOKEN = -1,
  IDENTIFIER = 1,
  CONSTANT = 2,
  INT_KW = 3,
  VOID_KW = 4,
  RET_KW = 5,
  OPEN_PAREN = 6,
  CLOSE_PAREN = 7,
  OPEN_BRACE = 8,
  CLOSE_BRACE = 9,
  SEMICOLON = 10,
} TokenName;
typedef struct {
  const TokenName token_name;
  const char *regex;
} Token;
typedef struct {
  const TokenName token_name;
  const char *value;
} TokenInstance;
