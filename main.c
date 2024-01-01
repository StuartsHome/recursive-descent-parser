#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Enum.
typedef enum {
	becomes,
	beginsym,
	callsym,
	constsym,
	comma,
	endsym,
	eql,
	geq,
	gtr,
	ident,
	ifsym,
	leq,
	lparen,
	lss,
	minus,
	neq,
	number,
	oddsym,
	plus,
	period,
	procsym,
	rparen,
	semicolon,
	slash,
	thensym,
	times,
	varsym,
} Symbol;

Symbol sym;

const char *cursor;
char string[64];
int peek;

void block(void);
void error(const char*);
int expect(Symbol s);
void expression(void);

#define is_num(c) ((c) >= '0' && (c) <= '9')

int main() {
	sym = beginsym;
	block();
	expect(period);

	return 0;
}

void nextsym(void) {
	peek = getchar();
	printf("nextsym top %d\n", peek);
	if (peek == ' ' | peek == '\t') return;
	else if (peek == EOF) return;
	else if (peek == 10) {
		sym = endsym;
		return;
	}
	switch (peek) {
		case '+': sym = plus; return;
		case '-': sym = minus; return;
		case '*': sym = times; return;
		case ';': sym = semicolon; return;
		case ':': sym = comma; return;
	}
	printf("nextsym bottom %d\n", peek);
	if(is_num(peek)) {
    	sym = number;
    	return;
  	}
}

int accept(Symbol s) {
	if (sym == s) {
		nextsym();
		return 1;
	}
	return 0;
}

// expect.
int expect(Symbol s) {
	if (accept(s))
		return 1;
	error("expect: unexpected symbol");
	return 0;
}

void error(const char *msg) {
	fprintf(stderr, "error: %s\n", msg);
	exit(1);
}

// factor.
void factor(void){
	if (accept(ident)) {
		;
	} else if (accept(number)) {
		;
	} else if (accept(lparen)) {
		expression();
		expect(rparen);
	} else {
		error("factor: syntax error");
		nextsym();
	}
}

// term.
void term(void) {
	factor();
	while (sym == times || sym == slash) {
		nextsym();
		factor();
	}
}

// expression.
void expression(void) {
	if (sym == plus || sym == minus)
		nextsym();
	term();
	while (sym == plus || sym == minus) {
		nextsym();
		term();
	}
}

// condition.
void condition(void) {
	if (accept(oddsym)) {
		expression();
	} else {
		expression();
		if (sym == eql || sym  == neq || sym == lss || sym == leq || sym == gtr || sym == geq) {
			nextsym();
			expression();
		} else {
			error("condition: invalid operator");
			nextsym();
		}
	}
}

// statement.
void statement(void) {
	if (accept(ident)) {
		expect(becomes);
		expression();
	} else if (accept(callsym)) {
		expect(ident);
	} else if (accept(beginsym)) {
		do {
			expression();
			statement();
		} while (accept(semicolon));
		expect(endsym);
	} else if (accept(ifsym)) {
		condition();
		expect(thensym);
		statement();
	} else if (accept(semicolon)) {
		printf("finish");
		return;
		
	} else {
		printf("statement: syntax %d\n", peek);
		error("statement: syntax error");
		nextsym();
	}
}

// block.
void block(void) {
	if (accept(constsym)) {
		do {
			expect(ident);
			expect(eql);
			expect(number);
		} while (accept(comma));
		expect(semicolon);
		return;
	}
	if (accept(varsym)) {
		do {
			expect(ident);
		} while (accept(comma));
		expect(semicolon);
	}
	while (accept(procsym)) {
		expect(ident);
		expect(semicolon);
		block();
		expect(semicolon);
	}
	statement();
}

// 1+2*3+1;