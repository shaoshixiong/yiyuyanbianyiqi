/* bootpack相关的 */

#include "bootpack.h"
#include <stdio.h>

#define KEYCMD_LED	0xed

void close_constask(struct TASK *task);
void close_console(struct SHEET *sht);
struct SHEET *search_sheet(struct SHTCTL *shtctl, int mx, int my);
struct SHEET *open_omnaomi(struct SHTCTL *shtctl, unsigned int memtotal, struct SHEET *oldsht);
void task_b_main(struct SHEET *sht_win_b)
{
	struct FIFO32 fifo;
	struct TIMER *timer_1s;
	int i, fifobuf[128], count = 0, count0 = 0;
	char s[12];

	fifo32_init(&fifo, 128, fifobuf, 0);
	timer_1s = timer_alloc();
	timer_init(timer_1s, &fifo, 100);
	timer_settime(timer_1s, 100);

	for (;;) {
		count++;
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (i == 100) {
				sheet_refresh(sht_win_b, 0, 0, sht_win_b->bxsize, sht_win_b->bysize);
				sprintf(s, "%11d", count - count0);
				putfonts_asc_sht(sht_win_b, 24, 28, 0x000000, 0xffffff, s, 0);
				
				count0 = count;
				timer_settime(timer_1s, 100);
				
			}
		}
	}
}
void sysclock_task(void)
{struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	int i, j;
	char err, cnt;
	unsigned char s[6];
	static unsigned char adr[7] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09, 0x32 };
	static unsigned char max[7] = { 0x60, 0x59, 0x23, 0x31, 0x12, 0x99, 0x99 };
	struct TASK *task = task_now();
	struct TIMER *clock_timer = timer_alloc();
	timer_init(clock_timer, &task->fifo, 1);
	timer_settime(clock_timer, 100);
unsigned char t[7];
	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i == 1) {
				for (cnt = 0; cnt < 3; cnt++) {
					err = 0;
					for (j = 0; j < 7; j++) {
						io_out8(0x70, adr[j]);
						t[j] = io_in8(0x71);
					}
					for (j = 0; j < 7; j++) {
						io_out8(0x70, adr[j]);
						if (t[j] != io_in8(0x71) || (t[j] & 0x0f) > 9 || t[j] > max[j]) {
							err = 1;
						}
					}
					if (err == 0) {
						break;
					}
				}
				struct SHTCTL *ctl = (struct SHTCTL *) *((int *) 0x0fe4);
				struct SHEET *sht = &ctl->sheets0[1];
				sprintf(s, "%02X:%02X\0", t[2], t[1]);
				putfonts_asc_sht((unsigned int *)sht,binfo->scrnx-60,7, 0xffffff, 0x7A8A9D,s, 5);
	// sheet_refresh(sht, 0, 0, sht->bxsize, sht->bysize);
								   
				timer_settime(clock_timer, 100);
			}
		}
	}
}
void task_shijian_main(struct SHEET *sht_shijian)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct FIFO32 fifo;
	struct TIMER *timer_1s;
	int i, fifobuf[128], count = 0, count0 = 0;
	char s[12];

	fifo32_init(&fifo, 128, fifobuf, 0);
	timer_1s = timer_alloc();
	timer_init(timer_1s, &fifo, 100);
	timer_settime(timer_1s, 100);

	for (;;) {
		count++;
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (i == 100) {
				int ix, t[3];
	char sx[5];
	for (ix = 0; ix < 3; ix++) 
	{
		t[ix] = rtc_get(ix+3);
	}
	// sprintf(sx, "%02d:%02d:%02d\n",  t[0], t[1],t[2]);
	 sprintf(sx, "%02d:%02d\n",  t[0], t[1] );
	putfonts_asc_sht((unsigned int *)sht_shijian,binfo->scrnx-60,7, 0xffffff, 0x7A8A9D,sx, 5);
	sheet_refresh(sht_shijian, 0, 0, sht_shijian->bxsize, sht_shijian->bysize);
				timer_settime(timer_1s, 6000);
				
			}
		}
	}
	
	
	// int ix, t[3];
	// char sx[5];
	// for (ix = 0; ix < 3; ix++) 
	// {
		// t[ix] = rtc_get(ix+3);
	// }
	// sprintf(sx, "%02d:%02d:%02d\n",  t[0], t[1],t[2]);
	 
	// putfonts_asc_sht((unsigned int *)sht_shijian,90,7, 0xffffff, 0x7A8A9D,sx, 1);
	// sheet_refresh(sht_shijian, 0, 0, sht_shijian->bxsize, sht_shijian->bysize);
}

