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
	else { input_mode(); }
	return TRUE;
}

void select_mode() {
	int i; char p[100], m[100];
	char clcd_str[20] = "";

	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();

	printf("\n");
	printf("************************************\n");
	printf("*    SHARE THE EXPENSES EQUALLY   *\n");
	printf("*       press any key to start       *\n");
	printf("*         press 'e' to exit        *\n");
	printf("************************************\n\n");
	sel.all = 0xFF;
}

void input_mode() {
	int key_count, key_value;
	char clcd_str[20];
	key_count = keyboard_read( &key_value );
	fnd_write( key_value , 7 );
	sprintf( clcd_str, "money : %d", key_value );
	clcd_set_DDRAM( 0x00 );

	usleep(0);
}
	
	
	
}

void mini_game() {
	int boom;
	int p[people];
	int diff;
	int loser;
	int min = 16;
	
	srand( time(NULL) );	// seed
	boom =  rand()%16;
	
	for( i=0; i<people; i++ ) {
		scanf("%d", p[i]);
		clcd_write();
		diff = abs( boom - p[i] );
		
		if ( min > diff ) {
			min = diff;
			loser = i;
		}
		else { min = min; }
	}
	clcd_write();
}
	