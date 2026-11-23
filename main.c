#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* Controle das impressoras Elgin i8/i9 usando E1_Impressora01.dll. */
typedef int (*FnAbreConexaoImpressora)(int tipo, const char *modelo, const char *conexao, int param);
typedef int (*FnFechaConexaoImpressora)(void);
typedef int (*FnImpressaoTexto)(const char *dados, int posicao, int estilo, int tamanho);
typedef int (*FnCorte)(int avanco);
typedef int (*FnImpressaoQRCode)(const char *dados, int tamanho, int nivelCorrecao);
typedef int (*FnImpressaoCodigoBarras)(int tipo, const char *dados, int altura, int largura, int HRI);
typedef int (*FnAvancaPapel)(int linhas);
typedef int (*FnStatusImpressora)(int param);
typedef int (*FnAbreGavetaElgin)(int pino, int ti, int tf);
typedef int (*FnAbreGaveta)(int pino, int ti, int tf);
typedef int (*FnSinalSonoro)(int qtd, int tempoInicio, int tempoFim);
typedef int (*FnModoPagina)(void);
typedef int (*FnLimpaBufferModoPagina)(void);
typedef int (*FnImprimeModoPagina)(void);
typedef int (*FnModoPadrao)(void);
typedef int (*FnPosicaoImpressaoHorizontal)(int posicao);
typedef int (*FnPosicaoImpressaoVertical)(int posicao);
typedef int (*FnImprimeXMLSAT)(const char *dados, int param);
typedef int (*FnImprimeXMLCancelamentoSAT)(const char *dados, const char *assQRCode, int param);

static FnAbreConexaoImpressora AbreConexaoImpressora;
static FnFechaConexaoImpressora FechaConexaoImpressora;
static FnImpressaoTexto ImpressaoTexto;
static FnCorte Corte;
static FnImpressaoQRCode ImpressaoQRCode;
static FnImpressaoCodigoBarras ImpressaoCodigoBarras;
static FnAvancaPapel AvancaPapel;
static FnStatusImpressora StatusImpressora;
static FnAbreGavetaElgin AbreGavetaElgin;
static FnAbreGaveta AbreGaveta;
static FnSinalSonoro SinalSonoro;
static FnModoPagina ModoPagina;
static FnLimpaBufferModoPagina LimpaBufferModoPagina;
static FnImprimeModoPagina ImprimeModoPagina;
static FnModoPadrao ModoPadrao;
static FnPosicaoImpressaoHorizontal PosicaoImpressaoHorizontal;
static FnPosicaoImpressaoVertical PosicaoImpressaoVertical;
static FnImprimeXMLSAT ImprimeXMLSAT;
static FnImprimeXMLCancelamentoSAT ImprimeXMLCancelamentoSAT;

static HMODULE dll = NULL;
static int conexaoAberta = 0;
static int entradaEncerrada = 0;
static int tipo = 1;
static char modelo[50] = "i9";
static char conexao[50] = "USB";
static int parametro = 0;

static const char *ASS_QRCODE =
    "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/"
    "q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+"
    "TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+"
    "ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==";

static int carregaFuncao(void *destino, const char *nome) {
    FARPROC funcao = GetProcAddress(dll, nome);
    if (funcao == NULL) {
        printf("Comando '%s' nao encontrado na DLL.\n", nome);
        return 0;
    }
    memcpy(destino, &funcao, sizeof(funcao));
    return 1;
}

