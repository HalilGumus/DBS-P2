//
// DBS-Praktikum Aufgabe 2
//    Header abstraktes DB-Interface
//
// Christoph Dalitz & Klaus Weidenhaupt
// Hochschule Niederrhein
// 2020/11/17
//

#ifndef DB_H
#define DB_H

#include <string>

using namespace std;

// ----------------------
// Datenbank-Login/Logout
// ----------------------

// Datenbank-Login
// rc: 0 = ok, -1 = error
int db_login(const string &user, const string &password, const string &host, const string &port, const string &dbname);

// Datenbank-Logout
void db_logout();

// -------------------
// Transaktionsbefehle
// -------------------

// rc: 0 = ok, -1 = error
int db_begin();
int db_commit();
int db_rollback();

// ----------------------------------
// DB-Schema anlegen und zurücksetzen
// ----------------------------------

// Hersteller-Tabelle anlegen
// rc: 0 = ok, -1 = error
int db_create_table_hersteller();

// Produkt-Tabelle anlegen
// rc: 0 = ok, -1 = error
int db_create_table_produkt();

// Tabelle tablename löschen und aus DB-Schema entfernen
// rc: 0 = ok, -1 = error
int db_drop_table(const string &tablename);

// -------------------------------------
// Existenz/Anzahl von Tupel zurückgeben
// -------------------------------------

// Prüfen, ob hnr in Hersteller schon vorhanden ist?
// rc: 0 = noch nicht da, 1 = schon da, -1 = error
int db_check_hnr(const string &hnr);

// Prüfen, ob pnr in Produkt schon vorhanden ist?
// rc: 0 = noch nicht da, 1 = schon da, -1 = error
int db_check_pnr(const string &pnr);

// Anzahl der Tupel in der Tabelle tablename zurückgeben
// rc: n = Anzahl der Tupel (n>=0), -1 = error
int db_count(const string &tablename);

// -------------------------------
// Tupel einfügen, ändern, löschen
// -------------------------------

// Einfuegen Produkt
// rc: 0 = ok, -1 = error
int db_insert_produkt(const string &pnr, const string &name, const string &preis, const string &hnr);

// Einfuegen Hersteller
// rc: 0 = ok, -1 = error
int db_insert_hersteller(const string &hnr, const string &name, const string &stadt);

// Ändern Produkt
// rc: 0 = ok, -1 = error
int db_update_produkt(const string &pnr, const string &name, const string &preis, const string &hnr);

// Ändern Hersteller
// rc: 0 = ok, -1 = error
int db_update_hersteller(const string &hnr, const string &name, const string &stadt);

// Löschen Produkt
// rc: 0 = ok, -1 = error
int db_delete_produkt(const string &pnr);

// Löschen Hersteller und aller abhängigen Produkte
// rc: 0 = ok, -1 = error
int db_delete_hersteller(const string &hnr);

// Loeschen des kompletten Tabelleninhalts beider Tabellen
// rc: 0 = ok, -1 = error
int db_delete();

#endif
