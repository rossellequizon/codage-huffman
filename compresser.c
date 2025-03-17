/*Projet Huffman - Compression et Décompression de fichiers en C
Auteur : Rosselle QUIZON
Description du code : Implémente la compression de fichiers en utilisant l'algorithme de Huffman*/
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <string.h>

//structure représentant un noeud de l'arbre de Huffman
typedef struct _noeud
{
	uint32_t lettre; //Code ASCII du caractère
	uint32_t poids; //Nombre d'apparitions du caractère
	struct _noeud *parent;
	struct _noeud *gauche;
	struct _noeud *droite;
	struct _noeud *suivant; 
} noeud;				

//structure qui va représenter une liste triée des fréquences des caractères
typedef struct
{
	uint64_t taille; 
	noeud *tete; 
	noeud *queue;
} liste; 

//structure pour stocker les codes Huffman
typedef struct _maillon
{
    noeud *n;
    char code[32];
    struct _maillon *suivant;
} maillon;				


typedef struct {
    maillon *tete;
    maillon *queue;
    uint32_t taille;
} File;					

uint32_t* apparitions(FILE *f);
noeud* creer_noeud(uint32_t d, uint32_t i);
uint32_t min(uint32_t *tab);
liste* liste_croissante(uint32_t *tab);
void add_a_la_position(liste *l, noeud *m);
noeud* rem_tete(liste *l);
noeud* creer_arbre(liste *l);
void verifier_arbre(noeud *racine);
void free_arbre(noeud *racine);
void free_liste(liste *l);
void codes_huffman(noeud *racine, char codes[256][32]);
noeud* defiler(File *f, char *code);
void enfiler(File *f, noeud *n, char *code);
uint32_t en_tete(FILE *out, char codes[256][32]);
void compresser(char *fichier, char *sortie, char codes[256][32]);

int main(int argc, char *argv[])
{
    	if (argc < 3) 				
    	{
        	fprintf(stderr, "Usage: %s fichier_entree fichier_sortie\n", argv[0]);	
        	return EXIT_FAILURE; //si il n'y a pas le fichier à compresser et le fichier de sortie dans l'execution
    	}
    	
    	FILE *f = fopen(argv[1], "rb");
    	if (f == NULL) 
    	{
        	fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s\n", argv[1]);
        	return EXIT_FAILURE;
    	}
    
    	uint32_t *tab = apparitions(f); //tableau avec le nombre d'apparitions d'un caractère
    	fclose(f);
    
    	if (tab == NULL) 
    	{
        	fprintf(stderr, "Erreur lors du comptage des fréquences.\n");
        	return EXIT_FAILURE;
    	}
    
    
    	liste *l = liste_croissante(tab); //liste où sont rangés les noeuds par ordre croissant de leur poids 
    
    	noeud *racine = creer_arbre(l); //Création de l'arbre de Huffman
    
    	char codes[256][32] = {0}; //tableau où on va stocker tous nos codes créés avec huffman
    	codes_huffman(racine, codes);
    
    	compresser(argv[1], argv[2], codes);
    
    	free_arbre(racine);
    	free_liste(l);
    	free(tab);
    
    	return EXIT_SUCCESS;
}

//fonction pour compter le nombre d'apparitions de chaque caractère dans un fichier 
uint32_t* apparitions(FILE *f)
{
	uint32_t *tab = malloc(256 * sizeof(uint32_t)); 
	
	if (tab == NULL)
	{
		fprintf(stderr, "Erreur dans la création du tableau de nombre d'apparitions\n");
		return NULL;
	}
	
	for (int i = 0; i < 256; i++) 
	{
		tab[i] = 0; //on initialise tout le tableau à 0
	}
	
	int e;
	while ((e=fgetc(f)) != EOF) //on parcourt le fichier caractère par caractère
 	{
		tab[e] += 1; //la frequence à l'indice i correspond au caractère dont le nombre ascii associé est i
	}
	
	return tab;
}

