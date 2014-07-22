/*
 * fft.cl
 *
 * FFT OpenCL kernel
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
 *
 * NOTE (to make it clear): For the purpose of this license, any software
 * making use of this kernel (or derivative thereof) is considered to be
 * a derivative work (i.e. "a work based on the Program").
 */

#define M_PIf (3.141592653589f)

/* ------------------------------------------------------------------------ */
/* Complex numbers                                                          */
/* ------------------------------------------------------------------------ */

/*
 * float2  (re,im)
 * float4  (re0,im0,re1,im1)
 */

#define CMUL_RE(a,b) (a.even*b.even - a.odd*b.odd)
#define CMUL_IM(a,b) (a.even*b.odd  + a.odd*b.even)

float2 cmul_1(float2 a, float2 b)
{
	float2 x;
	x.even = CMUL_RE(a,b);
	x.odd  = CMUL_IM(a,b);
	return x;
}

float4 cmul_2(float4 a, float4 b)
{
	float4 x;
	x.even = CMUL_RE(a,b);
	x.odd  = CMUL_IM(a,b);
	return x;
}

/* ------------------------------------------------------------------------ */
/* DFT bases                                                                */
/* ------------------------------------------------------------------------ */

/* Return a * e^(k * alpha * 1j) */
float2
twiddle(float2 a, int k, float alpha)
{
	float cv, sv;
//	sv = sincos(k * alpha, &cv);
	sv = native_sin(k*alpha);
	cv = native_cos(k*alpha);
	return cmul_1(a, (float2)(cv,sv));
}

/* Optimized constant version */
__constant float SQRT_1_2 = 0.707106781188f; /* cos(PI/4) */

#define mul_p0q1(a) (a)

#define mul_p0q2 mul_p0q1
float2  mul_p1q2(float2 a) { return (float2)(a.y,-a.x); }

#define mul_p0q4 mul_p0q2
float2  mul_p1q4(float2 a) { return (float2)(SQRT_1_2)*(float2)(a.x+a.y,-a.x+a.y); }
#define mul_p2q4 mul_p1q2
float2  mul_p3q4(float2 a) { return (float2)(SQRT_1_2)*(float2)(-a.x+a.y,-a.x-a.y); }


/* in-place DFT */
__attribute__((always_inline)) void
dft2(float2 *r0, float2 *r1)
{
	float2 tmp;

	tmp = *r0 - *r1;
	*r0 = *r0 + *r1;
	*r1 = tmp;
}

__attribute__((always_inline)) void
dft4(float2 *r0, float2 *r1, float2 *r2, float2 *r3)
{
	/* 2x DFT2 */
	dft2(r0,r2);
	dft2(r1,r3);

	/* Twiddle */
	*r2 = mul_p0q2(*r2);	/* nop */
	*r3 = mul_p1q2(*r3);

	/* 2x DFT2 */
	dft2(r0,r1);
	dft2(r2,r3);
}

__attribute__((always_inline)) void
dft8(float2 *r0, float2 *r1, float2 *r2, float2 *r3,
     float2 *r4, float2 *r5, float2 *r6, float2 *r7)
{
	/* 4x DFT2 */
	dft2(r0, r4);
	dft2(r1, r5);
	dft2(r2, r6);
	dft2(r3, r7);

	/* Twiddle */
	*r4 = mul_p0q4(*r4);	/* nop */
	*r5 = mul_p1q4(*r5);
	*r6 = mul_p2q4(*r6);
	*r7 = mul_p3q4(*r7);

	/* 4x DFT2 */
	dft2(r0, r2);
	dft2(r1, r3);
	dft2(r4, r6);
	dft2(r5, r7);

	/* Twiddle */
	*r2 = mul_p0q2(*r2);	/* nop */
	*r3 = mul_p1q2(*r3);
	*r6 = mul_p0q2(*r6);	/* nop */
	*r7 = mul_p1q2(*r7);

	/* 4x DFT2 */
	dft2(r0, r1);
	dft2(r2, r3);
	dft2(r4, r5);
	dft2(r6, r7);
}


/* ------------------------------------------------------------------------ */
/* FFT radices                                                              */
/* ------------------------------------------------------------------------ */

/* FFT radix 2 */

__attribute__((always_inline)) void
fft_radix2_exec(float2 *r)
{
	/* In-place DFT2 */
	dft2(&r[0], &r[1]);
}

