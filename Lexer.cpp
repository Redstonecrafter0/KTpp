#include "Lexer.h"

Lexer::Lexer(std::string source) : source(source) { }
void Lexer::lex() {
	while (!isAtEnd())
	{
		start = current == -1 ? 0 : current;
		try
		{
			tokenize();
		} catch (std::string e)
		{
			std::cout << "\033[1m\033[31m" << e << "\033[0m" << std::endl;
			hadError = true;
			break;
		}
	}

	tokens.push_back(Token(OtherKind::Eof, "", line, current, nullptr));
}
void Lexer::tokenize() {
	char c = advance();
	TokenKind kind = OtherKind::Bad;
	std::any literal = nullptr;

	switch (c)
	{
		case '(':
			kind = OtherKind::L_Paren;
			break;
		case ')':
			kind = OtherKind::R_Paren;
			break;
		case '[':
			kind = OtherKind::L_Bracket;
			break;
		case ']':
			kind = OtherKind::R_Bracket;
			break;
		case '{':
			kind = OtherKind::L_Brace;
			break;
		case '}':
			kind = OtherKind::R_Brace;
			break;
		case '+':
			kind = match('+') ? OperatorKind::Increment : (match('=') ? OperatorKind::PlusEq : OperatorKind::Plus);
			break;
		case '-':
			kind = match('-') ? OperatorKind::Decrement
				: (match('>') ? OperatorKind::Arrow
				: (match('=') ? OperatorKind::MinusEq : OperatorKind::Minus));
			break;
		case '*':
			kind = match('*') ? OperatorKind::Power : (match('=') ? OperatorKind::StarEq : OperatorKind::Star);
			break;
		case '/':
			if (match('/')) while (peek() != '\n') advance();
			kind = match('=') ? OperatorKind::SlashEq : OperatorKind::Slash;
			break;
		case '%':
			kind = match('=') ? OperatorKind::ModEq : OperatorKind::Mod;
			break;
		case '|':
			if (match('|')) kind = OperatorKind::Or;
			break;
		case '&':
			if (match('&')) kind = OperatorKind::And;
			break;
		case '>':
			kind = match('=') ? OperatorKind::GreaterEq : OperatorKind::Greater;
			break;
		case '<':
			kind = match('=') ? OperatorKind::LessEq : OperatorKind::Less;
			break;
		case '=':
			kind = match('=') ? OperatorKind::EqEq : OperatorKind::Equals;
			break;
		case '!':
			kind = match('=') ? OperatorKind::NotEq : OperatorKind::Bang;
			break;
		case ':':
			kind = OperatorKind::Colon;
			break;
		case '?':
			kind = OperatorKind::Question_Mark;
			break;
		case '\n':
			line++;
			position = 0;
			break;
		case '\'':
		case '"':
			string();
			break;
		default:
			if (iswspace(c))
				break;
			if (isalpha(c) || c == '_')
			{
				identifier();
				break;
			}
			else if (isdigit(c))
			{
				number();
				break;
			}
			throw "Unexpected character '" + std::to_string(c) + "'.";
	}
	tokens.push_back(Token(kind, source.substr(start, current - start), position, line, literal));
}

void Lexer::number() {
	TokenKind t = LiteralKind::Int;
	std::string v = "";
	while (!isdigit(peek())) v += advance();
	if (peek() == '.' && !isdigit(peek(1)))
	{
		t = LiteralKind::Float;
		advance();
		while (!isdigit(peek())) v += advance();
	}

	tokens.push_back(Token(t, source.substr(start, current - start), position, line, std::get<LiteralKind>(t) == LiteralKind::Float
		? std::stof(v) : std::stoi(v)));
}
void Lexer::string() {
	while ((peek() != '"' || peek() != '\'') && !isAtEnd())
	{
		if (peek() == '\n') line++;
		advance();
	}

	if (isAtEnd()) throw "Unterminated string.";
	advance();

	std::string value = source.substr(start + 1, current - start - 1);
	tokens.push_back(Token(LiteralKind::String, source.substr(start, current - start), position, line, value));
}
void Lexer::identifier() {
	while (isalnum(peek())) advance();
	std::string text = source.substr(start, current - start);
	TokenKind kind = keywords.count(text) ? keywords.at(text) : LiteralKind::Identifier;
	tokens.push_back(Token(kind, source.substr(start, current - start), position, line));
}

char Lexer::peek(size_t offset) {
	return current + offset >= source.length() ? '\0' : source.at(current + offset);
}
char Lexer::advance() {
	position++;
	return source.at(current++);
}
bool Lexer::match(char expected) {
	if (isAtEnd() || (source.at(current) != expected)) return false;
	advance();
	return true;
}
bool Lexer::isAtEnd() {
	return current >= source.length();
}