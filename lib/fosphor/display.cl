/*
 * display.cl
 *
 * Display OpenCL kernel. Formats the raw FFT output into GL objects we
 * use for display.
 *
 * Copyright (C) 2013 Sylvain Munaut
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
 *
 * NOTE (to make it clear): For the purpose of this license, any software
 * making use of this kernel (or derivative thereof) is considered to be
 * a derivative work (i.e. "a work based on the Program").
 */

/* Enable or not use of NV SM11 histogram algo (set automatically) */
/* #define USE_NV_SM11_ATOMICS */

/* Enable or not the use of cl_khr_local_int32_base_atomics to
 * implement atomic add (set automatically) */
/* #define USE_EXT_ATOMICS */

#ifdef USE_EXT_ATOMICS
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
#endif

#define CLAMP

//#define MAX_HOLD_LIVE
//#define MAX_HOLD_HISTO
//#define MAX_HOLD_NORMAL
#define MAX_HOLD_DECAY


#ifdef USE_NV_SM11_ATOMICS

#define LOG2_WARP_SIZE 5U
#define UINT_BITS 32U
#define WARP_SIZE (1U << LOG2_WARP_SIZE)
#define TAG_MASK ( (1U << (UINT_BITS - LOG2_WARP_SIZE)) - 1U )

/* See NVidia OpenCL Histogram256 example for how/why this works */
inline void nv_sm11_atomic_inc(volatile __local uint *p, uint tag)
{
	uint count;
	do {
		count = *p & TAG_MASK;
		count = tag | (count + 1);
		*p = count;
	} while (*p != count);
}

#endif /* USE_NV_SM11_ATOMICS */


