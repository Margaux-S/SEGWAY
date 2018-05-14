/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lib_android.h
 * Author: Léo
 *
 * Created on 12 mai 2018, 15:50
 */

#ifndef LIB_ANDROID_H
#define LIB_ANDROID_H

#include "../../includes.h"

#ifdef __cplusplus
extern "C" {
#endif
    int init_socket_server();
    void read_socket_values(int sckt, int client_sckt, float puissance, float angle, int sens);
    int update_values(int socket_desc, float puissance, float angle, int sens);



#ifdef __cplusplus
}
#endif

#endif /* LIB_ANDROID_H */

