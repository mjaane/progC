/*****************
 *
 *@Proposito: Programa que organiza subastas, creandolas, actualizandolas y cerrandolas, ademas de mostrar su informacion, con la diferenacia
 *             que esta vez se tenia que hacer con memoria dinamica
 *@Autor: -----
 *@Fecha creacion:----
 *@Fecha ultima modificacion:----
 *
 ****************/
//Bibliotecas del sistema
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[50];                                                      //Nombre de la que apuesta
    float bid;                                                          //Cantidad apostada
}Bid;

//Declaramos estructura para guardar los objetos
typedef struct{
    char name[50];                                                      //nombre/descrpicion de los objetos
    int id;                                                             //Id de cada objeto
    float price;                                                        //Precio de los objetos
    int n_bids;                                                         //Numero de apuestas
    int max_bids;                                                       //Numero maximo de almacenamiento de apuestas
    int status;                                                         //Estado de la subasta, 1 abierta 0 cerrada
    int bigger;                                                         //Contador para saber la posicion de la apuesta mas grande
    Bid *bids;                                                          //Info de cada apuesta, con limite dinamico
}Objects;

typedef struct {    
    char user_id[50];                                                   //Nombre usuario
    int object_id;                                                      //Id objeto
    float bid_price;                                                    //Precio objeto
} FileBid;
/*****************
*
@Finalidad: Pasar numero char a int.
@Parámetros: int char inp[] == Parametro char 
@Retorno: res == char pasado a int
*
****************/
int myAtoi(char inp[]){
    int res = 0;                                                        //Resultado
    int i = 0;                                                          //Contador
    //Va pasando por cada posicion multiplicando por 10
    for (i = 0; inp[i] != '\0'; ++i)
        res = res * 10 + inp[i] - '0';
 
    return res;
}

/*****************
*
@Finalidad: Cargar la informacion del archivo .txt a un struct.
@Parámetros: Objects **obj == estructura donde almacenaremos toda la informacion
@Retorno: n_objects == devolvemos el numero de objetos que contiene la estructura.
*
****************/
int loadInfo(Objects **obj){
    char filename[50];                                                  //Nombre del archivo
    int n_objects;                                                      //Numero de objetos que contiene
    int i;                                                              //Contador para el bucle
    FILE *f;                                                            //Declaracion archivo

    printf("Enter auction file name: ");
    scanf("%s", filename);
    
    //Abrimos archivo y comprobamos que existe
    f = fopen(filename, "r");
    if (f == NULL) {

        printf("\n(ERROR) Auction file not found\n");
        n_objects = 0;
    } else {
        //Guardamos el numero de objetos que contiene
        fscanf(f, "%d objects in auction\n", &n_objects);                                       //Leer el número de objetos en la subasta
        *obj = (Objects *)malloc(n_objects * sizeof(Objects));                                  //Liberar espacio correspondiente para que quepa la informacion
        //Comprobamos que se asigna correctamente la memoria
        if (*obj != NULL) {
            // Bucle para guardar la informacion del .txt hasta llegar al numero de objetos
            for (i = 0; i < n_objects; i++) {
                //Pongo el cursor despues del : para solo copiar el nombre de la obra
                fseek(f, 6, SEEK_CUR);
                fgets((*obj)[i].name, 100, f);
                (*obj)[i].name[strlen((*obj)[i].name) - 1] = '\0';
                            
                fscanf(f, "Id: %d\n", &(*obj)[i].id);
                fscanf(f, "Initial price: %f euros\n", &(*obj)[i].price);
                (*obj)[i].n_bids = 0; //Inicializar el número de ofertas
                (*obj)[i].status = 1; //Inicializar el estado de la subasta
                (*obj)[i].bigger = 0; //inicializar el estado de mayor o menor
                (*obj)[i].bids = NULL;//Inicilaizar el numero de pujas a 0
            }
        }
        // Cerrar el archivo
        fclose(f);        
    }
    return n_objects;
}
/*****************
*
@Finalidad: Mostrar el menu para escoger la opcion deseada.
@Parámetros: -----
@Retorno: option == Opcion del menu deasead
*
****************/
int showMenu() {
    int option;                                                 //opcion del menu en entero
    char input[10];                                             //Opcion del menu antes depasar por myAtoi

    //Bucle hasta que la opcion sea una de las propuestas
    do {
        printf("\n1. Show objects | 2. Submit bid | 3. Automatic bids | 4. Object status | 5. Close auction | 6. Quit\n\n");
        printf("Enter option: "),
        scanf("%s", input);

        option = myAtoi(input);

        //Comprobacion por si no es correcta la opcion
        if(option < 1 || option > 6) {
            printf("\n(ERROR) Wrong option\n");
        }
    } while (option < 1 || option > 6);

    return option;
}

