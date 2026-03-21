

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
//void añadirProducto(Producto p);
//void listarProductos();
//void modificarProducto(Producto *p);
//void borrarProducto(Producto p);

#endif /* PRODUCTO_H_ */
