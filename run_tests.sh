set -u

RAIZ=$(cd "$(dirname "$0")" && pwd)
COMPILADOR="$RAIZ/build/imagemc"

if [ ! -x "$COMPILADOR" ]; then
    echo "erro: '$COMPILADOR' nao encontrado. Rode 'make' antes de testar." >&2
    exit 2
fi

passou=0
falhou=0
fase_filtro="${1:-}"

for entrada in "$RAIZ"/tests/*/*/*.img; do
    esperado="${entrada%.img}.esperado"
    grupo=$(dirname "$entrada")
    fase=$(basename "$(dirname "$grupo")")

    if [ -n "$fase_filtro" ] && [ "$fase" != "$fase_filtro" ]; then
        continue
    fi

    nome=$fase/$(basename "$grupo")/$(basename "$entrada")

    case "$fase" in
        lexico)        opcoes="--tokens" ;;
        sintatico)     opcoes="--ast" ;;
        semantico)     opcoes="--tabela" ;;
        intermediario) opcoes="--ri" ;;
        otimizacao)    opcoes="--ri-otim" ;;
        codegen)       opcoes="--c" ;;
        *)             opcoes="" ;;
    esac

    if [ ! -f "$esperado" ]; then
        echo "FALHOU  $nome  (sem arquivo .esperado)"
        falhou=$((falhou + 1))
        continue
    fi

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
