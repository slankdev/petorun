
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dpdk/wrap.h>
#include <thread>

void print_banner()
{
	printf("\n");
	printf("Hello, this is petorun (version 0.0.0).\n");
	printf("Copyright 2018 Hiroki SHIROKURA.\n");
	printf("\n");
	printf("                        .o8\n");
	printf(" oo.ooooo.   .ooooo.  .o888oo  .ooooo.  oooo d8b oooo  oooo  ooo. .oo.\n");
	printf("  888' `88b d88' `88b   888   d88' `88b `888""8P `888  `888  `888P\"Y88b\n");
	printf("  888   888 888ooo888   888   888   888  888      888   888   888   888\n");
	printf("  888   888 888    .o   888 . 888   888  888      888   888   888   888\n");
	printf("  888bod8P' `Y8bod8P'   \"888\" `Y8bod8P' d888b     `V88V\"V8P' o888o o888o\n");
	printf("  888\n");
	printf(" o888o\n");
	printf("\n");
}

int main(int argc, char** argv)
{
	print_banner();
}


