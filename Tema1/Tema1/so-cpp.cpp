#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define macro_no_value ""


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
	const char s[] = "=";
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
		char* no_value = (char*)malloc(strlen(macro_no_value) * sizeof(char));
		strcpy(no_value, macro_no_value);
		return no_value;
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

void testare()
{
	char ex7[] = "-Inume_dir";

	char* nume_var7 = get_directory_name(ex7);
	fprintf(stdout, "Nume director: %s\n", nume_var7);


}

//voi afisa fisierul preprocesat la stdout
/* structura: so-cpp [-D <SYMBOL>[=<MAPPING>]] [-I <DIR>] [<INFILE>] [ [-o] <OUTFILE>] */
/* so-cpp -D macro[=valoare] -I dir infile -o outfile*/
int main(int argc, char* argv[])
{
	testare();
	char* denumire_macro = NULL, * valoare_macro = NULL, * directory_name = NULL;
	/* argc = 7 maxim
	   argc = 4 minim*/
	if (argc == 1) fprintf(stdout, "No arguments. Exiting....");
	else
	{
		// -D macro[=valoare]
		if (check_space(argv[2]) == 1) // daca am spatiu intre -D si numele variabilei
		{
			denumire_macro = get_macro_name_with_space_before(argv[3]);
			valoare_macro = get_value(argv[3]);
			// -I dir
			if (check_space(argv[4])) directory_name = argv[5];
			else directory_name = get_directory_name(argv[4]);
		}
		else
		{
			denumire_macro = get_macro_name_without_space_before(argv[2]);
			valoare_macro = get_value(argv[2]);
			// -I dir
			if (check_space(argv[3])) directory_name = argv[4];
			else directory_name = get_directory_name(argv[3]);
		}

		
	}
}