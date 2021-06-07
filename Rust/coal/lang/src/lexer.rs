use std::error::Error;
use std::fmt::{self, Display, Formatter};
use std::mem;
use std::str::CharIndices;

use crate::token;

#[inline]
fn is_id_start(ch: char) -> bool {
    ch == '_' || ch.is_ascii_alphabetic()
}

#[inline]
fn is_id_continue(ch: char) -> bool {
    ch == '_' || ch.is_ascii_digit()
}

pub type Location = usize;

#[derive(Debug, PartialEq)]
pub enum LexicalError {
    InvalidCharacter { ch: char, location: Location },
    UnterminatedString { location: Location },
}

impl Display for LexicalError {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        match self {
            LexicalError::InvalidCharacter { ch, location } => {
                write!(f, "Invalid character '{}' found at {}", ch, location)
            }
            LexicalError::UnterminatedString { location } => {
                write!(f, "String starting at {} was not terminated", location)
            }
        }
    }
}

impl Error for LexicalError {}

pub type SpanResult<'input> = Result<(Location, token::Tok<'input>, Location), LexicalError>;

pub struct Lexer<'input> {
    source: &'input str,
    chars: CharIndices<'input>,
    lookahead: Option<(usize, char)>,
    lookahead2: Option<(usize, char)>,
}

impl<'input> Lexer<'input> {
    pub fn new(source: &'input str) -> Lexer<'input> {
        let mut chars = source.char_indices();
        let lookahead = chars.next();
        let lookahead2 = chars.next();

        Lexer {
            source,
            chars,
            lookahead,
            lookahead2,
        }
    }

    fn bump(&mut self) -> Option<(usize, char)> {
        mem::replace(
            &mut self.lookahead,
            mem::replace(&mut self.lookahead2, self.chars.next()),
        )
    }

    fn take_until<F>(&mut self, mut terminate: F) -> Option<usize>
    where
        F: FnMut(char) -> bool,
    {
        while let Some((i, ch)) = self.lookahead {
            if terminate(ch) {
                return Some(i);
            } else {
                self.bump();
            }
        }

        None
    }

    fn take_while<F>(&mut self, mut condition: F) -> Option<usize>
    where
        F: FnMut(char) -> bool,
    {
        self.take_until(|ch| !condition(ch))
    }

    fn skip_to_line_end(&mut self) {
        self.take_while(|ch| ch != '\n');
    }

    fn skip_whitespace(&mut self) {
        self.take_while(|ch| ch.is_whitespace());
    }

    fn read_string(&mut self, pos: usize) -> SpanResult<'input> {
        match self.take_until(|ch| ch == '"') {
            Some(i) => {
                self.bump();
                Ok((pos, token::Tok::StringDatum(&self.source[pos + 1..i]), i + 1))
            }
            None => Err(LexicalError::UnterminatedString { location: pos }),
        }
    }

    fn read_string_literal(&mut self, pos: usize) -> SpanResult<'input> {
        match self.take_until(|ch| ch == '\'') {
            Some(i) => {
                self.bump();
                Ok((pos, token::Tok::StringLiteralDatum(&self.source[pos + 1..i]), i + 1))
            }
            None => Err(LexicalError::UnterminatedString { location: pos }),
        }
    }

    fn read_number(&mut self, pos: usize) -> SpanResult<'input> {
        let mut end = self.take_while(|ch| ch.is_ascii_digit());

        let mut as_integer = true;

        if let Some((_, '.')) = self.lookahead {

            as_integer = false;

            // Check if it's a decimal or a field access
            if let Some((_, next_ch)) = self.lookahead2 {
                if next_ch.is_ascii_digit() {
                    self.bump();
                    end = self.take_while(|ch| ch.is_ascii_digit());
                }
            }
        }

        let end = end.unwrap_or_else(|| self.source.len());

        if as_integer {
            Ok((
                pos,
                token::Tok::IntegerDatum(self.source[pos..end].parse().expect("unparsable integer")),
                end,
            ))
        } else {
            Ok((
                pos,
                token::Tok::FloatDatum(self.source[pos..end].parse().expect("unparsable float")),
                end,
            ))
        }
    }

    fn read_identifier(&mut self, pos: usize) -> SpanResult<'input> {
        let end = self
            .take_while(|ch| is_id_start(ch) || is_id_continue(ch))
            .unwrap_or_else(|| self.source.len());

        let token = match &self.source[pos..end] {
            "import" => token::Tok::Import(pos),
            "dir"    => token::Tok::Dir(pos),
            "return" => token::Tok::Return,
            "module" => token::Tok::Module,
            "static" => token::Tok::Static,
            "int"    => token::Tok::Integer,
            "bool"   => token::Tok::Bool,
            "float"  => token::Tok::Float,
            "string" => token::Tok::String,
            "addr"   => token::Tok::Addr,
            "dict"   => token::Tok::Dict,
            "list"   => token::Tok::List,
            "extern" => token::Tok::Extern,
            "members"=> token::Tok::Members,
            "def"    => token::Tok::Def,
            "if"     => token::Tok::If(pos),
            "elif"   => token::Tok::Elif(pos),
            "else"   => token::Tok::Else,
            "for"    => token::Tok::For,
            "while"  => token::Tok::While(pos),
            "none"   => token::Tok::None,
            "unit"   => token::Tok::Unit,
            id       => token::Tok::Identifier(id),
        };

        Ok((pos, token, end))
    }
}

