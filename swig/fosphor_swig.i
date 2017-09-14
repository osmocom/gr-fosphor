/* -*- c++ -*- */

#define GR_FOSPHOR_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "fosphor_swig_doc.i"

%{
#include "gnuradio/fosphor/glfw_sink_c.h"
#include "gnuradio/fosphor/qt_sink_c.h"
%}

%typemap(in) gr::fft::window::win_type {
        $1 = (gr::fft::window::win_type)(PyInt_AsLong($input));
}

%include "gnuradio/fosphor/base_sink_c.h"

#ifdef ENABLE_GLFW
%nodefaultctor gr::fosphor::glfw_sink_c;        // bug workaround
%include "gnuradio/fosphor/glfw_sink_c.h"
GR_SWIG_BLOCK_MAGIC2(fosphor, glfw_sink_c);
#endif

#ifdef ENABLE_QT
%include "gnuradio/fosphor/qt_sink_c.h"
GR_SWIG_BLOCK_MAGIC2(fosphor, qt_sink_c);
#endif
