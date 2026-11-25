# Sistema de impressão Elgin i8/i9 em C

Aplicação de terminal acadêmica para controlar uma impressora térmica Elgin i8 ou i9 por meio da biblioteca do fabricante `E1_Impressora01.dll`. O programa implementa login, menu, configuração da conexão, impressão de texto/QR Code/código de barras/XML SAT e comandos de gaveta e buzzer.

## Requisitos

- Windows 10/11 e GCC para Windows (MinGW-w64, por exemplo).
- Impressora Elgin i8 ou i9 e driver do fabricante instalados, para testar comunicação física.
- `E1_Impressora01.dll` x64 incluída na raiz do repositório; compile com MinGW/GCC 64-bit.
- `XMLSAT.xml` e/ou `CANC_SAT.xml` na pasta de execução se usar as opções SAT.

Esta pasta contém a DLL x64 fornecida para o trabalho. Ela não é compatível com um compilador de 32 bits.

## Compilar e executar

Abra o terminal na pasta raiz do projeto e confira a arquitetura do GCC:

```bash
gcc --version
gcc -dumpmachine
```

Compile como solicitado no trabalho:

```bash
gcc main.c -o impressora.exe
```

Ou, para exibir avisos úteis durante a compilação:

```bash
gcc -std=c99 -Wall -Wextra -o impressora.exe main.c
```

No Prompt de Comando ou PowerShell:

```powershell
.\impressora.exe
```

No Git Bash:

```bash
./impressora.exe
```

Mantenha a DLL escolhida ao lado do executável. Os arquivos XML são procurados relativamente à pasta de onde o programa foi iniciado.

## Login e menu

Credenciais demonstrativas: usuário `admin`, senha `1234` (três tentativas). Não use essas credenciais como proteção em um sistema de produção.

| Opção | Ação |
| --- | --- |
| 1 | Configurar conexão |
| 2 | Abrir conexão |
| 3 | Fechar conexão |
| 4 | Imprimir texto e informar a quantidade de cópias |
| 5 | Imprimir QR Code |
| 6 | Imprimir código de barras CODE 128 de exemplo |
| 7 | Imprimir `XMLSAT.xml` |
| 8 | Imprimir `CANC_SAT.xml` |
| 9 | Acionar gaveta Elgin |
| 10 | Acionar gaveta padrão |
| 11 | Emitir sinal sonoro |
| 0 | Fechar conexão ativa e sair |

Operações de impressão e periféricos verificam primeiro se há conexão aberta. A API `ImpressaoTexto` retorna a quantidade de dados enviados; por isso o programa considera sucesso qualquer retorno não negativo. Para os demais comandos, retorno zero significa sucesso.

## Verificações automatizadas

O conjunto de testes deste repositório usa substitutos locais das APIs do Windows e da impressora. Ele permite verificar compilação e fluxos básicos sem uma impressora conectada; não testa a DLL real, a arquitetura, a porta USB nem a qualidade da impressão. Consulte `tests/README.md` para executá-lo.

## Solução de problemas

- **A DLL não carrega:** confirme que ela está junto ao executável, que todas as dependências do SDK estão disponíveis e que a arquitetura x86/x64 corresponde a `gcc -dumpmachine`.
- **A impressora não responde:** ligue-a, confira USB/driver/modelo e tente primeiro as opções 1 e 2 do menu.
- **Falha nos arquivos SAT:** confira os nomes `XMLSAT.xml` e `CANC_SAT.xml` e execute a partir da pasta que contém esses arquivos.
- **GCC não reconhecido:** instale MinGW-w64/MSYS2 e inclua o diretório `bin` no `PATH`.

## Referência da atividade

Implementado a partir do enunciado “Trabalho de Programação em C — Controlando uma Impressora Térmica Elgin”.