//fonction pour créer un noeud de l'arbre de Huffman
noeud* creer_noeud(uint32_t d, uint32_t i)
{
	noeud *m = malloc(sizeof(noeud)); 
	if(m==NULL)
	{
		assert(0);
	}
	m->poids = d;
	m->lettre = i;
	m->parent = NULL;
	m->gauche = NULL;
	m->droite = NULL;
	m->suivant = NULL;
	
	return m;
}	

//fonction pour trouver l'indice du caractère avec la fréquence minimale 
uint32_t min(uint32_t *tab) 
{
	uint32_t i_min = 0; 
	while (tab[i_min] == 0) 
	{
        	i_min++; 
        }
	
	uint32_t min = tab[i_min];
	    
	//on a choisi le premier indice i_min dont la valeur au tableau n'est pas égal à 0
	//pour pouvoir initialiser le minimum avec cette valeur avant de trouver le vrai minimum à l'aide du for suivant
	for (uint32_t i = 0; i < 256; i++) 
	{
		if (tab[i] > 0 && tab[i] < min) 
		{ 
			min = tab[i];
			i_min = i;
		}
	}
	
	return i_min; 
}

//fonction pour construire une liste croissante des fréquences des caractères
liste* liste_croissante(uint32_t *tab) 
{
	liste *r = malloc(sizeof(liste)); //on initialise une liste 
	if(r==NULL)
	{
		assert(0);
	}
	r->taille = 0;
	r->tete = NULL;
	r->queue = NULL;
	
	uint32_t nb = 0; //cet entier va compter le nombre de caractères présents dans le fichier 
	//(car il n'y a pas forcément les 256 caractères de la table ascii)
	for (uint32_t i = 0; i < 256; i++)
	{
		if (tab[i] > 0)
		{
			nb++;
		}
	}
	
	uint32_t j = 0; 
	while (j < nb)
	{
	    	uint32_t k = min(tab); 
		uint32_t poids = tab[k]; 
		noeud *t = creer_noeud(poids, k); 
		add_a_la_position(r, t); 
		tab[k] = 0; //on remet à 0 la valeur du tableau afin de ne pas reprendre le même caractère
		j++;
	}
	return r;
}
	
//fonction qui ajoute un noeud à sa position correcte dans la liste triée
void add_a_la_position(liste *l, noeud *m)
{
	if (l->tete == NULL|| m->poids < l->tete->poids) 
	{
	       
		m->suivant = l->tete;
		l->tete = m;
		if (l->taille == 0) 
		{
			l->queue = m;
		}
	} 
	else
	{
		noeud *n = l->tete;
		while (n->suivant !=NULL && n->suivant->poids <= m->poids) 
		{
			n = n->suivant;
		}
		m->suivant = n->suivant;
		n->suivant = m;
	}
	l->taille++;
}


//fonction qui supprime et retourne le premier noeud de la liste
noeud* rem_tete(liste *l)
{
	if (l->tete == NULL) 
	{
		return NULL;
	}
	
	noeud *t = l->tete;
	l->tete = l->tete->suivant;
	l->taille -= 1;
	if( l->taille == 0 )
	{
		l->queue = NULL;
	}
	return t;
}

//fonction qui construit l'arbre de Huffman à partir de la liste triée 
noeud* creer_arbre(liste *l)
{
	while (l->taille > 1)
	{
		noeud *min1 = rem_tete(l);
		noeud *min2 = rem_tete(l);
		
		//on choisit comme valeur arbitraire 500 (pas dans la table ascii) pour indiquer qu'il ne s'agit pas d'une feuille
		noeud *fusion = creer_noeud(min1->poids + min2->poids, 500); 
		
		min1->parent = fusion;
		min2->parent = fusion;
		fusion->gauche = min1;
		fusion->droite = min2;
		
		add_a_la_position(l, fusion);	
	
	}

	return l->tete; //lorsque toutes les fusions sont faites, il ne nous reste plus que la racine
}

