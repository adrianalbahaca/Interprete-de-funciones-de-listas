#include "parser.h"
#include "estructuras/ast.h"
#include "estructuras/string-auxiliar.h"
#include "estructuras/tokens.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * Para ver cada token, se declara de forma global la lista de tokens a analizar
 */
static TokenNodo *siguiente;

/* ------ Funciones Auxiliares ------ */

/**
 * next: TokenNodo* -> void
 * Si se le pasa un nodo de la lista de tokens, lo asigna en la variable
 * universal Sino, se mueve al siguiente nodo
 */
void next(TokenNodo *lista) {
  if (lista != NULL) {
    siguiente = lista;
  } 
  else if (siguiente == NULL) {
    return;
  }
  else {
    siguiente = siguiente->next;
  }
  return;
}

/**
 * match: TipoDeToken -> bool
 * Si el tipo de nodo coincide con el tipo de token, se mueve al siguiente y
 * retorna true Sino, no se hace nada y se retorna false
 */
bool match(TipoDeToken tipo) {
  if (siguiente == NULL) return false;
  if (siguiente->tipo == tipo)
    return true;
  else
    return false;
}

/**
 * error: String ASTTree -> ASTNodo*
 * Si ocurre algún error de gramática, puede mostrar un mensaje, si se le provee,
 * y destruir el arbol, si se provee. Retornando un nodo de tipo AST_ERROR para detectar
 * errores en llamadas futuras
 */
ASTNodo* error(String mensaje, ASTTree arbol) {
  if (mensaje != NULL)
    printf("%s\n", (String)mensaje);
  if (arbol != NULL)
    destruir_arbol(arbol);
  ASTNodo* nodoError = crear_nodo_arbol(NULL, AST_ERROR);
  return nodoError;
}

/* -------------------------------------------- */

/**
 * Reglas gramaticales
 * Forma de proceso de token: Verificar token -> hacer comandos -> Siguiente token
 */

/**
 * def: void -> ASTNodo*
 * Crear un nodo de una definición del AST si sigue la gramática adecuada
 */
ASTNodo* def() {
  ASTNodo* nodoDef = crear_arbol();
  if (match(TOKEN_DEF)) {
    nodoDef = crear_nodo_arbol(siguiente->token, AST_DEF);
  }
  else if (match(TOKEN_ERROR)) {
    nodoDef = error("Definición incorrecta", nodoDef);
  }
  return nodoDef;
}

/**
 * primitiva: void -> ASTNodo*
 * Se crea un nodo AST de tipo primitiva, si sigue la gramática adecuada
 */
ASTNodo* primitiva() {
  ASTNodo* nodoPrim = crear_arbol();
  if (match(TOKEN_PRIMITIVA)) {
    nodoPrim = crear_nodo_arbol(siguiente->token, AST_PRIMITIVA);
  }
  else if (match(TOKEN_ERROR)) {
    nodoPrim = crear_nodo_arbol(NULL, AST_ERROR);
  }
  return nodoPrim;
}

/**
 * funcs: void -> ASTTree
 * Funcs ::= DEF Funcs | PRIMITIVA Funcs | DEF | PRIMITIVA
 */
ASTTree funcs() {
  ASTTree nodoFunc = crear_arbol();
  if (match(TOKEN_DEF)) {
    nodoFunc = def();
  }
  else if (match(TOKEN_PRIMITIVA)) {
    nodoFunc = primitiva();
  }
  else if (match(TOKEN_ERROR)) {
    nodoFunc = error("ERROR: definición de error incorrecta", NULL);
    return nodoFunc;
  }
  else {
    return NULL;
  }
  next(NULL);

  // Funcs
  ASTTree nodoFuncs = funcs();
  if (nodoFuncs != NULL) {
    if (nodoFuncs->tipo != AST_ERROR) {
      anadir_hijo(nodoFunc, nodoFuncs);
    }
    else {
      destruir_arbol(nodoFuncs);
      nodoFunc = error(NULL, nodoFunc);
    }
  }
  return nodoFunc;
}

/**
 * elementos: void -> ASTTree
 * Elementos ::= DIGITO COMA Elementos | DIGITO | e
 */