static int carregarDLL(void) {
    dll = LoadLibraryA("E1_Impressora01.dll");
    if (dll == NULL) {
        printf("Nao foi possivel carregar E1_Impressora01.dll (erro %lu).\n", GetLastError());
        printf("Confira se a DLL esta na pasta do executavel e tem a arquitetura correta (32/64 bits).\n");
        return 0;
    }
    int ok = 1;
    ok &= carregaFuncao(&AbreConexaoImpressora, "AbreConexaoImpressora");
    ok &= carregaFuncao(&FechaConexaoImpressora, "FechaConexaoImpressora");
    ok &= carregaFuncao(&ImpressaoTexto, "ImpressaoTexto");
    ok &= carregaFuncao(&Corte, "Corte");
    ok &= carregaFuncao(&ImpressaoQRCode, "ImpressaoQRCode");
    ok &= carregaFuncao(&ImpressaoCodigoBarras, "ImpressaoCodigoBarras");
    ok &= carregaFuncao(&AvancaPapel, "AvancaPapel");
    ok &= carregaFuncao(&StatusImpressora, "StatusImpressora");
    ok &= carregaFuncao(&AbreGavetaElgin, "AbreGavetaElgin");
    ok &= carregaFuncao(&AbreGaveta, "AbreGaveta");
    ok &= carregaFuncao(&SinalSonoro, "SinalSonoro");
    ok &= carregaFuncao(&ModoPagina, "ModoPagina");
    ok &= carregaFuncao(&LimpaBufferModoPagina, "LimpaBufferModoPagina");
    ok &= carregaFuncao(&ImprimeModoPagina, "ImprimeModoPagina");
    ok &= carregaFuncao(&ModoPadrao, "ModoPadrao");
    ok &= carregaFuncao(&PosicaoImpressaoHorizontal, "PosicaoImpressaoHorizontal");
    ok &= carregaFuncao(&PosicaoImpressaoVertical, "PosicaoImpressaoVertical");
    ok &= carregaFuncao(&ImprimeXMLSAT, "ImprimeXMLSAT");
    ok &= carregaFuncao(&ImprimeXMLCancelamentoSAT, "ImprimeXMLCancelamentoSAT");
    if (!ok) {
        FreeLibrary(dll);
        dll = NULL;
    }
    return ok;
}

static void capturarEntrada(const char *mensagem, char *destino, size_t tamanho) {
    printf("%s", mensagem);
    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        destino[0] = '\0';
        entradaEncerrada = 1;
        return;
    }
    size_t fim = strcspn(destino, "\r\n");
    if (destino[fim] == '\0' && fim == tamanho - 1) {
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF) { }
        printf("Entrada muito longa; tente novamente.\n");
        destino[0] = '\0';
        return;
    }
    destino[fim] = '\0';
}

static int lerInteiro(const char *mensagem) {
    char linha[128];
    while (!entradaEncerrada) {
        char *inicio;
        char *fim;
        long valor;
        capturarEntrada(mensagem, linha, sizeof(linha));
        if (entradaEncerrada) return 0;
        inicio = linha;
        while (isspace((unsigned char)*inicio)) inicio++;
        errno = 0;
        valor = strtol(inicio, &fim, 10);
        while (isspace((unsigned char)*fim)) fim++;
        if (inicio != fim && *fim == '\0' && errno != ERANGE && valor >= INT_MIN && valor <= INT_MAX) {
            return (int)valor;
        }
        printf("Digite um numero inteiro valido.\n");
    }
    return 0;
}

static char *lerArquivo(const char *caminho) {
    FILE *arquivo = fopen(caminho, "rb");
    long tamanho;
    char *conteudo;
    size_t lidos;
    if (arquivo == NULL) return NULL;
    if (fseek(arquivo, 0, SEEK_END) != 0 || (tamanho = ftell(arquivo)) < 0 ||
        fseek(arquivo, 0, SEEK_SET) != 0) {
        fclose(arquivo);
        return NULL;
    }
    conteudo = (char *)malloc((size_t)tamanho + 1);
    if (conteudo == NULL) {
        fclose(arquivo);
        return NULL;
    }
    lidos = fread(conteudo, 1, (size_t)tamanho, arquivo);
    if (ferror(arquivo)) {
        free(conteudo);
        fclose(arquivo);
        return NULL;
    }
    conteudo[lidos] = '\0';
    fclose(arquivo);
    return conteudo;
}

static int fazerLogin(void) {
    char usuario[50], senha[50];
    for (int tentativa = 1; tentativa <= 3 && !entradaEncerrada; tentativa++) {
        printf("\n--- TELA DE LOGIN (Tentativa %d de 3) ---\n", tentativa);
        capturarEntrada("Usuario: ", usuario, sizeof(usuario));
        capturarEntrada("Senha: ", senha, sizeof(senha));
        if (entradaEncerrada) return 0;
        if (strcmp(usuario, "admin") == 0 && strcmp(senha, "1234") == 0) {
            printf("Login realizado com sucesso!\n");
            return 1;
        }
        if (tentativa < 3) printf("Usuario ou senha incorretos. Restam %d tentativa(s).\n", 3 - tentativa);
    }
    printf("Numero maximo de tentativas atingido. Acesso negado.\n");
    return 0;
}

