#include "Lexer.h"

int main() {
	while (true)
	{
		std::string source;
		std::cin >> source;
		Lexer lexer = Lexer(source);
		lexer.lex();

		if (lexer.hadError) continue;
		for (Token t : lexer.tokens)
			std::cout << "Lexeme: " << t.lexeme << "\nPosition: " << t.position << "\n" << std::endl;
	}
}