void HariMain(void)
{
	/* System */
		struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
		struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
		unsigned int memtotal;
		struct FIFO32 fifo, keycmd;
		int fifobuf[128], keycmd_buf[32];
		struct TASK *task_a, *task;
		struct FILEINFO *finfo;
		int *fat;
		int minlist[MAX_MINID];
		int bpp = get_bpp();
	/* Keyboard */
		struct MOUSE_DEC mdec;
		static char keytable0[0x80] = {
			/* 一般文字 */
			  0, 0x01, '1',  '2', '3', '4', '5', '6', '7', '8', '9', '0',  '-',  '^', 0x08, 0x09,
			'Q',  'W', 'E',  'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0x0a,    0,  'A',  'S',
			'D',  'F', 'G',  'H', 'J', 'K', 'L', ';', ':',   0,   0, ']',  'Z',  'X',  'C',  'V',
			'B',  'N', 'M',  ',', '.', '/',   0, '*',   0, ' ',   0,   0,    0,    0,    0,    0,
			  0,    0,   0,    0,   0,   0,   0, '7', '8', '9', '-', '4',  '5',  '6',  '+',  '1',
			'2',  '3', '0',  '.',   0,   0,   0,   0,   0,   0,   0,   0,    0,    0,    0,    0,
			  0,    0,   0,    0,   0,   0,   0,   0,   0,   0,   0,   0,    0,    0,    0,    0,
			  0,    0,   0, 0x5c,   0,   0,   0,   0,   0,   0,   0,   0,    0, 0x5c,    0,    0
		};
		static char keytable1[0x80] = {
			/*  按下 shift */
			  0,    0, '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~',  '=', '~', 0x08, 0x09,
			'Q',  'W', 'E',  'R', 'T', 'Y', 'U', 'I',  'O', 'P', '`', '{', 0x0a,   0,  'A',  'S',
			'D',  'F', 'G',  'H', 'J', 'K', 'L', '+',  '*',   0,   0, '}',  'Z', 'X',  'C',  'V',
			'B',  'N', 'M',  '<', '>', '?',   0, '*',    0, ' ',   0,   0,    0,   0,    0,    0,
			  0,    0,   0,    0,   0,   0,   0, '7',  '8', '9', '-', '4',  '5', '6',  '+',  '1',
			'2',  '3', '0',  '.',   0,   0,   0,   0,    0,   0,   0,   0,    0,   0,    0,    0,
			  0,    0,   0,    0,   0,   0,   0,   0,    0,   0,   0,   0,    0,   0,    0,    0,
			  0,    0,   0,  '_',   0,   0,   0,   0,    0,   0,   0,   0,    0, '|',    0,    0
		};
		int key_shift = 0, key_ctrl = 0, key_alt = 0, flag_e0 = 0;
		int key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	/* Mouse */
		int mx, my, x = 0, y = 0, mmx = -1, mmy = -1, new_mx = -1, new_my = 0;
		int mmx2 = 0, new_wx = 0x7fffffff, new_wy = 0, flag_mouse = 0;
	/* IME */
		unsigned char *nihongo;
		extern char ascii[4096];
		int oldlang;
	/* Sheets */
		struct SHTCTL *shtctl;
		unsigned char *buf_back, *buf_mouse, *buf_naomi;
		struct SHEET  *sht_back, *sht_mouse, *sht_naomi, *sht = 0, *key_win, *sht2 = 0;
	/* Balloon */
		unsigned char *buf_balloon;
		struct SHEET  *sht_balloon;
		struct BALLOON *startmenu;
		int flag_balloon = 0;
	/* Misc */
		unsigned char s[40];
		int i, j;


	init_gdtidt();
	init_pic();
	io_sti(); /* 由于IDT / 片的初始化结束，取消了CPU中断禁止*/
	fifo32_init(&fifo, 128, fifobuf, 0);
	*((int *) 0x0fec) = (int) &fifo;
	init_pit();
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	io_out8(PIC0_IMR, 0xf8); /* 允许PIT，片1和键盘(11111000) */
	io_out8(PIC1_IMR, 0xef); /* 允许鼠标(11101111) */
	fifo32_init(&keycmd, 32, keycmd_buf, 0);

	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /*	0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);
	*((int *) 0x0fe2) = (int) memtotal;

	init_timerctl();
	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);
	*((int *) 0x0fe4) = (int) shtctl;
	task_a->langmode = 1;

	/* sht_back */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * (bpp >> 3));
	sheet_setbuf(sht_back,  buf_back,  binfo->scrnx, binfo->scrny, -1);	/* 没有透明色 */
	init_screen((unsigned int *) buf_back, binfo->scrnx, binfo->scrny);
	sht_back->flags |= 0x01;	/* 背景 */

	/* sht_naomi */
	sht_naomi = sheet_alloc(shtctl);
	buf_naomi = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * (bpp >> 3));
	sheet_setbuf(sht_naomi, buf_naomi, binfo->scrnx,30, 99);	/* 透明色可用 */
	//init_task((unsigned int *) buf_naomi, binfo->scrnx, binfo->scrny);
	boxfill((unsigned int *)buf_naomi, binfo->scrnx, 0x7A8A9D,  0,     0, binfo->scrnx -  1, binfo->scrny - 28);
	// boxfill((unsigned int *)buf_naomi, binfo->scrnx, 0x7A8A9D,  0,     binfo->scrny - 28, binfo->scrnx -  1, binfo->scrny - 28);
	// boxfill((unsigned int *)buf_naomi, binfo->scrnx, 0x000000,  0,     binfo->scrny - 27, binfo->scrnx -  1, binfo->scrny - 27);
	// boxfill((unsigned int *)buf_naomi, binfo->scrnx, 0x7A8A9D,  0,     binfo->scrny - 26, binfo->scrnx -  1, binfo->scrny -  1);
	//put_naomi((unsigned int *) buf_naomi);
	
	// int ix, t[3];
	// char sx[5];
	// for (ix = 0; ix < 3; ix++) 
	// {
		// t[ix] = rtc_get(ix+3);
	// }
	// sprintf(sx, "%02d:%02d:%02d\n",  t[0], t[1],t[2]);
	putfonts_asc_sht((unsigned int *)sht_naomi, 9,7, 0xffffff, 0x7A8A9D,"menu", 1);
	// putfonts_asc_sht((unsigned int *)sht_naomi, binfo->scrnx-90,7, 0xffffff, 0x7A8A9D,sx, 1);
	sheet_refresh(sht_naomi, 0, 0, sht_naomi->bxsize, sht_naomi->bysize);
	sht_naomi->flags |= 0x100;	/* 特殊窗口 */
// ///时间
struct TASK *clock = task_alloc();
	int *clock_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	clock->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
	clock->tss.eip = (int) &sysclock_task;
	clock->tss.es = 1 * 8;
	clock->tss.cs = 2 * 8;
	clock->tss.ss = 1 * 8;
	clock->tss.ds = 1 * 8;
	clock->tss.fs = 1 * 8;
	clock->tss.gs = 1 * 8;
	task_run(clock, 1, 2); /* level=1, priority=2 */
	fifo32_init(&clock->fifo, 128, clock_fifo, clock);
