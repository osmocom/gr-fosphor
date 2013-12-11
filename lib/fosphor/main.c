#if !defined(_WIN32) && (defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__))
# define _WIN32
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include "fosphor.h"
#include "private.h"

struct app_state
{
	struct fosphor *fosphor;

	FILE *src_fh;
	void *src_buf;

	int w, h;
	int db_ref, db_per_div_idx;
};

static struct app_state _g_as, *g_as = &_g_as;

static const int k_db_per_div[] = { 1, 2, 5, 10, 20 };


/* ------------------------------------------------------------------------ */
/* Timing utils                                                             */
/* ------------------------------------------------------------------------ */

#ifdef _WIN32

#include <time.h>
#include <Windows.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
# define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
# define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};

static int
gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}

#else /* _WIN32 */
# include <sys/time.h>
#endif /* _WIN32 */


uint64_t
time_now(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000UL) + tv.tv_usec;
}

static uint64_t tic;

void
time_tic(void)
{
	tic = time_now();
}

uint64_t
time_toc(const char *str)
{
	uint64_t d = time_now() - tic;
	printf("%s: %d us\n", str, (int)d);
	return d;
}


/* ------------------------------------------------------------------------ */
/* GLFW                                                                     */
/* ------------------------------------------------------------------------ */

#define BATCH_LEN	128
#define BATCH_COUNT	4

static void
glfw_render(GLFWwindow *wnd)
{
	static int fc = 0;
	int c, r, o;

	/* Timing */
	if (!fc)
		time_tic();
	if (fc == 99) {
		uint64_t t;
		float bw;

		t = time_toc("100 Frames time");

		bw = (1e6f * FOSPHOR_FFT_LEN * BATCH_LEN * BATCH_COUNT) / ((float)t / 100.0f);
		fprintf(stderr, "BW estimated: %f Msps\n", bw / 1e6);
	}

	fc = (fc+1) % 100;

	/* Clear everything */
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT);

	/* Process some samples */
	for (c=0; c<BATCH_COUNT; c++) {
		r = sizeof(float) * 2 * FOSPHOR_FFT_LEN * BATCH_LEN;
		o = 0;

		while (r) {
			int rc;

			rc = fread((char*)g_as->src_buf + o, 1, r, g_as->src_fh);
			if (rc <= 0) {
				if (fseek(g_as->src_fh, 0, SEEK_SET))
					abort();
				continue;
			}

			r -= rc;
			o += rc;
		}

		fosphor_process(g_as->fosphor, g_as->src_buf, FOSPHOR_FFT_LEN * BATCH_LEN);
	}

	/* Draw fosphor */
	fosphor_draw(g_as->fosphor, g_as->w, g_as->h);

	/* Done, swap buffer */
	glfwSwapBuffers(wnd);
}

static void
glfw_cb_reshape(GLFWwindow *wnd, int w, int h)
{
	if (w < 0 || h < 0)
		glfwGetFramebufferSize(wnd, &w, &h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (double)w, 0.0, (double)h, -1.0, 1.0);

	glViewport(0, 0, w, h);

	g_as->w = w;
	g_as->h = h;
}

static void
glfw_cb_key(GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		exit(0);
		break;

	case GLFW_KEY_UP:
		g_as->db_ref -= k_db_per_div[g_as->db_per_div_idx];
		break;

	case GLFW_KEY_DOWN:
		g_as->db_ref += k_db_per_div[g_as->db_per_div_idx];
		break;

	case GLFW_KEY_LEFT:
		if (g_as->db_per_div_idx > 0)
			g_as->db_per_div_idx--;
		break;

	case GLFW_KEY_RIGHT:
		if (g_as->db_per_div_idx < 4)
			g_as->db_per_div_idx++;
		break;
	}

	fosphor_set_power_range(g_as->fosphor, g_as->db_ref, k_db_per_div[g_as->db_per_div_idx]);
}

static GLFWwindow *
glfw_init(void)
{
	GLFWwindow *wnd;

	/* Main glfw init */
	glfwInit();

	/* Window init */
	wnd = glfwCreateWindow(1024, 1024, "Fosphor test", NULL, NULL);
	if (!wnd)
		return NULL;

	glfwMakeContextCurrent(wnd);

#ifdef _WIN32
	/* Init GLEW (on win32) */
	glewInit();
#endif

	/* Disable VSync to test speed */
	glfwSwapInterval(0);

	/* Callbacks */
	glfwSetFramebufferSizeCallback(wnd, glfw_cb_reshape);
	glfwSetKeyCallback(wnd, glfw_cb_key);

	/* Force inital window size config */
	glfw_cb_reshape(wnd, -1, -1);

	return wnd;
}

static void
glfw_cleanup(GLFWwindow *wnd)
{
	glfwDestroyWindow(wnd);
	glfwTerminate();
}


/* ------------------------------------------------------------------------ */
/* Main                                                                     */
/* ------------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
	GLFWwindow *wnd = NULL;
	int rv;

	/* Open source file */
	if (argc == 2) {
		g_as->src_fh = fopen(argv[1], "rb");
		if (!g_as->src_fh) {
			fprintf(stderr, "[!] Failed to open input file\n");
			return -EIO;
		}
	} else if (argc == 1) {
		g_as->src_fh = stdin;
	} else {
		fprintf(stderr, "Usage: %s filename.cfile\n", argv[0]);
		return -EINVAL;;
	}

	g_as->src_buf = malloc(2 * sizeof(float) * FOSPHOR_FFT_LEN * FOSPHOR_FFT_MAX_BATCH);
	if (!g_as->src_buf) {
		rv = -ENOMEM;
		goto error;
	}

	/* Init our state */
	g_as->db_per_div_idx = 3;
	g_as->db_ref = 0;

	/* Init GLFW */
	wnd = glfw_init();
	if (!wnd) {
		fprintf(stderr, "[!] Failed to initialize GLFW window\n");
		rv = -EIO;
		goto error;
	}

	/* Init fosphor */
	g_as->fosphor = fosphor_init();
	if (!g_as->fosphor) {
		fprintf(stderr, "[!] Failed to initialize fosphor\n");
		rv = -EIO;
		goto error;
	}

	fosphor_set_power_range(g_as->fosphor, g_as->db_ref, k_db_per_div[g_as->db_per_div_idx]);

	/* Run ! */
	while (!glfwWindowShouldClose(wnd))
	{
		glfw_render(wnd);
		glfwPollEvents();
	}

	rv = 0;

	/* Cleanup */
error:
	if (g_as->fosphor)
		fosphor_release(g_as->fosphor);

	if (wnd)
		glfw_cleanup(wnd);

	free(g_as->src_buf);
	fclose(g_as->src_fh);

	return rv;
}
