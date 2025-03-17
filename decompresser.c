/*Projet Huffman - Compression et Décompression de fichiers en C
Auteur : Rosselle QUIZON
Description du code : Implémente la décompression de fichiers en utilisant l'algorithme de Huffman*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

//structure représentant un noeud de l'arbre de Huffman 
typedef struct _noeud {
	uint32_t lettre;
    	uint32_t poids;
    	struct _noeud *parent;
    	struct _noeud *gauche;
   	struct _noeud *droite;
   	struct _noeud *suivant;
} noeud;

noeud* creer_noeud();
void remplace(noeud *racine, const char *code, uint8_t valeur);
void free_arbre(noeud *racine);
void decompresser(const char *fichier, const char *sortie);

int main(int argc, char *argv[]) 
{
    	if (argc < 3) 
    	{
        	fprintf(stderr, "Usage: %s fichier_compresse fichier_sortie\n", argv[0]);
        	return EXIT_FAILURE; //si il n'y a pas le fichier à décompresser et le fichier de sortie dans l'execution
    	}
    
    	decompresser(argv[1], argv[2]);
    	return EXIT_SUCCESS;
}

//fonction pour créer un noeud de l'arbre de Huffman
noeud* creer_noeud() 
{
    	noeud *n = malloc(sizeof(noeud));
    	if (n == NULL) 
    	{
       		assert(0);
    	}
    	n->lettre = 0; 
   	n->poids = 0;
    	n->parent = NULL;
    	n->gauche = NULL;
   	n->droite = NULL;
    	n->suivant = NULL;
    	return n;
}

//fonction qui va insérer les codes huffman dans l'arbre créé en partant de la racine
void remplace(noeud *racine, const char *code, uint8_t valeur) 
{
    	noeud *n = racine;
    	for (int i = 0; code[i] != '\0'; i++) 
    	{
        	if (code[i] == '0') //si le bit est '0', on va vers le sous-arbre gauche
        	{
            		if (n->gauche == NULL)
            		{
                		n->gauche = creer_noeud(); //on crée un noeud si ce n'est pas déjà fait
            		}
            		n = n->gauche;
        	} 
        	else if (code[i] == '1') //si le bit est '1', on va vers le sous-arbre droit
        	{
            		if (n->droite == NULL) 
            		{
                		n->droite = creer_noeud();
            		}
            		n = n->droite;
        	} 
        	else 
        	{
            		fprintf(stderr, "Erreur: caractère invalide '%c' dans le code Huffman.\n", code[i]);
            		exit(EXIT_FAILURE);
        	}
    	}
    	n->lettre = valeur; //on affecte le caractère à la feuille à la fin du chemin
}

//fonction pour libérer la mémoire allouée à l'arbre
void free_arbre(noeud *racine) 
{
    	if (racine == NULL)
    	{
    		return;
    	}
    	free_arbre(racine->gauche);
    	free_arbre(racine->droite);
    	free(racine);
}

//fonction principale de decompression
void decompresser(const char *fichier, const char *sortie) 
{
    	FILE *in = fopen(fichier, "rb"); //on lit le fichier d'entrée en mode binaire
    	if (in == NULL) 
    	{
        	fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s\n", fichier);
        	exit(EXIT_FAILURE);
    	}
    	
    	FILE *out = fopen(sortie, "wb"); //on va écrire dans le fichier de sortie en mode binaire 
    	if (out == NULL) 
    	{
		fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s pour écriture.\n", sortie);
		fclose(in);
		exit(EXIT_FAILURE);
    	}
    
    	uint16_t nb_codes;
    	if (fread(&nb_codes, sizeof(uint16_t), 1, in) != 1) //on va lire le nombre de codes stockés dans l'en tête 
    	{
        	fprintf(stderr, "Erreur lors de la lecture du nombre de codes.\n");
        	fclose(in);
        	exit(EXIT_FAILURE);
    	}
 
    	char codes[256][32] = {0}; //on déclare un tableau pour stocker les codes Huffman pour chaque caractère
    	
    	for (int i = 0; i < nb_codes; i++) 
    	{
		uint8_t valeur;
		uint8_t longueur;
		
		if (fread(&valeur, sizeof(uint8_t), 1, in) != 1) //on lit la valeur du code ascii du caractère
		{
		    fprintf(stderr, "Erreur lors de la lecture d'une valeur.\n");
		    fclose(in);
		    exit(EXIT_FAILURE);
		}
		
		if (fread(&longueur, sizeof(uint8_t), 1, in) != 1) { //on lit la longueur du code huffman associé
		    fprintf(stderr, "Erreur lors de la lecture de la longueur d'un code.\n");
		    fclose(in);
		    exit(EXIT_FAILURE);
		}
		
		if (fread(codes[valeur], sizeof(char), longueur, in) != longueur) //on lit le code huffman
		{
		    fprintf(stderr, "Erreur lors de la lecture du code %d.\n", valeur);
		    fclose(in);
		    exit(EXIT_FAILURE);
		}
		
		codes[valeur][longueur] = '\0'; //pour marquer la fin du code Huffman lu et stocké dans codes[valeur]
    	}
    
    	uint8_t bits_complet;
    	if (fread(&bits_complet, sizeof(uint8_t), 1, in) != 1) //on récupère le nombre de bits du dernier octet qui font partis du fichier compressé
    	{
		fprintf(stderr, "Erreur lors de la lecture du bits_complet.\n");
		fclose(in);
		exit(EXIT_FAILURE);
    	}
 
    	noeud *racine = creer_noeud(); //on va reconstruire l'arbre des codes Huffman à partir du tableau codes

    	for (int i = 0; i < 256; i++) 
    	{
        	if (codes[i][0] != '\0') 
        	{
            		remplace(racine, codes[i], (uint8_t)i);
        	}
    	}
    	
    	long position_debut = ftell(in); //position actuelle juste après l'en-tête
    	fseek(in, 0, SEEK_END);
    	long position_fin = ftell(in);  //position finale du fichier
    	long taillevraie = position_fin - position_debut; //on récupère la taille du fichier compressé
    	
    	fseek(in, position_debut, SEEK_SET); //on revient au debut du fichier
    
    	noeud *n = racine;  
    	unsigned char octet;
    	int nb_bits; //nombre de bits à lire dans l'octet courant
    	int bit;
    	
        int i = 0; //compteur pour identifier le dernier octet
        while (fread(&octet, sizeof(unsigned char), 1, in) != 0) 
        {

        
        	if (i == taillevraie - 1) 
        	{
            		nb_bits = bits_complet; //pour le dernier octet
		}
       	 	else
       	 	{
      			nb_bits = 8;
      		}

        	for (int j = 7; j >= (8 - nb_bits); j--) //on parcourt des bits de l'octet en partant du bit de poids fort (jusqu'au bit 8-nb_bits pour le dernier octet)
        	{
            		bit = (octet >> j) % 2;
            		if (bit == 0)
            		{
                		n = n->gauche;
			}
            		else
            		{
                		n = n->droite;
                	}
            
            		if (n->gauche == NULL && n->droite == NULL) //quand on a atteint une feuille, on a réussi à retrouver un caractère du fichier d'origine
            		{
		        	uint8_t valeur = (uint8_t) n->lettre;
		        	fwrite(&valeur, sizeof(uint8_t), 1, out); //on écrit ce caractère dans le fichier de sortie
		        	n = racine; //on revient à la racine pour le prochain caractère
            		}
        	}
        	i++;
        }
    
    	fclose(in);
    	fclose(out);
    	free_arbre(racine);
}
