#ifndef __TOKENS__
#define __TOKENS__
#include "string-auxiliar.h"

/**
 * Se declara la lista de tipos de tokens que puede tener cada token
 */
typedef enum {
  TOKEN_DEFF, TOKEN_DEFL, TOKEN_APPLY, TOKEN_SEARCH, TOKEN_QUIT,
  TOKEN_DEF, TOKEN_PRIMITIVA, TOKEN_DIGITO,
  TOKEN_IGUAL, TOKEN_COR_ABRE, TOKEN_COR_CIERRA, TOKEN_LLAVE_ABRE, TOKEN_LLAVE_CIERRA,
  TOKEN_COMA,TOKEN_PUNTO_COMA, TOKEN_ANG_ABRE, TOKEN_ANG_CIERRA, TOKEN_ERROR, TOKEN_EOF,
} TipoDeToken;

/**
 * Se declara la lista de tokens como una lista simplemente enlazada que guarda
 * el token como un String y el tipo de token
 */

typedef struct _TokenNodo {
  String token;
  TipoDeToken tipo;
  struct _TokenNodo *next;
} TokenNodo;

/**
 * Se declara la lista de tokens en este archivo con una estructura que apunta al principio y fin de la
 * lista, por practicidad al añadir tokens
 */

typedef struct _TokenList {
  TokenNodo *head;
  TokenNodo *tail;
} _TokenList;

typedef _TokenList *TokenList;

/**
 * crear_lista: void -> TokenList
 * Crea una lista simplemente enlazada de tokens vacía
 */
TokenList crear_lista();

/**
 * anadir_token: TokenList String -> TokenList
 * Añade el token dado al final de la lista de tokens
 */
TokenList anadir_token(TokenList l, String token, TipoDeToken tipo);

/**
 * destruir_lista: TokenList -> void
 * Destruye una lista de tokens y sus elementos
 */
void destruir_lista(TokenList lista);

#endif