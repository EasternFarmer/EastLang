use std::io;
use std::io::Write;

mod errors;
mod lexer_types;
mod lexer;

use lexer_types::TokenType;

fn log_token(token: &TokenType) {
    match token {
        TokenType::Int(value) => print!("Int({}) ", value),
        TokenType::Identifier(value) => print!("Identifier({}) ", value),
        TokenType::String(value) => print!("String({}) ", value),
        TokenType::Float(value) => print!("Float({}) ", value),
        TokenType::Callable => print!("Callable() "),
        TokenType::Const => print!("Const() "),
        TokenType::Local => print!("Local() "),
        TokenType::BinaryOperator(value) => print!("BinaryOperator({}) ", value),
        TokenType::BitwiseShift(value) => print!("BitwiseShift({}) ", value),
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
        TokenType::EndOfFile => print!("EndOfFile() "),
        TokenType::Colon => print!("Colon() "),
        TokenType::SemiColon => print!("SemiColon() "),
    }
}

fn main() {
    let mut input = String::new();
    print!(">>> ");

    io::stdout().flush().expect("Failed to flush stdout");

    io::stdin()
        .read_line(&mut input)
        .expect("Failed to read line");

    let result = lexer::tokenize(&input);

    if result.is_ok() {
        let tokens = result.unwrap();
        for token in &tokens {
            log_token(token);
        }
        println!();

        println!("Executed \"{}\"", input.trim());
    } else {
        errors::log_error(result.err().unwrap());
    }
}
