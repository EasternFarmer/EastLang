use std::collections::VecDeque;

use crate::errors::Errors;

#[derive(Debug)]
pub(crate) enum TokenType {
    // types
    String(String),
    Int(i32),
    Float(f32),
    // List,
    Identifier(String),
    Callable,
    Const,
    Local,
    BinaryOperator(String),
    BitwiseShift(String),
    Equals,

    If,
    Else,
    ElseIf,
    While,

    LogicalExpr(String),
    Not,
    ComparisonExpr(String),

    OpenParen,
    ClosedParen,

    OpenBrace,   // {
    ClosedBrace, // }

    OpenBracket,   // [
    ClosedBracket, // ]

    Comma,
    Dot,
    Colon,
    SemiColon,
    Monkey, // @

    EndOfFile,
}

fn get_keyword(keyword: String) -> Option<TokenType> {
    if keyword == "local" {
        return Some(TokenType::Local)
    } else if keyword == "const" {
        return Some(TokenType::Const)
    } else if keyword == "if" {
        return Some(TokenType::If)
    } else if keyword == "else" {
        return Some(TokenType::Else)
    } else if keyword == "else_if" {
        return Some(TokenType::ElseIf)
    } else if keyword == "callable" {
        return Some(TokenType::Callable)
    } else if keyword == "not" {
        return Some(TokenType::Not)
    } else if keyword == "and" {
        return Some(TokenType::LogicalExpr("and".to_owned()))
    } else if keyword == "or" {
        return Some(TokenType::LogicalExpr("or".to_owned()))
    } else if keyword == "xor" {
        return Some(TokenType::LogicalExpr("xor".to_owned()))
    } else if keyword == "while" {
        return Some(TokenType::While)
    }
    None
}