ASTTree elementos() {
  // DIGITO COMA Elementos | DIGITO | e
  ASTTree nodoElemento = crear_arbol();
  if(match(TOKEN_DIGITO)) {
    nodoElemento = crear_nodo_arbol(siguiente->token, AST_DIGITO);
    next(NULL);
  }
  else if (match(TOKEN_ERROR)) {
    nodoElemento = error("ERROR: definición errónea de un elemento", nodoElemento);
    return nodoElemento;
  }
  else if(!match(TOKEN_COR_CIERRA)) {
    nodoElemento = error("ERROR: definición errónea de un elemento", nodoElemento);
    return nodoElemento;
  }

  // COMA Elementos
  if (match(TOKEN_COMA)) {
    next(NULL);
    ASTTree arbolElementos = elementos();
    anadir_hijo(nodoElemento, arbolElementos);
    if (arbolElementos->tipo == AST_ERROR) {
      nodoElemento = error(NULL, nodoElemento);
    }
  }
  else if (!match(TOKEN_COR_CIERRA)) {
    nodoElemento = error("ERROR: Falta la ',' entre dígitos", nodoElemento);
  }

  return nodoElemento;
}

/**
 * deff: void -> ASTTree
 * Deff ::= "deff" DEF IGUAL Funcs PUNTO_COMA | "deff" DEF IGUAL Funcs "<" Funcs ">" Funcs PUNTO_COMA
 */
ASTTree deff() {
  next(NULL);
  ASTTree arbolDeff = crear_nodo_arbol(NULL, AST_DEFF);

  // DEF IGUAL Funcs | DEF IGUAL Funcs "<" Funcs ">" Funcs
  ASTNodo* nodoDef = def();
  anadir_hijo(arbolDeff, nodoDef);
  
  if (nodoDef == NULL) {
    arbolDeff = error("ERROR: Dato incorrecto para la definición", arbolDeff);
    return arbolDeff;
  }
  else if (nodoDef->tipo == AST_ERROR) {
    arbolDeff = error(NULL, arbolDeff);
    return arbolDeff;
  }
  next(NULL);

  // IGUAL Funcs | IGUAL Funcs "<" Funcs ">" Funcs
  if(!match(TOKEN_IGUAL)) {
    arbolDeff = error("ERROR: Falta el símbolo '='", arbolDeff);
    return arbolDeff;
  }
  next(NULL);

  // Funcs | Funcs "<" Funcs ">" Funcs
  ASTTree arbolFuncs = funcs();
  if (arbolFuncs->tipo == AST_ERROR) {
    destruir_arbol(arbolFuncs);
    arbolDeff = error(NULL, arbolDeff);
    return arbolDeff;
  }

  // "<" Funcs ">" Funcs
  if(match(TOKEN_ANG_ABRE)) {
    next(NULL);
    ASTTree arbolRep = crear_nodo_arbol(NULL, AST_REP);
    anadir_hijo(arbolRep, arbolFuncs);
    anadir_hijo(arbolDeff, arbolRep);

    // Funcs ">" Funcs
    ASTTree arbolFuncs2 = funcs();
    anadir_hijo(arbolRep, arbolFuncs2);
    if (arbolFuncs2->tipo == AST_ERROR) {
      arbolDeff = error(NULL, arbolDeff);
      return arbolDeff;
    }

    // ">" Funcs
    if(!match(TOKEN_ANG_CIERRA)) {
      arbolDeff = error("ERROR: No se ha cerrado con '>'", arbolDeff);
      return arbolDeff;
    }
    next(NULL);

    // Funcs
    ASTTree arbolFuncs3 = funcs();
    anadir_hijo(arbolRep, arbolFuncs3);
    if (arbolFuncs3->tipo == AST_ERROR) {
      arbolDeff = error(NULL, arbolDeff);
      return arbolDeff;
    }
  }
  else if (!match(TOKEN_PUNTO_COMA)) {
    arbolDeff = error("ERROR: Hubo un problema en la declaración de funciones", arbolDeff);
    return arbolDeff;
  }
  else {
    anadir_hijo(arbolDeff, arbolFuncs);
  }

  return arbolDeff;
}

/**
 * defl: void -> ASTTree
 * Defl ::= "defl" DEF IGUAL COR_ABRE Elementos COR_CIERRA PUNTO_COMA
 */
