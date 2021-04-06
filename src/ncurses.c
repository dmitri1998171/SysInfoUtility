#include "../include/protocol.h"

void ncurses_sys_output(WINDOW *win, int line_pos) {
	mvwprintw(win, line_pos,     1, "CPU avg: %s", sys_info.cpuavg);
	mvwprintw(win, line_pos + 1, 1, "GPU: %i Mb", sys_info.gpuavg);
	mvwprintw(win, line_pos + 2, 1, "RAM: %i / %i Mb", mem.memAvail, mem.memTotal);
	mvwprintw(win, line_pos + 3, 1, "Swap: %i / %i Mb", mem.swapAvail, mem.swapTotal);
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
	mvwprintw(win, line_pos + 4, 1, "GPU: ");
	mvwprintw(win, line_pos + 5, 1, "Resolution: %s", hard_info.resolution);
	mvwprintw(win, line_pos + 6, 1, "RAM: %i Mb", mem.memTotal);
	mvwprintw(win, line_pos + 7, 1, "Swap: %i Mb", mem.swapTotal);

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

void *ncurses_output() {
	WINDOW *subwindows[2];
	WINDOW *text_blocks[2];
	WINDOW *blocks[6];
	char text_title[2][14] = {"Hardware info", "System info"};
	char blocks_title[6][13] = {"CPU avg", "GPU load", "Memory load", "CPU t*C", "GPU t*C", "HDD/SSD load"};
	
	get_hard_info();			// сбор хар-к ПК
	
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
	for(int i = 0; i < 6; i++) {
		blocks[i] = derwin(subwindows[1], 3, (COLS / 2) - 2, (i * 3) + 1, 1);
		box(blocks[i], 0, 0);
		mvwprintw(blocks[i], 0, 2, "%s", blocks_title[i]);
	}

	ncurses_hw_output(text_blocks[0], 1);
	
	while(1) {
		get_sys_info();			// сбор текущих значений
		ncurses_sys_output(text_blocks[1], 1);
		mvprintw(LINES - 1, 1, "F1 - Quit");
		wrefresh(subwindows[0]);
		wrefresh(subwindows[1]);
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