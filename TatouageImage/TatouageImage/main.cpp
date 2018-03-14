/*
*
* These routines read PGM bitmaps (types P2 and P5)
* and write out PGM files in binary (P5) format.
* Note that lines in PGM files should be no longer than 70
* characters long.
*
* PGM files have a maximum value of 255 for each pixel (8 bit greyscale)
*
* NOTE:
* Width and height paramaters must appear on the same line separated by
* a space in column size - number of rows order.
*/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

typedef struct {
	unsigned char red, green, blue;
} PPMPixel;

typedef struct {
	int x, y;
	PPMPixel *data;
} PPMImage;

#define RGB_COMPONENT_COLOR 255

#define MAXROWS 512
#define MAXCOLS 512
#define MAXLENGTH 256
#define MAXVALUE 255

static PPMImage *readPPM(const char *filename)
{
	char buff[16];
	PPMImage *img;
	FILE *fp;
	int c, rgb_comp_color;
	//open PPM file for reading
	fopen_s(&fp, filename, "rb");
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//read image format
	if (!fgets(buff, sizeof(buff), fp)) {
		perror(filename);
		exit(1);
	}

	//check the image format
	if (buff[0] != 'P' || buff[1] != '6') {
		fprintf(stderr, "Invalid image format (must be 'P6')\n");
		exit(1);
	}

	//alloc memory form image
	img = (PPMImage *)malloc(sizeof(PPMImage));
	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	//check for comments
	c = getc(fp);
	while (c == '#') {
		while (getc(fp) != '\n');
		c = getc(fp);
	}

	ungetc(c, fp);
	//read image size information
	if (fscanf_s(fp, "%d %d", &img->x, &img->y) != 2) {
		fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
		exit(1);
	}

	//read rgb component
	if (fscanf_s(fp, "%d", &rgb_comp_color) != 1) {
		fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
		exit(1);
	}

	//check rgb component depth
	if (rgb_comp_color != RGB_COMPONENT_COLOR) {
		fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
		exit(1);
	}

	while (fgetc(fp) != '\n');
	//memory allocation for pixel data
	img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	//read pixel data from file
	if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
		fprintf(stderr, "Error loading image '%s'\n", filename);
		exit(1);
	}

	fclose(fp);
	return img;
}
void writePPM(const char *filename, PPMImage *img)
{
	FILE *fp;
	//open file for output
	fopen_s(&fp, filename, "wb");
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//write the header file
	//image format
	fprintf(fp, "P6\n");

	//image size
	fprintf(fp, "%d %d\n", img->x, img->y);

	// rgb component depth
	fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);

	// pixel data
	fwrite(img->data, 3 * img->x, img->y, fp);
	fclose(fp);
}

using namespace std;
int readPGM(string Nfile, long &rows, long &cols, unsigned char image[MAXROWS][MAXCOLS])
{
	ifstream f(Nfile.c_str(), std::ios_base::binary);
	char c;


	string ligne;
	if (f.eof())
	{
		cout << "fichier vide";
		return 0;
	}
	f >> c;

	while (c == '#')
	{
		// Commentaire
		getline(f, ligne);
		f >> c;
	}

	if (c != 'P')
	{
		cout << "Format incorrect !" << endl;
		return 0;
	}
	else
	{
		f >> c;
		if (c != '2' && c != '5')
		{
			cout << "Format incorrect!!" << endl;
			return 0;
		}
	}

	f >> rows >> cols;
	int nbniveauxgris;
	f >> nbniveauxgris;
	f >> ws;

	long indice = f.tellg();
	int compt = 0;
	f.seekg(indice);
	f >> image[0][0] >> ws;

	indice++;

	for (long i = 0;i<rows;i++)
	{
		for (long j = 0;j<cols;j++)
		{
			f.seekg(indice);
			f >> image[i][j] >> ws;
			indice++;
		}

	}

	return 1;

}

