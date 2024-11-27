#include <stdio.h>
#include <wchar.h>
#include <locale.h>

#define W_ROOK 0x00002656

int main(void)
{
    setlocale(LC_CTYPE, "");
    wchar_t star = 0x00002656;
    wprintf(L"%lc\n", star);

    wchar_t whitespace = 0x000025A0;
    wprintf(L"%lc\n%lc", whitespace, whitespace);
    return 0;
}