__attribute__((reqd_work_group_size(16, 16, 1)))
__kernel void display(
	/* FFT Input */
	__global const float2 *fft,		/* [ 0] Input FFT (complex)      */
	const uint fft_log2_len,		/* [ 1] log2(FFT length)         */
	const uint fft_batch,			/* [ 2] # spectrums in the input */

	/* Waterfall */
	__write_only image2d_t wf_tex,		/* [ 3] Texture handle          */
	const uint wf_offset,			/* [ 4] Y Offset in the texture */

	/* Histogram */
	__read_only  image2d_t histo_tex_r,	/* [ 5] Texture read handle  */
	__write_only image2d_t histo_tex_w,	/* [ 6] Texture write handle */
	const float histo_t0r,			/* [ 7] Rise time constant   */
	const float histo_t0d,			/* [ 8] Decay time constant  */
	const float histo_scale,		/* [ 9] Val->Bin: scaling    */
	const float histo_ofs,			/* [10] Val->Bin: offset     */

	/* Live spectrum */
	__global float2 *spectrum_vbo,		/* [11] Vertex Buffer Object    */
	const float live_alpha)			/* [12] Averaging time constant */
{
	int gidx;
	float max_pwr = - 1000.0f;

	/* Local memory */
	__local float live_buf[16 * 16];	/* get_local_size(0) * get_local_size(1) */
	__local float max_buf[16 * 16];		/* get_local_size(0) * get_local_size(1) */
	__local uint  histo_buf[16 * 128];

	/* Local shortcuts */
	const float live_one_minus_alpha = 1.0f - live_alpha;

	/* Transposition & Atomic emulation */
#ifdef USE_NV_SM11_ATOMICS
	__local float pwr_buf[16 * 16];		/* pwr transpose buffer */

	uint tib = (get_local_id(0) + get_local_id(1)) & 15;
	uint ti0 = tib | (get_local_id(0) << 4);
	uint ti1 = tib | (get_local_id(1) << 4);

	const uint tag =  get_local_id(0) << (UINT_BITS - LOG2_WARP_SIZE);
#endif

	/* Clear buffers */
	live_buf[get_local_id(1) * get_local_size(0) + get_local_id(0)] = 0.0f;

	__local uint *h = &histo_buf[get_local_id(1) * get_local_size(0) + get_local_id(0)];

	h[   0] = 0;
	h[ 256] = 0;
	h[ 512] = 0;
	h[ 768] = 0;
	h[1024] = 0;
	h[1280] = 0;
	h[1536] = 0;
	h[1792] = 0;

	/* Wait for all clears to be done by everyone */
	barrier(CLK_LOCAL_MEM_FENCE);

	/* Main loop */
	for (gidx=0; gidx<fft_batch; gidx+=get_local_size(1))
	{
		/* Read fft & compute power */
		int fft_idx = ((gidx + get_local_id(1)) << fft_log2_len) + get_global_id(0);
		float2 fft_value = fft[fft_idx];

		float pwr = log10(hypot(fft_value.x,fft_value.y));

		/* Maximum pwr */
		max_pwr = max(max_pwr, pwr);

		/* Write to Waterfall texture */
		int2 coord;
		coord.x = get_global_id(0);
		coord.y = get_local_id(1) + wf_offset + gidx;

		write_imagef(wf_tex, coord, (float4)(pwr, 0.0f, 0.0f, 0.0f));

		/* Add to Live Spectrum buffer */
		live_buf[get_local_id(1) * get_local_size(0) + get_local_id(0)] +=
			pwr * native_powr(live_one_minus_alpha, (float)(fft_batch - gidx - get_local_id(1) - 1));

#ifdef USE_NV_SM11_ATOMICS
		/* Transposition */
		barrier(CLK_LOCAL_MEM_FENCE);	/* Sync */
		pwr_buf[ti0] = pwr;		/* Store power */
		barrier(CLK_LOCAL_MEM_FENCE);	/* Sync */
		pwr = pwr_buf[ti1];		/* Read power */
#endif

		/* Map to bin */
		int bin = (int)round(histo_scale * (pwr + histo_ofs));

		if (bin < 0 || bin > 127)
#ifdef CLAMP
			bin = (bin < 0) ? 0 : 127;
#else
			continue;
#endif

		/* Atomic Bin increment */
#if defined(USE_NV_SM11_ATOMICS)
		nv_sm11_atomic_inc(&histo_buf[(bin << 4) + get_local_id(1)], tag);
#elif defined(USE_EXT_ATOMICS)
		atom_inc(&histo_buf[(bin << 4) + get_local_id(0)]);
#else
		atomic_inc(&histo_buf[(bin << 4) + get_local_id(0)]);
#endif
	}

	max_buf[get_local_id(1) * get_local_size(0) + get_local_id(0)] = max_pwr;

	/* Wait for everyone before the final merges */
	barrier(CLK_LOCAL_MEM_FENCE);

	/* Live Spectrum merging */
	__global float2 *live_vbo = &spectrum_vbo[0];

	if (get_global_id(1) == 0)
	{
		int i,n;
		float sum;
		float2 vertex;

		/* Compute sum */
		sum = 0.0f;
		for (i=0; i<get_local_size(1); i++)
			sum += live_buf[i * get_local_size(0) + get_local_id(0)];

		/* Position in spectrum */
		n = get_global_size(0) >> 1;
		i = get_global_id(0) ^ n;

		/* Compute vertex position */
		vertex = live_vbo[i];

		vertex.x = ((float)i / (float)n) - 1.0f;
		vertex.y = vertex.y * native_powr(live_one_minus_alpha, (float)fft_batch) +
		           sum      * live_alpha;

		live_vbo[i] = vertex;
	}

	/* Histogram merging */
	for (gidx=0; gidx<128; gidx+=get_local_size(1))
	{
		const sampler_t direct_sample = CLK_NORMALIZED_COORDS_FALSE | CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;

		/* Histogram coordinates */
		int2 coord;
		coord.x = get_global_id(0);
		coord.y = gidx + get_local_id(1);

		/* Fetch previous histogram value */
		float4 hv = read_imagef(histo_tex_r, direct_sample, coord);

		/* Fetch hit count */
		uint hc = histo_buf[coord.y * get_local_size(0) + get_local_id(0)]
#ifdef USE_NV_SM11_ATOMICS
			& TAG_MASK
#endif
		;

		/* Fast exit if possible ... */
		if ((hv.x <= 0.01f) && (hc == 0))
			continue;

		/* Apply the rise / decay */
		float a = (float)hc / (float)fft_batch;
		float b = a * native_recip(histo_t0r);
		float c = b + native_recip(histo_t0d);
		float d = b * native_recip(c);
		float e = native_powr(1.0f - c, (float)fft_batch);

		hv.x = (hv.x - d) * e + d;

		/* Clamp value (we don't clear the texture so we get crap) */
		hv.x = clamp(hv.x, 0.0f, 1.0f);

		/* Write new histogram value */
		write_imagef(histo_tex_w, coord, hv);
	}

	/* Max hold */
	__global float2 *max_vbo = &spectrum_vbo[1 << fft_log2_len];

	if (get_global_id(1) == 0)
	{
		int i, j, n;
		float2 vertex;

#ifdef MAX_HOLD_HISTO
		vertex.y = - histo_ofs;

		for (gidx=0; gidx<128; gidx++)
		{
			const sampler_t direct_sample = CLK_NORMALIZED_COORDS_FALSE | CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;

			/* Histogram coordinates */
			int2 coord;
			coord.x = get_global_id(0);
			coord.y = gidx;

			/* Fetch histogram value */
			float4 hv = read_imagef(histo_tex_r, direct_sample, coord);

			/* Set vertex position */
			if (hv.x > 0.1f)
				vertex.y = ((float)gidx / histo_scale)- histo_ofs;
		}
#endif

		/* Position in spectrum */
		n = get_global_size(0) >> 1;
		i = get_global_id(0) ^ n;

		vertex.x = ((float)i / (float)n) - 1.0f;
#ifdef MAX_HOLD_LIVE
		vertex.y = max(live_vbo[i].y, max_vbo[i].y);
#endif
#ifdef MAX_HOLD_NORMAL
		max_pwr = max_vbo[i].y;
		for (i=0; i<get_local_size(1); i++)
			max_pwr = max(max_pwr, max_buf[i * get_local_size(0) + get_local_id(0)]);
		vertex.y = max_pwr;
#endif
#ifdef MAX_HOLD_DECAY
		max_pwr = max_vbo[i].y * 0.999f + 0.001f * live_vbo[i].y;
		for (j=0; j<get_local_size(1); j++)
			max_pwr = max(max_pwr, max_buf[j * get_local_size(0) + get_local_id(0)]);
		vertex.y = max_pwr;
#endif

		max_vbo[i] = vertex;

	}
}

/* vim: set syntax=c: */
