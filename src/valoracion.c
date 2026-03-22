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
	        "INSERT INTO valoracion (id, id_usuario, id_producto, puntuacion, comentario) "
	        "VALUES (%d, %d, %d, %d, '%s');",
	        v.id, v.id_usuario, v.id_producto, v.puntuacion, v.comentario);

	    if(sqlite3_exec(db, sql, 0, 0, 0) != SQLITE_OK){
	        printf("Error insertando valoracion\n");
	        fflush(stdout);
	    }
}
