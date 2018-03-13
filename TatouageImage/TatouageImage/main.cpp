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
/* INPUT: a filename (char*),row and column dimension variables (long), and
*   a pointer to a 2D array of unsigned char's of size MAXROWS x MAXCOLS
*   (row major).
* OUTPUT: an integer is returned indicating whether or not the
*   file was read into memory (in row major order).  1 is returned if the
*   file is read correctly, 0 if it is not.  If there are
*   too few pixels, the function still returns 1, but returns an error
*   message.  Error messages are also returned if a file cannot be open,
*   or if the specifications of the file are invalid.
* NOTE: The case where too many pixels are in a file is not detected.
*/
int pgmRead(char *fileName, long *rows, long *cols,
	unsigned char image[MAXROWS][MAXCOLS]) {
	ifstream filePointer;    /* for file buffer */
	char *line; /* for character input from file */
	int maximumValue = 0; /* max value from header */
	int binary;           /* flag to indicate if file is binary (P5)*/
	long numberRead = 0;  /* counter for number of pixels read */
	long i, j;             /* (i,j) for loops */
	int temp;        /* for detecting EOF(test) and temp storage */
	string tmpline;

						   /* Open the file, return an error if necessary. */
	filePointer.open(fileName);
	if (filePointer.fail())
	{
		cout << "ERROR: file open failed, incorrect file name" << endl;
		return 0;
	}

	/* Initialize columnsidth, and height */
	*cols = *rows = 0;

	/* Check the file signature ("Magic Numbers" P2 and P5); skip comments
	* and blank lines (CR with no spaces before it).*/
	getline(filePointer, tmpline);
	line = const_cast<char*>(tmpline.c_str());
	while (line[0] == '#' || line[0] == '\n')
	{
		getline(filePointer, tmpline);
		line = const_cast<char*>(tmpline.c_str());
	}
	// Format P2
	if (line[0] == 'P' && (line[1] == '2')) {
		binary = 0;
	}
	// Format P5
	else if (line[0] == 'P' && (line[1] == '5')) {
		binary = 1;
	}
	else {
		printf("ERROR: incorrect file format\n\n");
		filePointer.close();
		return (0);
	}

	/* Input the width, height and maximum value, skip comments and blank
	* lines. */
	getline(filePointer, tmpline);
	line = const_cast<char*>(tmpline.c_str());
	while (line[0] == '#' || line[0] == '\n')
	{
		getline(filePointer, tmpline);
		line = const_cast<char*>(tmpline.c_str());
	}
	sscanf_s(line, "%ld %ld", cols, rows);

	getline(filePointer, tmpline);
	line = const_cast<char*>(tmpline.c_str());
	while (line[0] == '#' || line[0] == '\n')
	{
		getline(filePointer, tmpline);
		line = const_cast<char*>(tmpline.c_str());
	}
	sscanf_s(line, "%d", &maximumValue);

	/* display specifications and return an error if h,w, or
	*  maximum value is illegal.*/
	/*    printf("Rows: %ld   Columns: %ld\n",*rows,*cols); */
	/*       printf("Maximum value: %d\n\n",maximumValue); */

	if ((*cols)<1 || (*rows)<1 || maximumValue<0 || maximumValue>MAXVALUE) {
		printf("ERROR: invalid file specifications (cols/rows/max value)\n\n");
		filePointer.close();
		return (0);
	}
	else if ((*cols) > MAXCOLS || (*rows) > MAXROWS) {
		printf("ERROR: increase MAXROWS/MAXCOLS in PGM.h");
		filePointer.close();
		return (0);
	}

	/* Read in the data for binary (P5) or ascii (P2) PGM formats   */
	if (binary) {
		for (i = 0; i < (*rows); i++) {
			for (j = 0; j < (*cols); j++) {
				filePointer >> image[i][j];
				if (filePointer.eof()) break;
			}
			if (filePointer.eof()) break;
		}
	}
	else {
		for (i = 0; i < (*rows); i++) {
			for (j = 0; j < (*cols); j++) {
				filePointer >> image[i][j];
				if (filePointer.eof()) break;
			}
			if (filePointer.eof()) break;
		}
	}

	/*    printf ("  Rows * Columns = %ld\n",(*rows)*(*cols)); */
	/*       printf ("  Number of pixels read = %ld\n\n",numberRead); */

	/* close the file and return 1 indicating success */
	filePointer.close();
	return (1);
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

int main()
{
	long rows, cols;
	int debutcarre1, debutcarre2, taillecarres;
	char nomfich[20];
	unsigned char image[MAXROWS][MAXCOLS];
	//PPMImage *image;
	cout << "Nom du fichier :";
	cin >> nomfich;
	pgmRead(nomfich, &rows, &cols, image);
	//image = readPPM(nomfich);
	/*
	patchworkPGM(image, rows, cols, debutcarre1, debutcarre2, taillecarres);
	cout << "debut carre 1 :\t" << debutcarre1 << endl;
	cout << "debut carre 2 :\t" << debutcarre2 << endl;
	cout << "taille des carres :\t" << taillecarres << endl;
	*/
	dctPGM(image, rows, cols);
	pgmWrite("lenares.pgm", rows, cols, image, "format pgm");
	//writePPM("samarch.ppm", image);
	system("pause");



}