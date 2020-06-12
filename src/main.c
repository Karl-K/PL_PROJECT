//-------| src/main.c |-------//
#include "main.h"


static off_t IEB_DOT[MAX_DOT] = {
	IEB_DOT1,
	IEB_DOT2,
	IEB_DOT3,
	IEB_DOT4,
	IEB_DOT5
};
static off_t IEB_FND[MAX_FND] = {
	IEB_FND0,
	IEB_FND1,
	IEB_FND2,
	IEB_FND3,
	IEB_FND4,
	IEB_FND5,
	IEB_FND6,
	IEB_FND7
};

static int fd;
static int map_counter = 0;
static void * map_data[100];
static selection_t sel; 
int people = 0;
int money = 0;
int mode = 0;
int mini = 0;


int main(int argc, char* argv[]) {
	
	int i;
	short * led, * dot[MAX_DOT], * fnd[MAX_FND];
	short * clcd_cmd, * clcd_data, * keypad_out, * keypad_in;
	
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		fprintf(stderr, "Cannot open /dev/mem file");
		exit(EXIT_FAILURE);
	}
	
	led = mapper(IEB_LED, PROT_WRITE);
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = mapper(IEB_DOT[i], PROT_WRITE);
	}
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = mapper(IEB_FND[i], PROT_WRITE);
	}
	clcd_cmd  = mapper(IEB_CLCD_CMD, PROT_WRITE);
	clcd_data = mapper(IEB_CLCD_DATA, PROT_WRITE);
	//keypad_out  = mapper(IEB_KEY_W, PROT_WRITE);
	//keypad_in = mapper(IEB_KEY_R, PROT_READ);
	
	init_led(led);
	init_dot(dot);
	init_fnd(fnd);
	init_clcd(clcd_cmd, clcd_data);
	//init_keypad(keypad_out, keypad_in);
	
	sel.all = 0;
	while( logic() == TRUE ) {	}
	
	unmapper();
	close(fd);
	return 0;
}

short * mapper(off_t offset, int prot) {
	map_data[map_counter] = mmap(NULL, sizeof(short), prot, MAP_SHARED, fd, offset);
	if ( map_data[map_counter] == MAP_FAILED ) {
		fprintf(stderr, "Cannot do mmap()");
		emergency_closer();
	}
	return (short *)map_data[map_counter++];
}

void unmapper() {
	int i;
	for( i=0; i<map_counter; i++) {
		munmap(map_data[i], sizeof(short));
	}
	map_counter = 0;
}

void emergency_closer() {
	unmapper();
	close(fd);
	exit(EXIT_FAILURE);
}

truth_t logic() {
	if( sel.all == 0 ) { select_mode(); }
	else if( sel.exit == 1 ) { return FALSE; }
	else if( sel.input == 1 ) { input_mode(); }
	else if( sel.mini == 1 ) { mini_game(); }
	else { output_mode(); }
	return TRUE;
}

void select_mode() {
	int i; char buf[1];
	
	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();

	printf("\n");
	printf("************************************\n");
	printf("*    SHARE THE EXPENSES EQUALLY    *\n");
	printf("*   1 : 10    2 : 100    3 : 1000  *\n");
	printf("*         press 'e' to exit        *\n");
	printf("************************************\n\n");
	scanf("%s", buf);

	if( buf[0] == 'e' ) { sel.exit = 1;}
	else {
		sel.input = 1;
		if( buf[0] == '1' ) { mode = 10; }
		else if( buf[0] == '2' ) { mode = 100; }
		else if( buf[0] == '3' ) { mode = 1000; }
		else {
			printf("ERRPR"); sel.all = 0;
		}
	}
	
}

void input_mode() {
	int key_value, i, j;
	char clcd_str1[20];
	char clcd_str2[20];
	sprintf( clcd_str1, "People : ");
	sprintf( clcd_str2, "Money : ");
	clcd_set_DDRAM( 0x00 );
	clcd_write_string( clcd_str1 );
	clcd_set_DDRAM( 0x40 );
	clcd_write_string( clcd_str2 );
	
	i = keyboard_read( &key_value );
	people = key_value;
	dot_write( people );
	sprintf( clcd_str1, "People : %d", people );
	clcd_set_DDRAM( 0x00 );
	clcd_write_string( clcd_str1 );
	
	j = keyboard_read( &key_value );
	money = key_value;
	fnd_deci_number( money );
	sprintf( clcd_str2, "Money : %d", money );
	clcd_set_DDRAM( 0x40 );
	clcd_write_string( clcd_str2 );


	if( (money/people)%mode == 0 ) {
		sel.input = 0; sel.output = 1;
	}
	else {
		sel.input = 0; sel.mini = 1;
	}


	usleep(4000000);
}
	

