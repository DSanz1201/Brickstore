/*
 * usuario.c
 *
 *  Created on: 21 mar 2026
 *      Author: l.esquibel
 */

#include"producto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"
#include "usuario.h"

int loginUsuario(sqlite3 *db, char* email, char* password, Usuario *u) {
    sqlite3_stmt *stmt;
    char *sql = "SELECT id, nombre, email, password, admin FROM usuario WHERE email = ? AND password = ?;";
    int result = 0;

    //Preparamos la consulta
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar el login\n");
        return 0;
    }

    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    //Ejecutamos la consulta
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u->id = sqlite3_column_int(stmt, 0);
        strcpy(u->nombre, (char*)sqlite3_column_text(stmt, 1));
        strcpy(u->email, (char*)sqlite3_column_text(stmt, 2));
        strcpy(u->password, (char*)sqlite3_column_text(stmt, 3));
        u->admin = sqlite3_column_int(stmt, 4);
        result = 1;

    } else {
        printf("Credenciales incorrectas.\n");
    }
    sqlite3_finalize(stmt);
    return result;
}

void registrarAdmin(sqlite3 *db) {
    char nombre[50], email[80], password[30];
    char sql[300];

    printf("\n--- REGISTRAR NUEVO ADMINISTRADOR ---\n");
    printf("Nombre: ");
    scanf(" %[^\n]s", nombre);
    printf("Email: ");
    scanf("%s", email);
    printf("Contraseña: ");
    scanf("%s", password);

    sprintf(sql, "INSERT INTO usuario (nombre, email, password, admin) VALUES ('%s', '%s', '%s', 1);",
            nombre, email, password);

    if (sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK) {
        printf("Administrador registrado correctamente.\n");
    } else {
        printf("Error al registrar el administrador (es posible que el email ya exista).\n");
    }
}
