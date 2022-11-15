#include <iostream>
#include <string>
#include <fstream>
#include "db.h"
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
                if (db_begin() == -1)
                {
                    cout << "Failed to begin" << endl;
                    db_logout();
                    return RC_ERROR;
                }

                // try block
                try
                {
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
                        delete[] buffer;
                        if (args[0] == "n")
                        {
                            if (db_drop_table("produkt") != 0)
                                throw "Dropping produkt failed!";
                            if (db_drop_table("hersteller") != 0)
                                throw "Dropping hersteller failed!";
                            if (db_create_table_hersteller() != 0)
                                throw "Creating table hersteller failed!";
                            if (db_create_table_produkt() != 0)
                                throw "Creating table prodkt failed!";
                        }
                        else if (args[0] == "ih")
                        {
                            cout << "command: " << args[0] << endl;
                            if (db_check_hnr(args[1]) == -1)
                            {
                                throw "Checking hnr " + args[1] + " failed!";
                            }
                            else if (db_check_hnr(args[1]) == 0)
                            {
                                if (db_insert_hersteller(args[1], args[2], args[3]) == -1)
                                    throw "Inserting into hersteller failed!";
                            }
                            else if (db_check_hnr(args[1]) == 1)
                            {
                                if (db_update_hersteller(args[1], args[2], args[3]) == -1)
                                    throw "Updating of hersteller failed!";
                            }
                        }
                        else if (args[0] == "ip")
                        {
                            cout << "command: " << args[0] << endl;
                            if (db_check_pnr(args[1]) == -1)
                            {
                                throw "Checking pnr " + args[1] + " failed!";
                            }
                            else if (db_check_pnr(args[1]) == 0)
                            {
                                if (db_insert_produkt(args[1], args[2], args[3], args[4]) == -1)
                                    throw "Inserting into produkt failed!";
                            }
                            else if (db_check_pnr(args[1]) == 1)
                            {
                                if (db_update_produkt(args[1], args[2], args[3], args[4]) == -1)
                                    throw "Updating of produkt failed!";
                            }
                        }
                        else if (args[0] == "i")
                        {
                            cout << "command: " << args[0] << endl;
                            if (db_check_hnr(args[1]) == -1)
                            {
                                throw "Checking hnr " + args[1] + " failed!";
                            }
                            else if (db_check_hnr(args[1]) == 0)
                            {
                                if (db_insert_hersteller(args[1], args[2], args[3]) == -1)
                                    throw "Inserting into hersteller failed!";
                            }
                            else if (db_check_hnr(args[1]) == 1)
                            {
                                if (db_update_hersteller(args[1], args[2], args[3]) == -1)
                                    throw "Updating of hersteller failed!";
                            }
                            if (db_check_pnr(args[4]) == -1)
                            {
                                throw "Checking pnr " + args[4] + " failed!";
                            }
                            else if (db_check_pnr(args[4]) == 0)
                            {
                                if (db_insert_produkt(args[4], args[5], args[6], args[1]) == -1)
                                    throw "Inserting into produkt failed!";
                            }
                            else if (db_check_pnr(args[4]) == 1)
                            {
                                if (db_update_produkt(args[4], args[5], args[6], args[1]) == -1)
                                    throw "Updating of produkt failed!";
                            }
                        }
                        else if (args[0] == "dh")
                        {
                            cout << "command: " << args[0] << endl;
                            if (db_delete_hersteller(args[1]) == -1)
                                throw "Deleting hersteller " + args[1] + " failed!";
                        }
                        else if (args[0] == "dp")
                        {
                            cout << "command: " << args[0] << endl;
                            if (db_delete_produkt(args[1]) == -1)
                                throw "Deleting produkt " + args[1] + " failed!";
                        }
                        else if (args[0] == "ch")
                        {
                            cout << "command: " << args[0] << endl;
                            int count = db_count("hersteller");
                            if (count == -1)
                                throw "Counting hersteller failed!";
                            cout << "Anzahl Hersteller: " << count << endl;
                        }
                        else if (args[0] == "cp")
                        {
                            cout << "command: " << args[0] << endl;
                            int count = db_count("produkt");
                            if (count == -1)
                                throw "Counting produkt failed!";
                            cout << "Anzahl Produkte: " << count << endl;
                        }
                    }
                    if (db_commit() == -1) // Wenn alles gut läuft commit
                        throw "Commit failed!";
                }
                catch (const char *e)
                {
                    cout << "ERROR: " << e << endl;
                    if (db_rollback() == -1) // Wenn irgendwas schief geht dann rollback
                        cout << "ROLLBACK failed! Check DB!" << endl;
                }
                db_logout(); // In jedem Fall ausloggen
            }
            else
            {
                return RC_ERROR;
            }
        }
        commandfile.close();
    }
    else
    {
        // wenn nicht genug Argumente übergeben wurden bricht das Programm ab
        cout << "Usage: " << argv[0] << " <connect options> <commandfile>" << endl;
        return RC_ERROR;
    }
    return RC_OK;
}