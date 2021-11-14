#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
//#include <unistd.h> // biblioteca pt unix care ma ajuta sa verific daca un fisier exista
#include <io.h> // biblioteca pt windows care ma ajuta sa verific daca un fisier exista


#define no_value ""
#define max_size 200


int check_space(char* sir)
{
	char caracter;
	memcpy(&caracter, &sir[2], sizeof(char));
	char spatiu = ' ';
	
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

int check_existing_file(char* complete_file_name)
{
	if (access(complete_file_name, ENOENT) == 0) // ENOENT e folosit pt File name or path not found
		return 0;
	else return 1;
	// == 0 inseamna ca expresia e adevarata, deci fisierul nu exista
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

int get_another_macro_or_directory(char* argv[], int index, char** pereche_macro_valoare, char option)
{
	if (argv[index][1] == option && option == 'D') // daca am optiunea respectiva
	{
		char* noua_pereche_macro_valoare = NULL;
		char* denumire_macro = NULL;
		char* valoare_macro = NULL;
		
		if (check_space(argv[index]) == 1) // -D macro[=valoare]
		{
			index++;
			denumire_macro = get_macro_name_with_space_before(argv[index]);
			valoare_macro = get_value(argv[index]);
			noua_pereche_macro_valoare = (char*)malloc((strlen(denumire_macro) + strlen(valoare_macro) + 2) * sizeof(char));
			strcpy(noua_pereche_macro_valoare, denumire_macro);
			strcpy(noua_pereche_macro_valoare, valoare_macro);
			strcpy(*pereche_macro_valoare, noua_pereche_macro_valoare);
		}
		else // -Dmacro[=valoare]
		{
			denumire_macro = get_macro_name_with_space_before(argv[index]);
			valoare_macro = get_value(argv[index]);
		}

		return 1;
	}
	else if (argv[index][1] == option && option == 'I')
	{
		index++;
		char* directory_name = NULL;

		if (check_space(argv[index])) // -I dir sau -Idir
		{
			index++;
			directory_name = argv[index]; // -I dir

			
		}
		else // -Idir
		{
			directory_name = get_directory_name(argv[index]);
		}

		return 1;
	}

	return 0;
}

void get_all_details(char* argv[], int index, int continua, char** pereche_macro_valoare, char* directory_name, char* nume_infile, char* nume_outfile, int total_argumente_pt_argv)
{
	char* noua_pereche_macro_valoare = NULL;
	char* denumire_macro = NULL;
	char* valoare_macro = NULL;

	if (check_space(argv[index]) == 1) // -D macro[=valoare]
	{
		index++;
		denumire_macro = get_macro_name_with_space_before(argv[index]);
		valoare_macro = get_value(argv[index]);
		noua_pereche_macro_valoare = (char*)malloc((strlen(denumire_macro) + strlen(valoare_macro) + 2) * sizeof(char));
		strcpy(noua_pereche_macro_valoare, denumire_macro);
		strcpy(noua_pereche_macro_valoare, valoare_macro);
		strcpy(*pereche_macro_valoare, noua_pereche_macro_valoare);

		get_directory(argv, index, continua, directory_name, nume_infile, nume_outfile, total_argumente_pt_argv, "-D macro[=valoare]");
	}
	else // -Dmacro[=valoare]
	{
		denumire_macro = get_macro_name_with_space_before(argv[index]);
		valoare_macro = get_value(argv[index]);
		noua_pereche_macro_valoare = (char*)malloc((strlen(denumire_macro) + strlen(valoare_macro) + 2) * sizeof(char));
		strcpy(noua_pereche_macro_valoare, denumire_macro);
		strcpy(noua_pereche_macro_valoare, valoare_macro);
		strcpy(*pereche_macro_valoare, noua_pereche_macro_valoare);

		get_directory(argv, index, continua, directory_name, nume_infile, nume_outfile, total_argumente_pt_argv, "-Dmacro[=valoare]");
	}
}

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

void check_if_structure(int any_if_found)
{
	if (any_if_found == 1) fprintf(stdout, "Structura #if buna\n"); // all ok
	else fprintf(stdout, "Structura #if nu e buna\n");
}

void scriere_stdout_pt_include(char* sir, char* directory_name, char* buf)
{
	char* filename = (char*)malloc((strlen(directory_name) + strlen(sir) + 2) * sizeof(char));
	strcpy(filename, directory_name);
	strcpy(filename, sir);

	FILE* f = fopen(filename, "r");
	fseek(stdout, 0, SEEK_END);

	while (!feof(f))
	{
		fgets(buf, max_size, f);
		fputs(buf, stdout);
	}

	fprintf(stdout, "\n");
	fclose(f);
}

void scriere_in_fisier_pt_include(char* sir, char* directory_name, char* buf, char* outfile)
{
	char* filename = (char*)malloc((strlen(directory_name) + strlen(sir) + 2) * sizeof(char));
	strcpy(filename, directory_name);
	strcpy(filename, sir);

	FILE* f = fopen(filename, "r");
	FILE* g = fopen(outfile, "a");
	fseek(g, 0, SEEK_END);

	while (!feof(f))
	{
		fgets(buf, max_size, f);
		fputs(buf, g);
	}

	fprintf(g, "\n");
	fclose(f);
	fclose(g);
}

void read_from_file_for_include(char* sir, char* outfile, char* directory_name)
{
	// #include <>
	// #include ""

	const char s[] = "<>""";
	char* token;
	char buf[max_size];
	token = strtok(sir, s);

	if (strcmp(outfile, no_value) == 0) scriere_stdout_pt_include(token, directory_name, buf);
	else scriere_in_fisier_pt_include(token, directory_name, buf, outfile);
}

char* define_found(char* token, char* s)
{
	token = strtok(NULL, s);
	char* macro = token;
	token = strtok(NULL, s);
	char* valoare = token;

	char* pereche_macro_valoare = (char*)malloc((strlen(macro) + strlen(valoare) + 2) * sizeof(char));
	strcpy(pereche_macro_valoare, macro);
	strcpy(pereche_macro_valoare, valoare);

	return pereche_macro_valoare;
}

int ifdef_found(char* nume_macro_de_verificat, char** pereche_macro_valoare, int nr_macros)
{
	if (pereche_macro_valoare == NULL || nume_macro_de_verificat == NULL) return 0;

	for (int i = 0; i < nr_macros; i+=2)
	{
		if (strcmp(nume_macro_de_verificat, pereche_macro_valoare[i]) == 0) return 1;
	}
	return 0;
}

int ifndef_found(char* nume_macro_de_verificat, char** pereche_macro_valoare, int nr_macros)
{
	if (pereche_macro_valoare == NULL || nume_macro_de_verificat == NULL) return 0;

	for (int i = 0; i < nr_macros; i += 2)
	{
		if (strcmp(nume_macro_de_verificat, pereche_macro_valoare[i]) == 0) return 0;
	}
	
	return 1;
}

void realocare(char** item, int old_size, int new_size)
{
	item = (char**)realloc(item, new_size * sizeof(char*));

	for (int i = old_size; i < new_size; i++)
	{
		item[i] = (char*)realloc(item[i], max_size * sizeof(char));
	}
}

char** create_statement(char* sir, char* delim)
{
	char** statement = (char**)malloc(sizeof(char*));
	statement[0] = (char*)malloc(max_size * sizeof(char));
	int i = 1;

	while (strcmp(sir, "else") != 0 || strcmp(sir, "elif") != 0 || strcmp(sir, "endif") != 0)
	{
		char* single_thing = sir;
		strcpy(statement[i - 1], single_thing);
		i++;
		realocare(statement, i - 1, i);
	}
	
	return statement;
}

int check_statement(char** statement)
{
	const char* semne[] = { "==","<=",">=","<",">" };


	char* prima = NULL, * a_doua = NULL, * semn = NULL;
	/*
	if a semn b
	*/
	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0: // a (prima bucata de dupa if)
		{
			prima = statement[i];
			break;
		}
		case 1: // semnul
		{
			semn = statement[i];
			break;
		}
		case 2: // b (bucata de dupa semn)
		{
			a_doua = statement[i];
			break;
		}
		default:
			break;
		}
	}


	/*
	i = 0 -> ==
	i = 1 -> <=
	i = 2 -> >=
	i = 3 -> <
	i = 4 -> >
	*/
	for (int i = 0; i < 5; i++)
	{
		switch (i)
		{
		case 0: // ==
		{
			if (strcmp(prima, a_doua) == 0) return 1;
			else return 0;
			break;
		}
		case 1: // <=
		{
			if (strcmp(prima, a_doua) <= 0) return 1;
			else return 0;
			break;
		}
		case 2: // >=
		{
			if (strcmp(prima, a_doua) >= 0) return 1;
			else return 0;
			break;
		}
		case 3: // <
		{
			if (strcmp(prima, a_doua) < 0) return 1;
			else return 0;
			break;
		}
		case 4: // >
		{
			if (strcmp(prima, a_doua) > 0) return 1;
			else return 0;
			break;
		}
		default:
			break;
		}
	}
}

