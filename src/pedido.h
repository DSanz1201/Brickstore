/*
 * pedido.h
 *
 *  Created on: 21 mar 2026
 *      Author: l.esquibel
 */

#ifndef PEDIDO_H_
#define PEDIDO_H_

#include "sqlite3.h"

typedef struct{
    int id;
    int id_usuario;
    float total;
    char fecha[20];
} Pedido;

#endif /* PEDIDO_H_ */