/*****************
*
@Finalidad: Muestra los objetos con su informacion guardada en el estruct
@Parámetros: Objects *obj == estructura donde almacenaremos toda la informacion
             int n_objects == Numero de objetos que contiene la estructura
@Retorno: ----.
*
****************/
void showObjects(Objects *obj, int n_objects){
    int i = 0;                                                                      //Contador

    printf("\nObjects in auction: \n");

    //Bucle para mostrar toda la lista de objetos, con sus apuestas y su nombre                                                                               
    for(i = 0; i < n_objects; i++){
        if(obj[i].status == 1) {
            printf("\t%d\t(%d bids)\t%s\n", obj[i].id, obj[i].n_bids, obj[i].name);
        }
    }

}
/*****************
*
@Finalidad: Reservar el espacio en la memoria dinamica correspondiente, ademas de devolver al array el max_bids que caben
@Parámetros: Objects *obj == estructura donde almacenaremos toda la informacion
@Retorno: ----.
*
****************/
void expansionBids(Objects *obj) {
    int i;

    //Si es el primer bid se inicializa en 1, sino se van añadiendo
    if (obj->max_bids == 0) {
        obj->max_bids = 1;
    } else {
        obj->max_bids *= 2;
    }

    // Asignar memoria para el nuevo array
    Bid *temp = (Bid *)malloc(obj->max_bids * sizeof(Bid));
    if (temp == NULL) {
        
    } else {
        //Bucle para ir copiando la info del array antiguo en un array temporal
        for (i = 0; i < obj->n_bids; i++) {
            temp[i] = obj->bids[i];
        }

        
        if(obj->bids != NULL) {
            free(obj->bids);                                            // Liberar la memoria de la matriz anterior
            obj->bids = NULL;                                           //Inicializar el array a NULL
        }
        if(temp != NULL){
        obj->bids = temp;                                               // Asignar la nueva matriz
        
        }
    
    }
}
/*****************
*
@Finalidad: Añadir manualmente apuestas
@Parámetros: Objects *obj == estructura donde almacenaremos toda la informacion
             int n_objects == Numero de objetos que contiene la estructura
@Retorno: ----.
*
****************/
void submitBid(Objects *obj, int n_objects) {
    char name[50];                                                      //Nombre de la persona que realiza la apuesta
    int id;                                                             //Id del objeto que quieren hacer la apuesta
    float price;                                                        //Valor de la apuesta a relaizar
    int i;                                                              //Contador
    int id_encontrado = 0;                                              //Contador que nos permite saber si hay coincidencias en el id
    int id_invalid = 0;                                                 //Contador que nos permite saber si la apuesta es valida o no
                                      
    //Pedimos el nombre de la que apuesta, el id de la subasta y el precio que quiere ofrecer
    printf("\nEnter bid: ");
    scanf("%s %d %f", name, &id, &price);
    
    id_encontrado = 0;
    i = 0;
                                          
        //Bucle que se hace mientras i sea menor que el numero de objetos, que no se haya encontrado aun niguna coincidencia y que la subasta siga abierta
        while(i < n_objects && id_encontrado == 0) {
            
            //Solo entra si encontramos alguna coincidencia en el id
            if(id == obj[i].id) {
                id_encontrado = 1;

                if(id_encontrado == 1 && obj[i].status == 1) {
                    //Comprobamos si es la primera y miramos si se puede o no hacer la subasta, sino invalid = 1
                    if(obj[i].n_bids == 0 && price < obj[i].price) {
                        id_invalid = 1;
                        
                    } else {
                        //Comprobando que es la primera y sabiendo que se puede hacer, copiamos cada info en su lugar correceto
                        if (obj[i].n_bids == 0 && price >= obj[i].price){
                            
                            //Si es el primer bid, ejecutamos la funcion para reservar la memoria
                            if (obj[i].max_bids == 0) {
                                expansionBids(&obj[i]);
                            }
                            
                            //Almacenamos la nueva informacion en la memoria reservada
                            obj[i].bids[0].bid = price;
                            strcpy(obj[i].bids[0].name, name);
                            obj[i].n_bids = 1;
                            id_invalid = 0;
                            obj[i].bigger = 0;
                             
                        }else {
                            
                            //Si el precio es mayor que el inicial del objeto añadimos la apuesta
                            if(price > obj[i].price) {
                                    
                                    //Si el numero de bids no supera a los maximos permitidos por la memoria, inicializamos la funcion para reservarla
                                    if (obj[i].n_bids >= obj[i].max_bids) {
                                        expansionBids(&obj[i]);
                                    }
                                    //Almacenamos la nueva informacion en la memoria reservada
                                    obj[i].bids[obj[i].n_bids].bid = price;
                                    strcpy(obj[i].bids[obj[i].n_bids].name, name);
                                    obj[i].n_bids++;
                                    id_invalid = 0;
                                
                                    //Comprobando si la apuesta es mayor o menor, comparandola con su anterior mayor
                                    if(obj[i].bids[obj[i].n_bids-1].bid > obj[i].bids[obj[i].bigger].bid) {
                                        obj[i].bigger = obj[i].n_bids-1;
                                    }
                                
                            }else {
                                id_invalid = 1;
                            }   
                        }
                    }
                } 
                
            } 
            //Que solo sume si no hemos encontrado el id, porque sino al salir los errores era con una posicion mas
            if(id_encontrado == 0) {
                i++;
            }            
        }
        
        //Prints de errores dependiendo de cual va primero
        if(id_encontrado == 0) {
            printf("\n(ERROR) This object does not exist\n");
        } else {
            if(obj[i].status == 0 && id_encontrado == 1) {
                printf("\n(ERROR) The auction for this object is already closed\n");
            } else {
                if(id_invalid == 1) {
                    printf("\n(ERROR) The bid is not valid\n");
                }
            }
        } 
}
/*****************
*
@Finalidad: Añadir automaticamente desde un archivo binario nuevas apuestas
@Parámetros: Objects **obj == estructura donde almacenaremos toda la informacion
             int n_objects == Numero de objetos que contiene la estructura
@Retorno: ----.
*
****************/