int if_elif_statement(char* token, char* s)
{
	token = strtok(NULL, s);

	char** statement = create_statement(token, s);

	if (check_statement(statement) == 1) return 1;
	else return 0;
}

void initializare_elemente_necesare_rularii(char* token, char* s, char* directory_name, char* outfile, char** pereche_macro_valoare, int* nr_perechi, int* nr_total)
{
	int if_def_found, if_ndef_found, if_elif, any_type_if_found = 0;

	while (token != NULL)
	{
		if (check_define(token))
		{
			token = strtok(NULL, s);
			char* macro = token;
			token = strtok(NULL, s);
			char* valoare = token;

			if (*nr_perechi >= 1)
			{
				pereche_macro_valoare = (char**)realloc(pereche_macro_valoare, *nr_perechi * sizeof(char*));
				pereche_macro_valoare[*nr_perechi - 1] = (char*)malloc(max_size * sizeof(char));
			}
			/*
			[0] = nume macro
			[1] = valoare macro
			*/
			pereche_macro_valoare[*nr_perechi - 1] = define_found(token, (char*)s);
			*nr_perechi++;
			*nr_total = *nr_perechi * 2;
		}
		else if (check_include(token))
		{
			token = strtok(NULL, s);
			read_from_file_for_include(token, outfile, directory_name);
		}
		else if (check_if(token))
		{
			any_type_if_found = 1;
			if_elif = if_elif_statement(token, (char*)s);
		}
		else if (check_elif(token))
		{
			if (if_elif == 0) if_elif = if_elif_statement(token, (char*)s);
		}
		else if (check_endif(token))
		{
			check_if_structure(any_type_if_found);
		}
		else if (check_ifdef(token))
		{
			any_type_if_found = 1;
			token = strtok(NULL, s);

			if_def_found = ifdef_found(token, pereche_macro_valoare, *nr_total);
		}
		else if (check_ifndef(token))
		{
			any_type_if_found = 1;
			token = strtok(NULL, s);

			if_ndef_found = ifndef_found(token, pereche_macro_valoare, *nr_total);
		}

		token = strtok(NULL, s);
	}
}

