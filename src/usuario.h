/*
 * usuario.h
 *
 *  Created on: 21 mar 2026
 *      Author: l.esquibel
 */

#ifndef USUARIO_H_
#define USUARIO_H_

#include "sqlite3.h"

typedef struct{
    int id;
    char nombre[50];
    char email[80];
    char password[30];
    int admin;  // 1 si es admin / 0 si es usuario normal
} Usuario;


#endif /* USUARIO_H_ */