/* INPUT: a filename (char*), the dimensions of the pixmap (rows,cols of
*   type long), and a pointer to a 2D array (MAXROWS x MAXCOLS) in row
*   major order.
* OUTPUT: an integer is returned indicating if the desired file was written
*   (in P5 PGM format (binary)).  A 1 is returned if the write was completed
*   and 0 if it was not.  An error message is returned if the file is not
*   properly opened.
*/
int pgmWrite(char* filename, long rows, long cols,
	unsigned char image[MAXROWS][MAXCOLS], char* comment_string) {
	ofstream file;        /* pointer to the file buffer */
	int maxval;        /* maximum value in the image array */
	long nwritten = 0; /* counter for the number of pixels written */
	long i, j;          /* for loop counters */

						/* return 0 if the dimensions are larger than the image array. */
	if (rows > MAXROWS || cols > MAXCOLS) {
		printf("ERROR: row/col specifications larger than image array:\n");
		return (0);
	}

	/* open the file; write header and comments specified by the user. */
	file.open(filename);
	if (file.fail())
	{
		cout << "ERROR: file open failed, incorrect file name" << endl;
		return 0;
	}
	file << "P5\n";

	if (comment_string != NULL)
	{
		file << "# " << comment_string << "\n";
	}

	/* write the dimensions of the image */
	file << cols << " " << rows << endl;

	/* NOTE: MAXIMUM VALUE IS WHITE; COLOURS ARE SCALED FROM 0 - */
	/* MAXVALUE IN A .PGM FILE. */

	/* WRITE MAXIMUM VALUE TO FILE */
	file << (int)255 << endl;

	/* Write data */
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			file << image[i][j];

	file.close();
	return(1);
}

// Utilise la méthode du patchwork (PPM)
void patchworkPPM(PPMImage *image, int &debutcarre1, int &debutcarre2, int &taillecarres)
{
	srand(time(NULL));
	debutcarre1 = rand() % (image->x * image->y);
	debutcarre2 = rand() % (image->x * image->y);
	taillecarres = 30;
	
	for (int i = 0; i < taillecarres; i++)
	{
		for (int j = 0; j < taillecarres; j++)
		{
			image->data[debutcarre1 + j + i * 512].red = (unsigned char)((int)(image->data[debutcarre1 + j + i * 512].red) - 1);
			image->data[debutcarre1 + j + i * 512].green = (unsigned char)((int)(image->data[debutcarre1 + j + i * 512].green) - 1);
			image->data[debutcarre1 + j + i * 512].blue = (unsigned char)((int)(image->data[debutcarre1 + j + i * 512].blue) - 1);

			image->data[debutcarre2 + j + i * 512].red = (unsigned char)((int)(image->data[debutcarre1 + j + i * 512].red) + 1);
			image->data[debutcarre2 + j + i * 512].green = (unsigned char)((int)(image->data[debutcarre1 + j + i * 512].green) + 1);
			image->data[debutcarre2 + j + i * 512].blue = (unsigned char)((int)(image->data[debutcarre1 + j + i * 512].blue) + 1);
		}
	}
}

// Utilise la méthode du patchwork (PGM)
void patchworkPGM(unsigned char image[MAXROWS][MAXCOLS], long rows, long cols, int &debutcarre1, int &debutcarre2, int &taillecarres)
{
	srand(time(NULL));
	debutcarre1 = rand() % (rows * cols);
	int debutcarre1x = debutcarre1 / cols;
	int debutcarre1y = debutcarre1 % rows;
	debutcarre2 = rand() % (rows * cols);
	int debutcarre2x = debutcarre2 / cols;
	int debutcarre2y = debutcarre2 % rows;
	taillecarres = 30;

	for (int i = 0; i < taillecarres; i++)
	{
		for (int j = 0; j < taillecarres; j++)
		{
			image[debutcarre1y + i][debutcarre1x + j] = (unsigned char)((int)(image[debutcarre1y + i][debutcarre1x + j]) - 1);

			image[debutcarre2y + i][debutcarre2x + j] = (unsigned char)((int)(image[debutcarre2y + i][debutcarre2x + j]) + 1);
		}
	}
}

