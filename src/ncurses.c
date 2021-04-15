#include "../include/protocol.h"

void ncurses_sys_output(WINDOW *win, int line_pos) {
	mvwprintw(win, line_pos,     1, "CPU avg: %s", sys_info.cpu);
	mvwprintw(win, line_pos + 1, 1, "GPU: %i Mb", sys_info.gpu_total);
	mvwprintw(win, line_pos + 2, 1, "RAM: %i / %i Mb", mem.mem_used, mem.mem_total);
	mvwprintw(win, line_pos + 3, 1, "Swap: %i / %i Mb", mem.swap_used, mem.swap_total);
	mvwprintw(win, line_pos + 4, 1, "CPU temp: %i C", sys_info.cpu_temp_avg);
}

void ncurses_hw_output(WINDOW *win, int line_pos) {
	mvwprintw(win, line_pos, 1, "Version: %s", hard_info.version);
		
    mvwprintw(win, line_pos + 1, 1, "Network interfaces:");
	move(line_pos + 2, 21);
	for(int i = 0; i < hard_info.net_int_count; i++) {
		addch(' ');
		for(int j = 0; j < strlen(hard_info.net_int[i]); j++) 
			addch(hard_info.net_int[i][j]);
	}
	
	mvwprintw(win, line_pos + 2, 1, "CPU: %s", hard_info.cpu);
	mvwprintw(win, line_pos + 3, 1, "CPU CORES: %i", hard_info.cpu_cores);
	mvwprintw(win, line_pos + 4, 1, "GPU: %s", hard_info.gpu);
	mvwprintw(win, line_pos + 5, 1, "Resolution: %s", hard_info.resolution);
	mvwprintw(win, line_pos + 6, 1, "RAM: %i Mb", mem.mem_total);
	mvwprintw(win, line_pos + 7, 1, "Swap: %i Mb", mem.swap_total);

	// Вывод дисков
	mvwprintw(win, line_pos + 8, 1, "HDD/SSD load: ");
	for(int i = 0; i < volumes_info.volumes_count; i++) {
		mvwprintw(win, line_pos + 9 + i, 5, "* %s ", volumes_info.volumes[i]);
		if(volumes_info.vol_size[i] < 1024)
			mvwprintw(win, line_pos + 9 + i, 11, "%.0f Mb", volumes_info.vol_size[i]);
		else
			mvwprintw(win, line_pos + 9 + i, 11, "%.1f Gb", volumes_info.vol_size[i] / 1024);

		// Вывод разделов диска
		for(int j = 0; j < volumes_info.partitions_count; j++) {
			mvwprintw(win, line_pos + 9 + i + j + 1, 10, "- %s", volumes_info.partitions[j]);
			if(volumes_info.part_size[j] < 1024)
				mvwprintw(win, line_pos + 9 + i + j + 1, 10 + 8, "%.0f Mb", volumes_info.part_size[j]);
			else
				mvwprintw(win, line_pos + 9 + i + j + 1, 10 + 8, "%.1f Gb", volumes_info.part_size[j] / 1024);
		}
	}
}

void ncurses_graph_load(WINDOW *block, float current_value, float max_value, int max_length) {
	float x = (current_value * 100) / max_value;
	float line_length = (x * max_length) / 100;

	for(int j = 1; j < line_length; j++)
		mvwprintw(block, 1, j, "|");
	wrefresh(block);
}

void draw_graph_blocks(WINDOW *subwindow, WINDOW **blocks, char *title, int count, int line_length, int pos) {
	int max_length = (COLS / 2) - 2;
	
	for(int i = 0; i < count; i++) {
		blocks[i] = derwin(subwindow, 3, max_length, (i * 3) + pos, 1);
		box(blocks[i], 0, 0);
		mvwprintw(blocks[i], 0, 2, "%s", title + (i * line_length));
	}
}