// /* sht_win_b */
 
// struct TASK *task_shijian;
	 
		// sht_naomi= sheet_alloc(shtctl);
		// // buf_shijian = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * (bpp >> 3));
		// sheet_setbuf(sht_naomi, buf_naomi, binfo->scrnx,30, 99);	/* 透明色可用 */
		 
		// task_shijian = task_alloc();
		// task_shijian->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
		// task_shijian->tss.eip = (int) &task_shijian_main;
		// task_shijian->tss.es = 1 * 8;
		// task_shijian->tss.cs = 2 * 8;
		// task_shijian->tss.ss = 1 * 8;
		// task_shijian->tss.ds = 1 * 8;
		// task_shijian->tss.fs = 1 * 8;
		// task_shijian->tss.gs = 1 * 8;
		// *((int *) (task_shijian->tss.esp + 4)) = (int) sht_naomi;
		// task_run(task_shijian, 2, -1);
			// sheet_slide(sht_naomi,  0,  0);
			// sheet_updown(sht_naomi, 10);
	 
/////////////////

///多任务
/* sht_win_b */
// struct SHEET *sht_win_b[3];
// unsigned char *buf_win_b;
// struct TASK *task_b[3];
	// for (i = 0; i < 3; i++) {
		// sht_win_b[i] = sheet_alloc(shtctl);
		// buf_win_b = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * (bpp >> 3));
		// sheet_setbuf(sht_win_b[i], buf_win_b, 144, 52, -1); /* */
		// sprintf(s, "task_b%d", i);
		// make_window((unsigned int *) buf_win_b, 144, 52, s, 0,0);
		// task_b[i] = task_alloc();
		// task_b[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
		// task_b[i]->tss.eip = (int) &task_b_main;
		// task_b[i]->tss.es = 1 * 8;
		// task_b[i]->tss.cs = 2 * 8;
		// task_b[i]->tss.ss = 1 * 8;
		// task_b[i]->tss.ds = 1 * 8;
		// task_b[i]->tss.fs = 1 * 8;
		// task_b[i]->tss.gs = 1 * 8;
		// *((int *) (task_b[i]->tss.esp + 4)) = (int) sht_win_b[i];
		// task_run(task_b[i], 2, i + 1);
			// sheet_slide(sht_win_b[i],  0,  0);
			// sheet_updown(sht_win_b[i], 10+i);
	// }
