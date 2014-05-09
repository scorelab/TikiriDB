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


#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>

struct termios oldtermios;

int
ttyraw(int fd)
{
  struct termios newtermios;
  if (tcgetattr(fd, &oldtermios) < 0)
    return (-1);
  newtermios = oldtermios;

  newtermios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  /* OK, why IEXTEN? If IEXTEN is on, the DISCARD character
   is recognized and is not passed to the process. This
   character causes output to be suspended until another
   DISCARD is received. The DSUSP character for job control,
   the LNEXT character that removes any special meaning of
   the following character, the REPRINT character, and some
   others are also in this category.
   */

  newtermios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  /* If an input character arrives with the wrong parity, then INPCK
   is checked. If this flag is set, then IGNPAR is checked
   to see if input bytes with parity errors should be ignored.
   If it shouldn't be ignored, then PARMRK determines what
   character sequence the process will actually see.

   When we turn off IXON, the start and stop characters can be read.
   */

  newtermios.c_cflag &= ~(CSIZE | PARENB);
  /* CSIZE is a mask that determines the number of bits per byte.
   PARENB enables parity checking on input and parity generation
   on output.
   */

  newtermios.c_cflag |= CS8;
  /* Set 8 bits per character. */

  newtermios.c_oflag &= ~(OPOST);
  /* This includes things like expanding tabs to spaces. */

  newtermios.c_cc[VMIN] = 1;
  newtermios.c_cc[VTIME] = 0;

  /* You tell me why TCSAFLUSH. */
  if (tcsetattr(fd, TCSAFLUSH, &newtermios) < 0)
    return (-1);
  return (0);
}

int
ttyreset(int fd)
{
  if (tcsetattr(fd, TCSAFLUSH, &oldtermios) < 0)
    return (-1);

  return (0);
}

void
sigcatch(int sig)
{
  ttyreset(0);
  exit(0);
}

/*
 * UP
 33
 133
 101

   Down
 33
 133
 102
 */
void
main()
{
  int i;
  char c;

  /* Catch the most popular signals. */
  if ((int) signal(SIGINT, sigcatch) < 0)
    {
      perror("signal");
      exit(1);
    }
  if ((int) signal(SIGQUIT, sigcatch) < 0)
    {
      perror("signal");
      exit(1);
    }
  if ((int) signal(SIGTERM, sigcatch) < 0)
    {
      perror("signal");
      exit(1);
    }

  /* Set raw mode on stdin. */
  if (ttyraw(0) < 0)
    {
      fprintf(stderr, "Can't go to raw mode.\n");
      exit(1);
    }

  while ((i = read(0, &c, 1)) == 1)
    {
      if ((c &= 255) == 0177){
      printf("%o\n\r", c);
      }
      break;
    }

  if (ttyreset(0) < 0)
    {
      fprintf(stderr, "Cannot reset terminal!\n");
      exit(-1);
    }

  if (i < 0)
    {
      fprintf(stderr, "Read error.\n");
      exit(-1);
    }

  exit(0);
}
