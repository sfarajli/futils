#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <ctype.h>

int date_parser(char *, struct timespec *);
int get_referance(char *, struct timespec *);
int time_parser(char *, struct timespec *);

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

	while ((option = getopt(argc, argv, "acd:m:t:")) != -1) {
		switch (option) {
		case 'a':
			a_flg = 1;
			break;
		case 'c':
			c_flg = 1;
			break;
		case 'm':
			m_flg = 1;
			break;
		case 'd':
			error_exist = date_parser(optarg, &touch_time);
			if (error_exist) {
				fprintf(stderr, "touch: Invalid date format '%s'", optarg);
				return 1;
			}
			break;
		case 'r':
			error_exist = get_referance(optarg, &touch_time);
			if (error_exist)
				return 1;
			break;
		case 't':
			error_exist = time_parser(optarg, &touch_time);
			if (error_exist) {
				fprintf(stderr, "touch: Invalid time format '%s'", optarg);
				return 1;
			}
			break;
		}
	}

	if (a_flg == 0 && m_flg == 0)
		a_flg = m_flg = 1;

	for (int i = optind; i < argc; i++) {

		char * path = argv[i];

		struct stat sb;
		error_exist = stat(path, &sb);

		if (error_exist && errno != 2) {
			perror(path);
			error_exist = 0;
			continue;
		}

		if (errno == 2) {
			if (c_flg)
				continue;
			else
				error_exist = creat(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
						S_IROTH | S_IWOTH);

			if (error_exist == -1) {
				perror(path);
				error_exist = 0;
				continue;
			}
		}

		times[0] = touch_time;
		times[1] = touch_time;

		if (!a_flg)
			times[0].tv_nsec = UTIME_OMIT;

		if (!m_flg)
			times[1].tv_nsec = UTIME_OMIT;

		error_exist = utimensat(AT_FDCWD, path, times, 0);

		if (error_exist) {
			perror(path);
			error_exist = 0;
		}
	}
}

int
date_parser(char *input, struct timespec *ts)
{
	struct tm t = {0};

	size_t length = strlen(input);
	char *tmp = malloc(sizeof(char) * (length + 1));
	strcpy(tmp, input);
	tmp[length] = '\0';

	char *str = tmp;

	size_t T_index = length;
	for (size_t i = 0; i < length; i++) {
		if (str[i] == 'T' || str[i] == ' ') {
			T_index = i;
			break;
		}
	}

	if (T_index == length)
		return 1;

	str[T_index] = '\0';

	if (strlen(str) != 10 || str[4] != '-' || str[7] != '-')
		return 1;

	str[4] = '\0';
	t.tm_year = strtol(str, NULL, 10);
	str += 4 + 1;
	str[2] = '\0';
	t.tm_month = strtol(str, NULL, 10);
	str += 2 + 1;
	t.tm_day = strtol(str, NULL, 10);
	str += 2 + 1;

	bool comma_exists = false;
	if (strlen(str) < 8)
		return 1;
	else if (strlen(str) > 8 && (str[8] != ',' && str[8] != '.'))
		return 1;
	else if (strlen(str) > 8)
		comma_exists = true;

	if (str[2] != ':' || str[5] != ':')
		return 1;

	str[8] = '\0';

	str[2] = '\0';
	t.tm_hour = strtol(str, NULL, 10);
	str += 2 + 1;
	str[2] = '\0';
	t.tm_min = strtol(str, NULL, 10);
	str += 2 + 1;
	t.tm_sec = strtol(str, NULL, 10);
	str += 2 + 1;

	if (!comma_exists)
		goto done;

	bool isUTC = false;
	if (str[strlen(str) - 1] == 'Z') {
		isUTC = true;
		str[strlen(str) - 1] = '\0';
	}

    	time_t timestamp = mktime(&t);

	for (size_t i = 0; i < strlen(str); i++) {
		if (!isdigit(str[i]))
			return 1;
	}

	str -= 2;
	str[0] = '0';
	str[1] = '.';

	time_t nsec = strtol(str, NULL, 10) * 1e9;
	time_t sec = mktime(&t);
	if (sec == -1) {
		printf("Failed to convert time.\n");
		return 1;
	}

	*ts.tv_nsec = nsec;
done:
	*ts.tv_sec = sec;
	free(tmp);

	return 0;
}


int
parse_time(char *input)
{
	struct tm t = {0};
	int first_digits_year = -1;
	int second_digits_year = -1;

	size_t length = strlen(input);
	char *buf = malloc(sizeof(char) * length + 1);
	strcpy(buf, input);
	char *str = buf;

	if (strlen(str) - 3 > 0 && str[strlen(str) - 3] == '.') {
		int comma_index = strlen(str) - 3;
		t.tm_sec = strtol(str + comma_index, NULL, 10);
		str[comma_index] = '\0';
	}

	char tmp;
	switch(strlen(str)) {
	case 12:
		tmp = str[2];
		str [2]= '\0';
		first_digits_year = strtol(str, NULL, 10);
		str[2] = tmp;
		str += 2;
		/* FALLTHROUGH */
	case 10:
		tmp = str[2];
		str[2] = '\0';
		second_digits_year = strtol(str, NULL, 10);
		str[2] = tmp;
		str += 2;
		/* FALLTHROUGH */
	case 8:
		tmp = str[2];
		str[2] = '\0';
		t.tm_mon = strtol(str, NULL, 10);
		str[2] = tmp;
		str += 2;

		tmp = str[2];
		str[2] = '\0';
		t.tm_mday = strtol(str, NULL, 10);
		str[2] = tmp;
		str += 2;

		tmp = str[2];
		str[2] = '\0';
		t.tm_hour = strtol(str, NULL, 10);
		str[2] = tmp;
		str += 2;

		tmp = str[2];
		str[2] = '\0';
		t.tm_min = strtol(str, NULL, 10);
		str[2] = tmp;
		str += 2;
		break;

	default:
		return 1;
	}
	time_t sec = mktime(t);

	if (sec == -1) {
		printf("Failed to convert time.\n");
		return 1;
	}

	*t.tm_sec = sec;
	free(buf);
	return 0;
}
