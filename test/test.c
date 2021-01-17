#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


void mem_info(){
    char name1[] = "/proc/meminfo";
    char *p, *d, *s, *a;
	char str[128];

    FILE *f = fopen(name1, "r");
    while (fgets(str, 128, f)) {
        if(strstr(str, "MemTotal")){
            p = str;
            printf("%s\n", p); 
        }
        if(strstr(str, "MemAvailable")){
            d = str;
            printf("%s\n", d); 
        }
        if(strstr(str, "SwapTotal")){
            s = str;
            printf("%s\n", s); 
        }
        if(strstr(str, "SwapFree")){
            a = str;
            printf("%s\n", a); 
        }
    }
    fclose(f);
}

int main(void) {
    mem_info();

    return 0;
}