double alphaDCT(int x, int N)
{
	if (x == 0)
	{
		return 1.0 / sqrt(N);
	}
	else
	{
		return sqrt(2.0 / N);
	}
}

// Calcule la DCT d'un bloc de 8 * 8 et renvoie le tableau qui contient les résultats (PGM)
// Ne marche pas
unsigned char dctBlocPGM(unsigned char image[MAXROWS][MAXCOLS], int starti, int startj)
{
	double tmp;
	double ret = 0;
	for (int i = starti; i < starti + 8; i++)
	{
		tmp = 0;
		for (int j = startj; j < startj + 8; j++)
		{
			tmp += (double)image[i][j] * cos((startj * M_PI * (2.0 * i + 1.0)) / 2.0 * 8.0) * cos((starti * M_PI * (2.0 * j + 1.0)) / 2.0 * 8.0);
		}
		ret += tmp;
	}
	ret *= alphaDCT(starti, 8) * alphaDCT(startj, 8);
	return (unsigned char) ret;
}

// Calcule la DCT entière et remplace l'image par celle-ci (PGM)
// Ne marche pas
void dctPGM(unsigned char image[MAXROWS][MAXCOLS], long rows, long cols)
{
	for (int i = 0; i < rows; i)
	{
		for (int j = 0; j < cols; j)
		{
			image[i][j] = dctBlocPGM(image, i, j);
		}
	}
	return;
}

// Calcule la DCT d'un bloc de 8 * 8 et renvoie le tableau qui contient les résultats (PPM)
// Ne marche pas
PPMPixel dctBlocPPM(PPMImage *image, int starti, int startj)
{
	double tmpr;
	double tmpg;
	double tmpb;
	double retr;
	double retg;
	double retb;
	PPMPixel ret;
	retr = 0;
	retg = 0;
	retb = 0;
	for (int i = starti; i < starti + 8; i++)
	{
		tmpr = 0;
		tmpg = 0;
		tmpb = 0;
		for (int j = startj; j < startj + 8; j++)
		{
			tmpr += (double)image->data[i * image->x + j].red * cos((startj * M_PI * (2.0 * i + 1.0)) / 2.0 * 8.0) * cos((starti * M_PI * (2.0 * j + 1.0)) / 2.0 * 8.0);
			tmpg += (double)image->data[i * image->x + j].green * cos((startj * M_PI * (2.0 * i + 1.0)) / 2.0 * 8.0) * cos((starti * M_PI * (2.0 * j + 1.0)) / 2.0 * 8.0);
			tmpb += (double)image->data[i * image->x + j].blue * cos((startj * M_PI * (2.0 * i + 1.0)) / 2.0 * 8.0) * cos((starti * M_PI * (2.0 * j + 1.0)) / 2.0 * 8.0);
		}
		retr += tmpr;
		retg += tmpg;
		retb += tmpb;
	}
	retr *= alphaDCT(starti, 8) * alphaDCT(startj, 8);
	retg *= alphaDCT(starti, 8) * alphaDCT(startj, 8);
	retb *= alphaDCT(starti, 8) * alphaDCT(startj, 8);
	ret.red = (unsigned char)retr;
	ret.green = (unsigned char)retg;
	ret.blue = (unsigned char)retb;
	return ret;
}

// Calcule la DCT entière et remplace l'image par celle-ci (PPM)
// Ne marche pas
void dctPPM(PPMImage *image)
{
	PPMPixel tmp;
	for (int i = 0; i < image->x; i++)
	{
		for (int j = 0; j < image->y; j++)
		{
			tmp = dctBlocPPM(image, i, j);
			image->data[i * image->x + j].red = tmp.red;
			image->data[i * image->x + j].green = tmp.green;
			image->data[i * image->x + j].blue = tmp.blue;
		}
	}
	return;
}