static void exibirMenu(void) {
    printf("\n*************************************************\n");
    printf("*************** MENU IMPRESSORA *****************\n");
    printf("*************************************************\n");
    printf("1  - Configurar conexao\n2  - Abrir conexao\n3  - Fechar conexao\n");
    printf("4  - Imprimir texto\n5  - Imprimir QR Code\n6  - Imprimir codigo de barras\n");
    printf("7  - Imprimir XML SAT\n8  - Imprimir XML de cancelamento SAT\n");
    printf("9  - Abrir gaveta (Elgin)\n10 - Abrir gaveta\n11 - Emitir sinal sonoro\n0  - Sair\n");
}

static int exigirConexao(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return 0;
    }
    return 1;
}

static void cortarPapel(void) {
    int retorno = Corte(5);
    if (retorno != 0) printf("Erro ao cortar o papel. Codigo de erro: %d\n", retorno);
}

static void configurarConexao(void) {
    if (conexaoAberta) {
        printf("Feche a conexao antes de alterar seus parametros.\n");
        return;
    }
    tipo = lerInteiro("Digite o tipo de conexao (1 para USB, 2 para Serial, etc.): ");
    if (entradaEncerrada) return;
    capturarEntrada("Digite o modelo da impressora (i8 ou i9): ", modelo, sizeof(modelo));
    if (entradaEncerrada) return;
    capturarEntrada("Digite a porta de conexao (ex: USB): ", conexao, sizeof(conexao));
    if (entradaEncerrada) return;
    parametro = lerInteiro("Digite o parametro adicional (ex: 0 para padrao): ");
    if (!entradaEncerrada) printf("Parametros de conexao configurados com sucesso.\n");
}

static void abrirConexao(void) {
    if (conexaoAberta) {
        printf("Conexao ja esta aberta.\n");
        return;
    }
    int retorno = AbreConexaoImpressora(tipo, modelo, conexao, parametro);
    if (retorno == 0) {
        conexaoAberta = 1;
        printf("Conexao aberta com sucesso.\n");
    } else printf("Erro ao abrir conexao. Codigo de erro: %d\n", retorno);
}

static void fecharConexao(void) {
    if (!conexaoAberta) {
        printf("Nenhuma conexao aberta para fechar.\n");
        return;
    }
    int retorno = FechaConexaoImpressora();
    if (retorno == 0) {
        conexaoAberta = 0;
        printf("Conexao fechada.\n");
    } else printf("Erro ao fechar conexao. Codigo de erro: %d\n", retorno);
}

static void imprimirTexto(void) {
    char texto[200];
    int copias;
    if (!exigirConexao()) return;
    capturarEntrada("Texto a imprimir: ", texto, sizeof(texto));
    if (entradaEncerrada) return;
    if (texto[0] == '\0') {
        printf("O texto nao pode ficar vazio.\n");
        return;
    }
    do {
        copias = lerInteiro("Quantas copias (1 a 100)? ");
        if (entradaEncerrada) return;
        if (copias < 1 || copias > 100) printf("Informe uma quantidade entre 1 e 100.\n");
    } while (copias < 1 || copias > 100);
    for (int i = 1; i <= copias; i++) {
        int retorno = ImpressaoTexto(texto, 1, 4, 0);
        /* Este comando devolve a quantidade de dados enviada: retorno >= 0 significa sucesso. */
        if (retorno >= 0) printf("Copia %d enviada para impressao.\n", i);
        else printf("Erro ao imprimir. Codigo: %d\n", retorno);
    }
    cortarPapel();
}

static void imprimirQRCode(void) {
    char dados[300];
    if (!exigirConexao()) return;
    capturarEntrada("Conteudo do QR Code: ", dados, sizeof(dados));
    if (entradaEncerrada) return;
    if (dados[0] == '\0') {
        printf("O conteudo do QR Code nao pode ficar vazio.\n");
        return;
    }
    int retorno = ImpressaoQRCode(dados, 6, 4);
    if (retorno == 0) {
        cortarPapel();
        printf("QR Code impresso com sucesso.\n");
    } else printf("Erro ao imprimir QR Code. Codigo de erro: %d\n", retorno);
}

