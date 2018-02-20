#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <ios>



int readPGM(string Nfile,long &rows,long &cols,unsigned char **image)
{
	ifstream f(Nfile.c_str(),std::ios_base::binary);
	char c;
	

	string ligne;
	if(f.eof())
	{
		cout<<"fichier vide";
		return 0;
	}
	f>>c;

	while (c=='#')
	{
		//cout<<"Commentaire !"<<endl;
		getline(f,ligne);
		f>>c;
	}
	
	if(c!='P')
	{
		cout<<"Format incorrect !"<<endl;
		return 0;
	}
	else
	{
		f>>c;
		if(c!='2' && c!='5')
		{
			cout<<"Format incorrect!!"<<endl;
			return 0;
		}
	}
	
	//cout<<"OK jusque là"<<endl;
	f>>rows>>cols;
	int nbniveauxgris;
	f>>nbniveauxgris;
//	cout<<rows<<" rows et "<<cols<<" cols"<<endl;
	f>>ws;
	
	long indice=f.tellg();
	int compt=0;
	f.seekg(indice);
	f>>image[0][0]>>ws;
	
	indice++;
	
	for(long i=0;i<rows;i++)
		{
			for(long j=0;j<cols;j++)
			{
				
				f.seekg(indice);
				f>>image[i][j]>>ws;
				indice++;
				//compt++;
//				if(f.eof()){cout<<i<<" "<<j<<endl;
//								cout<<compt<<endl;
//								system("pause");
//				}
			}

		}
		
		//cout<<"ON A TOUT ! "<<endl;
		return 1;
		
}


int pgmWrite(string filename, long rows,long cols,unsigned char **image,int nuances) 
{
    ofstream f(filename.c_str(), ios_base::binary);
    string magic="P5";
    f << magic<<endl;
    f << rows << endl << cols << endl << nuances<< endl;


    for (int i=0; i<rows; ++i)
        for (int j=0; j<cols; j++)
	{
            f << image[i][j];
	}


    f.close();

}