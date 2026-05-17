/*
 * valoracion.c
 *
 *  Created on: 22 mar 2026
 *      Author: l.esquibel
 */

#include "valoracion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

void InsertarValoracion(sqlite3 *db, Valoracion v){
    char sql[500];

    sprintf(sql,
        "INSERT OR IGNORE INTO valoracion (id, id_usuario, id_producto, puntuacion, comentario) "
        "VALUES (%d, %d, %d, %d, '%s');",
        v.id, v.id_usuario, v.id_producto, v.puntuacion, v.comentario);

    if(sqlite3_exec(db, sql, 0, 0, 0) != SQLITE_OK){
        printf("Error insertando valoracion\n");
    }
}

void cargarValoracionesFichero(char *nombreFichero, sqlite3 *db) {
    FILE *f = fopen(nombreFichero, "r");

    if (f != NULL) {
        Valoracion v;

        while (fscanf(f, " %d;%d;%d;%d;%199[^\n\r]",
                      &v.id,
                      &v.id_usuario,
                      &v.id_producto,
                      &v.puntuacion,
                      v.comentario) == 5) {
            InsertarValoracion(db, v);
        }

        fclose(f);
    } else {
        printf("Error abriendo fichero de valoraciones\n");
    }
}

void listarValoraciones(sqlite3 *db) {
    sqlite3_stmt *stmt;

    char *sql =
        "SELECT v.puntuacion, v.comentario, u.nombre, p.nombre "
        "FROM valoracion v "
        "LEFT JOIN usuario u ON v.id_usuario = u.id "
        "LEFT JOIN producto p ON v.id_producto = p.id;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al consultar valoraciones.\n");
        return;
    }

    printf("\n--- VALORACIONES DE CLIENTES ---\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("Usuario: %s | Producto: %s | Nota: %d | Comentario: %s\n",
               sqlite3_column_text(stmt, 2),
               sqlite3_column_text(stmt, 3),
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1));
    }

    sqlite3_finalize(stmt);
}

void listarValoracionesServidor(sqlite3 *db, char *respuesta) {
    sqlite3_stmt *stmt;

    char *sql =
        "SELECT v.puntuacion, v.comentario, u.nombre, p.nombre "
        "FROM valoracion v "
        "LEFT JOIN usuario u ON v.id_usuario = u.id "
        "LEFT JOIN producto p ON v.id_producto = p.id;";

    strcpy(respuesta, "\n--- VALORACIONES DE CLIENTES ---\n");

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        strcpy(respuesta, "ERROR;No se pudo consultar valoraciones");
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char linea[500];

        sprintf(linea,
                "Usuario: %s | Producto: %s | Nota: %d | Comentario: %s\n",
                sqlite3_column_text(stmt, 2),
                sqlite3_column_text(stmt, 3),
                sqlite3_column_int(stmt, 0),
                sqlite3_column_text(stmt, 1));

        strcat(respuesta, linea);
    }

    sqlite3_finalize(stmt);
}