void getAutomatic (Objects **obj, int n_objects) {

    FILE *g;                                                    //Fichero binario
    FileBid bid;                                                //Declaramos estructura nueva
    char filename[30];                                          //Nombre fichero        
    int i;                                                      //Contador para bucle
    int bids_aceptadas;                                         //Contador de bids acepetadas
    int bids_rechazadas;                                        //Contador de bids rechazadas
    int objeto_encontrado = 0;                                  //Contador de objeto encontrado


    //Pedimos el nombre de archivo
    printf("\nEnter bids file name: ");
    scanf("%s", filename);

    bids_aceptadas = 0;
    bids_rechazadas = 0;

    //Comprobamos que el fichero se abra correctamente, sino es asi mostramos el error por pantalla
    g = fopen(filename, "rb");
    if(g == NULL) {
        printf("\n(ERROR) Bids file not found\n");
    } else {
        //Vamos guardando la info del archivo en una estructura
        while(fread(&bid, sizeof(FileBid), 1, g)) {
            objeto_encontrado = 0;

            //Bucle para recorrer todos los objetos de la primera estructura
            for(i = 0; i< n_objects; i++) {
                
                //Comprobamos si el id coincide
                if((*obj)[i].id == bid.object_id) {
                    objeto_encontrado = 1;
                    //Comprobamos que la bid es aceptable, es decir es mas grande que el precio inicial
                    if(bid.bid_price > (*obj)[i].price && (*obj)[i].status == 1) {
                        
                        //Reasignamos la memoria para los bids necesarios
                        if((*obj)[i].n_bids == 0) {
                            expansionBids(&(*obj)[i]);
                        } else {

                            //Si existe, ampliamos la memoria que ya teniamos
                            if((*obj)[i].n_bids >= (*obj)[i].max_bids) {
                                expansionBids(&(*obj)[i]);
                            }
                            
                        }

                        //Almacenamos la nueva informacion en la memoria reservada
                        strcpy((*obj)[i].bids[(*obj)[i].n_bids].name, bid.user_id);
                        (*obj)[i].bids[(*obj)[i].n_bids].bid = bid.bid_price;
                        (*obj)[i].n_bids++;

                        //Comprobamos si se ha de actualizar el mas grande
                        if (bid.bid_price > (*obj)[i].bids[(*obj)[i].bigger].bid) {
                            (*obj)[i].bigger = (*obj)[i].n_bids - 1;
                        }
                        bids_aceptadas++;                                                   //Sumamos uno a las apuestas aceptadas
                    } else {
                        bids_rechazadas++;                                                  //sumamos uno a las apuestas rechazadas
                    }
                } 
            }
            //Si el objeto no existe sumamos uno a bid rechazadas
            if(!objeto_encontrado) {
                bids_rechazadas++;
            }
        }
        //Mostramos las bids aceptadas y las bids rechazadas
        printf("\n%d new bids accepted\n", bids_aceptadas);
        printf("%d new bids discarded\n", bids_rechazadas);

        //Cerramos el archivo
        fclose(g);
    }
}

