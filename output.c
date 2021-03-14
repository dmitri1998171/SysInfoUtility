#include "protocol.h"   

void graph_strings_Func() {
	strcpy(graph_strings.string_name[0], "HDD/SSD load");
	strcpy(graph_strings.string_name[1], "CPU t*C");
	strcpy(graph_strings.string_name[2], "GPU t*C");
	strcpy(graph_strings.string_name[3], "CPU avg");
	strcpy(graph_strings.string_name[4], "GPU load");
	strcpy(graph_strings.string_name[5], "Memory load");

	// graph_strings.string_load[0] = 
	// graph_strings.string_load[0] = 
	// graph_strings.string_load[0] = 
	graph_strings.string_load[3] = (sys_info.cpu_load / hard_info.cpu_cores) * 100;
	// graph_strings.string_load[0] = 
	graph_strings.string_load[5] = sys_info.gpuavg / 10;
}

void openFile(char name[], char attr) {
	if ((fp = fopen(name, &attr)) == NULL){
		printf("Не удалось открыть файл\n");
		exit(-1);}
}

char* parsing(char *str, char *symbol) {
	char *p = strtok(str, symbol);
	p = strtok(NULL, symbol);
	return p;
}

void get_number_from_str(char* str, int* value) {
	*value = atoi(str);
	*value = *value / 1000; // перевод Kb->Mb			??? 1000 или 1024
	// https://habr.com/ru/post/193256/
}

// ##### OUTPUT #############################

void DieWithError(char *errorMessage)
{
    printf("\nERROR: %s\n\n", errorMessage);
    exit(1);
}

void current_values_output() {
	printf("CPU avg: %s\n", sys_info.cpuavg);
	printf("GPU: %i Mb\n", sys_info.gpuavg);
	printf("RAM: %i / %i Mb\n", mem.memAvail, mem.memTotal);
	printf("Swap: %i / %i Mb\n", mem.swapAvail, mem.swapTotal);
	printf("CPU temp: %i C\n\n", sys_info.cpu_temp_mid);
}

void out() {
    printf("Version: %s\n", hard_info.version);
    printf("Network interfaces: ");
	
	for(int i=0; i < hard_info.count; i++)	
		printf("%s ", hard_info.net_int[i]);
	
	printf("\nCPU: %s\n", hard_info.cpu);
	printf("CPU CORES: %i\n", hard_info.cpu_cores);
	printf("GPU: \n");
	printf("Resolution: %s", hard_info.resolution);
	printf("RAM: %i Mb\n", mem.memTotal);
	printf("Swap: %i Mb\n\n", mem.swapTotal);
}

void write_to_log() {
	char filename[] = {"sysInfo.log"};
	openFile(filename, 'w');

	fprintf(fp, "Version: %s\n", hard_info.version);
    fprintf(fp, "Network interfaces: ");
	
	for(int i = 0; i < hard_info.count; i++)	
		fprintf(fp, "%s ", hard_info.net_int[i]);

	fprintf(fp, "\nCPU:\t%s\n", hard_info.cpu);
	fprintf(fp, "CPU CORES:\t%i\n", hard_info.cpu_cores);
	fprintf(fp, "GPU: \n");
	fprintf(fp, "RAM: %i Mb\n", mem.memTotal);
	fprintf(fp, "Swap: %i Mb\n\n", mem.swapTotal);

	fprintf(fp, "CPU avg: %s\n", sys_info.cpuavg);
	fprintf(fp, "GPU:\n");
	fprintf(fp, "RAM: %i / %i Mb\n", mem.memAvail, mem.memTotal);
	fprintf(fp, "Swap: %i / %i Mb\n", mem.swapAvail, mem.swapTotal);
	fprintf(fp, "CPU temp: %i C\n\n", sys_info.cpu_temp_mid);
	fclose(fp);
}

void generate_html() {
	char str[ARR_SIZE];
    FILE *fsrc, *fdst;

	graph_strings_Func();

    if ((fsrc = fopen("sysInfo.log", "r")) == NULL) {
		perror("fopen"); exit(-1); }
    if ((fdst = fopen("logfile.html", "w")) == NULL) {
		perror("fopen"); exit(-1); }

    fprintf(fdst, "<html>\n");
	fprintf(fdst, "\t<head>\n");
	fprintf(fdst, "\t\t<title>logfile.html</title>\n");
	fprintf(fdst, "\t\t<link rel=\"stylesheet\" href=\"style.css\"></link>\n");
	fprintf(fdst, "\t</head>\n");
    fprintf(fdst, "\t<body>\n");
	fprintf(fdst, "\t\t<div class=\"bg_block\">\n");
	fprintf(fdst, "\t\t\t<div class=\"block\">\n");
	fprintf(fdst, "\t\t\t\t<div class=\"subblock\" style=\"padding-bottom: 14px;\">\n");
	
    while(fgets(str, ARR_SIZE, fsrc)) {
		if(strstr(str, "CPU avg:")) { // Отделение текущ. инфы в отдельный блок
			fprintf(fdst, "\t\t\t\t</div>\n");
			fprintf(fdst, "\t\t\t\t<div class=\"subblock\"><br></div>\n");
			fprintf(fdst, "\t\t\t\t<div class=\"subblock\">\n");
		}
        fprintf(fdst, "\t\t\t\t\t%s<br>", str);
    }
    fprintf(fdst, "\n\t\t\t\t</div>\n");
    fprintf(fdst, "\t\t\t</div>\n");
	fprintf(fdst, "\t\t\t<div class=\"block\">\n");

	for(int i = 0; i < 6; i++) {
		if(i == 3)
        	fprintf(fdst, "<div class=\"subblock\"><br></div>\n");

		fprintf(fdst, "\t\t\t\t<div class=\"subblock\">\n");
		fprintf(fdst, "\t\t\t\t<table  width=\"100%\">\n");
		fprintf(fdst, "\t\t\t\t\t<tr>\n");

		fprintf(fdst, "\t\t\t\t\t<td class=\"td_line\" width=\"80%\"> <hr style=\"width: %f%;\"> </td>\n", graph_strings.string_load[i]);
		fprintf(fdst, "\t\t\t\t\t<td align=\"center\"> %s </td>\n", graph_strings.string_name[i]);
		
		fprintf(fdst, "\t\t\t\t\t</tr>\n");
		fprintf(fdst, "\t\t\t\t</table>\n");
		fprintf(fdst, "\t\t\t\t</div>\n");
	}
    fprintf(fdst, "\t\t\t</div>\n");
    fprintf(fdst, "\t\t</div>\n");
    fprintf(fdst, "\t</body>\n");
    fprintf(fdst, "</html>");

    fclose(fsrc);
    fclose(fdst);
}

void full_output() {
	get_hard_info();			// сбор хар-к ПК
	get_sys_info();				// сбор текущих значений
    
	printf("\n\tSystem information\n\n");
	out();						// Вывод хар-к ПК
	printf("-------------------------------\n\n");
	current_values_output();	// Вывод текущих значений
}

