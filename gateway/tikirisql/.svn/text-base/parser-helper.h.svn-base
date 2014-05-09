/*
 * Copyright (c) 2009, Wireless Ad-Hoc Sensor Network Laboratory -
 * University of Colombo School of Computing.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * \file
 *      This file is part of the tikiridb system.
 * \author
 *      nayanajith Laxaman <nml@ucsc.cmb.ac.lk>
 */


#ifndef __PARSER_HELPER_H__
#define __PARSER_HELPER_H__

/*convert char array to int*/
int
char_arr_to_int(unsigned char *pBytes, int size);

/*convert int to char array*/
void
int_to_char_arr(int val, unsigned char *pBytes);

/*Store the SAMPLE PERIOD of the query*/
int
set_sample_period(int i);

/*Store the FOR duration of the query*/
int
set_for_period(int i);

/*Record fields(eg: TEMP, HUMID) from the query in the field queue*/
int
add_field(unsigned char name[]);

/*Record tables(eg: sensors, buffer) from the query in the table queue*/
int
add_table(unsigned char name[]);

/* SerialForwarder over the cient socket*/
int
send_query_to_sf(unsigned char *host, int port);

unsigned char
get_field_id(unsigned char field_name[]);

unsigned char
get_operator_id(unsigned char operator[]);

#endif /* __PARSER_HELPER_H__ */
