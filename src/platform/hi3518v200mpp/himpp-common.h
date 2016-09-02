/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-common.h
 * Copyright (C) 2015 Watson Xu <xuhuashan@gmail.com>
 *
 * live-streamer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * live-streamer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HIMPP_COMMON_H_
#define _HIMPP_COMMON_H_

#define HIMPP_SYS_ALIGN_WIDTH      16
#define HIMPP_PIXEL_FORMAT         PIXEL_FORMAT_YUV_SEMIPLANAR_420

#define ARRAY_SIZE(x)		(sizeof(x)/sizeof(x[0]))

#define HIMPP_PRINT(fmt, ...)      \
    printf("[%s:%d]:" fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif // _HIMPP_COMMON_H_