void scriere_stdout_pt_reasamblare(char* buf)
{
	fseek(stdout, 0, SEEK_END);
	fputs(buf, stdout);
	fprintf(stdout, "\n");
}

void scriere_in_fisier_pt_reasamblare(char* buf, char* outfile)
{
	FILE* f = fopen(outfile, "a");
	fseek(f, 0, SEEK_END);
	fputs(buf, f);

	fprintf(f, "\n");
	fclose(f);
}

void reasamblare(char* sir, char* s, char* outfile, char** pereche_macro_valoare, int* nr_perechi)
{
	char* token;
	char buf[max_size];
	int gasit = 0;
	
	/* get the first token */
	token = strtok(sir, s);

	/* walk through other tokens */
	while (token != NULL)
	{
		for (int i = 0; i < *nr_perechi; i+=2)
		{
			if (strcmp(token, pereche_macro_valoare[i]) == 0)
			{
				strcpy(buf, pereche_macro_valoare[i + 1]);
				gasit = 1;
			}
		}

		if (gasit == 1) strcpy(buf, " ");

		strcpy(buf, token);

		token = strtok(NULL, s);
	}

	if (strcmp(outfile, no_value) == 0) scriere_stdout_pt_reasamblare(buf);
	else scriere_in_fisier_pt_reasamblare(buf, outfile);
}

void dezlipire(char* sir, char* directory_name, char* infile, char* outfile, char** pereche_macro_valoare, int* nr_perechi, int* nr_total)
{
	// posibil sa am directive de define sau include in structurile conditionale, asa ca pt ele resetez flag-urile pt include si define
	const char s[] = " \t\n";
	char* token;
	*nr_perechi = 1;
	*nr_total = *nr_perechi * 2;
	int if_def_found, if_ndef_found, if_elif, any_type_if_found = 0;


	/* get the first token */
	token = strtok(sir, s);
	if (token[0] == '#')
		initializare_elemente_necesare_rularii(token, (char*)s, directory_name, outfile, pereche_macro_valoare, nr_perechi, nr_total);
	else reasamblare(sir, (char*)s, outfile, pereche_macro_valoare, nr_perechi);
	
}

void executing(char* denumire_macro, char* valoare_macro, char* directory_name, char* nume_infile, char* nume_outfile)
{
	int define_found = 0, if_found = 0, elif_found = 0, else_found = 0, ifdef_found = 0, ifndef_found = 0, endif_found = 0;
	char buf[max_size];
	char* pereche_macro_valoare = NULL;
	int nr_perechi, nr_total;

	if (strcmp(nume_infile, no_value)) // citesc de la stdin
	{
		while (!feof(stdin))
		{
			fgets(buf, max_size, stdin);
			dezlipire(buf, directory_name, nume_infile, nume_outfile, &pereche_macro_valoare, &nr_perechi, &nr_total);
		}
	}
	else
	{
		char* filename = (char*)malloc((strlen(directory_name) + strlen(nume_infile) + 2) * sizeof(char));
		strcpy(filename, directory_name);
		strcpy(filename, nume_infile);

		FILE* f = fopen(filename, "r");

		while (!feof(f))
		{
			fgets(buf, max_size, f);
			dezlipire(buf, directory_name, nume_infile, nume_outfile, &pereche_macro_valoare, &nr_perechi, &nr_total);
		}

		fclose(f);
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
	char* pereche_macro_valoare = NULL;
	/* argc = 8 maxim
	   argc = 4 minim
	*/
	// argv[0] e numele programului

	if (argc == 1) fprintf(stdout, "No arguments. Exiting....");
	else
	{
		get_all_details(argv, x, continua, &pereche_macro_valoare, directory_name, nume_infile, nume_outfile, total_argumente_pt_argv);

		executing(denumire_macro, valoare_macro, directory_name, nume_infile, nume_outfile);
	}
}