/* -*- c++ -*- */

#define GR_FOSPHOR_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "fosphor_swig_doc.i"

%{
#include "gnuradio/fosphor/glfw_sink_c.h"
#include "gnuradio/fosphor/wx_core_sink_c.h"
%}


%include "gnuradio/fosphor/base_sink_c.h"

%nodefaultctor gr::fosphor::glfw_sink_c;        // bug workaround
%include "gnuradio/fosphor/glfw_sink_c.h"
GR_SWIG_BLOCK_MAGIC2(fosphor, glfw_sink_c);

%include "gnuradio/fosphor/wx_core_sink_c.h"
GR_SWIG_BLOCK_MAGIC2(fosphor, wx_core_sink_c);
