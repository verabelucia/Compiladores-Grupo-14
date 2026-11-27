#!/bin/sh
# FGA0003 - Compiladores 1
# Curso de Engenharia de Software
# Universidade de Brasilia (UnB)
# ImagemLang - testes de erro em tempo de execucao do runtime (com ASan/UBSan)

set -u

RAIZ=$(cd "$(dirname "$0")/../.." && pwd)
COMPILADOR="${COMPILADOR:-$RAIZ/build/imagemc}"
WORK="$RAIZ/build/runtime-tests"

if [ ! -x "$COMPILADOR" ]; then
    echo "erro: '$COMPILADOR' nao encontrado. Rode 'make' antes de testar." >&2
    exit 2
fi

rm -rf "$WORK"
mkdir -p "$WORK"
cp "$RAIZ/docs/assets/img/runtime/original.jpg" "$WORK/foto.jpg"

passou=0
falhou=0

testar_erro() {
    nome="$1"
    arquivo_img="$2"
    esperado="$3"
    fonte="$RAIZ/tests/runtime/$arquivo_img"
    c_out="$WORK/$nome.c"
    bin="$WORK/$nome"

    if ! "$COMPILADOR" --c "$fonte" > "$c_out" 2>"$WORK/$nome.comp.err"; then
        echo "FALHOU  runtime/$arquivo_img  (falha ao gerar C)"
        cat "$WORK/$nome.comp.err" >&2
        falhou=$((falhou + 1))
        return
    fi

    if ! gcc -Wall -Wextra -Werror -g -fsanitize=address,undefined \
             -I"$RAIZ/runtime" -o "$bin" "$c_out" "$RAIZ/runtime/runtime.c" -lm \
             2>"$WORK/$nome.gcc.err"; then
        echo "FALHOU  runtime/$arquivo_img  (falha ao compilar C gerado)"
        cat "$WORK/$nome.gcc.err" >&2
        falhou=$((falhou + 1))
        return
    fi

    saida=$(cd "$WORK" && "./$nome" 2>&1)
    rc=$?

    if [ "$rc" -eq 0 ]; then
        echo "FALHOU  runtime/$arquivo_img  (esperado erro, retornou 0)"
        printf '%s\n' "$saida" | sed 's/^/        /'
        falhou=$((falhou + 1))
        return
    fi

    if printf '%s\n' "$saida" | grep -E -q 'Sanitizer|runtime error:'; then
        echo "FALHOU  runtime/$arquivo_img  (erro de memoria/sanitizer)"
        printf '%s\n' "$saida" | sed 's/^/        /'
        falhou=$((falhou + 1))
        return
    fi

    if printf '%s\n' "$saida" | grep -F -q -- "$esperado"; then
        echo "PASSOU  runtime/$arquivo_img"
        passou=$((passou + 1))
    else
        echo "FALHOU  runtime/$arquivo_img  (mensagem inesperada)"
        echo "        esperado trecho: $esperado"
        printf '%s\n' "$saida" | sed 's/^/        /'
        falhou=$((falhou + 1))
    fi
}

testar_erro "01-arquivo-inexistente" "01-arquivo-inexistente.img" \
    "erro de execucao: nao foi possivel carregar 'nao-existe.jpg'"

testar_erro "02-recorte-fora-dos-limites" "02-recorte-fora-dos-limites.img" \
    "erro de execucao: recortar: area de 50, 50 tamanho 80x80 fora da imagem 100x100"

testar_erro "03-extensao-invalida" "03-extensao-invalida.img" \
    "erro de execucao: salvar: extensao nao suportada em 'saida.bmp' (use .png ou .jpg)"

echo
echo "$passou passou, $falhou falhou"

[ "$falhou" -eq 0 ]