__attribute__((always_inline)) void
fft_radix2_twiddle(float2 *r, int k, int p)
{
	float alpha = -M_PIf * (float)k / (float)(p);

	r[1] = twiddle(r[1],1,alpha);
}

__attribute__((always_inline)) void
fft_radix2_load(__local float2 *buf, float2 *r, int i, int t)
{
	buf += i;

	r[0] = buf[  0];
	r[1] = buf[  t];
}

__attribute__((always_inline)) void
fft_radix2_store(__local float2 *buf, float2 *r, int i, int k, int p)
{
	int j = ((i-k)<<1) + k;

	buf += j;

	buf[  0] = r[0];
	buf[  p] = r[1];
}

__attribute__((always_inline)) void
fft_radix2(
	__local float2 *buf, float2 *r,
	int p, int i, int t, int twiddle)
{
	int k = i & (p-1);

	fft_radix2_load(buf, r, i, t);

	if (twiddle)
		fft_radix2_twiddle(r, k, p);

	fft_radix2_exec(r);

	barrier(CLK_LOCAL_MEM_FENCE);

	fft_radix2_store(buf, r, i, k, p);

	barrier(CLK_LOCAL_MEM_FENCE);
}


/* FFT radix 4 */

__attribute__((always_inline)) void
fft_radix4_exec(float2 *r)
{
	/* In-place DFT4 */
	dft4(&r[0], &r[1], &r[2], &r[3]);
}

__attribute__((always_inline)) void
fft_radix4_twiddle(float2 *r, int k, int p)
{
	float alpha = -M_PIf * (float)k / (float)(2*p);

	r[1] = twiddle(r[1],1,alpha);
	r[2] = twiddle(r[2],2,alpha);
	r[3] = twiddle(r[3],3,alpha);
}

__attribute__((always_inline)) void
fft_radix4_load(__local float2 *buf, float2 *r, int i, int t)
{
	buf += i;

	r[0] = buf[  0];
	r[1] = buf[  t];
	r[2] = buf[2*t];
	r[3] = buf[3*t];
}

__attribute__((always_inline)) void
fft_radix4_store(__local float2 *buf, float2 *r, int i, int k, int p)
{
	int j = ((i-k)<<2) + k;

	buf += j;

	buf[  0] = r[0];
	buf[  p] = r[2];
	buf[2*p] = r[1];
	buf[3*p] = r[3];
}

__attribute__((always_inline)) void
fft_radix4(
	__local float2 *buf, float2 *r,
	int p, int i, int t, int twiddle)
{
	int k = i & (p-1);

	fft_radix4_load(buf, r, i, t);

	if (twiddle)
		fft_radix4_twiddle(r, k, p);

	fft_radix4_exec(r);

	barrier(CLK_LOCAL_MEM_FENCE);

	fft_radix4_store(buf, r, i, k, p);

	barrier(CLK_LOCAL_MEM_FENCE);
}


/* FFT radix 8 */

__attribute__((always_inline)) void
fft_radix8_exec(float2 *r)
{
	/* In-place DFT8 */
	dft8(&r[0], &r[1], &r[2], &r[3], &r[4], &r[5], &r[6], &r[7]);
}

__attribute__((always_inline)) void
fft_radix8_twiddle(float2 *r, int k, int p)
{
	float alpha = -M_PIf * (float)k / (float)(4*p);

	r[1] = twiddle(r[1], 1, alpha);
	r[2] = twiddle(r[2], 2, alpha);
	r[3] = twiddle(r[3], 3, alpha);
	r[4] = twiddle(r[4], 4, alpha);
	r[5] = twiddle(r[5], 5, alpha);
	r[6] = twiddle(r[6], 6, alpha);
	r[7] = twiddle(r[7], 7, alpha);
}

__attribute__((always_inline)) void
fft_radix8_load(__local float2 *buf, float2 *r, int i, int t)
{
	buf += i;

	r[0] = buf[  0];
	r[1] = buf[  t];
	r[2] = buf[2*t];
	r[3] = buf[3*t];
	r[4] = buf[4*t];
	r[5] = buf[5*t];
	r[6] = buf[6*t];
	r[7] = buf[7*t];
}

