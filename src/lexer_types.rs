#[derive(Debug, PartialEq)]
pub(crate) enum LogicalExpr {
  And,
  Or,
  Xor
}

#[derive(Debug, PartialEq)]
pub(crate) enum ComparisonExpr {
  Equals,
  NotEquals,
  Greater,
  GreaterEqual,
  Less,
  LessEqual
}

#[derive(Debug, PartialEq)]
pub(crate) enum TokenType {
    // types
    String(Box<str>),
    Int(i32),
    Float(f32),
    // List,
    Identifier(Box<str>),
    Callable,
    Const,
    Local,
    BinaryOperator(char), // + or - or * or / or %
    BitwiseShift(char), // l or r
    Equals,

    If,
    Else,
    ElseIf,
    While,

    LogicalExpr(LogicalExpr),
    Not,
    ComparisonExpr(ComparisonExpr),

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