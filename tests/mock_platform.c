#include "mock_windows/windows.h"
#include <string.h>

static int fake_module;
static int open_printer(int t, const char *m, const char *c, int p) { (void)t; (void)m; (void)c; (void)p; return 0; }
static int close_printer(void) { return 0; }
static int print_text(const char *d, int p, int s, int z) { (void)d; (void)p; (void)s; (void)z; return 42; }
static int cut_paper(int a) { (void)a; return 0; }
static int print_qr(const char *d, int s, int e) { (void)d; (void)s; (void)e; return 0; }
static int print_barcode(int t, const char *d, int h, int w, int r) { (void)t; (void)d; (void)h; (void)w; (void)r; return 0; }
static int advance_paper(int n) { (void)n; return 0; }
static int printer_status(int p) { (void)p; return 0; }
static int open_elgin_drawer(int p, int ti, int tf) { (void)p; (void)ti; (void)tf; return 0; }
static int open_drawer(int p, int ti, int tf) { (void)p; (void)ti; (void)tf; return 0; }
static int beep(int q, int ti, int tf) { (void)q; (void)ti; (void)tf; return 0; }
static int page_mode(void) { return 0; }
static int clear_page(void) { return 0; }
static int print_page(void) { return 0; }
static int standard_mode(void) { return 0; }
static int horizontal_position(int p) { (void)p; return 0; }
static int vertical_position(int p) { (void)p; return 0; }
static int print_sat(const char *d, int p) { (void)d; (void)p; return 0; }
static int print_sat_cancel(const char *d, const char *q, int p) { (void)d; (void)q; (void)p; return 0; }

HMODULE LoadLibraryA(const char *nome) {
    return strcmp(nome, "E1_Impressora01.dll") == 0 ? &fake_module : NULL;
}

FARPROC GetProcAddress(HMODULE modulo, const char *nome) {
    if (modulo != &fake_module) return NULL;
#define MAP(symbol, function) \
    if (strcmp(nome, symbol) == 0) { \
        return (FARPROC)function; \
    }
    MAP("AbreConexaoImpressora", open_printer)
    MAP("FechaConexaoImpressora", close_printer)
    MAP("ImpressaoTexto", print_text)
    MAP("Corte", cut_paper)
    MAP("ImpressaoQRCode", print_qr)
    MAP("ImpressaoCodigoBarras", print_barcode)
    MAP("AvancaPapel", advance_paper)
    MAP("StatusImpressora", printer_status)
    MAP("AbreGavetaElgin", open_elgin_drawer)
    MAP("AbreGaveta", open_drawer)
    MAP("SinalSonoro", beep)
    MAP("ModoPagina", page_mode)
    MAP("LimpaBufferModoPagina", clear_page)
    MAP("ImprimeModoPagina", print_page)
    MAP("ModoPadrao", standard_mode)
    MAP("PosicaoImpressaoHorizontal", horizontal_position)
    MAP("PosicaoImpressaoVertical", vertical_position)
    MAP("ImprimeXMLSAT", print_sat)
    MAP("ImprimeXMLCancelamentoSAT", print_sat_cancel)
#undef MAP
    return NULL;
}

int FreeLibrary(HMODULE modulo) { return modulo == &fake_module; }
unsigned long GetLastError(void) { return 126UL; }
