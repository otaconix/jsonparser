#include "parser.h"
#include <stdlib.h>
#include <wctype.h>

#ifdef      DEBUG
static int depth = 0;
#endif  //  DEBUG
static int col = 0;
static int row = 1;

bool noexpect(wchar_t *c) {
    bool success = false;

    while (next != WEOF && *c && *c != next) {
        c++;
    }
    if (*c == '\0') {
        success = true;
        scan();
    }

    return success;
}

bool expecti(wchar_t c) {
    bool success = false;

    if ((success = next == c))
        scani();

    return success;
}

bool expect(wchar_t c) {
    bool success = false;

    if ((success = next == c))
        scan();

    return success;
}

void scan() {
    if (next != WEOF) {
        next = getwchar();
        col++;
        if (iswspace(next) && !iswblank(next)) {
            col = 0;
            row++;
        }
    }
}

void scani() {
    if (next != WEOF) {
        do {
            scan();
        } while (next != WEOF && iswspace(next));
    }
}

wchar_t *next_to_string() {
    static wchar_t string[2];
    if (next == WEOF) {
        return L"EOF";
    } else {
        swprintf(string, 2, L"%lc", next);
        return string;
    }
}

void error(char *err) {
    wprintf(L"ERROR(%d:%d): %s (next: '%ls')\n", row, col, err, next_to_string());
    exit(EXIT_FAILURE);
}

#ifdef      DEBUG
void enter(char *what) {
    int i;
    for (i = 0; i < depth; i++) {
        wprintf(L"| ");
    }
    wprintf(L"+-%s: enter\tnext: '%ls' (%d:%d)\n", what, next_to_string(), row, col);
    depth++;
}

void leave(char *what) {
    int i;
    depth--;
    for (i = 0; i < depth; i++) {
        wprintf(L"| ");
    }
    wprintf(L"+-%s: leave\tnext: '%ls' (%d:%d)\n", what, next_to_string(), row, col);
}
#endif //   DEBUG
