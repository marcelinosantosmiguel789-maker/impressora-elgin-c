# Testes locais

Execute, a partir da raiz do projeto:

```bash
make test
# ou diretamente:
bash tests/run_tests.sh
```

O script compila `main.c` com GCC usando `tests/mock_windows/windows.h` e `tests/mock_platform.c`, que simulam apenas as APIs Win32 e os retornos da DLL. Os cenários verificam login com três tentativas, bloqueio de impressão sem conexão, fluxo do menu, cópias, comandos SAT e periféricos.

Esses testes não substituem uma compilação com MinGW para Windows, não carregam `E1_Impressora01.dll` e não validam comunicação física, USB ou o papel impresso. Para a validação final, compile no Windows com a DLL e a impressora Elgin i8/i9 corretas.