//fonction pour liberer la mémoire de la liste
void free_liste(liste *l)
{
	free(l);
}

//fonction pour liberer la mémoire de l'arbre
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

//fonction pour ajouter un noeud dans la file
void enfiler(File *f, noeud *n, char *code)
{
    	maillon *m = malloc(sizeof(maillon)); //on initialise un maillon
    	if (m == NULL)
    	{	
    		assert(0);
    	}

    	m->n = n;
    	strcpy(m->code, code);
    	m->suivant = NULL;

    	if (f->taille > 0)
    	{
        	f->queue->suivant = m;
    	}
    	else
    	{
        	f->tete = m;
    	}

    	f->queue = m;
    	f->taille += 1;
}

//fonction pour retirer un noeud de la file
noeud* defiler(File *f, char *code)
{
    	maillon *tmp = f->tete;
    	noeud *n = tmp->n;  
    	strcpy(code, tmp->code);  

    	f->tete = tmp->suivant;
    	free(tmp);
    	f->taille -= 1;
    	
    	if (f->taille == 0)
    	{
    		f->queue = NULL; 
    	}
  
    	return n;  
}

//fonction pour générer les codes Huffman
void codes_huffman(noeud *racine, char codes[256][32])
{
    
    	File *f = malloc(sizeof(File)); //on initialise une file
    	if (f == NULL)
    	{
    		assert(0);
    	}
    	f->tete = NULL;
    	f->queue = NULL;
    	f->taille = 0;
    	
    	enfiler(f, racine, "");  

    	while (f->taille > 0)  
    	{
        	char code[32];
        	noeud *n = defiler(f, code);  //on recupere le code de la tete de la file

        	if (n->gauche == NULL && n->droite == NULL)  //si on est arrivé à une feuille de l'arbre
        	{
            		strcpy(codes[n->lettre], code); //on copie ce code dans notre tableau
        	}
        	else  
        	{
            		char gauche_code[32], droite_code[32];

            		strcpy(gauche_code, code); //on passe au noeud suivant gauche
            		strcat(gauche_code, "0");
            		enfiler(f, n->gauche, gauche_code); //
	
            		strcpy(droite_code, code); //on passe au noeud suivant gauche
            		strcat(droite_code, "1");
            		enfiler(f, n->droite, droite_code);
        	}
    	}
    
    	maillon *n = f->tete;
    	while (n != NULL) //on libère la mémoire de la file
    	{	
        	maillon *tmp = n;
        	n = n->suivant;
        	free(tmp);  
    	}	
    	free(f); 
}

//fonction pour creer l'en tete du fichier compressé
uint32_t en_tete(FILE *out, char codes[256][32])
{
    	uint32_t decalage = 0;

    	uint16_t nb_codes = 0; //on va calculer le nombre de codes huffman, donc le nombre de caractères présents dans le fichier 
    	for (uint16_t i = 0; i < 256; i++) 
    	{
        	if (codes[i][0] != '\0') 
        	{
            		nb_codes++;
        	}
    	}
    	

    	fwrite(&nb_codes, sizeof(uint16_t), 1, out); //on ecrit le nombre total de codes au debut
    	decalage = decalage + 2;
    
    	for (uint16_t i = 0; i < 256; i++) //on va parcourir le tableau codes pour recuperer les codes huffman
    	{
        	if (codes[i][0] != '\0') 
        	{
            		uint8_t valeur = i;
            		fwrite(&valeur, sizeof(uint8_t), 1, out); //on ecrit d'abord le code ascii du caractère
            		decalage += 1;
            		
            		uint8_t code_length = (uint8_t)strlen(codes[i]); //on écrit ensuite la longueur du code huffman
            		fwrite(&code_length, sizeof(uint8_t), 1, out); 
            		decalage += 1;
            		
            		fwrite(codes[i], sizeof(char), code_length, out); //enfin, on écrit le code
            		
            		decalage = decalage + code_length; //on additionne code_length à decalage pour la taille totale de l'en tête
        	}
    	}

    	uint32_t position = decalage; //on sauvegarde la position à partir de laquelle commence la partie compressée
    	
    	uint8_t remplissage = 0; //on ecrit un octet de remplissage
    	fwrite(&remplissage, sizeof(uint8_t), 1, out);
    	decalage += 1;

    	return position;
}

