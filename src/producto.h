

// LEGOS Y VIDEOJUEGOS
#ifndef PRODUCTO_H_
#define PRODUCTO_H_

#include "sqlite3.h"

typedef struct{
    int id;
    char nombre[50];
    int stock;
    float precio;
} Producto;

void insertarProducto(sqlite3 *db, Producto p);
void anyadirProducto(sqlite3 *db);
void listarProductos(sqlite3 *db);
void modificarProducto(sqlite3 *db);
void eliminarProducto(sqlite3 *db);

/* Funciones para servidor sockets */
void inicializarFichero(char *nombreFichero, sqlite3 *db);
void listarProductosServidor(sqlite3 *db, char *respuesta);
void insertarProductoServidor(sqlite3 *db, Producto p, char *respuesta);
void modificarProductoServidor(sqlite3 *db, int id, int stock, float precio, char *respuesta);
void eliminarProductoServidor(sqlite3 *db, int id, char *respuesta);

#endif
