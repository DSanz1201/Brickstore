/*
 * usuario.h
 *
 *  Created on: 21 mar 2026
 *      Author: l.esquibel
 */

#ifndef USUARIO_H_
#define USUARIO_H_

#include "sqlite3.h"

typedef struct{
    int id;
    char nombre[50];
    char email[80];
    char password[30];
    int admin;
} Usuario;

int loginUsuario(sqlite3 *db, char* email, char* password, Usuario *u);
void insertarUsuario(sqlite3 *db, Usuario u);
void inicializarFicheroUsuarios(char *nombreFichero, sqlite3 *db);
void registrarAdmin(sqlite3 *db);

/* Función para servidor sockets */
void registrarAdminServidor(sqlite3 *db, Usuario u, char *respuesta);

#endif
