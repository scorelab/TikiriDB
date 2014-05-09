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
 * This file is part of the tikiridb system.
 */

/**
 * \file
 *         Message Serializer Header.
 * \author
 *         N. M. Laxaman
 *
 */

#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include "messages.h"
/*Serialize message structures */
char *
message_header_serialize(message_header_t *mh);
char *
rfield_serialize(rfield_t *rf);
char *
qresult_header_serialize(qresult_header_t *qrh);
char *
field_serialize(field_t *field);
char *
expression_serialize(expression_t *exp);
char *
qmessage_header_serialize(qmessage_header_t *qmh);
char *
smessage_header_serialize(smessage_header_t *smh);

/*Resemble message structures */
message_header_t *
message_header_resemble(char msg[]);
rfield_t *
rfield_resemble(char msg[]);
qresult_header_t *
qresult_header_resemble(char msg[]);
field_t *
field_resemble(char msg[]);
expression_t *
expression_resemble(char msg[]);
qmessage_header_t *
qmessage_header_resemble(char msg[]);
smessage_header_t *
smessage_header_resemble(char msg[]);
#endif /* __SERIALIZER_H__ */
