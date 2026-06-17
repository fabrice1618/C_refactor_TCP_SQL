#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>

int main()
{
    int socket_ecoute;
    int socket_client;
    socklen_t taille_adresse_client;
    struct sockaddr_in adresse_serveur;
    struct sockaddr_in adresse_client;
    char tampon[1024];
    float *mesures;
    MYSQL *bdd;
    char q[512];

    socket_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    // TODO: vérifier valeur retour != -1 erreur

    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(1618);
    adresse_serveur.sin_addr.s_addr = INADDR_ANY;

    bind(socket_ecoute, (struct sockaddr*)&adresse_serveur, sizeof(adresse_serveur));
    // TODO: Retour : 0 si succès, -1 si erreur (port déjà pris, droits insuffisants…).

    listen(socket_ecoute, 5);
    // TODO:  Retour : 0 si succès, -1 si erreur.

    bdd = mysql_init(NULL);
    // TODO: vérifier Retour : MYSQL* initialisé, ou NULL si mémoire insuffisante.
    mysql_real_connect(bdd, "localhost", "meteo", "abcd", "meteo", 0, NULL, 0);
    // TODO: vérifier Retour : MYSQL* (= bdd) si succès, NULL si échec (connexion refusée, mauvais identifiants…).

    // TODO: permettre l'arret du process avec un signal pour un arret propre
    while(1)
    {
        taille_adresse_client = sizeof(adresse_client);
        socket_client = accept(socket_ecoute, (struct sockaddr*)&adresse_client, &taille_adresse_client);
        // TODO: Retour : un nouveau descripteur de socket (int ≥ 0) pour ce client, -1 si erreur.

        read(socket_client, tampon, 1024);
        // TODO: Retour : nombre d'octets réellement lus (ssize_t), 0 si le client a fermé la connexion, -1 si erreur.

        mesures = (float*)tampon;

        float temperature = mesures[0];
        float humidite = mesures[1];
        float pression = mesures[2];
        // TODO: vérifier la validité des valeurs lues et de la taille du buffer

        sprintf(q, "INSERT INTO data VALUES(%f,%f,%f)", temperature, humidite, pression);

        mysql_query(bdd, q);
        // TODO: vérfifier  Retour : int — 0 si succès, ≠ 0 si erreur.

        printf("temperature=%f humidite=%f pression=%f\n", temperature, humidite, pression);

        close(socket_client);
        // TODO: Retour : 0 si succès, -1 si erreur.
    }

    mysql_close(bdd);

    return 0;
}
