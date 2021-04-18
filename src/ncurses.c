#include "../include/protocol.h"

void ncurses_sys_output(WINDOW *win, int line_pos) {
	mvwprintw(win, line_pos,     1, "CPU avg: %s", sys_info.cpu);
	mvwprintw(win, line_pos + 1, 1, "GPU: %i Mb", sys_info.gpu_total);
	mvwprintw(win, line_pos + 2, 1, "RAM: %i / %i Mb", mem.mem_used, mem.mem_total);
	mvwprintw(win, line_pos + 3, 1, "Swap: %i / %i Mb", mem.swap_used, mem.swap_total);
	mvwprintw(win, line_pos + 4, 1, "CPU temp: %i / %i C", sys_info.cpu_temp_avg, sys_info.cpu_temp_max);
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
	
	mvwprintw(win, line_pos + 2, 1, "CPU:%s", hard_info.cpu);
	mvwprintw(win, line_pos + 3, 1, "CPU CORES: %i", hard_info.cpu_cores);
	mvwprintw(win, line_pos + 4, 1, "GPU: %s", hard_info.gpu);
	mvwprintw(win, line_pos + 5, 1, "Resolution: %s", hard_info.resolution);
	mvwprintw(win, line_pos + 6, 1, "RAM: %i Mb", mem.mem_total);
	mvwprintw(win, line_pos + 7, 1, "Swap: %i Mb", mem.swap_total);

	// Вывод разделов диска
	mvwprintw(win, line_pos + 8, 1, "HDD/SSD load: ");
	for(int j = 0; j < volumes_info.partitions_count; j++) {
		mvwprintw(win, line_pos + 9 + j, 5, "- %s", volumes_info.partitions[j]);
		if(volumes_info.part_size[j] < 1024)
			mvwprintw(win, line_pos + 9 + j, 13, "%.0f / %.0f Mb", volumes_info.part_free[j], volumes_info.part_size[j]);
		else
			mvwprintw(win, line_pos + 9 + j, 13, "%.1f / %.1f Gb", volumes_info.part_free[j] / 1024, volumes_info.part_size[j] / 1024);
	}
}

void draw_graph_blocks(WINDOW *subwindow, WINDOW *blocks, char *title, int title_length, int max_length, float current_value, float max_value, int pos, int i) {		
	int value_pct = 0;
	blocks = derwin(subwindow, 3, max_length, pos, 1);
	box(blocks, 0, 0);
	
	if(current_value > 0) {
		value_pct = (current_value / max_value) * 100;
		int load_pct = (current_value / max_value) * max_length;
		for(int j = 1; j < load_pct; j++)
			mvwprintw(blocks, 1, j, "|");
	}
	
	mvwprintw(blocks, 0, 2, "%s %i%%", title + (i * title_length), value_pct);
	wrefresh(blocks);
}

void *ncurses_output() {
	WINDOW *subwindows[2];
	WINDOW *text_blocks[2];
	WINDOW *graph_blocks[5];
	WINDOW *hdd_ssd_blocks[4];
	int block_width = COLS / 2;
	char text_title[2][14] = {"Hardware info", "System info"};
	char blocks_title[5][13] = {"CPU avg", "GPU load", "Memory load", "CPU t*C", "GPU t*C"};
	float line_length = 0;
	
	get_hard_info();
	
	int text_blocks_params[2][2] = {11 + volumes_info.partitions_count, 0, 
									7,  text_blocks_params[0][0]}; // i - кол-во строк; j - положение по 'y'(зависит от кол-ва строк в пред. блоке)

	for(int i = 0; i < 2; i++) {
		subwindows[i] = newwin(LINES - 1, block_width, 0, block_width * i);				// Окно
		text_blocks[i] = derwin(subwindows[0], text_blocks_params[i][0], block_width - 2, text_blocks_params[i][1] + 1, 1); // Блоки текста
		box(subwindows[i], 0, 0);
		box(text_blocks[i], 0, 0);
		mvwprintw(text_blocks[i], 0, 2, "%s", text_title[i]);	// Заголовок блока
	}
	
	mvwprintw(subwindows[1], 16, 2, "HDD/SSD load");
	whline(subwindows[1], ACS_HLINE, (COLS / 2) - 16);

	ncurses_hw_output(text_blocks[0], 1);
	wrefresh(subwindows[0]);
	wrefresh(subwindows[1]);

	while(1) {
		get_sys_info();
		ncurses_sys_output(text_blocks[1], 1);

		float load_params[5][2] = {
			sys_info.cpu_load_avg, hard_info.cpu_cores,
			sys_info.gpu_used, sys_info.gpu_total,
			mem.mem_used, mem.mem_total,
			sys_info.cpu_temp_avg, sys_info.cpu_temp_max,
			sys_info.gpu_temp_avg, sys_info.gpu_temp_max
		};

		for(int i = 0; i < 5; i++) 
			draw_graph_blocks(subwindows[1], graph_blocks[i], (char*) blocks_title, 13, block_width - 2, load_params[i][0], load_params[i][1], i * 3 + 1, i);
		
		for(int i = 0; i < volumes_info.partitions_count; i++) 
			draw_graph_blocks(subwindows[1], hdd_ssd_blocks[i], (char*) volumes_info.partitions, strlen(volumes_info.partitions[i]) + 1, block_width - 2, volumes_info.part_free[i], volumes_info.part_size[i], i * 3 + 17, i);

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
