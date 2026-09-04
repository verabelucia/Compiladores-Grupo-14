#!/bin/sh
# FGA0003 - Compiladores 1
# Curso de Engenharia de Software
# Universidade de Brasilia (UnB)
# ImagemLang - executa a suite de testes do analisador lexico
#
# Cada arquivo tests/<grupo>/<nome>.img tem um par <nome>.esperado com a
# saida completa (stdout e stderr) que o scanner deve produzir. O teste
# passa quando a saida observada e identica a esperada.

set -u

RAIZ=$(cd "$(dirname "$0")" && pwd)
SCANNER="$RAIZ/build/scanner"

if [ ! -x "$SCANNER" ]; then
    echo "erro: '$SCANNER' nao encontrado. Compile antes de testar." >&2
    exit 2
fi

passou=0
falhou=0

for entrada in "$RAIZ"/tests/validos/*.img "$RAIZ"/tests/invalidos/*.img; do
    esperado="${entrada%.img}.esperado"
    nome=$(basename "$(dirname "$entrada")")/$(basename "$entrada")

    if [ ! -f "$esperado" ]; then
        echo "FALHOU  $nome  (sem arquivo .esperado)"
        falhou=$((falhou + 1))
        continue
    fi

    # Executa dentro do diretorio do teste para que as mensagens de erro
    # citem apenas o nome do arquivo, sem o caminho da maquina.
    obtido=$(cd "$(dirname "$entrada")" && "$SCANNER" "$(basename "$entrada")" 2>&1)

    if [ "$obtido" = "$(cat "$esperado")" ]; then
        echo "PASSOU  $nome"
        passou=$((passou + 1))
    else
        echo "FALHOU  $nome"
        printf '%s\n' "$obtido" | diff -u "$esperado" - | sed 's/^/        /'
        falhou=$((falhou + 1))
    fi
done

echo
echo "$passou passou, $falhou falhou"

[ "$falhou" -eq 0 ]
