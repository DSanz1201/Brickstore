

// LEGOS Y VIDEOJUEGOS

#ifndef PRODUCTO_H_
#define PRODUCTO_H_

#include "sqlite3.h"
typedef struct{
	int id;
	char nombre[50];
	int stock;
	float precio;

}Producto;

void insertarProducto(sqlite3 *db, Producto p); // solo al inicio
void anyadirProducto(sqlite3 *db);
void listarProductos(sqlite3 *db);
void modificarProducto(sqlite3 *db);
void eliminarProducto(sqlite3 *db);

#endif /* PRODUCTO_H_ */
