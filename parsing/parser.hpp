#include "ast.hpp"
#include "lexer.hpp"
#include <deque>

class Parser {
  private:
    std::deque<Token> tokens;
    bool not_eof();
    Token curr();
    Token look_ahead(int n);
    Token advance();
    Token expect(TokenType expected_token, std::string error_message);

    OperatorType get_math_operator(std::string operatorLiteral);
    LogicalOperatorType get_logical_operator(std::string operatorLiteral);
    ComparisonOperatorType get_comparison_operator(std::string operatorLiteral);

  public:
    Program* parse_ast(std::string& sourceCode);
  
  private:
    Expr* parse_expr();
    Expr* parse_assignment_expr();
    Expr* parse_logical_expr();
    Expr* parse_comparison_expr();
    Expr* parse_additive_expr();
    Expr* parse_multiplicative_expr();
    Expr* parse_call_member_expr();
    Expr* parse_call_expr(Expr* caller);
    std::vector<Expr*> parse_call_args();
    Expr* parse_primary_expr();
};