//fonction principale de compression
void compresser(char *fichier, char *sortie, char codes[256][32])
{   	
    	FILE *in = fopen(fichier, "rb"); //on lit le fichier d'entrée en mode binaire
    	if (in == NULL)
    	{
    		assert(0);
    	}
    	
    	FILE *out = fopen(sortie, "wb"); //on va écrire dans le fichier de sortie en mode binaire 
    	if (out == NULL)
    	{
    		if (in != NULL)
        	{
            		fclose(in);
            	}
    		assert(0);
    	}

    
    	uint32_t position = en_tete(out, codes); 
    	//on écrit l'en tête dans le fichier de sortie et on recupère la position où commence la partie compressé

    	uint8_t buffer = 0;  //buffer temporaire pour stocker les bits avant de les écrire sous forme d'octet
    	int nb_bits = 0; //on va compter le nombre de bits actuellement stockés dans le buffer

    	int c;
    	while ((c = fgetc(in)) != EOF) //on lit le fichier caractère par caractère
    	{
        	char *code = codes[c];  //on récupère le code Huffman du caractère dans le tableau codes
        	if (code != NULL || code[0] != '\0')
        	{
			for (int i = 0; code[i] != '\0'; i++) //on parcourt le code huffman bit par bit
			{ 
		    		buffer = buffer << 1; //on décale le buffer d'un bit vers la gauche pour faire de la place pour le nouveau bit
		    		if (code[i] == '1')
		    		{
		        		buffer |= 1; //si le bit est '1', on met le bit de poids faible à 1
		        	}  
		    		nb_bits++;

				//lorsqu'on a atteint les 8 bits dans le buffer, on l'écrit dans le fichier de sortie
		    		if (nb_bits == 8) 
		    		{
		        		if (fputc(buffer, out) == EOF) //si il y a une erreur, fputc renvoie EOF
		        		{
				    		fprintf(stderr, "Erreur d'écriture dans le fichier de sortie.\n");
				    		fclose(in);
				    		fclose(out);
				    		return;
		        		}
		        		//on réinitialise le buffer et le compteur 
					buffer = 0;
					nb_bits = 0;
		    		}
			}
        	}
	}

    	if (nb_bits > 0) //si il reste des bits dans le buffer
    	{
        	for (int i = 0; i < (8 - nb_bits); i++) 
        	{
            		buffer = buffer << 1; //on complète le buffer avec des zéros pour atteindre 8 bits
        	}
        	//on écrit le dernier octet dans le fichier de sortie
       	 	if (fputc(buffer, out) == EOF) 
       	 	{
            		fprintf(stderr, "Erreur d'écriture dans le fichier de sortie.\n");
            		fclose(in);
            		fclose(out);
            		return;
        	}
    	}
    	
    	uint8_t bits_complet; //on va y inscrire le nombre de bits qui font vraiment partis du fichier compressé
	if (nb_bits > 0)
	{
    		bits_complet = nb_bits;
    	}
	else
	{
   		bits_complet = 8; 
   	}

    	fclose(out);
	
   	out = fopen(sortie, "rb+"); //on réouvre le fichier de sortie en mode lecture/écriture binaire
    	if (out == NULL) 
    	{
		fprintf(stderr, "Erreur de réouverture du fichier de sortie.\n");
		fclose(in);
		return;
    	}

    	fseek(out, position, SEEK_SET); //on se repositionne à l'endroit où s'arrete l'en tête avec position
    	fwrite(&bits_complet, sizeof(uint8_t), 1, out); //on écrit les bits vraiment utiles au fichier compressé

    	fclose(in);
    	fclose(out);
}
