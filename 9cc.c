#include <ctype.h> // 文字の種類の判定や文字の変換
#include <stdarg.h> // 可変長引数
#include <stdbool.h> // bool, true, false
#include <stdio.h>
#include <stdlib.h> // strtol
#include <string.h>

typedef enum {
	TK_RESERVED, // 記号
	TK_NUM, // 整数トークン
	TK_EOF // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struck Token {
	TokenKind kind; // トークンの型
	Token *next; // 次の入力トークン
	int val; // kindがTK_NUMの場合、その数値
	char *str; // トークン文字列
};

// 現在着目しているトークン
Token *token;

// エラーを報告する為の関数
// printfと同じ引数
void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// 次のトークが期待している記号の時には、トークンを1つ読み進めて真を返す、それ以外の場合には偽を返す
bool consume(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return true;
}

// 次のトークが期待している記号の時には、トークンを1つ読み進めて真を返す、それ以外の場合にはエラーを返す
void expect(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op)
		error("'%c'ではありません", op);
	token = token->next;
}

// 次のトークが数字の場合、トークンを1つ読み進めて真を返す、それ以外の場合にはエラーを返す
int expect_number(){
	if(token->kind != TK_NUM)
		error("数ではありません");
	int val = token->val;
	token = token->ext;
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind, Token *cur, char *str){
	Token *tok = calloc(1, sizeof(Token)); // メモリ割り当て
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while(*p){
		if(isspace(*p)){
			p++;
			continue;
		}

		if(*p == '+' || *p == '-'){
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error("トークナイズできません");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}


int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	token = tokenize(argv[1]);

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	printf("	mov rax, %ld\n", expect_number()); // ldはlong d, strtolは文字列をlongに変換

	while(!at_eof()){
		if(consume('+')){
			printf("	add rax, %d\n", expect_number());
		}

		expect('-');
		printf("	sub rax, %d\n", expect_number());
	}

	printf("	ret\n");
	return 0;
}