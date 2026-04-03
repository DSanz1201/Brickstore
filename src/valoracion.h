/*
 * valoracion.h
 *
 *  Created on: 22 mar 2026
 *      Author: l.esquibel
 */

#ifndef VALORACION_H_
#define VALORACION_H_

#include "sqlite3.h"

typedef struct{
    int id;
    int id_usuario;
    int id_producto;
    int puntuacion;
    char comentario[200];
} Valoracion;

void InsertarValoracion(sqlite3 *db,Valoracion v);

void listarValoraciones(sqlite3 *db);

#endif /* VALORACION_H_ */
