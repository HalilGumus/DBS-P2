#include <iostream>
#include <string>
#include <fstream>
#include "db.h"
// #include <stdio.h>
#include <string.h>
#include <vector>

// return codes
#define RC_OK 0
#define RC_ERROR -1

using namespace std;

int main(int argc, char *argv[])
{

    string user;
    string password;
    string host;
    string port;
    string dbname;
    // char *line;
    string test;

    if (argc == 12)
    {
        for (int i = 1; i < argc - 1; i += 2)
        {

            if (argv[i][0] == '-')
            {
                if (argv[i][1] == 'u')
                {
                    user = argv[i + 1];
                }
                if (argv[i][1] == 'c')
                {
                    password = argv[i + 1];
                }
                if (argv[i][1] == 'h')
                {
                    host = argv[i + 1];
                }
                if (argv[i][1] == 'p')
                {
                    port = argv[i + 1];
                }
                if (argv[i][1] == 'd')
                {
                    dbname = argv[i + 1];
                }
            }
        }

        // Datei einlesen
        ifstream commandfile(argv[11]);

        if (!commandfile.is_open())
        {
            perror("Error opening file");
            return RC_ERROR;
        }
        else
        {

            if (db_login(user, password, host, port, dbname) >= RC_OK)
            {
                bool rollback = false;
                db_begin();
                // while (fgets(mystring, 100, pFile) != NULL)
                while (getline(commandfile, test))
                {
                    vector<string> args;
                    // string to char*
                    char *buffer = new char[test.length() + 1];
                    strcpy(buffer, test.c_str());

                    char *arg = strtok(buffer, " ");
                    while (arg != nullptr)
                    {
                        args.push_back(string(arg));
                        arg = strtok(nullptr, " ");
                    }
                    string lastArg = args.back();
                    if (lastArg[lastArg.size() - 1] == '\r')
                    {
                        lastArg = lastArg.substr(0, lastArg.size() - 1);
                    }
                    args.pop_back(); // removes \r
                    args.push_back(lastArg);

                    if (args[0] == "n")
                    {
                        if (db_drop_table("produkt") != 0)
                            rollback = true;
                        if (db_drop_table("hersteller") != 0)
                            rollback = true;
                        if (db_create_table_hersteller() != 0)
                            rollback = true;
                        if (db_create_table_produkt() != 0)
                            rollback = true;
                    }
                    else if (args[0] == "ih")
                    {
                        cout << "command: " << args[0] << endl;
                        // cout << "hnr: " << args[1] << endl;
                        // cout << "name: " << args[2] << endl;
                        // cout << "stadt: " << args[3] << endl;
                        if (int res = db_check_hnr(args[1]) == 0)
                        {
                            cout << res << "!!!!!!!!!!" << endl;
                            db_insert_hersteller(args[1], args[2], args[3]);
                        }
                        else if (db_check_hnr(args[0]) == 1)
                        {
                            db_update_hersteller(args[1], args[2], args[3]);
                        }
                        else
                        {
                            cout << "Failed to insert hersteller: " << args[1] << endl;
                        }
                    }
                    else if (args[0] == "ip")
                    {
                        cout << "command: " << args[0] << endl;
                        // cout << "pnr: " << args[1] << endl;
                        // cout << "name: " << args[2] << endl;
                        // cout << "preis: " << args[3] << endl;
                        // cout << "hnr: " << args[4] << endl;
                        if (db_check_pnr(args[1]) == 0)
                        {
                            db_insert_produkt(args[1], args[2], args[3], args[4]);
                        }
                        else if (db_check_pnr(args[0]) == 1)
                        {
                            db_update_produkt(args[1], args[2], args[3], args[4]);
                        }
                        else
                        {
                            cout << "Failed to insert produkt: " << args[1] << endl;
                        }
                    }
                    delete[] buffer;
                    // cout << args[0] << endl;
                }

                // TODO: rollback logik wann und wo der rollbacken soll
                if (rollback)
                {
                    cout << rollback << " DB-Errors." << endl;
                    db_rollback();
                }
                else
                {
                    db_commit();
                }
                db_logout();
            }
            // commandfile.close();

            else
            {
                cout << "error" << endl;
                return RC_ERROR;
            }
        }
    }
    else
    {
        // wenn nicht genug Argumente übergeben wurden bricht das Programm ab
        return RC_ERROR;
    }

    return RC_OK;
}
