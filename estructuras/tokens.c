#include "tokens.h"
#include "string-auxiliar.h"
#include <stdlib.h>
#include <assert.h>

/**
 * crear_lista: void -> TokenList
 * Crea una lista simplemente enlazada de tokens vacía
 */
TokenList crear_lista() {

  TokenList list = malloc(sizeof(_TokenList));
  assert(list);
  list->head = list->tail = NULL;
  
  return list;
}

/**
 * anadir_token: TokenList String -> TokenList
 * Añade el token dado al final de la lista de tokens
 */
TokenList anadir_token(TokenList l, String token, TipoDeToken tipo) {

  // Crear nodo y copiar el token allí
  TokenNodo *nodo = malloc(sizeof(TokenNodo));
  assert(nodo);
  if (token != NULL) nodo->token = str_dup(token);
  else nodo->token = NULL;
  nodo->tipo = tipo;
  nodo->next = NULL;

  // Caso 1: Lista vacía
  if (l->head == NULL && l->tail == NULL) {
    l->head = l->tail = nodo;
  }
  else { // Caso 2: Lista con elementos
    l->tail->next = nodo;
    l->tail = nodo;
  }

  return l;

}

/**
 * destruir_lista: TokenList -> void
 * Destruye una lista de tokens y sus elementos
 */
void destruir_lista(TokenList lista) {
    TokenNodo* actual = lista->head;
    TokenNodo* sig;
    while (actual != NULL) {
        sig = actual->next;
        if (actual->token != NULL) free(actual->token);
        free(actual);
        actual = sig;
    }
    lista->head = lista->tail = NULL;
    free(lista);
    return;
}