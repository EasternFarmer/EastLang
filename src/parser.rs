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
        return Parser::parse_logical_expr(self); // later change to assignment
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

            let right = Parser::parse_expression(self);

            if right.is_ok() {
                return Ok(Ast::Not(Box::new(right.unwrap())));
            } else {
                return right;
            }
        }

        let left = Parser::parse_comparison_expr(self);
        if left.is_err() {
            return left;
        }
        match self.tokens[0] {
            TokenType::LogicalExpr(logical_expr) => {
                self.tokens.pop_front();
                if self.tokens[0] == TokenType::EOF {
                    return Err((
                        Errors::SyntaxError,
                        "missing value after the logical operator".to_owned(),
                    ));
                }
                let right = Parser::parse_comparison_expr(self);

                if right.is_ok() {
                    return Ok(Ast::LogicalExpr {
                        operator: logical_expr,
                        left: Box::new(left.unwrap()),
                        right: Box::new(right.unwrap()),
                    });
                } else {
                    return right;
                }
            }
            _ => left,
        }
    }

    fn parse_comparison_expr(&mut self) -> Result<Ast, (Errors, String)> {
        let left = Parser::parse_binary_shift(self);
        if left.is_err() {
            return left;
        }

        match self.tokens[0] {
            TokenType::ComparisonExpr(comp_type) => {
                self.tokens.pop_front();

                if self.tokens[0] == TokenType::EOF {
                    return Err((
                        Errors::SyntaxError,
                        "missing value after the comparison operator".to_owned(),
                    ));
                }

                let right = Parser::parse_binary_shift(self);

                if right.is_ok() {
                    return Ok(Ast::Comparison {
                        operator: comp_type,
                        left: Box::new(left.unwrap()),
                        right: Box::new(right.unwrap()),
                    });
                } else {
                    return right;
                }
            }
            _ => left,
        }
    }

    fn parse_binary_shift(&mut self) -> Result<Ast, (Errors, String)> {
        let left = Parser::parse_aditive_expr(self);
        if left.is_err() {
            return left;
        }

        match self.tokens[0] {
            TokenType::BitwiseShift(shift_direction) => {
                self.tokens.pop_front();

                if self.tokens[0] == TokenType::EOF {
                    return Err((
                        Errors::SyntaxError,
                        "missing value after the BitwiseShift operator".to_owned(),
                    ));
                }

                let right = Parser::parse_aditive_expr(self);

                if right.is_ok() {
                    return Ok(Ast::BitwiseShift {
                        shift_direction: shift_direction,
                        left: Box::new(left.unwrap()),
                        right: Box::new(right.unwrap()),
                    });
                } else {
                    return right;
                }
            }
            _ => left,
        }
    }

    fn parse_aditive_expr(&mut self) -> Result<Ast, (Errors, String)> { // TODO: Fix this and multiplicative by using a while loop
        let left = Parser::parse_multiplicative_expr(self);
        if left.is_err() {
            return left;
        }

        match self.tokens[0] {
            TokenType::BinaryOperator(
                operator @ (BinaryOperator::Add | BinaryOperator::Substract),
            ) => {
                self.tokens.pop_front();

                if self.tokens[0] == TokenType::EOF {
                    return Err((
                        Errors::SyntaxError,
                        "missing value after the Binary operator".to_owned(),
                    ));
                }

                let right = Parser::parse_multiplicative_expr(self);

                if right.is_ok() {
                    return Ok(Ast::BinaryOperator {
                        operator: operator,
                        left: Box::new(left.unwrap()),
                        right: Box::new(right.unwrap()),
                    });
                } else {
                    return right;
                }
            }
            _ => left,
        }
    }

    fn parse_multiplicative_expr(&mut self) -> Result<Ast, (Errors, String)> {
        let left = Parser::parse_call_member_expr(self);
        if left.is_err() {
            return left;
        }

        match self.tokens[0] {
            TokenType::BinaryOperator(
                operator @ (BinaryOperator::Multiply
                | BinaryOperator::Divide
                | BinaryOperator::Modulo),
            ) => {
                self.tokens.pop_front();

                if self.tokens[0] == TokenType::EOF {
                    return Err((
                        Errors::SyntaxError,
                        "missing value after the Binary operator".to_owned(),
                    ));
                }

                let right = Parser::parse_call_member_expr(self);

                if right.is_ok() {
                    return Ok(Ast::BinaryOperator {
                        operator: operator,
                        left: Box::new(left.unwrap()),
                        right: Box::new(right.unwrap()),
                    });
                } else {
                    return right;
                }
            }
            _ => left,
        }
    }

    fn parse_call_member_expr(&mut self) -> Result<Ast, (Errors, String)> {
        //TODO: implement parse_call_member_expr
        return Parser::parse_primitive_expr(self);
    }

    fn parse_primitive_expr(&mut self) -> Result<Ast, (Errors, String)> {
        if let Some(token) = self.tokens.pop_front() {
            match token {
                TokenType::String(string) => {
                    Ok(Ast::String(string.clone()))
                }
                TokenType::Int(int) => {
                    Ok(Ast::Int(int))
                }
                TokenType::Float(float) => {
                    Ok(Ast::Float(float))
                }
                TokenType::Identifier(iden) => {
                    Ok(Ast::Identifier(iden.clone()))
                }
                _ => Err((Errors::SyntaxError, format!("Unknown Token {:?}", token)))
            }
        } else {
            Err((Errors::SyntaxError, "Unexpected end of tokens".to_owned()))
        }
    }
}
