#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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
		char no_value[] = "";
		return no_value;
	}
}

void testare()
{
	char ex1[] = "-Dana";
	char ex2[] = "-D ana";
	char ex3[] = "ana=1";
	char ex4[] = "ana";
	char ex5[] = "-Dana";
	char ex6[] = "-Dana=1";

	if (check_space(ex1) == 0) fprintf(stdout, "Nu e spatiu\n");
	else fprintf(stdout, "E spatiu\n");

	if (check_space(ex2) == 0) fprintf(stdout, "Nu e spatiu\n");
	else fprintf(stdout, "E spatiu\n");

	if (check_macro_defined(ex3) == 1) fprintf(stdout, "Macro definit\n");
	else fprintf(stdout, "Macro nedefinit\n");

	if (check_macro_defined(ex4) == 1) fprintf(stdout, "Macro definit\n");
	else fprintf(stdout, "Macro nedefinit\n");

	char* nume_var1 = NULL;
	nume_var1 = get_macro_name_with_space_before(ex3);
	char* nume_var2 = get_macro_name_with_space_before(ex4);
	fprintf(stdout, "Nume variabila: %s\n", nume_var1);
	fprintf(stdout, "Nume variabila: %s\n", nume_var2);

	char* nume_var3 = get_macro_name_without_space_before(ex5);
	char* nume_var4 = get_macro_name_without_space_before(ex6);
	fprintf(stdout, "Nume variabila: %s\n", nume_var3);
	fprintf(stdout, "Nume variabila: %s\n", nume_var4);

	char* nume_var5 = get_value(ex5);
	char* nume_var6 = get_value(ex6);
	fprintf(stdout, "Valoare variabila: %s\n", nume_var5);
	fprintf(stdout, "Valoare variabila: %s\n", nume_var6);

}

//voi afisa fisierul preprocesat la stdout
/* structura: so-cpp [-D <SYMBOL>[=<MAPPING>]] [-I <DIR>] [<INFILE>] [ [-o] <OUTFILE>] */
/* so-cpp -D macro[=valoare] -I dir infile -o outfile*/
int main(int argc, char* argv[])
{
	testare();
	//char* denumire_macro = NULL;
	///* argc = 7 (maxin)
	//   argc = 4 minim*/
	//if (argc == 1) fprintf(stdout, "No arguments. Exiting....");
	//else
	//{
	//	// -D macro[=valoare]
	//	if (check_space(argv[2]) == 1) // daca am spatiu intre -D si numele variabilei
	//	{
	//		denumire_macro = get_macro_name_with_space_before(argv[3]);
	//	}
	//	else denumire_macro = get_macro_name_without_space_before(argv[2]);

	//}
}