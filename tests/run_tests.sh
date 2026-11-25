#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
bin="$(mktemp "${TMPDIR:-/tmp}/elgin-test.XXXXXX")"
out="$(mktemp "${TMPDIR:-/tmp}/elgin-output.XXXXXX")"
trap 'rm -f "$bin" "$out"' EXIT

gcc -std=c99 -Wall -Wextra -Wpedantic -Wno-cast-function-type -Itests/mock_windows \
    main.c tests/mock_platform.c -o "$bin"

# Três logins inválidos devem encerrar o programa sem mostrar o menu.
set +e
printf 'errado\nx\nerrado\nx\nerrado\nx\n' | "$bin" >"$out" 2>&1
status=$?
set -e
[ "$status" -eq 1 ]
grep -Fq 'Numero maximo de tentativas atingido' "$out"
! grep -Fq 'MENU IMPRESSORA' "$out"

# Sem conexão, a rotina precisa sair antes de pedir qualquer dado de impressão.
printf 'admin\n1234\n4\n0\n' | "$bin" >"$out" 2>&1
grep -Fq 'Nao tem conexao com impressora em aberto.' "$out"
! grep -Fq 'Texto a imprimir:' "$out"

# Percorre opções de configuração, conexão, impressão, XML e periféricos.
printf 'admin\n1234\n1\n1\ni9\nUSB\n0\n2\n4\nTeste de impressao\n2\n5\nhttps://exemplo.test\n6\n7\n8\n9\n10\n11\n2\n3\n0\n' | "$bin" >"$out" 2>&1
grep -Fq 'Conexao aberta com sucesso.' "$out"
grep -Fq 'Copia 1 impressa.' "$out"
grep -Fq 'Copia 2 impressa.' "$out"
grep -Fq 'QR Code impresso com sucesso.' "$out"
grep -Fq 'Codigo de barras impresso com sucesso.' "$out"
grep -Fq 'XML SAT impresso com sucesso' "$out"
grep -Fq 'XML de cancelamento impresso com sucesso' "$out"
grep -Fq 'Gaveta Elgin aberta com sucesso!' "$out"
grep -Fq 'Gaveta aberta com sucesso!' "$out"
grep -Fq 'Sinal sonoro emitido com sucesso!' "$out"
grep -Fq 'Conexao fechada.' "$out"
grep -Fq 'Encerrando programa...' "$out"

printf 'Todos os testes simulados passaram.\n'
