#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>

#define no_value ""


int check_space(char* sir)
{
	char caracter;
	memcpy(&caracter, &sir[2], sizeof(char));
	char spatiu = ' \0';
	
	if (caracter == spatiu) return 1;
	else return 0;
}

int check_macro_defined(char* sir)
{
	int counter = 0;
	char* token;
	char s[] = "=";
	/* get the first token */
	token = strtok(sir, s);

	/* walk through other tokens */
	while (token != NULL) {
		counter++;

		token = strtok(NULL, s);
	}

	if (counter == 1) return 1;
	else return 0;
}

char* get_macro_name_with_space_before(char* sir)
{
	if (check_macro_defined(sir) == 1) // daca macro-ul a fost definit
	{
		char* token;
		char s[] = "=";
		/* get the first token */
		token = strtok(sir, s);

		return token;
	}
	else return sir;
}

char* get_macro_name_without_space_before(char* sir)
{
	char* token;

	if (check_macro_defined(sir) == 1) // daca macro-ul a fost definit
	{
		char s[] = "-D=";

		/* get the first token */
		token = strtok(sir, s);

		return token;
	}
	else
	{
		char s[] = "-D";

		/* get the first token */
		token = strtok(sir, s);
	}
	return token;
}

char* get_value(char* sir)
{
	const char s[] = "[=]";
	char* token;
	int counter = 0;
	/* get the first token */
	token = strtok(sir, s);

	/* walk through other tokens */
	while (token != NULL)
	{
		counter++;

		if (counter == 2) return token;
		token = strtok(NULL, s);
	}

	if (counter == 1) // daca nu am valoare
	{
		char* str_no_value = (char*)malloc(strlen(no_value) * sizeof(char));
		strcpy(str_no_value, no_value);
		return str_no_value;
	}
}

char* get_directory_name(char* sir) // pt cazul in care nu am spatiu intre -Inume_dir
{
	char* token;

	char s[] = "-I";

	/* get the first token */
	token = strtok(sir, s);

	return token;

}

int check_infile(char* sir)
{
	if (strcmp(sir, "-o") != 0) return 1;
	else return 0;
}

int check_o_option(char* sir)
{
	if (check_infile(sir) == 0) // folosesc functia infile ca sa verific existensa sirului "-o", astfel nu mai dublez secvente de cod
		return 1;
	else return 0;
}

void find_outfile(char* argv[], int index, int necesita_increm, char* nume_outfile, int total_argumente_pt_argv, const char* message)
{
	if(necesita_increm == 1) index++;

	if (check_o_option(argv[index])) // -o e prezent
	{
		index++;
		nume_outfile = argv[index];
		// -D macro[=valoare] -I dir infile -o outfile
		strcpy((char*)message, " -o outfile");

		if (total_argumente_pt_argv == index) fprintf(stdout, "Am ajuns in cazul %s.\n", message);
		else fprintf(stdout, "Ceva e in neregula. Sunt in cazul %s.\n", message);
	}
	else
	{
		if (total_argumente_pt_argv == index)
		{
			nume_outfile = argv[index]; // daca outfile exista
			strcpy((char*)message, " nume_outfile");
			fprintf(stdout, "Am ajuns in cazul %s.\n", message);
		}
		else
		{
			strcpy(nume_outfile, no_value); // daca nu exista, semnalez cumva
			fprintf(stdout, "Am ajuns in cazul %s.\n", message);
		}
	}
}

void find_infile_and_outfile(char* argv[], int index, int continua, char* nume_infile, char* nume_outfile, int total_argumente_pt_argv, const char* message)
{
	//if(continua==1) => necesita incremenare
	index++;

	if (check_infile(argv[index])) // infile exista
	{
		nume_infile = argv[index];
		strcpy((char*)message, " infile");
		find_outfile(argv, index, continua, nume_outfile, total_argumente_pt_argv, message);
	}
	else // infile nu exista, ceea ce inseamna ca input-ul va fi primit de la stdin
	{
		strcpy(nume_infile, no_value); // voi citi de la stdin

		continua = 0; // resetare

		if (total_argumente_pt_argv == index) continua = 0;
		else continua = 1;

		if (continua == 1)
		{
			find_outfile(argv, index, continua, nume_outfile, total_argumente_pt_argv, message);
		}
		else // daca nu mai am argumente
		{
			strcpy(nume_outfile, no_value);
			fprintf(stdout, "Am ajuns in cazul %s.\n", message);
		}
	}
}

void check_for_other_options(char* argv[], int index, int continua, char* nume_infile, char* nume_outfile, int total_argumente_pt_argv, const char* message)
{
	if (total_argumente_pt_argv == index) continua = 0; // verific daca mai e ceva sau restul optiunilor lipsesc
	else continua = 1;

	if (continua == 1)
	{
		find_infile_and_outfile(argv, index, continua, nume_infile, nume_outfile, total_argumente_pt_argv, message);
	}
	else
	{
		strcpy(nume_infile, no_value); // nu exista infile; va fi citit de la stdin
		strcpy(nume_outfile, no_value); // nu exista outfile; va fi afisat la consola
		fprintf(stdout, "Am ajuns in cazul %s.\n", message);
	}

}

