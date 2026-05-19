/*
 * usuario.c
 *
 *  Created on: 21 mar 2026
 *      Author: l.esquibel
 */
#include "usuario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

int loginUsuario(sqlite3 *db, char* email, char* password, Usuario *u) {
    sqlite3_stmt *stmt;
    char *sql = "SELECT id, nombre, email, password, admin FROM usuario WHERE email = ? AND password = ?;";
    int result = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar el login: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u->id = sqlite3_column_int(stmt, 0);
        strcpy(u->nombre, (char*)sqlite3_column_text(stmt, 1));
        strcpy(u->email, (char*)sqlite3_column_text(stmt, 2));
        strcpy(u->password, (char*)sqlite3_column_text(stmt, 3));
        u->admin = sqlite3_column_int(stmt, 4);
        result = 1;
    }

    sqlite3_finalize(stmt);
    return result;
}

void insertarUsuario(sqlite3 *db, Usuario u){
    char sql[300];
    char *errMsg = 0;

    sprintf(sql,
        "INSERT OR IGNORE INTO usuario (id, nombre, email, password, admin) "
        "VALUES (%d, '%s', '%s', '%s', %d);",
        u.id, u.nombre, u.email, u.password, u.admin);

    if(sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK){
        printf("Error insertando usuario: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
}

void inicializarFicheroUsuarios(char *nombreFichero, sqlite3 *db){
    FILE *f = fopen(nombreFichero, "r");

    if(f == NULL){
        printf("Error abriendo fichero de usuarios\n");
        return;
    }

    Usuario u;
    char linea[300];
    int leidos;

    while(fgets(linea, sizeof(linea), f) != NULL){
        leidos = sscanf(linea, "%d;%49[^;];%79[^;];%29[^;];%d",
                        &u.id, u.nombre, u.email, u.password, &u.admin);

        if(leidos == 5){
            insertarUsuario(db, u);
        }
    }

    fclose(f);
}

void registrarAdmin(sqlite3 *db) {
    char nombre[50], email[80], password[30];
    Usuario u;

    printf("\n--- REGISTRAR NUEVO ADMINISTRADOR ---\n");

    printf("Nombre: ");
    scanf(" %[^\n]", nombre);

    printf("Email: ");
    scanf("%s", email);

    printf("Contraseña: ");
    scanf("%s", password);

    u.id = 0;
    strcpy(u.nombre, nombre);
    strcpy(u.email, email);
    strcpy(u.password, password);
    u.admin = 1;

    insertarUsuario(db, u);

    printf("Administrador registrado correctamente.\n");
}

void registrarAdminServidor(sqlite3 *db, Usuario u, char *respuesta) {
    sqlite3_stmt *stmt;

    char *sql =
        "INSERT INTO usuario (nombre, email, password, admin) "
        "VALUES (?, ?, ?, 1);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sprintf(respuesta,
                "ERROR;No se pudo preparar registro admin: %s",
                sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, u.nombre, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, u.email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, u.password, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        strcpy(respuesta, "OK;Administrador registrado correctamente");
    } else {
        sprintf(respuesta,
                "ERROR;No se pudo registrar admin: %s",
                sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}