__attribute__((always_inline)) void
fft_radix8_store(__local float2 *buf, float2 *r, int i, int k, int p)
{
	int j = ((i-k)<<3) + k;

	buf += j;

	buf[  0] = r[0];
	buf[  p] = r[4];
	buf[2*p] = r[2];
	buf[3*p] = r[6];
	buf[4*p] = r[1];
	buf[5*p] = r[5];
	buf[6*p] = r[3];
	buf[7*p] = r[7];
}

__attribute__((always_inline)) void
fft_radix8(
	__local float2 *buf, float2 *r,
	int p, int i, int t, int twiddle)
{
	int k = i & (p-1);

	fft_radix8_load(buf, r, i, t);

	if (twiddle)
		fft_radix8_twiddle(r, k, p);

	fft_radix8_exec(r);

	barrier(CLK_LOCAL_MEM_FENCE);

	fft_radix8_store(buf, r, i, k, p);

	barrier(CLK_LOCAL_MEM_FENCE);
}


/* ------------------------------------------------------------------------ */
/* FFT kernels                                                              */
/* ------------------------------------------------------------------------ */

__kernel void fft1D_512(
	__global   const float2 *input,
	__global         float2 *output,
	__constant const float  *win)
{
#define N 512
#define WG_SIZE (N / 8)

	__local float2 buf[N];

	float2 r[8];
	int lid = get_local_id(0);
	int i;

	/* Adjust ptr for batch */
	input  += N * get_global_id(1);
	output += N * get_global_id(1);

	/* Global load & window apply */
	for (i=lid; i<N; i+=WG_SIZE)
		buf[i] = input[i] * win[i];

	/* 1st pass: 1 * Radix-8 */
	fft_radix8(buf, r,  1, lid, WG_SIZE, 0);

	/* 2nd pass: 1 * Radix-8 */
	fft_radix8(buf, r,  8, lid, WG_SIZE, 1);

	/* 3rd pass: 1 * Radix-8 */
	fft_radix8(buf, r, 64, lid, WG_SIZE, 1);

	/* Global store */
	for (i=0; i<8; i++)
		output[i*WG_SIZE+lid] = buf[i*WG_SIZE+lid];

#undef WG_SIZE
#undef N
}


__kernel void fft1D_1024(
	__global   const float2 *input,
	__global         float2 *output,
	__constant const float  *win)
{
#define N 1024
#define WG_SIZE (N / 8)

	__local float2 buf[N];

	float2 r[8];
	int lid = get_local_id(0);
	int i;

	/* Adjust ptr for batch */
	input  += N * get_global_id(1);
	output += N * get_global_id(1);

	/* Global load & window apply */
	for (i=lid; i<N; i+=WG_SIZE)
		buf[i] = input[i] * win[i];

	/* 1st pass: 1 * Radix-8 */
	fft_radix8(buf, r,  1, lid, WG_SIZE, 0);

	/* 2nd pass: 1 * Radix-8 */
	fft_radix8(buf, r,  8, lid, WG_SIZE, 1);

	/* 3rd pass: 1 * Radix-8 */
	fft_radix8(buf, r, 64, lid, WG_SIZE, 1);

	/* 4th pass: 4 * Radix-2 */
	{
		const int p = 512;
		const int i = lid << 2;
		const int t = WG_SIZE << 2;
		const int k = i;

		fft_radix2_load(buf, r+0, i+0, t);
		fft_radix2_load(buf, r+2, i+1, t);
		fft_radix2_load(buf, r+4, i+2, t);
		fft_radix2_load(buf, r+6, i+3, t);

		fft_radix2_twiddle(r+0, k+0, p);
		fft_radix2_twiddle(r+2, k+1, p);
		fft_radix2_twiddle(r+4, k+2, p);
		fft_radix2_twiddle(r+6, k+3, p);

		fft_radix2_exec(r+0);
		fft_radix2_exec(r+2);
		fft_radix2_exec(r+4);
		fft_radix2_exec(r+6);

		barrier(CLK_LOCAL_MEM_FENCE);

		fft_radix2_store(buf, r+0, i+0, k+0, p);
		fft_radix2_store(buf, r+2, i+1, k+1, p);
		fft_radix2_store(buf, r+4, i+2, k+2, p);
		fft_radix2_store(buf, r+6, i+3, k+3, p);

		barrier(CLK_LOCAL_MEM_FENCE);
	}

	/* Global store */
	for (i=0; i<8; i++)
		output[i*WG_SIZE+lid] = buf[i*WG_SIZE+lid];

#undef WG_SIZE
#undef N
}

/* vim: set syntax=c: */
