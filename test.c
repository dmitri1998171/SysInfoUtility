#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ARR_SIZE 50

FILE *fp;

void openFile(char name[], char attr) {
	if ((fp = fopen(name, &attr)) == NULL){
		printf("Не удалось открыть файл\n");
		exit(-1);}
}

void cpu_sys_info() {
	char name2[] = "/proc/loadavg";
	char str[ARR_SIZE];
	
    openFile(name2, 'r');
	fgets(str, 15, fp);
	printf("%s\n", str);
	fclose(fp);

     char *ptr = strtok(str, " ");
     printf("%.2f\n", atof(ptr));
}

int main(){
    cpu_sys_info();

    return 0;
}
