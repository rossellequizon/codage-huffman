Projet Huffman - Compression et Décompression de fichiers

Description

Ce projet a été réalisé dans le cadre d’un devoir académique afin d’implémenter et comprendre les algorithmes de compression et de décompression de fichiers en langage C. L'algorithme de Huffman est une méthode de compression sans perte qui attribue des codes plus courts aux symboles fréquents et des codes plus longs aux symboles rares, optimisant ainsi la taille du fichier compressé.

Le projet contient deux programmes :
compression.c : Programme permettant de compresser un fichier en utilisant l'algorithme de Huffman.
decompression.c : Programme permettant de décompresser un fichier compressé avec l'algorithme de Huffman.

Fichiers du projet :
compression.c : Programme permettant de compresser un fichier en utilisant l'algorithme de Huffman
decompression.c :  Programme permettant de décompresser un fichier compressé avec l'algorithme de Huffman
README.md : Documentation du projet
LICENSE : Fichier contenant la licence du projet (MIT)

Pré-requis : compilateur C installé, tel que gcc

Programme de compression :

Compilation : gcc compresser.c -o compresser
Execution : ./compresser entree.txt compresse.huf

Programme de décompression : 

Compilation : gcc decompresser.c -o decompresser
Execution : ./decompresser compresse.huf sortie.txt

Explication de l'algorithme de Huffman :

Analyse du fichier : On compte le nombre d'apparitions de chaque caractère.
Construction de l'arbre de Huffman : Chaque caractère devient un nœud avec un poids égal à sa fréquence d'apparition. On fusionne ensuite les deux nœuds ayant les poids les plus faibles jusqu'à obtenir un arbre unique.
Génération des codes : On attribue un code binaire unique à chaque caractère en parcourant l'arbre.
Compression : Le fichier compressé contient l'en-tête (table de correspondance) suivi des données compressées.
Décompression : On reconstruit l'arbre à partir de l'en-tête et on décode des bits pour retrouver le texte original.

Auteur : Rosselle QUIZON - Étudiante en informatique à SupGalilée 

Licence
Ce projet est sous **licence MIT**. Voir le fichier [LICENSE](LICENSE).
