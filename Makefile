# FGA0003 - Compiladores 1
# Curso de Engenharia de Software
# Universidade de Brasilia (UnB)
# ImagemLang - compila o compilador e roda os testes

CC     = gcc
CFLAGS = -Wall -Wextra -g -Isrc -Ibuild
BUILD  = build
ALVO   = $(BUILD)/imagemc

all: $(ALVO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/parser.tab.c $(BUILD)/parser.tab.h: src/parser.y | $(BUILD)
	bison -d -o $(BUILD)/parser.tab.c src/parser.y

$(BUILD)/lex.yy.c: src/scanner.l $(BUILD)/parser.tab.h
	flex -o $@ src/scanner.l

$(ALVO): $(BUILD)/parser.tab.c $(BUILD)/lex.yy.c src/main.c
	$(CC) $(CFLAGS) -o $@ $(BUILD)/parser.tab.c $(BUILD)/lex.yy.c src/main.c

test: $(ALVO)
	./run_tests.sh

clean:
	rm -rf $(BUILD)

.PHONY: all test clean
