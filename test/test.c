#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char name1[] = "/proc/meminfo";
	char str[128];
    char *p, *d, *s, *a;

    FILE *f = fopen(name1, "r");
    while (fgets(str, 128, f)) {
        if(strstr(str, "MemTotal")){
            strcpy(p, str);
            printf("%s\n", p); }
        if(strstr(str, "MemAvailable")){
            strcpy(s, str);
            printf("%s\n", d); }
        // if(strstr(str, "SwapTotal")){
        //     strcpy(d, str);
        //     printf("%s\n", s); }
        // if(strstr(str, "SwapFree")){
        //     strcpy(a, str);
        //     printf("%s\n", a); }

        // if(feof)
        //     break;
    }


    fclose(f);
    return 0;
}
