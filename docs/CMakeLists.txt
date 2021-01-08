# Copyright 2011-2020 Free Software Foundation, Inc.
# Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
#
# This file is part of gr-fosphor
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_DOXYGEN)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(doxygen)

endif(ENABLE_DOXYGEN)
