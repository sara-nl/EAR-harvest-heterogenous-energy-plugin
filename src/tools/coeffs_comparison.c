unsigned int files_num;

int is_regular_file(const char *path)
{
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

void read_file(char *path, char *node)
{
	coefficient_t coeff;
	size_t r;
	int fd;

	//
	fd = open(path, O_RDONLY);

	if (fd < 0) {
		printf("ERROR while opening file %s\n", strerror(errno));
		return;
	}

	//
	db_read_applications_query();

	//
	#define SZ_COEFFS sizeof(coefficient_t)

	while((r = read(fd, &coeffs, SZ_COEFF)) == SZ_COEFF)
	{
		print_coefficients(&coeff);
	}

	close(fd);
}

int main(int argc, char *argv[])
{
	char *path_root;
	char *path_file;
	int i;

	//
	path = argv[1];

	//
	for (i = 2; i < argc; i++)
	{
		path_file = argv[i];

		/* The program reports coefficients in stdout and csv file */
		sprintf(path_file, "%s/coeffs.%s", path_root, path_file);

		if (is_regular_file(path_file))
		{
			read_file(path_file);

			//
			files_num += 1;
		}
	}

	return 0;
}