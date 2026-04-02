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
        "INSERT OR IGNORE INTO producto (id, nombre, stock, precio) "
        "VALUES (%d, '%s', %d, %f);",
        p.id, p.nombre, p.stock, p.precio);

    if(sqlite3_exec(db, sql, 0, 0, 0) != SQLITE_OK){
        printf("Error insertando producto\n");
    }
}
void anyadirProducto(sqlite3 *db) {
	Producto p;
	    printf("\n--- AÑADIR NUEVO PRODUCTO ---\n");
	    printf("ID: ");
	    scanf("%d", &p.id);
	    printf("Nombre: ");
	    fflush(stdin); // Limpiar buffer
	    scanf(" %[^\n]s", p.nombre); // Leer con espacios
	    printf("Stock inicial: ");
	    scanf("%d", &p.stock);
	    printf("Precio: ");
	    scanf("%f", &p.precio);

	    insertarProducto(db, p);
	    printf("Producto añadido correctamente a la base de datos.\n");
}

void listarProductos(sqlite3 *db) {
	sqlite3_stmt *stmt;
	    char *sql = "SELECT id, nombre, stock, precio FROM producto;";

	    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
	        printf("Error al preparar la consulta de listado.\n");
	        return;
	    }

	    printf("\n====================================================================\n");
	    printf("%-5s | %-35s | %-8s | %-10s\n", "ID", "Nombre del Producto", "Stock", "Precio");
	    printf("--------------------------------------------------------------------\n");

	    while (sqlite3_step(stmt) == SQLITE_ROW) {
	        int id = sqlite3_column_int(stmt, 0);
	        const unsigned char *nombre = sqlite3_column_text(stmt, 1);
	        int stock = sqlite3_column_int(stmt, 2);
	        double precio = sqlite3_column_double(stmt, 3);

	        printf("%-5d | %-35.35s | %-8d | %-8.2f EUR\n", id, nombre, stock, precio);
	    }
	    printf("====================================================================\n");

	    sqlite3_finalize(stmt);
}
void modificarProducto(sqlite3 *db) {
	int id, opcion;
	    float nuevoValor;
	    char sql[200];

	    printf("\n--- MODIFICAR PRODUCTO ---\n");
	    printf("Introduzca el ID del producto a modificar: ");
	    scanf("%d", &id);

	    printf("¿Qué desea modificar?\n1. Stock\n2. Precio\nSelección: ");
	    scanf("%d", &opcion);

	    if (opcion == 1) {
	        printf("Nuevo stock: ");
	        scanf("%f", &nuevoValor);
	        sprintf(sql, "UPDATE producto SET stock = %d WHERE id = %d;", (int)nuevoValor, id);
	    } else {
	        printf("Nuevo precio: ");
	        scanf("%f", &nuevoValor);
	        sprintf(sql, "UPDATE producto SET precio = %.2f WHERE id = %d;", nuevoValor, id);
	    }

	    if (sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK) {
	        printf("Producto actualizado con éxito.\n");
	    } else {
	        printf("Error al actualizar el producto.\n");
	    }
}
void eliminarProducto(sqlite3 *db) {
	int id;
	    char sql[100];
	    char confirmacion;

	    printf("\n--- ELIMINAR PRODUCTO ---\n");
	    printf("Introduzca el ID del producto que desea eliminar: ");
	    scanf("%d", &id);

	    printf("¿Está seguro de que desea eliminar el producto %d? (s/n): ", id);
	    scanf(" %c", &confirmacion);

	    if (confirmacion == 's' || confirmacion == 'S') {
	        sprintf(sql, "DELETE FROM producto WHERE id = %d;", id);
	        if (sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK) {
	            printf("Producto eliminado correctamente.\n");
	        } else {
	            printf("Error al eliminar el producto.\n");
	        }
	    } else {
	        printf("Operación cancelada.\n");
	    }
}
