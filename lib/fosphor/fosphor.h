/*
 * fosphor.h
 *
 * Main fosphor entry point
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/*! \defgroup fosphor
 *  @{
 */

/*! \file fosphor.h
 *  \brief Main fosphor entry point
 */

struct fosphor;
struct fosphor_render;


/* Main API */

struct fosphor *fosphor_init(void);
void fosphor_release(struct fosphor *self);

int  fosphor_process(struct fosphor *self, void *samples, int len);
void fosphor_draw(struct fosphor *self, struct fosphor_render *render);

void fosphor_set_fft_window_default(struct fosphor *self);
void fosphor_set_fft_window(struct fosphor *self, float *win);

void fosphor_set_power_range(struct fosphor *self, int db_ref, int db_per_div);
void fosphor_set_frequency_range(struct fosphor *self,
                                 double center, double span);


/* Render */

#define FOSPHOR_MAX_CHANNELS	8

struct fosphor_channel
{
	int   enabled;		/*!< \brief Showed (1) or hidden (0) */
	float center;		/*!< \brief Normalized center frequency */
	float width;		/*!< \brief Normalized bandwidth */
};

#define FRO_LIVE	(1<<0)	/*!< \brief Display live spectrum */
#define FRO_MAX_HOLD	(1<<1)	/*!< \brief Display max-hold spectrum */
#define FRO_HISTO	(1<<2)	/*!< \brief Display histogram */
#define FRO_WATERFALL	(1<<3)	/*!< \brief Display waterfall */
#define FRO_LABEL_FREQ	(1<<4)	/*!< \brief Display frequency labels */
#define FRO_LABEL_PWR	(1<<5)	/*!< \brief Display power labels */
#define FRO_LABEL_TIME	(1<<6)	/*!< \brief Display time labels */
#define FRO_CHANNELS	(1<<7)	/*!< \brief Display channels */
#define FRO_COLOR_SCALE	(1<<8)	/*!< \brief Display intensity color scale */

/*! \brief fosphor render options */
struct fosphor_render
{
	/* User fields */
	int   pos_x;		/*!< \brief X origin (lower left corner) */
	int   pos_y;		/*!< \brief Y origin (lower left corner) */
	int   width;		/*!< \brief Width  */
	int   height;		/*!< \brief Height */
	int   options;		/*!< \brief Options (See FRO_??? constants) */
	float histo_wf_ratio;	/*!< \brief Ratio histogram/waterfall ]0,1[ */
	int   freq_n_div;	/*!< \brief Number of frequency divisions */
	float freq_center;	/*!< \brief Frequency zoom center ]0,1[ */
	float freq_span;	/*!< \brief Frequency zoom span   ]0,1] */
	float wf_span;		/*!< \brief Waterfall time zoom   ]0,1] */

		/*! \brief Displayed channels */
	struct fosphor_channel channels[FOSPHOR_MAX_CHANNELS];

	/* Private fields */
	int   _wf_pos;		/*!< \brief (private) Waterfall position */

	float _x_div;		/*!< \brief (private) X divisions width */
	float _x[2];		/*!< \brief (private) X endpoints */
	float _x_label;		/*!< \brief (private) X location for labels */

	float _y_histo_div;	/*!< \brief (private) Y histogram divisions height */
	float _y_histo[2];	/*!< \brief (private) Y histogram endpoints */
	float _y_wf[2];		/*!< \brief (private) Y waterfall endpoints */
	float _y_label;		/*!< \brief (private) Y location for label */
};

void fosphor_render_defaults(struct fosphor_render *render);
void fosphor_render_refresh(struct fosphor_render *render);


/* Position Mapping */

double fosphor_pos2freq(struct fosphor *self, struct fosphor_render *render, int x);
float  fosphor_pos2pwr (struct fosphor *self, struct fosphor_render *render, int y);
int    fosphor_pos2samp(struct fosphor *self, struct fosphor_render *render, int y);
int    fosphor_freq2pos(struct fosphor *self, struct fosphor_render *render, double freq);
int    fosphor_pwr2pos (struct fosphor *self, struct fosphor_render *render, float pwr);
int    fosphor_samp2pos(struct fosphor *self, struct fosphor_render *render, int time);

int    fosphor_render_pos_inside(struct fosphor_render *render, int x, int y);


/*! @} */
