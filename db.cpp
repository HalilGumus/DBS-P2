#include <iostream>
#include <string>
#include "postgresql/libpq-fe.h"

using namespace std;

PGconn *conn;

// ----------------------
// Datenbank-Login/Logout
// ----------------------

// Datenbank-Login
// rc: 0 = ok, -1 = error
int db_login(const string &user, const string &password, const string &host, const string &port, const string &dbname)
{
    string conn_string;
    conn_string = "user=" + user + " " + "password=" + password + " " + "host=" + host + " " + "port=" + port + " " + "dbname=" + dbname;
    conn = PQconnectdb(conn_string.c_str());

    if (PQstatus(conn) != CONNECTION_OK)
    {
        cout << "Connection failed!" << endl;
        return -1;
    }
    else
    {
        cout << "Connection ok!" << endl;
        return 0;
    }
}

// Datenbank-Logout
void db_logout()
{
    PQfinish(conn);
    cout << "Connection closed" << endl;
}

// -------------------
// Transaktionsbefehle
// -------------------

// rc: 0 = ok, -1 = error
int db_begin()
{
    string query = "BEGIN;";
    cout << "BEGIN" << endl;
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        cout << "BEGIN failed" << endl;
        return -1;
    }

    PQclear(res);
    return 0;
}

int db_commit()
{
    string query = "COMMIT;";
    cout << "COMMIT" << endl;
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        cout << "COMMIT failed" << endl;
        return -1;
    }

    PQclear(res);
    return 0;
}

int db_rollback()
{
    string query = "ROLLBACK;";
    cout << "ROLLBACK" << endl;
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
}

// ----------------------------------
// DB-Schema anlegen und zurücksetzen
// ----------------------------------

// Hersteller-Tabelle anlegen
// rc: 0 = ok, -1 = error
int db_create_table_hersteller()
{
    string query = "CREATE TABLE IF NOT EXISTS hersteller (hnr varchar(4) NOT NULL PRIMARY KEY, name varchar(30), stadt varchar(30))";
    cout << "Creating table hersteller" << endl;
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
}

// Produkt-Tabelle anlegen
// rc: 0 = ok, -1 = error
int db_create_table_produkt()
{
    string query = "CREATE TABLE IF NOT EXISTS produkt (pnr varchar(4) NOT NULL PRIMARY KEY, name varchar(30), preis numeric(8,2), hnr varchar(4) REFERENCES hersteller(hnr) ON UPDATE CASCADE ON DELETE CASCADE)";
    cout << "Creating table prodkt" << endl;
    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
}

// Tabelle tablename löschen und aus DB-Schema entfernen
// rc: 0 = ok, -1 = error
int db_drop_table(const string &tablename)
{
    string query = "DROP TABLE IF EXISTS " + tablename;
    cout << "Dropping table: " << tablename << endl;
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }
    PQclear(res);
    return 0;
}

// -------------------------------------
// Existenz/Anzahl von Tupel zurückgeben
// -------------------------------------

// Prüfen, ob hnr in Hersteller schon vorhanden ist?
// rc: 0 = noch nicht da, 1 = schon da, -1 = error
int db_check_hnr(const string &hnr)
{
    string query = "SELECT COUNT(*) FROM hersteller WHERE hnr = '" + hnr + "';";
    cout << "Checking hnr: " << hnr << endl;
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        PQclear(res);
        return -1;
    }

    int count = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    if (count == 0)
    {
        return 0;
    }
    else if (count == 1)
    {
        return 1;
    }
    // um der Compilerwarnung zu entgehen
    return count;
}

// Prüfen, ob pnr in Produkt schon vorhanden ist?
// rc: 0 = noch nicht da, 1 = schon da, -1 = error
int db_check_pnr(const string &pnr)
{
    string query = "SELECT COUNT(*) FROM produkt WHERE pnr = '" + pnr + "';";
    cout << "Checking pnr: " << pnr << endl;
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        PQclear(res);
        return -1;
    }
    int count = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    if (count == 0)
    {
        return 0;
    }
    else if (count == 1)
    {
        return 1;
    }
    // um der Compilerwarnung zu entgehen
    return count;
}

// Anzahl der Tupel in der Tabelle tablename zurückgeben
// rc: n = Anzahl der Tupel (n>=0), -1 = error
int db_count(const string &tablename)
{
    string query = "SELECT COUNT(*) FROM " + tablename + ";";
    cout << "Counting tuples from " << tablename << endl;
    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        PQclear(res);
        return -1;
    }
    int count = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return count;
}

// -------------------------------
// Tupel einfügen, ändern, löschen
// -------------------------------

// Einfuegen Produkt
// rc: 0 = ok, -1 = error
int db_insert_produkt(const string &pnr, const string &name, const string &preis, const string &hnr)
{
    string query = "INSERT INTO produkt VALUES ('" + pnr + "','" + name + "','" + preis + "','" + hnr + "');";
    cout << "Inserting into produkt: " << pnr << ", " << name << ", " << preis << ", " << hnr << endl;

    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }
    PQclear(res);
    return 0;
}

// Einfuegen Hersteller
// rc: 0 = ok, -1 = error
int db_insert_hersteller(const string &hnr, const string &name, const string &stadt)
{
    string query = "INSERT INTO hersteller VALUES ('" + hnr + "','" + name + "','" + stadt + "')";
    cout << "Inserting into hersteller: " << hnr << ", " << name << ", " << stadt << endl;
    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }
    PQclear(res);
    return 0;
}

// Ändern Produkt
// rc: 0 = ok, -1 = error
int db_update_produkt(const string &pnr, const string &name, const string &preis, const string &hnr)
{
    string query = "UPDATE produkt SET name = '" + name + "', preis = '" + preis + "', hnr = '" + hnr + "' WHERE pnr = '" + pnr + "';";
    cout << "Updating produkt: " << pnr << endl;
    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }
    PQclear(res);
    return 0;
}

// Ändern Hersteller
// rc: 0 = ok, -1 = error
int db_update_hersteller(const string &hnr, const string &name, const string &stadt)
{
    string query = "UPDATE hersteller SET name = '" + name + "', stadt = '" + stadt + "' WHERE hnr = '" + hnr + "';";
    cout << "Updating hersteller: " << hnr << endl;
    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }
    PQclear(res);
    return 0;
}

// Löschen Produkt
// rc: 0 = ok, -1 = error
int db_delete_produkt(const string &pnr)
{
    string query = "DELETE FROM produkt WHERE pnr ='" + pnr + "';";
    cout << "Deleting from produkt: " << pnr << endl;
    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }
    PQclear(res);
    return 0;
}

// Löschen Hersteller und aller abhängigen Produkte
// rc: 0 = ok, -1 = error
int db_delete_hersteller(const string &hnr)
{
    string query = "DELETE FROM hersteller WHERE hnr ='" + hnr + "';";
    cout << "Deleting from hersteller: " << hnr << endl;
    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }
    PQclear(res);
    return 0;
}

// Loeschen des kompletten Tabelleninhalts beider Tabellen
// rc: 0 = ok, -1 = error
int db_delete()
{
    string query_produkt = "TRUNCATE TABLE produkt;";
    string query_hersteller = "TRUNCATE TABLE hersteller;";
    cout << "Truncating from produkt" << endl;
    PGresult *res = PQexec(conn, query_produkt.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }
    PQclear(res);
    cout << "Truncating from hersteller" << endl;
    res = PQexec(conn, query_hersteller.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        PQclear(res);
        return -1;
    }
    PQclear(res);
    return 0;
}