void mini_game() {
	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();

	int boom;
	int d = 0, draw = 0;
	int p[people];
	int re[people];
	int diff;
	int loser;
	int min = 16;
	char explain[20];
	char closer[20];
	char cboom[20];
	char restart1[20];
	char restart2[20];
	int key_value;

	led_blink_all();
	
	srand( time(NULL) );	// seed
	boom =  rand()%16;
	
	int i, j, k;
	do {
		if( d = 0 ) {
			for( i=0; i<people; i++ ) {
				clcd_clear_display();
				sprintf( explain, "Press key for %d", i + 1 );
				clcd_write_string( explain );
				k = keyboard_read( &key_value );
				p[i] = key_value;
		
				diff = abs( boom - p[i] );
		
				if( min > diff ) {
					min = diff;
					loser = i + 1;
					re[i] = 1;
				}
				else if( min == diff ) {
					re[i] = 1;
					draw = 1;
					d = 1;
				}
				else {}
			}
			
			if( draw = 1) {
				clcd_clear_display();
				sprintf( restart1, "Draw !! ");
				clcd_write_string( restart1 );
				clcd_set_DDRAM( 0x40 );
				sprintf( restart2, "Ready to Restart" );
				clcd_write_string( restart2 );
				usleep(2500000);
			}
			
		}

		else {
			clcd_clear_display();
			for ( i = 0; i<people; i++ ) {
				p[i] = 0;
			}
			
			draw = 0;

			for( i=0; i<people; i++ ) {
				if( re[i] == 1 ) {
					clcd_clear_display();
					sprintf( explain, "Press key for %d", re[i] );
					clcd_write_string( explain );
					k = keyboard_read( &key_value );
					p[i] = key_value;
		
					diff = abs( boom - p[i] );
		
					if( min > diff ) {
						min = diff;
						loser = i + 1;
						re[i] = 1;
					}
					else if( min == diff ) {
						re[i] = 1;
						draw = 1;
						d = 1;
					}
					else {}
				}
			}

			if( draw = 1) {
				clcd_clear_display();
				sprintf( restart1, "Draw !! ");
				clcd_write_string( restart1 );
				clcd_set_DDRAM( 0x40 );
				sprintf( restart2, "Ready to Restart" );
				clcd_write_string( restart2 );
				usleep(2500000);
			}
		}
	} while( draw = 0 );


	/* for( i=0; i<people; i++ ) {
		clcd_clear_display();
		sprintf( explain, "Press key for %d", i + 1 );
		clcd_write_string( explain );
		k = keyboard_read( &key_value );
		p[i] = key_value;
		
		diff = abs( boom - p[i] );
		
		if ( min >= diff ) {
			min = diff;
			loser = i + 1;
			re[i] = loser;
		}
		else { min = min; }
	}

	for( i=0; i<people; i++ ) {
		for( j=0; j<people; j++ ) {
			if( (i != j) && (re[i] != NULL) ) {
				clcd_clear_display();
				sprintf( restart1, "Draw !! ");
				clcd_write_stirng( restart1 );
				clcd_set_DDRAM( 0x40 );
				sprintf( restart2, "Ready to Restart" );
				clcd_write_display( restart2 );
			}
		}
	}
	*/

	clcd_clear_display();
	sprintf( cboom, "B : %d, p : %d", boom, p[loser-1] );
	clcd_write_string( cboom );
	sprintf( closer, "Looser : %d", loser );
	clcd_set_DDRAM( 0x40 );
	clcd_write_string( closer );
	dot_write( loser );

	usleep(5000000);
	mini = 1;
	sel.mini = 0;
	sel.output = 1;
}

void output_mode() {
	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();

	int money_each;
	int money_loser;
	int due;
	char money_for[20];
	char cmoney_each[20];
	char cmoney_loser[20];
	

	if( mini == 0 ) {
		sprintf( money_for, "Money for each : " );
		clcd_write_string( money_for );
		money_each = money / people;
		sprintf( cmoney_each, "%d", money_each );
		clcd_set_DDRAM( 0x40 );
		clcd_write_string( cmoney_each );
		dot_arrow();
		led_blink_all();
		fnd_deci_number( money_each );
		
		usleep(5000000);
	}
	else {
		due = (money/people) % mode;
		money_each = (money/people) - due;
		money_loser = money_each + money - (money_each * people);
		sprintf( cmoney_each, "each : %d", money_each );
		clcd_write_string( cmoney_each );
		sprintf( cmoney_loser, "loser : %d", money_loser );
		clcd_set_DDRAM( 0x40 );
		clcd_write_string( cmoney_loser );
		dot_arrow();
		led_blink_all();
		fnd_deci_number( money_each );

		usleep(10000000);
	}

	usleep(0);
	sel.all = 0;
}