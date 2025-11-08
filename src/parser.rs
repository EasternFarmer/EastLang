/*
Order of Operations

  - Assignment
  - Logic operators (and, or, xor, not)
  - Comparison operators (==, !=, >, <, >=, <=)
  - Bitwise shift
  - AdditiveExpr
  - MultiplicitaveExpr
  - Call   - ~~Member~~ // no members as of now `a().b.c()` - Subscript
  - PrimaryExpr // callable, if, while definitions (because if everything is an expression, these need to be evaled first)
*/

use std::collections::VecDeque;

use crate::ast_types::*;
use crate::errors::*;
use crate::lexer;
use crate::lexer_types::*;

pub(crate) struct Parser {
    tokens: VecDeque<TokenType>,
}

impl Parser {
    pub(crate) fn new(source_code: &str) -> Result<Self, (Errors, String)> {
        let result = lexer::tokenize(source_code);

        if result.is_ok() {
            Ok(Self {
                tokens: result.unwrap(),
            })
        } else {
            Err(result.err().unwrap())
        }
    }

    #[allow(dead_code)]
    pub(crate) fn dump_tokens(self) -> VecDeque<TokenType> {
        self.tokens
    }

    pub(crate) fn parse(&mut self) -> Result<Ast, (Errors, String)> {
        let mut program_body: Vec<Box<Ast>> = Vec::new();
        while self.tokens.len() > 1 {
            // last one is EOF
            let result = Parser::parse_expression(self);
            if result.is_ok() {
                program_body.push(Box::new(result.unwrap()));
            } else {
                return result;
            }
        }
        return Ok(Ast::Program(program_body));
    }

    fn parse_expression(&mut self) -> Result<Ast, (Errors, String)> {
        return Parser::parse_assignment_expr(self);
    }

    fn parse_assignment_expr(&mut self) -> Result<Ast, (Errors, String)> {
        if self.tokens[0] == TokenType::Const {
            self.tokens.pop_front();

            let identifier: Box<str>;
            let result = Parser::parse_primitive_expr(self);
            if result.is_err() {
                return result;
            }
            match result.unwrap() {
                Ast::Identifier(iden) => {
                    identifier = iden;
                }
                _ => {
                    return Err((
                        Errors::SyntaxError,
                        "Expected a Identifier after the const keyword".to_owned(),
                    ));
                }
            }
            let right: Result<Ast, (Errors, String)>;
            if self.tokens[0] == TokenType::Equals {
                self.tokens.pop_front(); // pop the equals off
                right = Parser::parse_expression(self);
                if right.is_err() {
                    return right;
                }
            } else {
                return Err((
                    Errors::SyntaxError,
                    "Constant declaration needs a value to be declared".to_owned(),
                ));
            }
            return Ok(Ast::VariableDeclaration {
                identifier: identifier,
                right: Box::new(right.unwrap()),
                constant: true,
            });
        } else if self.tokens[0] == TokenType::Local {
            self.tokens.pop_front();

            let identifier: Ast;
            let result = Parser::parse_primitive_expr(self);
            if result.is_err() {
                return result;
            }
            let token = result.unwrap();
            match token {
                Ast::Identifier(_) => {
                    identifier = token;
                }
                _ => {
                    return Err((
                        Errors::SyntaxError,
                        "Expected a Identifier after the local keyword".to_owned(),
                    ));
                }
            }
            let right: Result<Ast, (Errors, String)>;
            if self.tokens[0] == TokenType::Equals {
                self.tokens.pop_front(); // pop the equals off
                right = Parser::parse_expression(self);
                if right.is_err() {
                    return right;
                }
            } else {
                right = Ok(Ast::Identifier("empty".to_string().into_boxed_str()));
            }
            return Ok(Ast::Assignment {
                identifier: Box::new(identifier),
                right: Box::new(right.unwrap()),
                local: true,
            });
        }
        let mut left = Parser::parse_logical_expr(self);
        if left.is_err() {
            return left;
        }

        while let Some(token) = self.tokens.front() {
            match token {
                TokenType::Equals => {
                    self.tokens.pop_front();

                    if self.tokens[0] == TokenType::EOF {
                        return Err((
                            Errors::SyntaxError,
                            "missing value after the Comparison operator".to_owned(),
                        ));
                    }

                    let right = Parser::parse_binary_shift(self);

                    if right.is_ok() {
                        left = Ok(Ast::Assignment {
                            identifier: Box::new(left.unwrap()),
                            right: Box::new(right.unwrap()),
                            local: false,
                        });
                    } else {
                        return right;
                    }
                }
                _ => break,
            }
        }
        left
    }