impl<'input> Iterator for Lexer<'input> {
    type Item = SpanResult<'input>;

    fn next(&mut self) -> Option<SpanResult<'input>> {
        self.skip_whitespace();

        if let Some((i, ch)) = self.bump() {
            match ch {
                '{' => Some(Ok((i, token::Tok::LeftCurly(i), i + 1))),
                '}' => Some(Ok((i, token::Tok::RightCurly(i), i + 1))),
                '(' => Some(Ok((i, token::Tok::LeftParen, i + 1))),
                ')' => Some(Ok((i, token::Tok::RightParen, i + 1))),
                '[' => Some(Ok((i, token::Tok::LeftBracket(i), i + 1))),
                ']' => Some(Ok((i, token::Tok::RightBracket(i), i + 1))),
                ';' => Some(Ok((i, token::Tok::SemiColon(i), i + 1))),
                ':' => {
                    if let Some((_, ':')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::Eyes, i + 2)))
                    } else {
                        Some(Ok((i, token::Tok::Colon(i), i + 1)))
                    }
                },
                ',' => Some(Ok((i, token::Tok::Comma, i + 1))),
                '.' => Some(Ok((i, token::Tok::Dot, i + 1))),
                '+' => Some(Ok((i, token::Tok::Add, i + 1))),
                '-' => {
                    if let Some((_, '>')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::Arrow, i + 2)))
                    } else {
                        Some(Ok((i, token::Tok::Sub, i + 1)))
                    }
                },
                '*' => {
                    if let Some((_, '*')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::Pow, i + 2)))
                    } else {
                        Some(Ok((i, token::Tok::Mul, i + 1)))
                    }
                },
                '/' => {
                    if let Some((_, '/')) = self.lookahead {
                        self.skip_to_line_end();
                        self.next()
                    } else {
                        Some(Ok((i, token::Tok::Div, i + 1)))
                    }
                }

                '!' => {
                    if let Some((_, '=')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::Ne, i + 2)))
                    } else {
                        Some(Ok((i, token::Tok::Negate, i + 1)))
                    }
                }

                '=' => {
                    if let Some((_, '=')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::Comparison, i + 2)))
                    } else {
                        Some(Ok((i, token::Tok::Equal(i), i + 1)))
                    }
                }

                '>' => {
                    if let Some((_, '=')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::Gte, i + 2)))
                    } else {
                        Some(Ok((i, token::Tok::Gt, i + 1)))
                    }
                }

                '<' => {
                    if let Some((_, '=')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::Lte, i + 2)))
                    } else if let Some((_, '-')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::LeftArrow, i + 2)))
                    } else {
                        Some(Ok((i, token::Tok::Lt, i + 1)))
                    }
                }

                '&' => {
                    if let Some((_, '&')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::And, i + 2)))
                    } else {
                        Some(Ok((i, token::Tok::BwAnd, i + 1)))
                    }
                }

                '|' => {
                    if let Some((_, '|')) = self.lookahead {
                        self.bump();
                        Some(Ok((i, token::Tok::Or, i + 2)))
                    } else {
                        Some(Ok((i, token::Tok::BwOr, i + 1)))
                    }
                }

                '"' => Some(self.read_string(i)),
                '\'' => Some(self.read_string_literal(i)),
                ch if is_id_start(ch) => Some(self.read_identifier(i)),
                ch if ch.is_ascii_digit() => Some(self.read_number(i)),

                ch => Some(Err(LexicalError::InvalidCharacter { ch, location: i })),
            }
        } else {
            None
        }
    }
}