void *ncurses_output() {
	WINDOW *subwindows[2];
	WINDOW *text_blocks[2];
	WINDOW *graph_blocks[5];
	WINDOW *hdd_ssd_blocks[4];
	char text_title[2][14] = {"Hardware info", "System info"};
	char blocks_title[5][13] = {"CPU avg", "GPU load", "Memory load", "CPU t*C", "GPU t*C"};
	
	get_hard_info();
	
	// массив, содержащий кол-во строк и положение по 'y' для всех текстов. блоков. 
	// Положение по 'y' зависит от кол-ва строк в предыдущем блоке
	int text_blocks_params[2][2] = {12 + volumes_info.volumes_count + volumes_info.partitions_count, 0, 
									7,  text_blocks_params[0][0]};

	for(int i = 0; i < 2; i++) {
		subwindows[i] = newwin(LINES - 1, COLS / 2, 0, (COLS / 2) * i);				// Окно
		text_blocks[i] = derwin(subwindows[0], text_blocks_params[i][0], (COLS / 2) - 2, text_blocks_params[i][1] + 1, 1); // Блоки текста
		box(subwindows[i], 0, 0);
		box(text_blocks[i], 0, 0);
		mvwprintw(text_blocks[i], 0, 2, "%s", text_title[i]);	// Заголовок блока
	}
	
	// Ползунки 
	draw_graph_blocks(subwindows[1], graph_blocks, (char*) blocks_title, 5, 13, 1);
	mvwprintw(subwindows[1], 16, 2, "HDD/SSD load");
	whline(subwindows[1], ACS_HLINE, (COLS / 2) - 16);
	draw_graph_blocks(subwindows[1], hdd_ssd_blocks, (char*) volumes_info.partitions, volumes_info.partitions_count, 5, 17);

	ncurses_hw_output(text_blocks[0], 1);
	wrefresh(subwindows[0]);
	wrefresh(subwindows[1]);
	
	while(1) {
		get_sys_info();
		ncurses_sys_output(text_blocks[1], 1);
		
		ncurses_graph_load(graph_blocks[0], sys_info.cpu_load_avg, 10, (COLS / 2) - 3);
		ncurses_graph_load(graph_blocks[1], sys_info.gpu_used, sys_info.gpu_total, (COLS / 2) - 3);
		ncurses_graph_load(graph_blocks[2], mem.mem_used, mem.mem_total, (COLS / 2) - 3);				// 
		ncurses_graph_load(graph_blocks[3], sys_info.cpu_temp_avg, 100, (COLS / 2) - 3);
		ncurses_graph_load(graph_blocks[4], sys_info.gpu_temp_avg, 100, (COLS / 2) - 3);
		
		for(int i = 0; i < volumes_info.partitions_count; i++) 
			ncurses_graph_load(hdd_ssd_blocks[i], volumes_info.part_free[i], volumes_info.part_size[i], (COLS / 2) - 3);

		mvprintw(LINES - 1, 1, "F1 - Quit");
		wrefresh(text_blocks[1]);
		refresh();
		sleep(1);
	}
	return (NULL);
}

void *ncurses_input(void *threadArgs) {
	pthread_t* output_tid = (pthread_t*) threadArgs;
	int value;

	// Если нажата клавиша ESCAPE - закрываем поток вывода ncurses_output
	while(value != KEY_F(1)) value = wgetch(stdscr);
	pthread_cancel(*output_tid);
	
	return (NULL);
}

void ncurses_background() {
	initscr();
    curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
	
	pthread_mutex_init(&ncurses_mutex, NULL);
	pthread_t* threadID = (pthread_t*) malloc(2 * sizeof(pthread_t));
					
	if(pthread_create(&threadID[0], NULL, ncurses_output, NULL) != 0)
		DieWithError("pthread_create() failed");
	if(pthread_create(&threadID[1], NULL, ncurses_input, &threadID[0]) != 0)
		DieWithError("pthread_create() failed");
            
	if(pthread_join(threadID[0], NULL) != 0) 
		DieWithError("Joining the first ncurses_thread");
	if(pthread_join(threadID[1], NULL) != 0) 
		DieWithError("Joining the second ncurses_thread");

	pthread_mutex_destroy(&ncurses_mutex);
	endwin();
}