    fn parse_logical_expr(&mut self) -> Result<Ast, (Errors, String)> {
        if self.tokens[0] == TokenType::Not {
            self.tokens.pop_front();

            if self.tokens[0] == TokenType::EOF {
                return Err((
                    Errors::SyntaxError,
                    "missing value after the not operator".to_owned(),
                ));
            }

            let right = Parser::parse_comparison_expr(self);

            if right.is_ok() {
                return Ok(Ast::Not(Box::new(right.unwrap())));
            } else {
                return right;
            }
        }

        let mut left = Parser::parse_comparison_expr(self);
        if left.is_err() {
            return left;
        }

        while let Some(token) = self.tokens.front() {
            match token {
                TokenType::LogicalExpr(operator) => {
                    let op = *operator;
                    self.tokens.pop_front();

                    if self.tokens[0] == TokenType::EOF {
                        return Err((
                            Errors::SyntaxError,
                            "missing value after the Logical operator".to_owned(),
                        ));
                    }

                    let right = Parser::parse_comparison_expr(self);

                    if right.is_ok() {
                        left = Ok(Ast::LogicalExpr {
                            operator: op,
                            left: Box::new(left.unwrap()),
                            right: Box::new(right.unwrap()),
                        });
                    } else {
                        return right;
                    }
                }
                _ => break,
            }
        }
        left
    }

    fn parse_comparison_expr(&mut self) -> Result<Ast, (Errors, String)> {
        let mut left = Parser::parse_binary_shift(self);
        if left.is_err() {
            return left;
        }

        while let Some(token) = self.tokens.front() {
            match token {
                TokenType::ComparisonExpr(operator) => {
                    let op = *operator;
                    self.tokens.pop_front();

                    if self.tokens[0] == TokenType::EOF {
                        return Err((
                            Errors::SyntaxError,
                            "missing value after the Comparison operator".to_owned(),
                        ));
                    }

                    let right = Parser::parse_binary_shift(self);

                    if right.is_ok() {
                        left = Ok(Ast::Comparison {
                            operator: op,
                            left: Box::new(left.unwrap()),
                            right: Box::new(right.unwrap()),
                        });
                    } else {
                        return right;
                    }
                }
                _ => break,
            }
        }
        left
    }

    fn parse_binary_shift(&mut self) -> Result<Ast, (Errors, String)> {
        let mut left = Parser::parse_aditive_expr(self);
        if left.is_err() {
            return left;
        }

        while let Some(token) = self.tokens.front() {
            match token {
                TokenType::BitwiseShift(direction) => {
                    let dir = *direction;
                    self.tokens.pop_front();

                    if self.tokens[0] == TokenType::EOF {
                        return Err((
                            Errors::SyntaxError,
                            "missing value after the BitShift operator".to_owned(),
                        ));
                    }

                    let right = Parser::parse_aditive_expr(self);

                    if right.is_ok() {
                        left = Ok(Ast::BitwiseShift {
                            shift_direction: dir,
                            left: Box::new(left.unwrap()),
                            right: Box::new(right.unwrap()),
                        });
                    } else {
                        return right;
                    }
                }
                _ => break,
            }
        }
        left
    }

    fn parse_aditive_expr(&mut self) -> Result<Ast, (Errors, String)> {
        let mut left = Parser::parse_multiplicative_expr(self);
        if left.is_err() {
            return left;
        }

        while let Some(token) = self.tokens.front() {
            match token {
                TokenType::BinaryOperator(
                    operator @ (BinaryOperator::Add | BinaryOperator::Substract),
                ) => {
                    let op = *operator;
                    self.tokens.pop_front();

                    if self.tokens[0] == TokenType::EOF {
                        return Err((
                            Errors::SyntaxError,
                            "missing value after the Binary operator".to_owned(),
                        ));
                    }

                    let right = Parser::parse_multiplicative_expr(self);

                    if right.is_ok() {
                        left = Ok(Ast::BinaryOperator {
                            operator: op,
                            left: Box::new(left.unwrap()),
                            right: Box::new(right.unwrap()),
                        });
                    } else {
                        return right;
                    }
                }
                _ => break,
            }
        }
        left
    }

    fn parse_multiplicative_expr(&mut self) -> Result<Ast, (Errors, String)> {
        let mut left = Parser::parse_call_member_expr(self);
        if left.is_err() {
            return left;
        }

        while let Some(token) = self.tokens.front() {
            match token {
                TokenType::BinaryOperator(
                    operator @ (BinaryOperator::Multiply
                    | BinaryOperator::Divide
                    | BinaryOperator::Modulo),
                ) => {
                    let op = *operator;
                    self.tokens.pop_front();

                    if self.tokens[0] == TokenType::EOF {
                        return Err((
                            Errors::SyntaxError,
                            "missing value after the Binary operator".to_owned(),
                        ));
                    }

                    let right = Parser::parse_call_member_expr(self);

                    if right.is_ok() {
                        left = Ok(Ast::BinaryOperator {
                            operator: op,
                            left: Box::new(left.unwrap()),
                            right: Box::new(right.unwrap()),
                        });
                    } else {
                        return right;
                    }
                }
                _ => break,
            }
        }
        left
    }