/////////////////
	/* sht_balloon */
	sht_balloon = sheet_alloc(shtctl);
	buf_balloon = (unsigned char *) memman_alloc_4k(memman,binfo->scrnx * binfo->scrny  *(bpp >> 3));
	sheet_setbuf(sht_balloon, buf_balloon, 220,370,99);	/* 透明色可用 */
	// startmenu = make_balloon(memman, sht_balloon, 0, 0, 6, 19, 1);
	 boxfill((unsigned int *)buf_balloon, binfo->scrnx, 0xF0F0F0,  0,     0,  binfo->scrnx , 500);
	 putfonts_asc_sht((unsigned int *)sht_balloon, 9,7, 0x000000,0xF0F0F0,"cmd", 1);
	 putfonts_asc_sht((unsigned int *)sht_balloon, 9,25, 0x000000,0xF0F0F0,"shutdown", 1);
	//sheet_refresh(sht_balloon, 0, 0, sht_balloon->bxsize, sht_balloon->bysize);
	// putminifonts_asc_sht(sht_balloon, 4, 3, 0x000000, 0xffffff, "My name is naomisan", 19);
	sht_balloon->flags |= 0x100;	/* 特殊窗口 */

	/* sht_cons */
	key_win = open_console(shtctl, memtotal);
 // showColor();
	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	buf_mouse = (unsigned char *) memman_alloc_4k(memman, 16 * 16 * (bpp >> 3));
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);	/* 透明色番号は99 */
	init_mouse_cursor((unsigned int *) buf_mouse, 99/* 背景色 */);
	mx = (binfo->scrnx - 16) / 2; /* 按显示画面在中央计算坐标 */
	my = (binfo->scrny - 16) / 2;

	sheet_slide(sht_back,  0,  0);
	 sheet_slide(key_win,   8, 16);
	sheet_slide(sht_balloon, 0, binfo->scrny - 370);
	sheet_slide(sht_naomi, 0, binfo->scrny - 30);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back,  0);
	 //sheet_updown(key_win,   1);
	sheet_updown(sht_naomi, 2);
	sheet_updown(sht_mouse, 10);
	// sheet_updown(sht_balloon, 2);
	keywin_on(key_win);

	/* 设置初始键盘状态 */
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);

	/*汉语.字体的读取 */
	nihongo = (unsigned char *) memman_alloc_4k(memman, 0x5d5d * 32);
	fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
	finfo = file_search("nihongo.fnt", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo != 0) {
		file_loadfile(finfo->clustno, finfo->size, nihongo, fat, (char *) (ADR_DISKIMG + 0x003e00));
	} else {
		for (i = 0; i < 16 * 256; i++) {
			nihongo[i] = ascii[i]; /* 僼僅儞僩偑側偐偭偨偺偱敿妏晹暘傪僐僺乕 */
		}
		for (i = 16 * 256; i < 16 * 256 + 32 * 94 * 47; i++) {
			nihongo[i] = 0xff; /* 僼僅儞僩偑側偐偭偨偺偱慡妏晹暘傪0xff偱杽傔恠偔偡 */
		}
	}

	*((int *) 0x0fe8) = (int) nihongo;
	memman_free_4k(memman, (int) fat, 4 * 2880);

	
	/*加载壁纸*/
	// char *cmdlinex = (char *) memman_alloc_4k(memman, 10*10);
	// cmdlinex[0]='c';
	// cmdlinex[1]='i';
	// cmdlinex[2]='t';
	// cmdlinex[3]='y';
	// cmdlinex[4]='.';
	// cmdlinex[5]='j';
	// cmdlinex[6]='p';
	// cmdlinex[7]='g';	 		
	struct PICTURE *wall = picture_init("city.jpg", fat);	 
	picture_draw(sht_back, wall, (sht_back->bxsize-wall->info[2])/2, (sht_back->bysize-wall->info[3])/2);
	picture_free(wall);	 
	sheet_refresh(sht_back, 0, 0, sht_back->bxsize, sht_back->bysize);
	
	/*加???*/
	// struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	// struct FILEINFO *finfo;
	// struct TASK *task = task_now();
	// int i = 0, j, k, flag = 0;
	// char s[30];
	i=0;
	int  k,flag = 0;
	unsigned char *dirlist = 0;
	int dirlist_size, dir_clustno;
 
		finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	 	int xx=0,yy=0;
	for (i = 0; i < 224; i++) 
	{
		// if (i != 0 && (i % 11) == 0) {
			// putfonts_asc_sht((unsigned int *)sht_back, 9,75, 0x000000,0xF0F0F0,s, 1); 
			// for (;;) {
				// io_cli();
				// if (fifo32_status(&task->fifo) == 0) {
					// task_sleep(task);
					// io_sti();
				// } else {
					// k = fifo32_get(&task->fifo);
					// io_sti();
					// if (256 <= k && k < 512) {
						// //cons_newline(cons);
						// break;
					// }
				// }
			// }
		// }
		if (finfo[i].name[0] == 0x00) 
		{
			break;
		}
		if (finfo[i].name[0] != 0xe5) 
		{
			if ((finfo[i].type & 0x18) == 0) 
			{
				/* 普通文件 */
				// sprintf(s, "filename", finfo[i].size);
				for (j = 0; j < 8; j++) 
				{
					s[j] = finfo[i].name[j];
				}
				// s[10] = finfo[i].ext[0];
				// s[11] = finfo[i].ext[1];
				// s[12] = finfo[i].ext[2];
				
				int *fat1= (int *) memman_alloc_4k(memman, 4 * 2880);
 file_readfat(fat1, (unsigned char *) (ADR_DISKIMG + 0x000200));
	unsigned char *buf_tubiao;
	struct SHEET *sht_tubiao;
	sht_tubiao = sheet_alloc(shtctl);
	buf_tubiao = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny * (bpp >> 3));
	sheet_setbuf(sht_tubiao,  buf_tubiao,  32, 32, -1);	/* 没有透明的颜色 */
init_screen((unsigned int *) buf_tubiao, binfo->scrnx, binfo->scrny);	
	sht_tubiao->flags |= 0x01;	/* 背景 */
	
	// char *cmdlinex1 = (char *) memman_alloc_4k(memman, 10*10);
	// cmdlinex1[0]='n';
	// cmdlinex1[1]='y';
	// cmdlinex1[2]='a';
	// cmdlinex1[3]='n';
	// cmdlinex1[4]='p';
	// cmdlinex1[5]='o';
	// cmdlinex1[6]='.';
	// cmdlinex1[7]='i';
	// cmdlinex1[8]='c';
	// cmdlinex1[9]='o';	
	struct PICTURE *wall1 = picture_init("nyanpo.ico", fat1);	 
	picture_draw(sht_tubiao, wall1, (sht_tubiao->bxsize-wall1->info[2])/2, (sht_tubiao->bysize-wall1->info[3])/2);
	picture_free(wall1);	 
	

		
	sheet_slide(sht_tubiao,  50*xx+20,  80*yy+10);
		sheet_updown(sht_tubiao,  1);
		
				putfonts_asc_sht((unsigned int *)sht_back, 50*xx+20,80*yy+50, 0xFFFFFF,0x000000,s, 0); 
				// putfonts_asc_sht((unsigned int *)sht_back, 10,80*i+52, 0x413B31,0x000000,s, 0); 
				
				sheet_refresh(sht_tubiao, 0, 0, sht_tubiao->bxsize, sht_tubiao->bysize);
		sheet_refresh(sht_back, 0, 0, sht_back->bxsize, sht_back->bysize);
		
		if(i == 7)
		{ 	xx=1;
			yy=0;
			}
	else{
		yy=yy+1;
	}
			} 
		}
	}
	// if (flag == 1) {
			// memman_free_4k(memman, (int) dirlist, dirlist_size);
	// }
	 
	//计算机
	struct SHEET *sht_jisuanji = sheet_alloc(shtctl);
	unsigned char *buf_jisuanji = (unsigned char *) memman_alloc_4k(memman, (600) * (400) * (bpp >> 3));
	sheet_setbuf(sht_jisuanji, buf_jisuanji, 600, 400, -1);	/* 没有透明的颜色 */
	make_window((unsigned int *) buf_jisuanji,600, 400, "文件管理", 0, 0);
	sheet_slide(sht_jisuanji, 80, 80);
	sheet_updown(sht_jisuanji,  8);
	
	/* startup */
	finfo = file_search("osakkie.hsf", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	j = 2;
	if (finfo != 0) 
	{
		char *ss;
		i = finfo->size;
		ss = file_loadfile2(finfo->clustno, &i, fat);
		for (x = 0; x < i; x++) 
		{
			if (ss[x] == '#' && j == 2) 
			{
				/* 如果该行的开头有＃，请将其注释掉 */
				j = 1;
			}
			if (ss[x] == 0x0a) 
			{
				/* 由于已到达行尾，因此取消注释标记 */
				j = 2;
			}
			if (ss[x] != 0x0d && j != 1) 
			{
				fifo32_put(&key_win->task->fifo, ss[x] + 256);
			}
		}
		memman_free_4k(memman, (int) ss, i);
	}

	memman_free_4k(memman, (int) fat, 4 * 2880);

	/* minlist的初始化 */
	for(i = 0; i < MAX_MINID; i++) 
	{
		minlist[i] = -1;		/* 未使用 */
	}

	for (;;) 
	{
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) 
		{
			/*发送要发送到键盘的任何数据 */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if (fifo32_status(&fifo) == 0) 
		{
			/* fifoがカラッポになったので、保留している描画があれば実行 */
			if (new_mx >= 0) 
			{
				io_sti();
				sheet_slide(sht_mouse, new_mx, new_my);
				new_mx = -1;
			} 
			else if (new_wx != 0x7fffffff) 
			{
				io_sti();
				sheet_slide(sht, new_wx, new_wy);
				new_wx = 0x7fffffff;
			} 
			else 
			{
				task_sleep(task_a);
				io_sti();
			}
		} 
		else 
		{
			i = fifo32_get(&fifo);
			io_sti();
			if (key_win != 0 && key_win->flags == 0) 
			{
				/* 输入窗口已关闭 */
				if (shtctl->top == 2) 
				{
					/* 没有更多的鼠标和背景 */
					key_win = 0;
				} 
				else 
				{
					key_win = shtctl->sheets[shtctl->top - 2];
					keywin_on(key_win);
				}
			}
			if (256 <= i && i < 512) 
			{
				/* 键盘数据 */
				i -= 256;	// 先拉
				if (i == 0xe0) 
				{
					/*  E0扩展密钥 */
					flag_e0 = 1;
				}
				if (i < 0x80 && flag_e0 == 0) 
				{
					/* 将密钥代码转换为字符代码 */
					if (key_shift == 0) 
					{
						s[0] = keytable0[i];
					} 
					else if (key_shift == 1) 
					{
						s[0] = keytable1[i];
					}
				} 
				else 
				{
					s[0] = 0;
				}
				if ('A' <= s[0] && s[0] <= 'Z')	
				{
					/* 字母输入 */
					if (((key_leds & 4) == 0 && key_shift == 0) ||
						((key_leds & 4) != 0 && key_shift != 0)   ) 
					{
						s[0] += 0x20;	/* 转换为小写 */
					}
				}
				if (flag_e0 == 1) 
				{
					if (i == 0x35 || i == 0x1c) 
					{
						/* 回车键 */
						s[0] = keytable0[i];
					}
				}
				if (0x47 <= i && i <= 0x53) 
				{
					if (i != 0x4a && i != 0x4e) 
					{
						/* 数字小键盘和输入s，Del，Home，end，PgUp，PgDn，光标键*/
						if (flag_e0 == 1 || (key_leds & 2) != 0) 
						{
							s[0] = keytable0[i] + 0x80;		// 一律に0x80を足す
						}
					}
				}
				if (i == 0x0f && key_alt != 0) 
				{
					/*  Alt + Tab键不受代码传输的影响 */
					s[0] = 0;
				}
				if (s[0] != 0 && key_win != 0) 
				{
					/* 普通字符，Enter，BackSpace，Tab */
					if (key_ctrl == 0) 
					{
						/* 控制台 */
						fifo32_put(&key_win->task->fifo, s[0] + 256);
					}
				}
				if (i == 0x57 && shtctl->top > 2) 
				{
					/* F11键 */
					sheet_updown(shtctl->sheets[1], shtctl->top - 2);
				}
				if (i == 0x2a)
					key_shift |= 1;			// 左移on
				if (i == 0x36)
					key_shift |= 2;			// 右移on
				if (i == 0xaa)
					key_shift &= ~1;		// 左移off
				if (i == 0xb6)
					key_shift &= ~2;		// 右移off
				if (i == 0x1d) {
					if (flag_e0 == 0) 
					{
						key_ctrl |= 1;		// 左CTRLon
					} 
					else 
					{
						key_ctrl |= 2;		// 右CTRLon
					}
				}
				if (i == 0x9d) 
				{
					if (flag_e0 == 0) 
					{
						key_ctrl &= ~1;		// 左CTRLoff
					} 
					else 
					{
						key_ctrl &= ~2;		// 右CTRLoff
					}
				}
				if (i == 0x38) 
				{
					if (flag_e0 == 0) 
					{
						key_alt |= 1;		// 左ALTon
					}
					else 
					{
						key_alt |= 2;		// 右ALTon
					}
				}
				if (i == 0xb8) 
				{
					if (flag_e0 == 0) 
					{
						key_alt &= ~1;		// 左ALToff
					} 
					else 
					{
						key_alt &= ~2;		// 右ALToff
					}
				}
				if (i == 0x3a) 
				{
					/* CapsLock */
					key_leds ^= 4;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 0x45) 
				{
					/* NumLock */
					key_leds ^= 2;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 0x46) 
				{
					/* ScrollLock */
					key_leds ^= 1;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (((i == 0x0f && key_alt != 0) || i == 0x43) && key_win != 0) 
				{
					/* Tab修改了 Alt + Tab or F9 */
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0) 
					{
						j = shtctl->top - 2;
					}
					key_win = shtctl->sheets[j];
					keywin_on(key_win);
				}
				if (i == 0x2e && key_ctrl != 0 && key_win != 0) 
				{
					/* Shift + F1修改了Ctrl + C. */
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) 
					{
						task->langmode = task->langmode0;	/* APIで変更したlangmodeを元に戻す */
						oldlang = task_a->langmode;		/* task_aのlangmodeを一時的にタスクにあわせる */
						task_a->langmode = task->langmode;
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						task_a->langmode = oldlang;
						io_cli();
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						io_sti();
						task_run(task, -1, 0);
					}
				}
				if (i == 0x31 && key_ctrl != 0) 
				{
					/*  Shift + F2修改了 Ctrl + N  */
					/* 激活新创建的控制台 */
					keywin_off(key_win);
					key_win = open_console(shtctl, memtotal);
					sheet_slide(key_win, 8, 16);
					sheet_updown(key_win, shtctl->top - 1);
					keywin_on(key_win);
				}
				if (i != 0xe0 && flag_e0 == 1) 
				{
					/* E0扩展 */
					flag_e0 = 0;
				}
				if (i == 0xfa) 
				{
					/* 键盘接收数据 */
					keycmd_wait = -1;
				}
				if (i == 0xfe) 
				{
					/* 键盘无法接收数据 */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
			} 
			else if (512 <= i && i < 768) 
			{
				/* 鼠标 */
				if (mouse_decode(&mdec, i - 512)) 
				{
					/* 鼠标移动 */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0)
						mx = 0;
					if (my < 0)
						my = 0;
					if (mx > binfo->scrnx - 1)
						mx = binfo->scrnx - 1;
					if (my > binfo->scrny - 1)
						my = binfo->scrny - 1;
					new_mx = mx;
					new_my = my;
					if ((mdec.btn != 0 || flag_mouse != 0) && mmx < 0) 
					{
						/* makeがbreakがきている */
						/* 从底部开始按顺序搜索标签指向的工作表 */
						sht = search_sheet(shtctl, mx, my);
						x = mx - sht->vx0;
						y = my - sht->vy0;
						if (mdec.btn != 0 && sht != sht_naomi) 
						{
							sheet_updown(sht_balloon, -1);	/* 擦除气球 */
							flag_balloon = 0;
						}
					}
					if (mdec.btn & 0x01) 
					{
						/* 左键 */
						flag_mouse |= 0x01;
						if (mmx < 0) 
						{
							/* 正常模式 */
							if ((sht->flags & 0x100) != 0) 
							{
								/* Naomi-san或开始菜单 */
								if (sht == sht_naomi) 
								{
									if (flag_balloon == 0) 
									{
										flag_balloon = 1;
										sheet_updown(key_win,   1);
										// sheet_slide(sht_balloon, sht_naomi->vx0 - 80, sht_naomi->vy0 - 30);
										sheet_updown(sht_balloon, shtctl->top - 1);
										
										// putfonts_asc_sht((unsigned int *)sht_back, 100,300, 0xFFFFFF,0x000000,".epm file format error.\n", 0); 
											// sheet_refresh(sht_back, 0, 0, sht_back->bxsize, sht_back->bysize);
										///////////////////
										// showColor();
										struct CONSOLE cons;
	// struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
		// int *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
		// char *cmdlinexx = (char *) memman_alloc_4k(memman, 10*10);
// cmdlinexx[0]='n';
	// cmdlinexx[1]='c';
	// cmdlinexx[2]='s';
	// cmdlinexx[3]='t';
	// cmdlinexx[4]=' ';
	// cmdlinexx[5]='i';
	// cmdlinexx[6]='n';
	// cmdlinexx[7]='v';
	// cmdlinexx[8]='a';
	// cmdlinexx[9]='d';
	// cmdlinexx[10]='e';
	// cmdlinexx[11]='r';
	// cmdlinexx[12]='.';
	// cmdlinexx[13]='e';
	// cmdlinexx[14]='p';
	// cmdlinexx[15]='m'; 
										 cmd_ncst(&cons, "ncst invader.epm",memtotal);
										//////////////////
									}
									if (flag_balloon == 2) 
									{
										flag_balloon = 3;
										sheet_updown(key_win,   -1);
										sheet_updown(sht_balloon, -1);
									}
								}
							}
							if ((sht->flags & 0xf00) == 0x200) 
							{
								/* 窗口被最小化 */
								flag_mouse |= 0x0130;
							}
							if ((sht->flags & 0xf00) == 0) 
							{
								/* 像窗户一样 */
								sheet_updown(sht, shtctl->top - 2);
								if (sht != key_win && sht != 0) 
								{
									keywin_off(key_win);
									key_win = sht;
									keywin_on(key_win);
								}
								/* 一般窗口 */
								if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) 
								{
									mmx = mx;	/* 到窗口移动模式 */
									mmy = my;
									mmx2 = sht->vx0;
									new_wy = sht->vy0;
								}
								if (5 <= y && y < 19) 
								{
									if (sht->bxsize - 20 <= x && x < sht->bxsize - 5) 
									{
										/* [X]按键 */
										flag_mouse |= 0x0110;
									}
									if (sht->bxsize - 31 <= x && x < sht->bxsize - 21) 
									{
										/* [_]按键 */
										flag_mouse |= 0x0120;
									}
								}
							}
						} 
						else 
						{
							if ((flag_mouse & 0x0f00) == 0) 
							{
								x = mx - mmx;	/*移动计算 */
								y = my - mmy;
								/* 我没有按下按键等 */
								/* 窗口移动模式 */
								new_wx = (mmx2 + x + 2) & ~3;
								new_wy = new_wy + y;
								mmy = my;		/* 移动后更新坐标 */
							}
						}
					} 
					else if (mdec.btn & 0x02) 
					{
						/* 右键单击 */
						flag_mouse |= 0x02;
						if (mmx < 0) 
						{
							if (sht == sht_naomi || (sht->flags & 0x200) != 0) 
							{
								mmx = mx;	/*  要移动模式 */
								mmy = my;
								mmx2 = sht->vx0;
								new_wy = sht->vy0;
							}
						} 
						else 
						{
							if ((flag_mouse & 0x0f00) == 0) 
							{
								x = mx - mmx;	/*移动计算 */
								y = my - mmy;
								/* 我没有按下按键等 */
								/* 窗口移动模式 */
								new_wx = (mmx2 + x + 2) & ~3;
								new_wy = new_wy + y;
								mmy = my;		/* 移动后更新坐标 */
							}
						}
					} 
					else if (mdec.btn == 0) 
					{
						/* breakか、なにもない */
						if (flag_mouse & 0x01) 
						{
							/* 左break */
							if ((flag_mouse & 0x0f00) == 0x0100 && 5 <= y && y < 19) 
							{
								/* 窗户围栏 */
								if ((flag_mouse & 0xf0) == 0x10 && sht->bxsize - 20 <= x && x < sht->bxsize - 5) 
								{
									/* 窗口关闭 */
									task = sht->task;
									if ((sht->flags & 0x10) != 0) 
									{
										/* 是应用程序制作的窗口吗？ */
										task->langmode = task->langmode0;	/* APIで変更したlangmodeを元に戻す */
										oldlang = task_a->langmode;		/* task_aのlangmodeを一時的にタスクにあわせる */
										task_a->langmode = task->langmode;
										cons_putstr0(task->cons, "\nBreak(mouse) :\n");
										task_a->langmode = oldlang;
										io_cli();
										task->tss.eax = (int) &(task->tss.esp0);
										task->tss.eip = (int) asm_end_app;
										io_sti();
										task_run(task, -1, 0);
									} 
									else 
									{
										/* Console */
										sheet_updown(sht, -1);	/* 隐藏现在 */
										keywin_off(key_win);
										key_win = 0;
										if (shtctl->top > 2) 
										{
											key_win = shtctl->sheets[shtctl->top - 2];
											keywin_on(key_win);
										}
									}
								}
								if ((flag_mouse & 0xf0) == 0x20 && sht->bxsize - 31 <= x && x < sht->bxsize - 21) 
								{
									/* [_]按键点击 */
									sheet_updown(sht, -100);	/* 隐藏 */
									keywin_off(key_win);
									key_win = 0;
									sht2 = open_omnaomi(shtctl, memtotal, sht);
									sheet_slide(sht2, sht->vx0 + sht->bxsize - 45, sht->vy0);
									if (shtctl->top > 2) 
									{
										key_win = shtctl->sheets[shtctl->top - 2];
										keywin_on(key_win);
									}
									sheet_updown(sht2, 1);
									for(j = 0; j < MAX_MINID; j++) 
									{
										if (minlist[j] < 0) 
										{
											minlist[j] = 1;		/* min_id使用中 */
											sht2->min_id = j;	/*  匹配标签？ */
											sht->min_id = j;
											break;
										}
									}
								}
								if ((flag_mouse & 0xf0) == 0x30) 
								{
									/* 点击Omelette Naomi */
									keywin_off(key_win);
									for (j = 0; j < MAX_SHEETS; j++) 
									{
										key_win = &shtctl->sheets0[j];
										if (key_win->height < 0) 
										{
											if (key_win->min_id == sht->min_id) 
											{
												minlist[sht->min_id] = -1;
												sht->min_id = -1;
												key_win->min_id = -1;
												sheet_free(sht);
												sheet_slide(key_win, sht->vx0 - key_win->bxsize + 42, sht->vy0);
												sheet_updown(key_win, shtctl->top - 1);
												keywin_on(key_win);
												break;
											}
										}
									}
								}
							}
							/* Naomi-san或开始菜单 */
							if (flag_balloon == 1) 
							{
								flag_balloon = 2;
							} else if (flag_balloon == 3) 
							{
								flag_balloon = 0;
							}
						} else if (flag_mouse & 0x02) 
						{
							/* 右break */
						}
						flag_mouse = 0;
						mmx = -1;	/* 正常模式 */
						if (new_wx != 0x7fffffff) 
						{ 
							sheet_slide(sht, new_wx, new_wy);	/* 确认一次  */
							new_wx = 0x7fffffff;
						}
					}
				}
			} 
			else if (768 <= i && i < 1024) 
			{
				close_console(shtctl->sheets0 + (i - 768));
			} 
			else if (1024 <= i && i < 2024) 
			{
				close_constask(taskctl->tasks0 + (i - 1024));
			} 
			else if (2024 <= i && i < 2280) 
			{
				/* 仅关闭控制台 */
				sht2 = shtctl->sheets0 + (i - 2024);
				memman_free_4k(memman, (int) sht2->buf, 256 * 165);
				sheet_free(sht2);
			} 
			else if (i == 0x4000) 
			{
				/* key_win変更要求 from 应用程序接口.乙 */
				key_win = shtctl->sheets[shtctl->top - 2];
			} 
			else if (i == 0x4001) 
			{
				/* 刷新鼠标坐标 from 应用程序接口.乙 - 0x400a */
				mx = sht_mouse->vx0;
				my = sht_mouse->vy0;
			}
		}
	}
}
void showColor33()
{
	struct CONSOLE cons;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
		int *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
		char *cmdlinexx = (char *) memman_alloc_4k(memman, 10*10);
	cmdlinexx[0]='i';
	cmdlinexx[1]='n';
	cmdlinexx[2]='v';
	cmdlinexx[3]='a';
	cmdlinexx[4]='d';
	cmdlinexx[5]='e';
	cmdlinexx[6]='r';
	cmdlinexx[7]='.';
	cmdlinexx[8]='e';
	cmdlinexx[9]='p';
	cmdlinexx[10]='m'; 
	  cmd_app(&cons,fat,cmdlinexx);
	  return;
}
void showColor()
{
	 
struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
	struct FILEINFO *finfo;
	struct SHTCTL *shtctl;
	struct SHEET *sht;
	char name[18], *p, *q;
	struct TASK *task = task_now();
	int i;
	int segsize, datsize, esp, dathrb, appsize;

	task->langmode0 = task->langmode;
	char *cmdlinexx = (char *) memman_alloc_4k(memman, 10*10);
	cmdlinexx[0]='c';
	cmdlinexx[1]='o';
	cmdlinexx[2]='l';
	cmdlinexx[3]='o';
	cmdlinexx[4]='r';
	cmdlinexx[5]='.';
	cmdlinexx[6]='e';
	cmdlinexx[7]='p';
	cmdlinexx[8]='m'; 
/*  从命令行生成文件名 */
	// for (i = 0; i < 13; i++) {
		// if (cmdlinexx[i] <= ' ')
			// break;
		// name[i] = cmdlinexx[i];
	// }
	// name[i] = 0;
 
	finfo = file_search("invader.epm", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	// if (finfo == 0 && name[i - 1] != '.') {
		// /* 找不到，用EPM重试 */
		// name[i    ] = '.';
		// name[i + 1] = 'e';
		// name[i + 2] = 'p';
		// name[i + 3] = 'm';
		// name[i + 4] = 0;
		// finfo = file_search(cmdlinexx, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	// }
	// if (finfo == 0) {
		// /* 仍然不合理 */
		// // return 0;
	// }

	/* 有一个档案 */
	appsize = finfo->size;
	p = file_loadfile2(finfo->clustno, &appsize, fat);
	// if (appsize >= 36 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00) {
		segsize = *((int *) (p + 0x0000));
		esp     = *((int *) (p + 0x000c));
		datsize = *((int *) (p + 0x0010));
		dathrb  = *((int *) (p + 0x0014));
		q = (char *) memman_alloc_4k(memman, segsize);
		task->ds_base = (int) q;
		set_segmdesc(task->ldt + 0, appsize - 1, (int) p, AR_CODE32_ER + 0x60);
		set_segmdesc(task->ldt + 1, segsize - 1, (int) q, AR_DATA32_RW + 0x60);
		for (i = 0; i < datsize; i++)
			q[esp + i] = p[dathrb + i];
		start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
		shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
		for (i = 0; i < MAX_SHEETS; i++) 
		{
			sht = &(shtctl->sheets0[i]);
			if ((sht->flags & 0x11) == 0x11 && sht->flags && sht->task == task) 
			{
				/* 应用程序结束了，但仍有一个底层 */
				sheet_free(sht);		/* 关闭 */
			}
		}
		for (i = 0; i < 8; i++) 
		{
			/* 关闭未关闭的文件 */
			if (task->fhandle[i].buf != 0) 
			{
				memman_free_4k(memman, (int) task->fhandle[i].buf, task->fhandle[i].size);
				task->fhandle[i].buf = 0;
			}
		}
		task->langmode = task->langmode0;		// APIで変更されたlangmodeを元に戻す
		timer_cancelall(&task->fifo);
		memman_free_4k(memman, (int) q, segsize);
		task->langbyte1 = 0;
	// } else {
		// putfonts_asc_sht((unsigned int *)sht_back, 100,300, 0xFFFFFF,0x000000,".epm file format error.\n", 0); 
		// // cons_putstr0(cons, ".epm file format error.\n");
	// }
	memman_free_4k(memman, (int) p, appsize); 
}

void keywin_off(struct SHEET *key_win)
{
	change_wtitle(key_win, 0);
	if ((key_win->flags & 0x20) != 0) 
	{
		fifo32_put(&key_win->task->fifo, 3);	/* 控制台光标关闭 */
	}
	return;
}

void keywin_on(struct SHEET *key_win)
{
	change_wtitle(key_win, 1);
	if ((key_win->flags & 0x20) != 0) 
	{
		fifo32_put(&key_win->task->fifo, 2);	/* 控制台光标打开 */
	}
	return;
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_alloc();
	int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int) &console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2);
	fifo32_init(&task->fifo, 128, cons_fifo, task);
	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal)
{
/* Old Window size : 256 * 165 (TextBox : 240, 128) */
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	int bpp = get_bpp();
	unsigned char *buf = (unsigned char *) memman_alloc_4k(memman, (CONSOLE_SIZEX + 16) * (CONSOLE_SIZEY + 37) * (bpp >> 3));
	sheet_setbuf(sht, buf, CONSOLE_SIZEX + 16, CONSOLE_SIZEY + 37, -1);	/* 没有透明的颜色 */
	make_window((unsigned int *) buf, CONSOLE_SIZEX + 16, CONSOLE_SIZEY + 37, "控制台", 0, 0);
	make_textbox(sht, 8, 28, CONSOLE_SIZEX, CONSOLE_SIZEY, 0x000000);
	sht->task = open_constask(sht, memtotal);
	sht->flags |= 0x20;	/* 用光标 */
	return sht;
}

struct SHEET *open_omnaomi(struct SHTCTL *shtctl, unsigned int memtotal, struct SHEET *oldsht)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	int bpp = get_bpp();
	unsigned char *buf;

	buf = (unsigned char *) memman_alloc_4k(memman, 31 * 16 * (bpp >> 3));
	sheet_setbuf(sht, buf, 31, 16, 99);
	make_omnaomi(sht, 0, 0);
	sht->flags |= 0x0200;
	return sht;
}

void close_constask(struct TASK *task)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	task->flags = 0;	/* task_free(task);而不是 */
	
}

void close_console(struct SHEET *sht)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165);
	sheet_free(sht);
	close_constask(task);
	return;
}

struct SHEET *search_sheet(struct SHTCTL *shtctl, int mx, int my)
{
	struct SHEET *sht = 0;
	int i, x, y;
	for (i = shtctl->top - 1; i > 0; i--) 
	{
		sht = shtctl->sheets[i];
		x = mx - sht->vx0;
		y = my - sht->vy0;
		if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) 
		{
			if (sht->buf[y * sht->bxsize + x] != sht->col_inv) 
			{
				return sht;
			}
		}
	}
	return 0;
}