// Met les bits d'une image gris dans un pixel d'image de couleur en découpant un octet en 3 parties, 3, 3 et 2 qui sont mises dans les bits de poids faibles du pixel
void dissimulationPGMdansPPM(PPMImage *im_rvb, unsigned char im_gris[MAXROWS][MAXCOLS], long rows, long cols)
{
	if ((rows != im_rvb->x) || (cols != im_rvb->y))
	{
		cout << "Erreur, les deux images ne sont pas de la meme taille. La fonction a ete annulee." << endl;
		return;
	}
	unsigned char tmp1, tmp2, tmp3;
	
	for (int i = 0; i < im_rvb->x; i++)
	{
		for (int j = 0; j < im_rvb->y; j++)
		{
			// A cause de l'optimisation de compilateur (je suppose?) il faut séparer les lignes pour que ce soit bien des 0 qui remplacent les anciens bits
			tmp1 = im_gris[i][j] << 5;
			tmp1 = tmp1 >> 5;
			tmp2 = im_gris[i][j] << 2;
			tmp2 = tmp2 >> 5;
			tmp3 = im_gris[i][j] >> 6;

			// Pareil que juste avant
			im_rvb->data[i * im_rvb->x + j].red = im_rvb->data[i * im_rvb->x + j].red >> 3;
			im_rvb->data[i * im_rvb->x + j].red = im_rvb->data[i * im_rvb->x + j].red << 3;
			im_rvb->data[i * im_rvb->x + j].red = im_rvb->data[i * im_rvb->x + j].red | tmp1;
			im_rvb->data[i * im_rvb->x + j].green = im_rvb->data[i * im_rvb->x + j].green >> 3;
			im_rvb->data[i * im_rvb->x + j].green = im_rvb->data[i * im_rvb->x + j].green << 3;
			im_rvb->data[i * im_rvb->x + j].green = im_rvb->data[i * im_rvb->x + j].green | tmp2;
			im_rvb->data[i * im_rvb->x + j].blue = im_rvb->data[i * im_rvb->x + j].blue >> 2;
			im_rvb->data[i * im_rvb->x + j].blue = im_rvb->data[i * im_rvb->x + j].blue << 2;
			im_rvb->data[i * im_rvb->x + j].blue = im_rvb->data[i * im_rvb->x + j].blue | tmp3;
		}
	}
	return;
}

// Sort les bits d'une image gris à partir d'une image de couleur en récupérant les bits de poids faibles dans les composantes de couleurs
void extractionPGMdePPM(PPMImage *im_rvb, unsigned char im_gris[MAXROWS][MAXCOLS])
{
	unsigned char tmp1, tmp2, tmp3;

	for (int i = 0; i < im_rvb->x; i++)
	{
		for (int j = 0; j < im_rvb->y; j++)
		{
			tmp1 = im_rvb->data[i * im_rvb->x + j].red << 5;
			tmp1 = tmp1 >> 5;
			tmp2 = im_rvb->data[i * im_rvb->x + j].green << 5;
			tmp2 = tmp2 >> 2;
			tmp3 = im_rvb->data[i * im_rvb->x + j].blue << 6;

			im_gris[i][j] = tmp1 | tmp2 | tmp3;
		}
	}
	return;
}

int main()
{
	long rows, cols;
	int debutcarre1, debutcarre2, taillecarres;
	char nomfich[20];
	unsigned char photo[MAXROWS][MAXCOLS];
	unsigned char photo2[MAXROWS][MAXCOLS];
	PPMImage *image;
	cout << "Nom du fichier pgm :";
	cin >> nomfich;
	readPGM(nomfich, rows, cols, photo);
	cout << "Nom du fichier ppm :";
	cin >> nomfich;
	image = readPPM(nomfich);
	/*
	patchworkPGM(image, rows, cols, debutcarre1, debutcarre2, taillecarres);
	cout << "debut carre 1 :\t" << debutcarre1 << endl;
	cout << "debut carre 2 :\t" << debutcarre2 << endl;
	cout << "taille des carres :\t" << taillecarres << endl;
	*/
	//dctPGM(image, rows, cols);
	dissimulationPGMdansPPM(image, photo, rows, cols);
	extractionPGMdePPM(image, photo2);
	pgmWrite("lenares.pgm", rows, cols, photo2, "format pgm");
	writePPM("samarch.ppm", image);
	system("pause");



}