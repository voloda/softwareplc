/*
 * RTLinux C++ support
 * Written by Michael Barabanov, baraban@fsmlabs.com
 * (C) 2000 FSMLabs, GPL
 *
 */
#include <rtl_printf.h>
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>

int stderr = 0;

int fputs(const char *s, void *stream)
{
	rtl_printf(s);
	return 0;
}

void __pure_virtual (void)
{
	rtl_printf("__pure_virtual called\n");
}

void __cxa_pure_virtual (void)
{
	rtl_printf("__cxa_pure_virtual called\n");
}

int __cxa_atexit(void (*f)(void *), void *p, void* d)
{
	return 1;
}

void __this_fixmap_does_not_exist(void)
{
	rtl_printf("__this_fixmap_does_not_exist called\n");
}

void __assert_fail (char *assertion, char *file, unsigned int line,
		char *function)
{
	(void) rtl_printf ("rtl: assertion %s failed.\n", assertion);
}

int __isnan(double x)
{
	return 0;
}