    fn parse_call_member_expr(&mut self) -> Result<Ast, (Errors, String)> {
        //TODO: implement parse_call_member_expr
        let mut left = Parser::parse_primitive_expr(self);
        if left.is_err() {
            return left;
        }
        while self.tokens[0] == TokenType::Dot
            || self.tokens[0] == TokenType::OpenParen
            || self.tokens[0] == TokenType::OpenBracket
        {
            match self.tokens[0] {
                TokenType::Dot => {
                    self.tokens.pop_front(); // pop off the dot

                    let r_result = Parser::parse_primitive_expr(self);
                    if r_result.is_err() {
                        return r_result;
                    }

                    let right = r_result.unwrap();
                    if !matches!(right, Ast::Identifier(..)) {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a identifier after the dot expression".to_owned(),
                        ));
                    }

                    left = Ok(Ast::MemberExpression {
                        left: Box::new(left.unwrap()),
                        right: Box::new(right),
                    })
                }
                TokenType::OpenBracket => {
                    self.tokens.pop_front(); // pop off the opening bracket

                    if self.tokens[0] == TokenType::ClosedBracket {
                        return Err((
                            Errors::SyntaxError,
                            "Expected an expression inside of the square brackets".to_owned(),
                        ));
                    }

                    let right = Parser::parse_expression(self);
                    if right.is_err() {
                        return right;
                    }

                    if self.tokens[0] != TokenType::ClosedBracket {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a closing bracket after the membership expression".to_owned(),
                        ));
                    }
                    self.tokens.pop_front();

                    left = Ok(Ast::MemberExpression {
                        left: Box::new(left.unwrap()),
                        right: Box::new(right.unwrap()),
                    })
                }
                TokenType::OpenParen => {
                    self.tokens.pop_front(); // pop off the opening paren

                    if self.tokens[0] == TokenType::ClosedParen {
                        // handle no arguments
                        self.tokens.pop_front(); // pop off the closing paren
                        left = Ok(Ast::CallExpression {
                            left: Box::new(left.unwrap()),
                            arguments: Vec::new(),
                        });
                        continue;
                    }

                    let first_arg = Parser::parse_expression(self);
                    if first_arg.is_err() {
                        return first_arg;
                    }
                    let mut arg_vec = vec![first_arg.unwrap()];
                    while self.tokens[0] == TokenType::Comma {
                        self.tokens.pop_front();

                        let result = Parser::parse_expression(self);
                        if result.is_err() {
                            return result;
                        }

                        let arg = result.unwrap();
                        arg_vec.push(arg);
                    }

                    if self.tokens[0] != TokenType::ClosedParen {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a closing paren after the call expression".to_owned(),
                        ));
                    }
                    self.tokens.pop_front(); // pop off the closing paren

                    left = Ok(Ast::CallExpression {
                        left: Box::new(left.unwrap()),
                        arguments: arg_vec,
                    })
                }
                _ => {
                    return Err((
                        Errors::SyntaxError,
                        format!("Unexpected token {:?}", self.tokens[0]),
                    ));
                }
            }
        }
        left
    }

    fn parse_primitive_expr(&mut self) -> Result<Ast, (Errors, String)> {
        if let Some(token) = self.tokens.pop_front() {
            match token {
                // TODO: while, callable, open paren
                TokenType::String(string) => Ok(Ast::String(string.clone())),
                TokenType::Int(int) => Ok(Ast::Int(int)),
                TokenType::Float(float) => Ok(Ast::Float(float)),
                TokenType::Identifier(iden) => Ok(Ast::Identifier(iden.clone())),
                TokenType::If => {
                    let check = Parser::parse_expression(self);
                    if check.is_err() {
                        return check;
                    }

                    if self.tokens[0] != TokenType::OpenBrace {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a body after the if statement".to_owned(),
                        ));
                    }
                    self.tokens.pop_front(); // pop the opening brace

                    let mut body_vec: Vec<Ast> = Vec::new();
                    while self.tokens[0] != TokenType::ClosedBrace
                        && self.tokens[0] != TokenType::EOF
                    {
                        let result = Parser::parse_expression(self);
                        if result.is_err() {
                            return result;
                        }
                        body_vec.push(result.unwrap());
                    }

                    if self.tokens[0] != TokenType::ClosedBrace {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a closing brace for the if block".to_owned(),
                        ));
                    }
                    self.tokens.pop_front();

                    return Ok(Ast::IfExpression {
                        check: Box::new(check.unwrap()),
                        body: body_vec,
                    });
                }
                _ => Err((Errors::SyntaxError, format!("Unknown Token {:?}", token))),
            }
        } else {
            Err((Errors::SyntaxError, "Unexpected end of tokens".to_owned()))
        }
    }
}