/*****************
*
@Finalidad: Dependiendo del id que escogas muestra todas las apuestas realizadas para ese objeto, ademas de marcarte con(*) cual es la mas grande
@Parámetros: Objects **obj == estructura donde almacenaremos toda la informacion
             int n_objects == Numero de objetos que contiene la estructura
@Retorno: ----.
*
****************/

void showStatus(Objects *obj, int n_objects) {
    int i = 0, j = 0, k = 0;                                    //Contadores
    int id = 0;                                                 //Valor de id
    int id_valid;                                               //Contador para id_valid

    //Pedimos el id que desamos mostrar
    printf("\nEnter object id: ");
    scanf("%d", &id);
    
    i = 0;
    id_valid = 0;

    //Bucle para pasar por todos los objetos, para verificar cuando coincide el id
    while(i < n_objects) {
        //Comprobamos que el id concuerda
        if(obj[i].id == id) {
            printf("\nBids for: %s\n", obj[i].name);
            
            if(obj[i].n_bids > 0 && obj[i].bids != NULL) {

                //Asignacion de memoria dinamica
                Bid *copy = malloc(obj[i].n_bids * sizeof(Bid));

                //Copiamos el contenido del array en un array temporal
                for(j = 0; j < obj[i].n_bids; j++) {
                    copy[j] = obj[i].bids[j];
                }

                //Bucle para ordenarlos de mayor a menor
                for(j = 0; j < obj[i].n_bids -1; j++) {
                    for(k = 0; k < obj[i].n_bids - j - 1; k++) {
                        if(copy[k].bid < copy[k+1].bid) {
                            Bid temp = copy[k];                         //Estrcutra temp para usar, a la hora de ordenar
                            copy[k] = copy[k+1];
                            copy[k+1] = temp;
                        }
                    }
                }


                //Bucle para hacer el print del objeto con el nombre de la persona y la cantidad, ademas de que el mayor va acompañado de un asterisco
                for(j = 0; j < obj[i].n_bids; j++){
                    //Si es el mayor se pone un asterisco
                    if(j == 0) {
                        printf("%s\t%.2f\t(*)\n", copy[j].name, copy[j].bid);
                    } else {
                        printf("%s\t%.2f\n", copy[j].name, copy[j].bid);
                    }
                }

                //Libreamos memoria y luego la inicializamos a NULL 
                free(copy);
                copy = NULL;
            }
            id_valid = 1;
        } 
        i++;
    }

    //Error por si el objeto no existe
    if(id_valid == 0) {
    printf("\n(ERROR) This object does not exist\n");
   }

}
/*****************
*
@Finalidad: Para cerrar manualmente las apuestas activas
@Parámetros: Objects **obj == estructura donde almacenaremos toda la informacion
             int n_objects == Numero de objetos que contiene la estructura
@Retorno: ----.
*
****************/
void closeAction (Objects **obj, int n_objects) {

    int id;                                                                     //Id del archivo que queremos cerrar
    int id_encontrado = 0;                                                      //Contador para si encontramos el id
    int i = 0;                                                                  //Contador para bucles

    //Pedimos el id del objeto que queremos cerrar su apuesta
    printf("\nEnter object id: "),
    scanf("%d", &id);

    i = 0;
    id_encontrado = 0;
    //Bucle que pasa por todos los objetos de la estructura hasta encontrar una coincidencia
    while(i < n_objects && id_encontrado == 0) {
            
            //Solo entra si encontramos alguna coincidencia en el id
            if(id == (*obj)[i].id) { 
                
                id_encontrado = 1;

                //Comprobamos si el objeto tiene ya la apuesta cerrada para mostrar su error
                if((*obj)[i].status == 0) {
                    printf("(ERROR) The auction for this object is already closed\n");
                    
                } else {
                    //Comprobamos el numero de apuestas que tiene, sino tiene mostramos el siguiente mensaje
                    if((*obj)[i].n_bids == 0) {
                        printf("Auction closed with no bids\n");
                        (*obj)[i].status = 0;
                        
                    } else {
                        //Si tiene alguna apuesta cerramos la apuesta y mostramos por pantalla cual a sido la puja ganadora del objeto
                        
                            printf("\nAuction closed for: %s", (*obj)[i].name);
                            printf("\nWinning bid: %.2f (%s)\n", (*obj)[i].bids[(*obj)[i].bigger].bid, (*obj)[i].bids[(*obj)[i].bigger].name);
                    }
                        //Si no es NULL se libera
                        if ((*obj)[i].bids != NULL) {
                            free((*obj)[i].bids);                                           //Liberamos la memoria ya usada
                            (*obj)[i].bids = NULL;                                          // Asignar NULL después de liberar la memoria
                        } 
                        (*obj)[i].status = 0;                                               //Inicializacion a 0 del status
                }
            }
        //Que solo sume si no hemos encontrado el id, porque sino al salir los errores era con una posicion mas
        if(id_encontrado == 0) {
            i++;
        }  
    }
    //Si no se han encontrado coincidencias mostramos un error
    if(id_encontrado == 0) {
        printf("\n(ERROR) This object does not exist\n");
    }

}

