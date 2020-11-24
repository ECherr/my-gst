#include "../include/utils.h"

/* main directoyr
 *
 *
 *
 */
void main(int argc, char **argv)
{
	if (argc < 2){
		usage();
	}

	int flag = atoi(argv[1]);

	/* process opts */
	getopts(argc, argv);

	/* run main-loop */
	init(flag, argc, argv);
}

