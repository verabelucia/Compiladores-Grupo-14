#!/bin/sh
# FGA0003 - Compiladores 1
# Curso de Engenharia de Software
# Universidade de Brasilia (UnB)
# ImagemLang - executa a suite de testes do compilador
#
# Cada arquivo tests/<fase>/<grupo>/<nome>.img tem um par <nome>.esperado com
# a saida completa (stdout e stderr) que o compilador deve produzir. O teste
# passa quando a saida observada e identica a esperada.
#
# Fases:
#   lexico     - roda "imagemc --tokens" (sequencia de tokens)
#   sintatico  - roda "imagemc" (comandos reconhecidos e erros sintaticos)

set -u

RAIZ=$(cd "$(dirname "$0")" && pwd)
COMPILADOR="$RAIZ/build/imagemc"

if [ ! -x "$COMPILADOR" ]; then
    echo "erro: '$COMPILADOR' nao encontrado. Rode 'make' antes de testar." >&2
    exit 2
fi

passou=0
falhou=0

for entrada in "$RAIZ"/tests/*/*/*.img; do
    esperado="${entrada%.img}.esperado"
    grupo=$(dirname "$entrada")
    fase=$(basename "$(dirname "$grupo")")
    nome=$fase/$(basename "$grupo")/$(basename "$entrada")

    case "$fase" in
        lexico) opcoes="--tokens" ;;
        *)      opcoes="" ;;
    esac

    if [ ! -f "$esperado" ]; then
        echo "FALHOU  $nome  (sem arquivo .esperado)"
        falhou=$((falhou + 1))
        continue
    fi

    # Executa dentro do diretorio do teste para que as mensagens de erro
    # citem apenas o nome do arquivo, sem o caminho da maquina.
    obtido=$(cd "$grupo" && "$COMPILADOR" $opcoes "$(basename "$entrada")" 2>&1)

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
