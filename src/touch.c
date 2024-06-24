#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <utime.h>
#include <stdlib.h>
#include <fcntl.h>



int date_parser( char * str, struct timespec * outtime );
int get_referance( char * filename, struct timespec * outtime );
int time_parser( char * str, struct timespec * outtime );

int a_flg = 0;
int c_flg = 0;
int d_flg = 0;
int m_flg = 0;
int t_flg = 0;

int main(int argc, char ** argv)
{
	int option;
	int error_exist = 0;
	struct timespec touch_time;
	struct timespec times[2];
	touch_time.tv_nsec = UTIME_NOW;

	while (( option = getopt(argc, argv, "acd:m:t:") ) != -1) {
		switch(option){
		case('a'):
			a_flg = 1;
			break;
		case('c'):
			c_flg = 1;
			break;
		case('m'):
			m_flg = 1;
			break;
		case('d'):
			error_exist = date_parser(optarg, &touch_time);
			if( error_exist ) {
				fprintf(stderr, "touch: Invalid date format '%s'", optarg);
				return 1;
			}
			break;
		case('r'):
			error_exist = get_referance(optarg, &touch_time);
			if( error_exist )
				return 1;
			break;
		case('t'):
			error_exist = time_parser(optarg, &touch_time);
			if( error_exist ){
				fprintf(stderr, "touch: Invalid time format '%s'", optarg);
				return 1;
			}
			break;
		}
	}

	if(a_flg == 0 && m_flg == 0)
		a_flg = m_flg = 1;

	for(int i = optind; i < argc; i++) {

		char * path = argv[i];

		struct stat sb;
		error_exist = stat(path, &sb);

		if( error_exist && errno != 2 ) {
			perror(path);
			error_exist = 0;
			continue;
		}

		if( errno == 2 ) {
			if(c_flg)
				continue;
			else
				error_exist = creat(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
						S_IROTH | S_IWOTH);


			if(error_exist == -1 ) {
				perror(path);
				error_exist = 0;
				continue;
			}
		}

		times[0] = touch_time;
		times[1] = touch_time;

		if(!a_flg)
			times[0].tv_nsec = UTIME_OMIT;

		if(!m_flg)
			times[1].tv_nsec = UTIME_OMIT;

		error_exist = utimensat(AT_FDCWD, path, times, 0);

		if( error_exist ){
			perror(path);
			error_exist = 0;
		}
	}
}
