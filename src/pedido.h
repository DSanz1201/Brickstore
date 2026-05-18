/*
 * pedido.h
 *
 *  Created on: 21 mar 2026
 *      Author: l.esquibel
 */

#ifndef PEDIDO_H_
#define PEDIDO_H_

#include "sqlite3.h"
#include "lineaPedido.h"

typedef struct {
    int id_usuario;
    char fecha[20];
    LineaPedido *lineas;
} Pedido;

void registrarPedidoBD(sqlite3 *db, Pedido *p, char *respuesta);
void consultarPedidosBD(sqlite3 *db, int id_usuario, char *respuesta);
void liberarPedido(Pedido *p);

#endif /* PEDIDO_H_ */