int main() {
    Objects *obj = NULL;                                                    //Declaracion de la estructura principal
    int n_objects;                                                          //Numero de objetos
    int option;                                                             //Numero de opcion seleccionada por el usuario
    int i;                                                                  //Contador para bucle
    
    //Llamamos a la funcion para cargar la informacion
    n_objects = loadInfo(&obj);

    //Si hay mas de un objeto empezamos el programa
    if(n_objects == 0) {
    } else {
        printf("Welcome to myAuctions!\n");

        //Bucle que acabara cuando la opcion del menu sea 6
        do {        
            //Llamamos a la funcion que nos proporciona la opcion deseada y muestra el menu
            option = showMenu();

            //Switch para escoger cada opcion del menu
            switch (option){

                case 1:
                    //Llamamos a la funcion para mostrar los objetos de la estructura
                    showObjects(obj, n_objects);
                    break;
                case 2:
                    //Lamamos a la funcion para añadir manualmente las apuestas
                    submitBid(obj, n_objects);
                    break;
                case 3:
                    //Llamamos a la funcion para añadir automaticamente las apuestas
                    getAutomatic(&obj, n_objects);
                    break;
                case 4:
                    //Lamamos a la funcion que muestra el estado de las apuestas de un objeto en especial
                    showStatus(obj, n_objects);
                    break;
                case 5:
                    //Llamamos a la funcion que cierra manualmente las apuestas
                    closeAction(&obj, n_objects);
                    break;
                case 6:
                    //Mostramos el mensaje de despedida del programa
                    printf("\nSee you in the next auction!");
                    break;
            
            }
            
        } while(option != 6);

        //Bucle para liberar informacion de todos los objetos que hay en el array
        for(i = 0; i < n_objects; i++) {
            if (obj[i].bids != NULL && obj[i].status == 1) {
                free(obj[i].bids);      
                obj[i].bids = NULL;
            }
        }

        //Si el array no es NULL se vacia toda la informacion
        if(obj != NULL) {
            free(obj);
            obj = NULL;
        }
    }
    
    return 0;
}