use std::io;
use std::io::Write;

mod errors;
mod lexer_types;
mod ast_types;
mod lexer;
mod parser;

use lexer_types::TokenType;
use ast_types::Ast;

#[allow(dead_code)]
fn log_token(token: &TokenType) {
    match token {
        TokenType::Int(value) => print!("Int({}) ", value),
        TokenType::Identifier(value) => print!("Identifier({}) ", value),
        TokenType::String(value) => print!("String({}) ", value),
        TokenType::Float(value) => print!("Float({}) ", value),
        TokenType::Callable => print!("Callable() "),
        TokenType::Const => print!("Const() "),
        TokenType::Local => print!("Local() "),
        TokenType::BinaryOperator(value) => print!("BinaryOperator({:?}) ", value),
        TokenType::BitwiseShift(value) => print!("BitwiseShift({:?}) ", value),
        TokenType::Equals => print!("Equals() "),
        TokenType::If => print!("If() "),
        TokenType::Else => print!("Else() "),
        TokenType::ElseIf => print!("ElseIf() "),
        TokenType::While => print!("While() "),
        TokenType::LogicalExpr(value) => print!("LogicalExpr({:?}) ", value),
        TokenType::Not => print!("Not() "),
        TokenType::ComparisonExpr(value) => print!("ComparisonExpr({:?}) ", value),
        TokenType::OpenParen => print!("OpenParen() "),
        TokenType::ClosedParen => print!("ClosedParen() "),
        TokenType::OpenBrace => print!("OpenBrace() "),
        TokenType::ClosedBrace => print!("ClosedBrace() "),
        TokenType::OpenBracket => print!("OpenBracket() "),
        TokenType::ClosedBracket => print!("ClosedBracket() "),
        TokenType::Comma => print!("Comma() "),
        TokenType::Dot => print!("Dot() "),
        TokenType::Monkey => print!("Monkey() "),
        TokenType::EOF => print!("EOF() "),
        TokenType::Colon => print!("Colon() "),
        TokenType::SemiColon => print!("SemiColon() "),
    }
}

fn log_ast(program: &Ast, indent: i16) {
    let mut indent_str = String::new();
    for _ in 0..indent {
        indent_str.push_str(" ");
    }
    match &program {
        Ast::Program(asts) => {
            println!("{indent_str}Program(");
            for ast in asts {
                log_ast(ast.as_ref(), indent+2);
            } 
            println!("{indent_str})");
        },
        Ast::Not(ast) => {
            println!("{indent_str}Not(");
            log_ast(ast.as_ref(), indent+2);
            println!("{indent_str})");
        },
        Ast::Comparison { operator, left, right } => {
            println!("{indent_str}Comparison(");
            log_ast(left, indent+2);
            println!("{indent_str}{:?}", operator);
            log_ast(right, indent+2);
            println!("{indent_str})");
        },
        Ast::LogicalExpr { operator, left, right } => {
            println!("{indent_str}LogicalExpr(");
            log_ast(left, indent+2);
            println!("{indent_str}{:?}", operator);
            log_ast(right, indent+2);
            println!("{indent_str})");
        },
        Ast::BitwiseShift { shift_direction, left, right } => {
            println!("{indent_str}BitwiseShift(");
            log_ast(left, indent+2);
            println!("{indent_str}{:?}", shift_direction);
            log_ast(right, indent+2);
            println!("{indent_str})");
        },
        Ast::BinaryOperator { operator, left, right } => {
            println!("{indent_str}BinaryOperator(");
            log_ast(left, indent+2);
            println!("{indent_str}{:?}", operator);
            log_ast(right, indent+2);
            println!("{indent_str})");
        },
        Ast::String(str) => {
            println!("{indent_str}String({})", str.as_ref());
        },
        Ast::Int(int) => {
            println!("{indent_str}Int({})", int);
        },
        Ast::Float(float) => {
            println!("{indent_str}Float({})", float);
        },
        Ast::Identifier(iden) => {
            println!("{indent_str}Identifier({})", iden.as_ref());
        },
        Ast::VariableDeclaration { identifier, right, constant } => {
            println!("{indent_str}VariableDeclaration(");
            println!("{indent_str}  identifier=\"{}\"", identifier.as_ref());
            println!("{indent_str}  constant={}", constant);
            println!("{indent_str}  value=(");
            log_ast(&right, indent+4);
            println!("{indent_str}  )");
            println!("{indent_str})");
        },
        Ast::Assignment { identifier, right, local } => {
            println!("{indent_str}Assignment(");
            println!("{indent_str}  identifier=(");
            log_ast(&identifier, indent+4);
            println!("{indent_str}  )");
            println!("{indent_str}  local={}", local);
            println!("{indent_str}  value=(");
            log_ast(&right, indent+4);
            println!("{indent_str}  )");
            println!("{indent_str})");
        },
    }
}

fn main() {
    let mut input = String::new();
    print!(">>> ");

    io::stdout().flush().expect("Failed to flush stdout");

    io::stdin()
        .read_line(&mut input)
        .expect("Failed to read line");

    let result = parser::Parser::new(&input);

    if result.is_ok() {
        let program = result.unwrap().parse();
        if program.is_ok() {
            log_ast(&program.unwrap(), 0);
        } else {
            errors::log_error(program.err().unwrap());
        }
    } else {
        errors::log_error(result.err().unwrap());
    }
}
