/**
 * \file lib_serial.h
 * \author INSA Toulouse
 * \version 1.0
 * \date 20 Mai 2017
 */

#ifndef LIB_SERIAL_H
#define LIB_SERIAL_H

#include "../../includes.h"


#define MESSAGE_SERIAL_LENGTH 7
#define MESSAGE_SERIAL_LENGTH_READ 35
#define MESSAGE_TIMEOUT_NS 2000

typedef struct
{
  char label;
  float value;
} message_serial;


#ifdef	__cplusplus
extern "C" {
#endif

  int init_serial();
  message_serial* read_from_serial();
  int send_float_to_serial(float fl_value, char label);
  int send_int_to_serial(int int_value, char label);

  int bytes_to_array_of_message(unsigned char * mesg, message_serial *m);

  float bytes_to_float(unsigned char * bytes);
  int bytes_to_message(unsigned char * mesg, message_serial * m);
  int close_serial();

  void printf_trame(message_serial *m);
  
#ifdef	__cplusplus
}
#endif

#endif
