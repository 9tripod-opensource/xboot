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

static char ilda_buffer[SZ_1M];

static int do_galvo(int argc, char ** argv)
{
	struct galvo_ctx_t * ctx = galvo_alloc("fb-sandbox.0");
	struct ilda_file_t * ilda;
	int fd, len;
	int i;

	if(!ctx)
		return -1;

	fd = open("/romdisk/Evomorf.ild", O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	len = read(fd, (void *)(ilda_buffer), sizeof(ilda_buffer));
	close(fd);

	ilda = ilda_alloc(ilda_buffer, len);

	galvo_set_color(ctx, 0xffff, 0xff, 0);
	for(i = 0; i < ilda->count; i++)
	{
		printf("%f %f %f %d\r\n", ilda->points[i].x, ilda->points[i].y, ilda->points[i].z, ilda->points[i].blank);
		galvo_set_color(ctx, ilda->points[i].r, ilda->points[i].g, ilda->points[i].b);
		ilda->points[i].x *= 0.7;
		ilda->points[i].y *= 0.7;
		galvo_set_xy(ctx, ilda->points[i].x * 320 + 320, ilda->points[i].y * 240 + 240);
		galvo_refresh(ctx);
		mdelay(10);
	}

/*
	galvo_set_color(ctx, 0xffff, 0xff, 0);
	for(i = 0; i < 400; i++)
	{
		galvo_set_xy(ctx, i, i);
		galvo_refresh(ctx);
		mdelay(10);
	}
*/

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