pub(crate) fn tokenize(source_code: &str) -> Result<VecDeque<TokenType>, (Errors, String)> {

    let mut deque: VecDeque<TokenType> = VecDeque::new();

    let mut chars: VecDeque<char> = VecDeque::from_iter(source_code.trim().chars());

    while chars.len() > 0 {
        if chars[0] == '(' {
            deque.push_back(TokenType::OpenParen);
            chars.pop_front();
        } else if chars[0] == ')' {
            deque.push_back(TokenType::ClosedParen);
            chars.pop_front();
        } else if chars[0] == '{' {
            deque.push_back(TokenType::OpenBrace);
            chars.pop_front();
        } else if chars[0] == '}' {
            deque.push_back(TokenType::ClosedBrace);
            chars.pop_front();
        } else if chars[0] == '[' {
            deque.push_back(TokenType::OpenBracket);
            chars.pop_front();
        } else if chars[0] == ']' {
            deque.push_back(TokenType::ClosedBracket);
            chars.pop_front();
        } else if chars[0] == ':' {
            deque.push_back(TokenType::Colon);
            chars.pop_front();
        } else if chars[0] == ';' {
            deque.push_back(TokenType::SemiColon);
            chars.pop_front();
        } else if chars[0] == ',' {
            deque.push_back(TokenType::Comma);
            chars.pop_front();
        } else if chars[0] == '.' {
            deque.push_back(TokenType::Dot);
            chars.pop_front();
        } else if chars[0] == '@' {
            deque.push_back(TokenType::Monkey);
            chars.pop_front();
        } else if chars[0] == '!' {
            chars.pop_front();
            if chars[0] == '=' {
                deque.push_back(TokenType::ComparisonExpr("!=".to_owned()));
                chars.pop_front();
            } else {
                deque.push_back(TokenType::Not);
            }
        } else if chars[0] == '`' {
            chars.pop_front();
            while chars[0] != '`' {
                chars.pop_front();
            }
            chars.pop_front();
        } else if chars[0] == '+'
            || chars[0] == '-'
            || chars[0] == '*'
            || chars[0] == '/'
            || chars[0] == '%'
        {
            deque.push_back(TokenType::BinaryOperator(chars[0].to_string()));
            chars.pop_front();
        } else if chars[0] == '=' {
            chars.pop_front();
            if chars[0] == '=' {
                chars.pop_front();
                deque.push_back(TokenType::ComparisonExpr("==".to_owned()));
            } else {
                deque.push_back(TokenType::Equals);
            }
        } else if chars[0] == '>' {
            chars.pop_front();
            if chars[0] == '=' {
                chars.pop_front();
                deque.push_back(TokenType::ComparisonExpr(">=".to_owned()));
            } else if chars[0] == '>' {
                chars.pop_front();
                deque.push_back(TokenType::BitwiseShift(">>".to_owned()));
            } else {
                deque.push_back(TokenType::ComparisonExpr(">".to_owned()));
            }
        } else if chars[0] == '<' {
            chars.pop_front();
            if chars[0] == '=' {
                chars.pop_front();
                deque.push_back(TokenType::ComparisonExpr("<=".to_owned()));
            } else if chars[0] == '<' {
                chars.pop_front();
                deque.push_back(TokenType::BitwiseShift("<<".to_owned()));
            } else {
                deque.push_back(TokenType::ComparisonExpr("<".to_owned()));
            }
        } else if chars[0].is_digit(10) {
            // TODO: Add non-base10 notations ( 0b101101 )
            let mut value: Vec<char> = Vec::new();
            if chars.len() > 2 && chars[0] == '0' {
                if chars[1] == 'b' {
                    chars.pop_front();
                    chars.pop_front();
                    let more_than_one: bool = chars.len() > 0 && chars[0].is_digit(2);
                    while chars.len() > 0 && chars[0].is_digit(2) {
                        value.push(chars[0]);
                        chars.pop_front();
                    }
                    if !more_than_one {
                        return Err((Errors::SyntaxError, "Missing value after 0b".to_owned()));
                    }
                    let binary_string: String = value.into_iter().collect();
                    let decimal_number = i32::from_str_radix(&binary_string, 2).unwrap();
                    deque.push_back(TokenType::Int(decimal_number));
                    continue;
                } else if chars[1] == 'x' {
                    chars.pop_front();
                    chars.pop_front();
                    let more_than_one: bool = chars.len() > 0 && chars[0].is_digit(16);
                    while chars.len() > 0 && chars[0].is_digit(16) {
                        value.push(chars[0]);
                        chars.pop_front();
                    }
                    if !more_than_one {
                        return Err((Errors::SyntaxError, "Missing value after 0x".to_owned()));
                    }
                    let hex_string: String = value.into_iter().collect();
                    let decimal_number = i32::from_str_radix(&hex_string, 16).unwrap();
                    deque.push_back(TokenType::Int(decimal_number));
                    continue;
                } else if chars[1] == 'o' {
                    chars.pop_front();
                    chars.pop_front();
                    let more_than_one: bool = chars.len() > 0 && chars[0].is_digit(8);
                    while chars.len() > 0 && chars[0].is_digit(8) {
                        value.push(chars[0]);
                        chars.pop_front();
                    }
                    if !more_than_one {
                        return Err((Errors::SyntaxError, "Missing value after 0x".to_owned()));
                    }
                    let octal_string: String = value.into_iter().collect();
                    let decimal_number = i32::from_str_radix(&octal_string, 8).unwrap();
                    deque.push_back(TokenType::Int(decimal_number));
                    continue;
                }
            }
            let mut is_float: bool = false;

            while chars.len() > 0 && (chars[0].is_digit(10) || chars[0] == '.') {
                if chars[0] == '.' {
                    if is_float {
                        return Err((Errors::SyntaxError, "Multiple dots in a number".to_owned()));
                    } else {
                        is_float = true;
                    }
                }
                value.push(chars[0]);
                chars.pop_front();
            }
            let string_num = value.into_iter().collect::<String>();
            if is_float {
                deque.push_back(TokenType::Float(string_num.parse::<f32>().unwrap()));
            } else {
                deque.push_back(TokenType::Int(string_num.parse::<i32>().unwrap()));
            }
        } else if chars[0].is_alphanumeric() || chars[0] == '_' {
            let mut value: Vec<char> = Vec::new();
            while chars.len() > 0 && (chars[0].is_alphanumeric() || chars[0] == '_') {
                value.push(chars[0]);
                chars.pop_front();
            }

            let word: String = value.iter().collect();

            if let Some(token_type) = get_keyword(word) {
                deque.push_back(token_type);
            } else {
                deque.push_back(TokenType::Identifier(value.into_iter().collect::<String>()));
            }
        } else if chars[0] == '"' || chars[0] == '\'' {
            let opening_quote: char = chars[0];
            chars.pop_front(); // remove the first quote

            let mut value: Vec<char> = Vec::new();
            while chars.len() > 0 && (chars[0] != opening_quote) {
                if chars[0] == '\\' {
                    chars.pop_front();
                    if chars.len() == 0 {
                        break;
                    }
                    if chars[0] == '\\' {
                        chars.pop_front();
                        value.push('\\');
                    } else if chars[0] == '"' {
                        chars.pop_front();
                        value.push('"');
                    } else if chars[0] == '\'' {
                        chars.pop_front();
                        value.push('\'');
                    } else if chars[0] == 'n' {
                        chars.pop_front();
                        value.push('\n');
                    } else if chars[0] == 't' {
                        chars.pop_front();
                        value.push('\t');
                    } else if chars[0] == 'r' {
                        chars.pop_front();
                        value.push('\r');
                    } else if chars[0] == 'x' {
                        chars.pop_front();
                        let mut num = String::new();

                        let mut i: u8 = 0;
                        while chars[0].is_digit(16) {
                            if i == 6 {
                                break;
                            }
                            num.push_str(&chars[0].to_string());
                            chars.pop_front();
                            i += 1;
                        }
                        if i == 0 {
                            return Err((
                                Errors::SyntaxError,
                                "Invalid or Missing hex digits after \\x".to_owned(),
                            ));
                        }
                        let vect: Vec<u8> = num
                            .as_bytes()
                            .chunks(2)
                            .map(|pair| {
                                let s = std::str::from_utf8(pair).unwrap();
                                u8::from_str_radix(s, 16).unwrap()
                            })
                            .collect();
                        let result = String::from_utf8(vect);
                        if result.is_ok() {
                            value.extend(result.unwrap().chars());
                        } else {
                            return Err((
                                Errors::ValueError,
                                format!(
                                    "Invalid value after \\x, \"{}\" is not a valid utf-8 char",
                                    num
                                ),
                            ));
                        }
                    }
                    continue;
                }
                value.push(chars[0]);
                chars.pop_front();
            }
            if chars.len() == 0 {
                return Err((
                    Errors::SyntaxError,
                    format!(
                        "Unterminated string literal \"{}\"",
                        value.into_iter().collect::<String>()
                    ),
                ));
            }
            chars.pop_front(); // remove the second quote

            deque.push_back(TokenType::String(value.into_iter().collect::<String>()));
        } else if chars[0].is_whitespace() {
            chars.pop_front();
        } else {
            println!("Unexpected char: {}. skipping", chars[0]);
            chars.pop_front();
        }
    }

    deque.push_back(TokenType::EndOfFile);

    Ok(deque)
}
