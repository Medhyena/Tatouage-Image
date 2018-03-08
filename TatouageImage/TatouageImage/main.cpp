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
//#include "readpgm.h"

#define MAXROWS 512
#define MAXCOLS 512
#define MAXLENGTH 256
#define MAXVALUE 255

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
	if (line[0] == 'P' && (line[1] == '2')) {
		binary = 0;
		/*   printf ("\nFile Format: P2\n"); */
	}
	else if (line[0] == 'P' && (line[1] == '5')) {
		binary = 1;
		/*  printf ("\nFORMAT: P5\n"); */
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

	char tmp[MAXROWS][MAXCOLS];
	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			tmp[i][j] = (char)image[i][j];
		}
	}

	/* Read in the data for binary (P5) or ascii (P2) PGM formats   */
	if (binary) {
		for (i = 0; i < (*rows); i++) {
			for (j = 0; j < (*cols); j++) {
				/* pas s�r que �a marche donc je garde l'original en comm
				numberRead += fread((void *)&(image[i][0]),
					sizeof(unsigned char), (*cols), filePointer);*/
				filePointer.read(&(tmp[i][j]), sizeof(unsigned char));
				if (filePointer.eof()) break;
			}
		}
	}
	else {
		for (i = 0; i < (*rows); i++) {
			for (j = 0; j < (*cols); j++) {
				filePointer >> temp;
				if (temp == EOF) break;
				image[i][j] = (unsigned char)temp;
			}
			if (temp == EOF) break;
		}
	}

	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			image[i][j] = (unsigned char)tmp[i][j];
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
	FILE* file;        /* pointer to the file buffer */
	int maxval;        /* maximum value in the image array */
	long nwritten = 0; /* counter for the number of pixels written */
	long i, j;          /* for loop counters */

						/* return 0 if the dimensions are larger than the image array. */
	if (rows > MAXROWS || cols > MAXCOLS) {
		printf("ERROR: row/col specifications larger than image array:\n");
		return (0);
	}

	/* open the file; write header and comments specified by the user. */
	fopen_s(&file, filename, "w");
	fprintf(file, "P5\n");
	/*
	file.open(filename);
	if (file.fail())
	{
		cout << "ERROR: file open failed, incorrect file name" << endl;
		return 0;
	}
	file << "P5\n";*/

	if (comment_string != NULL)
	{
		//file << "# " << comment_string << "%s \n";
		fprintf(file, "# %s \n", comment_string);
	}

	/* write the dimensions of the image */
	//file << cols << " " << rows << endl;
	fprintf(file, "%ld %ld \n", cols, rows);

	/* NOTE: MAXIMUM VALUE IS WHITE; COLOURS ARE SCALED FROM 0 - */
	/* MAXVALUE IN A .PGM FILE. */

	/* WRITE MAXIMUM VALUE TO FILE */
	//file << (int)255 << endl;
	fprintf(file, "%d\n", (int)255);

	/* Write data */

	/* for (i=0; i < rows; i++) {
	Orig: nwritten += fwrite((void*)&(image[i][0]),sizeof(unsigned char), cols, file);}*/
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			//file << &(image[i][j]);
			//file.write(&(image[i][j]), sizeof(unsigned char));
			fwrite(&(image[i][j]), sizeof(unsigned char), 1, file);



	/*     printf ("\nNumber of pixels total(from rows*cols): %ld\n",rows*cols); */
	/*       printf ("Number of pixels written: %ld\n\n",nwritten); */

	//file.close();
	fclose(file);
	return(1);
}

int main()
{

	long rows, cols;
	char nomfich[20];
	unsigned char photo[MAXROWS][MAXCOLS];
	cout << "Nom du fichier :";
	cin >> nomfich;
	int a = pgmRead(nomfich, &rows, &cols, photo);
	int b = pgmWrite("lenares.pgm", rows, cols, photo, "format pgm");


	system("pause");



}