static void imprimirCodigoBarras(void) {
    if (!exigirConexao()) return;
    int retorno = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    if (retorno == 0) {
        cortarPapel();
        printf("Codigo de barras impresso com sucesso.\n");
    } else printf("Erro ao imprimir codigo de barras. Codigo de erro: %d\n", retorno);
}

static void imprimirXMLSAT(void) {
    if (!exigirConexao()) return;
    char *dadosXML = lerArquivo("XMLSAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler XMLSAT.xml na pasta de execucao.\n");
        return;
    }
    int retorno = ImprimeXMLSAT(dadosXML, 0);
    free(dadosXML);
    if (retorno == 0) {
        cortarPapel();
        printf("XML SAT impresso com sucesso.\n");
    } else printf("Erro ao imprimir XML SAT. Codigo de erro: %d\n", retorno);
}

static void imprimirXMLCancelamentoSAT(void) {
    if (!exigirConexao()) return;
    char *dadosXML = lerArquivo("CANC_SAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler CANC_SAT.xml na pasta de execucao.\n");
        return;
    }
    int retorno = ImprimeXMLCancelamentoSAT(dadosXML, ASS_QRCODE, 0);
    free(dadosXML);
    if (retorno == 0) {
        cortarPapel();
        printf("XML de cancelamento SAT impresso com sucesso.\n");
    } else printf("Erro ao imprimir cancelamento SAT. Codigo de erro: %d\n", retorno);
}

static void abrirGavetaElgin(void) {
    if (!exigirConexao()) return;
    int retorno = AbreGavetaElgin(1, 50, 50);
    if (retorno == 0) printf("Gaveta Elgin aberta com sucesso.\n");
    else printf("Erro ao abrir gaveta Elgin. Codigo de erro: %d\n", retorno);
}

static void abrirGaveta(void) {
    if (!exigirConexao()) return;
    int retorno = AbreGaveta(1, 5, 10);
    if (retorno == 0) printf("Gaveta aberta com sucesso.\n");
    else printf("Erro ao abrir gaveta. Codigo de erro: %d\n", retorno);
}

static void emitirSinalSonoro(void) {
    if (!exigirConexao()) return;
    int quantidade;
    do {
        quantidade = lerInteiro("Quantidade de apitos (1 a 20): ");
        if (entradaEncerrada) return;
        if (quantidade < 1 || quantidade > 20) printf("Informe uma quantidade entre 1 e 20.\n");
    } while (quantidade < 1 || quantidade > 20);
    int retorno = SinalSonoro(quantidade, 50, 5);
    if (retorno == 0) printf("Sinal sonoro emitido com sucesso.\n");
    else printf("Erro ao emitir sinal sonoro. Codigo de erro: %d\n", retorno);
}

int main(void) {
    if (!carregarDLL()) {
        printf("Programa encerrado.\n");
        return 1;
    }
    if (!fazerLogin()) {
        FreeLibrary(dll);
        return 1;
    }
    while (!entradaEncerrada) {
        exibirMenu();
        int opcao = lerInteiro("\nDigite a opcao desejada: ");
        if (entradaEncerrada) break;
        switch (opcao) {
            case 1: configurarConexao(); break;
            case 2: abrirConexao(); break;
            case 3: fecharConexao(); break;
            case 4: imprimirTexto(); break;
            case 5: imprimirQRCode(); break;
            case 6: imprimirCodigoBarras(); break;
            case 7: imprimirXMLSAT(); break;
            case 8: imprimirXMLCancelamentoSAT(); break;
            case 9: abrirGavetaElgin(); break;
            case 10: abrirGaveta(); break;
            case 11: emitirSinalSonoro(); break;
            case 0: fecharConexao(); entradaEncerrada = 1; printf("Encerrando programa...\n"); break;
            default: printf("Opcao invalida.\n"); break;
        }
    }
    if (conexaoAberta) fecharConexao();
    if (dll != NULL) FreeLibrary(dll);
    return 0;
}
