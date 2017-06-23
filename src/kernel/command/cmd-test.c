/*
 * kernel/command/cmd-test.c
 */

#include <command/command.h>
#include <ilda.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

static int do_test(int argc, char ** argv)
{
	struct scan_mirror_t * mir = scan_mirror_alloc("fb-sandbox.0");
	char * filename = "/romdisk/dolf.ild";
	int count = 1;
	int i;

	if(argc > 1)
		filename = argv[1];
	if(argc > 2)
		count = strtoul(argv[2], NULL, 0);

	for(i = 0; i < count; i++)
	{
		ilda_load_file(filename, mir);
	}
	scan_mirror_free(mir);

	return 0;
}

static struct command_t cmd_test = {
	.name	= "test",
	.desc	= "debug command for programmer",
	.usage	= usage,
	.exec	= do_test,
};

static __init void test_cmd_init(void)
{
	register_command(&cmd_test);
}

static __exit void test_cmd_exit(void)
{
	unregister_command(&cmd_test);
}

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);
