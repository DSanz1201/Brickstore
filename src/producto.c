/*
 * producto.c
 *
 *  Created on: 21 mar 2026
 *      Author: l.esquibel
 */

#include"producto.h"
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

void insertarProducto(sqlite3 *db, Producto p){
    char sql[300];

    sprintf(sql,
        "INSERT INTO producto (id, nombre, stock, precio) "
        "VALUES (%d, '%s', %d, %f);",
        p.id, p.nombre, p.stock, p.precio);

    if(sqlite3_exec(db, sql, 0, 0, 0) != SQLITE_OK){
        printf("Error insertando producto\n");
    }
}
//void añadirProducto(Producto p);
//void listarProductos();
//void modificarProducto(Producto *p);
//void borrarProducto(Producto p);
