/*
 * fosphor.h
 *
 * Main fosphor entry point
 *
 * Copyright (C) 2013-2014 Sylvain Munaut
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FOSPHOR_FOSPHOR_H__
#define __FOSPHOR_FOSPHOR_H__

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
	float freq_start;	/*!< \brief Frequency zoom start [0,1[ */
	float freq_stop;	/*!< \brief Frequency zoom stop  ]0,1] */
	float wf_span;		/*!< \brief Waterfall time zoom  ]0,1] */

		/* \brief Displayed channels */
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


/*! @} */

#endif /* __FOSPHOR_FOSPHOR_H__ */
