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
        let tokens = lexer::tokenize(source_code)?;
        Ok(Self { tokens })
    }

    #[allow(dead_code)]
    pub(crate) fn dump_tokens(self) -> VecDeque<TokenType> {
        self.tokens
    }

    pub(crate) fn parse(&mut self) -> Result<Ast, (Errors, String)> {
        let mut program_body: Vec<Box<Ast>> = Vec::new();
        while self.tokens.len() > 1 {
            // last one is EOF
            let result = Parser::parse_expression(self)?;
            program_body.push(Box::new(result));
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
            let result = Parser::parse_primitive_expr(self)?;
            match result {
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
            let right: Ast;
            if self.tokens[0] == TokenType::Equals {
                self.tokens.pop_front(); // pop the equals off
                right = Parser::parse_expression(self)?;
            } else {
                return Err((
                    Errors::SyntaxError,
                    "Constant declaration needs a value to be declared".to_owned(),
                ));
            }
            return Ok(Ast::VariableDeclaration {
                identifier: identifier,
                right: Box::new(right),
                constant: true,
            });
        } else if self.tokens[0] == TokenType::Local {
            self.tokens.pop_front();

            let identifier: Ast;
            let token = Parser::parse_primitive_expr(self)?;
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
            let right: Ast;
            if self.tokens[0] == TokenType::Equals {
                self.tokens.pop_front(); // pop the equals off
                right = Parser::parse_expression(self)?;
            } else {
                right = Ast::Identifier("empty".to_string().into_boxed_str());
            }
            return Ok(Ast::Assignment {
                identifier: Box::new(identifier),
                right: Box::new(right),
                local: true,
            });
        }
        let mut left = Parser::parse_logical_expr(self)?;

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

                    let right = Parser::parse_binary_shift(self)?;

                    left = Ast::Assignment {
                        identifier: Box::new(left),
                        right: Box::new(right),
                        local: false,
                    };
                }
                _ => break,
            }
        }
        Ok(left)
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

            let right = Parser::parse_comparison_expr(self)?;

            return Ok(Ast::Not(Box::new(right)));
        }

        let mut left = Parser::parse_comparison_expr(self)?;

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

                    let right = Parser::parse_comparison_expr(self)?;

                    left = Ast::LogicalExpr {
                        operator: op,
                        left: Box::new(left),
                        right: Box::new(right),
                    };
                }
                _ => break,
            }
        }
        Ok(left)
    }

    fn parse_comparison_expr(&mut self) -> Result<Ast, (Errors, String)> {
        let mut left = Parser::parse_binary_shift(self)?;

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

                    let right = Parser::parse_binary_shift(self)?;

                    left = Ast::Comparison {
                        operator: op,
                        left: Box::new(left),
                        right: Box::new(right),
                    };
                }
                _ => break,
            }
        }
        Ok(left)
    }

    fn parse_binary_shift(&mut self) -> Result<Ast, (Errors, String)> {
        let mut left = Parser::parse_aditive_expr(self)?;

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

                    let right = Parser::parse_aditive_expr(self)?;

                    left = Ast::BitwiseShift {
                        shift_direction: dir,
                        left: Box::new(left),
                        right: Box::new(right),
                    };
                }
                _ => break,
            }
        }
        Ok(left)
    }

    fn parse_aditive_expr(&mut self) -> Result<Ast, (Errors, String)> {
        let mut left = Parser::parse_multiplicative_expr(self)?;

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

                    let right = Parser::parse_multiplicative_expr(self)?;

                    left = Ast::BinaryOperator {
                        operator: op,
                        left: Box::new(left),
                        right: Box::new(right),
                    };
                }
                _ => break,
            }
        }
        Ok(left)
    }

    fn parse_multiplicative_expr(&mut self) -> Result<Ast, (Errors, String)> {
        let mut left = Parser::parse_call_member_expr(self)?;

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

                    let right = Parser::parse_call_member_expr(self)?;

                    left = Ast::BinaryOperator {
                        operator: op,
                        left: Box::new(left),
                        right: Box::new(right),
                    };
                }
                _ => break,
            }
        }
        Ok(left)
    }

    fn parse_call_member_expr(&mut self) -> Result<Ast, (Errors, String)> {
        let mut left = Parser::parse_primitive_expr(self)?;
        while self.tokens[0] == TokenType::Dot
            || self.tokens[0] == TokenType::OpenParen
            || self.tokens[0] == TokenType::OpenBracket
        {
            match self.tokens[0] {
                TokenType::Dot => {
                    self.tokens.pop_front(); // pop off the dot

                    let right = Parser::parse_primitive_expr(self)?;

                    if !matches!(right, Ast::Identifier(_)) {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a identifier after the dot expression".to_owned(),
                        ));
                    }

                    left = Ast::MemberExpression {
                        left: Box::new(left),
                        right: Box::new(right),
                    }
                }
                TokenType::OpenBracket => {
                    self.tokens.pop_front(); // pop off the opening bracket

                    if self.tokens[0] == TokenType::ClosedBracket {
                        return Err((
                            Errors::SyntaxError,
                            "Expected an expression inside of the square brackets".to_owned(),
                        ));
                    }

                    let right = Parser::parse_expression(self)?;

                    if self.tokens[0] != TokenType::ClosedBracket {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a closing bracket after the membership expression".to_owned(),
                        ));
                    }
                    self.tokens.pop_front();

                    left = Ast::MemberExpression {
                        left: Box::new(left),
                        right: Box::new(right),
                    }
                }
                TokenType::OpenParen => {
                    self.tokens.pop_front(); // pop off the opening paren

                    if self.tokens[0] == TokenType::ClosedParen {
                        // handle no arguments
                        self.tokens.pop_front(); // pop off the closing paren
                        left = Ast::CallExpression {
                            left: Box::new(left),
                            arguments: Vec::new(),
                        };
                        continue;
                    }

                    let first_arg = Parser::parse_expression(self)?;
                    let mut arg_vec = vec![first_arg];
                    while self.tokens[0] == TokenType::Comma {
                        self.tokens.pop_front();

                        let arg = Parser::parse_expression(self)?;

                        arg_vec.push(arg);
                    }

                    if self.tokens[0] != TokenType::ClosedParen {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a closing paren after the call expression".to_owned(),
                        ));
                    }
                    self.tokens.pop_front(); // pop off the closing paren

                    left = Ast::CallExpression {
                        left: Box::new(left),
                        arguments: arg_vec,
                    }
                }
                _ => {
                    return Err((
                        Errors::SyntaxError,
                        format!("Unexpected token {:?}", self.tokens[0]),
                    ));
                }
            }
        }
        Ok(left)
    }

    fn parse_call_arg(
        &mut self,
        parameters: &mut Vec<(Ast, Option<Ast>)>,
    ) -> Option<(Errors, String)> {
        let param: Ast;
        match Parser::parse_primitive_expr(self) {
            Ok(x) => param = x,
            Err(err) => return Some(err),
        }
        match param {
            Ast::Identifier(_) => {
                if self.tokens[0] != TokenType::Equals {
                    parameters.push((param, None));
                    return None;
                }
                self.tokens.pop_front(); // remove the equals
                match Parser::parse_expression(self) {
                    Ok(x) => parameters.push((param, Some(x))),
                    Err(err) => return Some(err),
                }
            }
            _ => {
                return Some((
                    Errors::SyntaxError,
                    "Expected a identifier in a callable parameter".to_owned(),
                ));
            }
        }
        return None;
    }

    fn parse_primitive_expr(&mut self) -> Result<Ast, (Errors, String)> {
        if let Some(token) = self.tokens.pop_front() {
            match token {
                TokenType::String(string) => Ok(Ast::String(string.clone())),
                TokenType::Int(int) => Ok(Ast::Int(int)),
                TokenType::Float(float) => Ok(Ast::Float(float)),
                TokenType::Identifier(iden) => Ok(Ast::Identifier(iden.clone())),
                TokenType::OpenParen => {
                    let result = Parser::parse_expression(self);
                    if result.is_err() {
                        return result;
                    }

                    if self.tokens[0] != TokenType::ClosedParen {
                        return Err((Errors::SyntaxError, "Expected closing paren".to_owned()));
                    }
                    self.tokens.pop_front();

                    result
                }
                TokenType::If => {
                    let check = Parser::parse_expression(self)?;

                    if self.tokens[0] != TokenType::OpenBrace {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a body after the if check".to_owned(),
                        ));
                    }
                    self.tokens.pop_front(); // pop the opening brace

                    let mut body_vec: Vec<Ast> = Vec::new();
                    while self.tokens[0] != TokenType::ClosedBrace
                        && self.tokens[0] != TokenType::EOF
                    {
                        let expr = Parser::parse_expression(self)?;
                        body_vec.push(expr);
                    }

                    if self.tokens[0] != TokenType::ClosedBrace {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a closing brace for the if block".to_owned(),
                        ));
                    }
                    self.tokens.pop_front();

                    return Ok(Ast::IfExpression {
                        check: Box::new(check),
                        body: body_vec,
                    });
                }
                TokenType::While => {
                    let check = Parser::parse_expression(self)?;

                    if self.tokens[0] != TokenType::OpenBrace {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a body after the while check".to_owned(),
                        ));
                    }
                    self.tokens.pop_front(); // pop the opening brace

                    let mut body_vec: Vec<Ast> = Vec::new();
                    while self.tokens[0] != TokenType::ClosedBrace
                        && self.tokens[0] != TokenType::EOF
                    {
                        let expr = Parser::parse_expression(self)?;
                        body_vec.push(expr);
                    }

                    if self.tokens[0] != TokenType::ClosedBrace {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a closing brace for the while block".to_owned(),
                        ));
                    }
                    self.tokens.pop_front();

                    return Ok(Ast::WhileExpression {
                        check: Box::new(check),
                        body: body_vec,
                    });
                }
                TokenType::Callable => {
                    if self.tokens[0] != TokenType::OpenParen {
                        return Err((
                            Errors::SyntaxError,
                            "Expected Opening Paren after the callable keyword".to_owned(),
                        ));
                    }
                    self.tokens.pop_front(); // eat the open paren

                    let mut parameters: Vec<(Ast, Option<Ast>)> = Vec::new();

                    if self.tokens[0] != TokenType::ClosedParen {
                        if self.tokens[0] == TokenType::EOF {
                            return Err((
                                Errors::SyntaxError,
                                "Expected a closing paren to the callable expression".to_owned(),
                            ));
                        }
                        match Parser::parse_call_arg(self, &mut parameters) {
                            Some(err) => return Err(err),
                            None => {}
                        }
                        while self.tokens[0] != TokenType::ClosedParen
                            && self.tokens[0] == TokenType::Comma
                        {
                            self.tokens.pop_front(); // pop off the comma
                            match Parser::parse_call_arg(self, &mut parameters) {
                                Some(err) => return Err(err),
                                None => {}
                            }
                        }
                    }
                    if self.tokens[0] != TokenType::ClosedParen {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a closing paren to the callable expression".to_owned(),
                        ));
                    }

                    self.tokens.pop_front(); // eat the closed paren

                    if self.tokens[0] != TokenType::OpenBrace {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a body after the callable params".to_owned(),
                        ));
                    }
                    self.tokens.pop_front(); // pop the opening brace

                    let mut body = Vec::new();
                    while self.tokens[0] != TokenType::ClosedBrace
                        && self.tokens[0] != TokenType::EOF
                    {
                        let expr = Parser::parse_expression(self)?;
                        body.push(expr);
                    }

                    if self.tokens[0] != TokenType::ClosedBrace {
                        return Err((
                            Errors::SyntaxError,
                            "Expected a closing brace after the callable block".to_owned(),
                        ));
                    }
                    self.tokens.pop_front();

                    return Ok(Ast::CallableExpression { parameters, body });
                }
                _ => Err((Errors::SyntaxError, format!("Unknown Token {:?}", token))),
            }
        } else {
            Err((Errors::SyntaxError, "Unexpected end of tokens".to_owned()))
        }
    }
}