ASTTree defl() {
  // "defl" DEF IGUAL COR_ABRE Elementos COR_CIERRA PUNTO_COMA
  next(NULL);
  ASTTree arbolDefl = crear_arbol();
  arbolDefl = crear_nodo_arbol(NULL, AST_DEFL);

  // DEF IGUAL COR_ABRE Elementos COR_CIERRA PUNTO_COMA
  ASTNodo* nodoDef = def();
  anadir_hijo(arbolDefl, nodoDef);
  
  if (nodoDef == NULL) {
    arbolDefl = error("ERROR: Dato incorrecto para la definición", arbolDefl);
    return arbolDefl;
  }
  else if (nodoDef->tipo == AST_ERROR) {
    arbolDefl = error(NULL, arbolDefl);
    return arbolDefl;
  }
  next(NULL);

  // IGUAL COR_ABRE Elementos COR_CIERRA PUNTO_COMA
  if(!match(TOKEN_IGUAL)) {
    arbolDefl = error("ERROR: Falta el símbolo '='", arbolDefl);
    return arbolDefl;
  }
  next(NULL);

  // COR_ABRE Elementos COR_CIERRA PUNTO_COMA
  if(!match(TOKEN_COR_ABRE)) {
    arbolDefl = error("ERROR: Falta iniciar con '['", arbolDefl);
    return arbolDefl;
  }
  next(NULL);

  // Elementos COR_CIERRA PUNTO_COMA
  ASTTree arbolElementos = elementos();
  anadir_hijo(arbolDefl, arbolElementos);
  if(arbolElementos != NULL && arbolElementos->tipo == AST_ERROR) {
    arbolDefl = error(NULL, arbolDefl);
    return arbolDefl;
  }
  
  // COR_CIERRA PUNTO_COMA
  if(!match(TOKEN_COR_CIERRA)) {
    arbolDefl = error("ERROR: Falta cerrar con ']'", arbolDefl);
    return arbolDefl;
  }
  next(NULL);

  return arbolDefl;
}

/**
 * apply: void -> ASTTree
 * Apply ::= "apply" DEF DEF | "apply" PRIMITIVA DEF | "apply" DEF LISTA | "apply" PRIMITIVA LISTA
 */
ASTTree apply() {
  next(NULL);
  ASTTree arbolApply = crear_arbol();
  arbolApply = crear_nodo_arbol(NULL, AST_APPLY);

  if (match(TOKEN_DEF)) {
    ASTNodo* nodoDef = def();
    anadir_hijo(arbolApply, nodoDef);
  }
  else if (match(TOKEN_PRIMITIVA)) {
    ASTNodo* nodoPrim = primitiva();
    anadir_hijo(arbolApply, nodoPrim);
  }
  else {
    arbolApply = error("ERROR: Definición de función no válida", arbolApply);
    return arbolApply;
  }

  next(NULL);

  if (match(TOKEN_DEF)) {
    ASTNodo* nodoDef2 = def();
    anadir_hijo(arbolApply, nodoDef2);
    next(NULL);
  }
  else if (match(TOKEN_COR_ABRE)) {
    next(NULL);

    ASTTree arbolElementos = elementos();
    anadir_hijo(arbolApply, arbolElementos);

    if(arbolElementos != NULL && arbolElementos->tipo == AST_ERROR) {
      arbolApply = error(NULL, arbolApply);
      return arbolApply;
    }

    if(!match(TOKEN_COR_CIERRA)) {
      arbolApply = error("ERROR: Falta cerrar con ']'", arbolApply);
      return arbolApply;
    }
    next(NULL);
  }
  else {
    arbolApply = error("ERROR: Definición de lista no válida", arbolApply);
    return arbolApply;
  }

  return arbolApply;
}
/*---------------------------------------------------------------*/

/**
 * parse: TokenList -> ASTTree
 * Toma una lista de tokens y la convierte en un árbol de sintáxis abstracto
 * para poder procesarlo Sentencia ::= Definicion ';' | Aplicacion  ';' |
 * Busqueda ';'
 */
ASTTree parse(TokenList tokens) {

  // Asignar lista de tokens a procesar y a inicializar el arbol
  next(tokens->head);
  ASTTree ast = crear_arbol();

  // Luego, empezamos a determinar el caso que tenemos
  switch (siguiente->tipo) {
    case TOKEN_DEFF:
      ast = deff();
      if (ast->tipo == AST_ERROR) {
        destruir_arbol(ast);
        ast = NULL;
      }
      else if (!match(TOKEN_PUNTO_COMA)) {
        ast = error("Falta el punto y coma para finalizar el comando", ast);
      }
      next(NULL);
      break;
    
    case TOKEN_DEFL:
      ast = defl();
      if (ast->tipo == AST_ERROR) {
        destruir_arbol(ast);
        ast = NULL;
      }
      else if (!match(TOKEN_PUNTO_COMA)) {
        ast = error("Falta el punto y coma para finalizar el comando", ast);
      }
      next(NULL);
      break;

      case TOKEN_APPLY:
      ast = apply();
      if (ast->tipo == AST_ERROR) {
        destruir_arbol(ast);
        ast = NULL;
      }
      else if (!match(TOKEN_PUNTO_COMA)) {
        ast = error("Falta el punto y coma para finalizar el comando", ast);
      }
      next(NULL);
      break;
    
    default:
      ast = error("ERROR: Comando no válido. Intente de nuevo", ast);
      break;
  }

  if (ast == NULL || ast->tipo == AST_ERROR) {
    printf("Parece que hubo un error con el parsing. Intente de nuevo\n");
  } else {
    printf("Parsing con éxito\n");
    siguiente = NULL;
  }

  destruir_lista(tokens);
  return ast;
}

