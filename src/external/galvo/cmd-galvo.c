/*
 * cmd-galvo.c
 */

#include <command/command.h>
#include <galvo.h>
#include <ilda.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    galvo [args ...]\r\n");
}

static int do_galvo(int argc, char ** argv)
{
	struct galvo_ctx_t * ctx = galvo_alloc("fb-sandbox.0");

	if(!ctx)
		return -1;

	ilda_load_file(ctx, "/romdisk/haloween.ild");
	galvo_free(ctx);

	return 0;
}

static struct command_t cmd_galvo = {
	.name	= "galvo",
	.desc	= "galvo scanning test program",
	.usage	= usage,
	.exec	= do_galvo,
};

static __init void galvo_cmd_init(void)
{
	register_command(&cmd_galvo);
}

static __exit void galvo_cmd_exit(void)
{
	unregister_command(&cmd_galvo);
}

command_initcall(galvo_cmd_init);
command_exitcall(galvo_cmd_exit);
