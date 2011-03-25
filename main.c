#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include "json.h"

int main(int argc, char **argv) {
    if (!setlocale(LC_CTYPE, "")) {
        fprintf(stderr, "Can't set the specified locale! "
                "Check LANG, LC_CTYPE, LC_ALL.\n");
        return 1;
    }

    scan();

    json();

    wprintf(L"Successful parse!\n");

    return 0;
}
