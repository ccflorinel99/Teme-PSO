Explicatii functii folosite:
check_space verifica daca am spatiu intre optiune (-D, -I) si obiectul declarat
check_macro_defined verifica daca pt un macro am o valoare declarata
get_macro_name_with_space_before ia numele macro-ului definit cu spatiu intre -D si numele lui
get_macro_name_without_space_before iau numele macro-ului definit fara spatiu intre -D si numele lui
get_value ia valoarea macro-ului
check_existing_file verifica daca exista fisierul (full path)
get_directory_name ia numele directorului in care se afla fisierul de intrare
check_infile verifica daca exista un fisier de intrare
check_o_option verifica daca optiunea -o e pusa
find_outfile cauta daca am un fisier de output
find_infile_and_outfile verifica daca exista fisierele de input si output
check_for_other_options verifica daca, pe langa -D si -I, exista si fisierele de input si output
get_directory ia directory-ul si cauta alte optiuni
get_another_macro_or_directory ia un alt macro sau director
get_all_details ia toate detaliile ce tine de forma fisierului so-cpp data in enunt
check_define verifica daca am intalnit un #define
check_include verifica daca am intalnit un #include
check_if verifica daca am intalnit un #if
check_elif verifica daca am intalnit un #elif
check_endif verifica daca am intalnit un #endif
check_ifdef verifica daca am intalnit un #ifdef
check_ifndef verifica daca am intalnit un #ifndef
check_if_structure verifica daca am intalnit un o structura #if (de orice tip) si ca s-a folosit #endif pt incheiere
scriere_stdout_pt_include scrie la stdout pt #include
scriere_in_fisier_pt_include scrie in fisier pt #include
read_from_file_for_include citeste din fisier pt #include
define_found pune perechea (nume, valoare) a macro-ului in vectorul corespunzator
ifdef_found verific daca un macro a fost definit
ifndef_found verific daca un macro nu a fost definit
realocare ma ajuta sa realoc vectorul de pointeri
create_statement e folosit pentru a construi blocurile #if
check_statement verific starea de adevar a expresiei
if_elif_statement verific starea de adevar a expresiei primite de #if si #elif
initializare_elemente_necesare_rularii iau toate elementele necesare rularii (blocurile #if, macro-uri, etc)
scriere_stdout_pt_reasamblare scrie toate informatiile aflate recent la stdout
scriere_in_fisier_pt_reasamblare scrie toate informatiile aflate recent in fisierul de output
reasamblare reface blocurile "destramate"
dezlipire "destrama" blocurile
executing face dezlipirea + reasamblarea dupa ce le citeste de la stdin sau din fisier


Link github: https://github.com/ccflorinel99/Teme-PSO