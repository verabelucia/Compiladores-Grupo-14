CC     = gcc
CFLAGS = -Wall -Wextra -g -Isrc -Ibuild
BUILD  = build
ALVO   = $(BUILD)/imagemc

all: $(ALVO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/parser.tab.c $(BUILD)/parser.tab.h: src/parser.y src/ast.h | $(BUILD)
	bison -d -o $(BUILD)/parser.tab.c src/parser.y

$(BUILD)/lex.yy.c: src/scanner.l $(BUILD)/parser.tab.h
	flex -o $@ src/scanner.l

FONTES  = src/main.c src/ast.c src/tabela.c src/semantico.c src/ri.c \
          src/otimizador.c src/codegen.c
HEADERS = src/ast.h src/tabela.h src/semantico.h src/ri.h \
          src/otimizador.h src/codegen.h

$(ALVO): $(BUILD)/parser.tab.c $(BUILD)/lex.yy.c $(FONTES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(BUILD)/parser.tab.c $(BUILD)/lex.yy.c $(FONTES)

test: $(ALVO)
	./run_tests.sh

$(BUILD)/programa_basico.c: $(ALVO) examples/programa_basico.img
	./$(ALVO) --c examples/programa_basico.img > $@

$(BUILD)/programa_basico: $(BUILD)/programa_basico.c runtime/runtime.c runtime/runtime.h
	$(CC) -Wall -Wextra -g -Iruntime -o $@ $(BUILD)/programa_basico.c runtime/runtime.c -lm

demo: $(BUILD)/programa_basico
	./$(BUILD)/programa_basico

clean:
	rm -rf $(BUILD)

.PHONY: all test demo clean
