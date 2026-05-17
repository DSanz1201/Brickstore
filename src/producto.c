/*
 * producto.c
 *
 *  Created on: 21 mar 2026
 *      Author: l.esquibel
 */
#include "producto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void inicializarFichero(char *nombreFichero, sqlite3 *db) {
    FILE *f = fopen(nombreFichero, "r");

    if (f != NULL) {
        Producto p;

        while (fscanf(f, " %d;%49[^;];%d;%f",
                      &p.id,
                      p.nombre,
                      &p.stock,
                      &p.precio) == 4) {
            insertarProducto(db, p);
        }

        fclose(f);
    } else {
        printf("Error abriendo fichero de productos\n");
    }
}

void listarProductosServidor(sqlite3 *db, char *respuesta) {
    sqlite3_stmt *stmt;
    char *sql = "SELECT id, nombre, stock, precio FROM producto;";

    strcpy(respuesta, "\n--- CATALOGO DE PRODUCTOS ---\n");

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        strcpy(respuesta, "ERROR;No se pudo consultar productos");
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char linea[300];

        sprintf(linea,
                "ID: %d | Nombre: %s | Stock: %d | Precio: %.2f\n",
                sqlite3_column_int(stmt, 0),
                sqlite3_column_text(stmt, 1),
                sqlite3_column_int(stmt, 2),
                sqlite3_column_double(stmt, 3));

        strcat(respuesta, linea);
    }

    sqlite3_finalize(stmt);
}

void insertarProductoServidor(sqlite3 *db, Producto p, char *respuesta) {
    insertarProducto(db, p);
    strcpy(respuesta, "OK;Producto insertado correctamente");
}

void modificarProductoServidor(sqlite3 *db, int id, int stock, float precio, char *respuesta) {
    sqlite3_stmt *stmt;

    char *sql =
        "UPDATE producto "
        "SET stock = ?, precio = ? "
        "WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        strcpy(respuesta, "ERROR;No se pudo preparar modificacion");
        return;
    }

    sqlite3_bind_int(stmt, 1, stock);
    sqlite3_bind_double(stmt, 2, precio);
    sqlite3_bind_int(stmt, 3, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {

        if (sqlite3_changes(db) > 0) {
            strcpy(respuesta, "OK;Producto modificado correctamente");
        } else {
            strcpy(respuesta, "ERROR;No existe ningun producto con ese ID");
        }

    } else {
        sprintf(respuesta, "ERROR;No se pudo modificar producto: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

void eliminarProductoServidor(sqlite3 *db, int id, char *respuesta) {
    sqlite3_stmt *stmt;

    char *sql = "DELETE FROM producto WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sprintf(respuesta, "ERROR;No se pudo preparar eliminacion: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {

        if (sqlite3_changes(db) > 0) {
            strcpy(respuesta, "OK;Producto eliminado correctamente");
        } else {
            strcpy(respuesta, "ERROR;No existe ningun producto con ese ID");
        }

    } else {
        sprintf(respuesta, "ERROR;No se pudo eliminar producto: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}
