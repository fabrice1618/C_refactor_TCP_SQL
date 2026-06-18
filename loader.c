#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>

typedef struct {
    float temperature;
    float humidite;
    float pression;
} Mesure;

// Variables globales
int socket_ecoute;
MYSQL *bdd;

/*
    Fonctions réseau
*/
int ouvrir_socket_serveur()
{
    struct sockaddr_in adresse_serveur;

    socket_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    // TODO: vérifier valeur retour != -1 erreur

    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(1618);
    adresse_serveur.sin_addr.s_addr = INADDR_ANY;

    bind(socket_ecoute, (struct sockaddr*)&adresse_serveur, sizeof(adresse_serveur));
    // TODO: Retour : 0 si succès, -1 si erreur (port déjà pris, droits insuffisants…).

    listen(socket_ecoute, 5);
    // TODO:  Retour : 0 si succès, -1 si erreur.

    return 0;
}

Mesure lire_socket_mesure_poll()
{
    socklen_t taille_adresse_client;
    struct sockaddr_in adresse_client;
    int socket_client;
    char tampon[1024];
    Mesure mesure;

    taille_adresse_client = sizeof(adresse_client);
    socket_client = accept(socket_ecoute, (struct sockaddr*)&adresse_client, &taille_adresse_client);
    // TODO: Retour : un nouveau descripteur de socket (int ≥ 0) pour ce client, -1 si erreur.

    read(socket_client, tampon, sizeof(tampon));
    memcpy(&mesure, tampon, sizeof(Mesure));
    // TODO: Retour : nombre d'octets réellement lus (ssize_t), 0 si le client a fermé la connexion, -1 si erreur.

    close(socket_client);
    // TODO: Retour : 0 si succès, -1 si erreur.

    return mesure;
}

/*
    Fonctions BDD
*/
int ouvrir_bdd()
{
    bdd = mysql_init(NULL);
    // TODO: vérifier Retour : MYSQL* initialisé, ou NULL si mémoire insuffisante.
    mysql_real_connect(bdd, "localhost", "meteo", "abcd", "meteo", 0, NULL, 0);
    // TODO: vérifier Retour : MYSQL* (= bdd) si succès, NULL si échec (connexion refusée, mauvais identifiants…).

    return 0;
}

int inserer_mesure(Mesure mesure)
{
    char q[512];

    sprintf(q, "INSERT INTO data VALUES(%f,%f,%f)", mesure.temperature, mesure.humidite, mesure.pression);
    mysql_query(bdd, q);
        // TODO: vérfifier  Retour : int — 0 si succès, ≠ 0 si erreur.

    return 0;
}

/* Foncton affichage
*/

void afficher_mesure(Mesure mesure)
{
    printf("temperature=%f humidite=%f pression=%f\n", mesure.temperature, mesure.humidite, mesure.pression);
}


int main()
{
    Mesure mesure;

    ouvrir_socket_serveur(&socket_ecoute);
    if (socket_ecoute == -1) {
        printf("erreur ouverture socket serveur");
    }

    ouvrir_bdd();

    // TODO: permettre l'arret du process avec un signal pour un arret propre
    while(1)
    {
        mesure = lire_socket_mesure_poll();

        inserer_mesure(mesure);

        afficher_mesure(mesure);
    }

    mysql_close(bdd);

    return 0;
}