void get_directory(char* argv[], int index, int continua, char* directory_name, char* nume_infile, char* nume_outfile, int total_argumente_pt_argv, const char* message)
{
	index++;
	if (check_space(argv[index])) // -I dir sau -Idir
	{
		index++;
		directory_name = argv[index]; // -I dir

		strcpy((char*)message, " -I dir");
		check_for_other_options(argv, index, continua, nume_infile, nume_outfile, total_argumente_pt_argv, message);
	}
	else // -Idir
	{
		directory_name = get_directory_name(argv[index]);

		strcpy((char*)message, " -Idir");
		check_for_other_options(argv, index, continua, nume_infile, nume_outfile, total_argumente_pt_argv, message);
	}

}

void get_all_details(char* argv[], int index, int continua, char* denumire_macro, char* valoare_macro, char* directory_name, char* nume_infile, char* nume_outfile, int total_argumente_pt_argv)
{
	if (check_space(argv[index]) == 1) // -D macro[=valoare]
	{
		index++; // x = 2
		denumire_macro = get_macro_name_with_space_before(argv[index]);
		valoare_macro = get_value(argv[index]);

		get_directory(argv, index, continua, directory_name, nume_infile, nume_outfile, total_argumente_pt_argv, "-D macro[=valoare]");
	}
	else // -Dmacro[=valoare]
	{
		denumire_macro = get_macro_name_with_space_before(argv[index]);
		valoare_macro = get_value(argv[index]);

		get_directory(argv, index, continua, directory_name, nume_infile, nume_outfile, total_argumente_pt_argv, "-Dmacro[=valoare]");
	}

}

#define max_size 200

int check_define(char* sir)
{
	if (strcmp(sir, "#define") == 0) return 1;
	else return 0;
}

int check_include(char* sir)
{
	if (strcmp(sir, "#include") == 0) return 1;
	else return 0;
}

int check_if(char* sir)
{
	if (strcmp(sir, "#if") == 0) return 1;
	else return 0;
}

int check_elif(char* sir)
{
	if (strcmp(sir, "#elif") == 0) return 1;
	else return 0;
}

int check_endif(char* sir)
{
	if (strcmp(sir, "#endif") == 0) return 1;
	else return 0;
}

int check_ifdef(char* sir)
{
	if (strcmp(sir, "#ifdef") == 0) return 1;
	else return 0;
}

int check_ifndef(char* sir)
{
	if (strcmp(sir, "#ifndef") == 0) return 1;
	else return 0;
}

void dezlipire(char* sir)
{
	const char s[] = " \t\n";
	char* token;

	/* get the first token */
	token = strtok(sir, s);

	/* walk through other tokens */
	while (token != NULL)
	{
		if (check_define(token))
		{
			//urmeaza ce e dupa define
		}
		else if (check_include(token))
		{
			// iau continutul fisierului de dupa #include si il afisez
		}
		else if (check_if(token))
		{
			// fac ce urmeaza dupa #if
		}
		else if (check_elif(token))
		{
			// fac ce urmeaza dupa #elif
		}
		else if (check_endif(token))
		{
			// s-a terminat partea de #if
		}
		else if (check_ifdef(token))
		{
			// fac ce urmeaza dupa #ifdef
		}
		else if (check_ifndef(token))
		{
			// fac ce urmeaza pt #ifndef
		}


		token = strtok(NULL, s);
	}
}

void executing(char* denumire_macro, char* valoare_macro, char* directory_name, char* nume_infile, char* nume_outfile)
{
	int define_found = 0, if_foud = 0, elif_found = 0, else_found = 0, ifdef_found = 0, ifndef_found = 0, endif_found = 0, include_found = 0;
	char buf[max_size];

	if (strcmp(nume_infile, no_value)) // citesc de la stdin
	{
		if (strcmp(nume_outfile, no_value) == 0) // afisez la stdout
		{
			fgets(buf, max_size, stdin);

		}
	}
}

/* structura: so-cpp [-D <SYMBOL>[=<MAPPING>]] [-I <DIR>] [<INFILE>] [ [-o] <OUTFILE>] */
/* so-cpp -D macro[=valoare] -I dir infile -o outfile*/
// testul 6 -> -o nu apare
int main(int argc, char* argv[])
{
	char* denumire_macro = NULL, * valoare_macro = NULL, * directory_name = NULL, * nume_infile = NULL, * nume_outfile = NULL;
	int x = 1; // ca sa tin evidenta indexului lui argv
	int total_argumente_pt_argv = argc - 1; // argv[1] - argv[argc-1]
	int continua = 0;
	/* argc = 8 maxim
	   argc = 4 minim*/
	// argv[0] e numele programului

	if (argc == 0) fprintf(stdout, "No arguments. Exiting....");
	else
	{
		get_all_details(argv, x, continua, denumire_macro, valoare_macro, directory_name, nume_infile, nume_outfile, total_argumente_pt_argv);
		
		executing(denumire_macro, valoare_macro, directory_name, nume_infile, nume_outfile);
	}
}