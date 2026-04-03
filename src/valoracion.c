/*
 * valoracion.c
 *
 *  Created on: 22 mar 2026
 *      Author: l.esquibel
 */


#include "valoracion.h"
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

void InsertarValoracion(sqlite3 *db,Valoracion v){
	char sql[500];

	    sprintf(sql,
	        "INSERT OR IGNORE INTO valoracion (id, id_usuario, id_producto, puntuacion, comentario) "
	        "VALUES (%d, %d, %d, %d, '%s');",
	        v.id, v.id_usuario, v.id_producto, v.puntuacion, v.comentario);

	    if(sqlite3_exec(db, sql, 0, 0, 0) != SQLITE_OK){
	        printf("Error insertando valoracion\n");
	        fflush(stdout);
	    }
}

void listarValoraciones(sqlite3 *db) {
    sqlite3_stmt *stmt;
    // Consulta con JOIN para ver nombres en lugar de solo IDs
    char *sql = "SELECT v.puntuacion, v.comentario, u.nombre, p.nombre "
                "FROM valoracion v "
                "LEFT JOIN usuario u ON v.id_usuario = u.id "
                "LEFT JOIN producto p ON v.id_producto = p.id;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al consultar valoraciones.\n");
        return;
    }
    printf("\n--- VALORACIONES DE CLIENTES ---\n");
      printf("%-15s | %-35s | %-5s | %-40s\n",
             "Usuario", "Producto", "Nota", "Comentario");
      printf("-----------------------------------------------------------------------------------------------\n");

      while (sqlite3_step(stmt) == SQLITE_ROW) {
          printf("%-15s | %-35s | %-5d | %-40s\n",
                 sqlite3_column_text(stmt, 2),
                 sqlite3_column_text(stmt, 3),
                 sqlite3_column_int(stmt, 0),
                 sqlite3_column_text(stmt, 1));
      }

      sqlite3_finalize(stmt);
}
