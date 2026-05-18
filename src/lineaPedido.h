/*
 * lineaPedido.h
 *
 *  Created on: 21 mar 2026
 *      Author: l.esquibel
 */

#ifndef LINEAPEDIDO_H_
#define LINEAPEDIDO_H_
#include "sqlite3.h"

typedef struct LineaPedido {
    int id_producto;
    int cantidad;
    struct LineaPedido *siguiente; // Puntero al siguiente elemento de la lista
} LineaPedido;


#endif /* LINEAPEDIDO_H_ */
