
#include "AuthSystem.h"

#include <iostream>
#include <string>
#include <limits>
#include <windows.h>

using namespace std;

AuthSystem::AuthSystem(Database& database)
    : database(database) {
}

bool AuthSystem::login(AuthenticatedUser& user) {

    string username;
    string password;

    cout << "\n============================================\n";
    cout << "              BEAUTRIX LOGIN\n";
    cout << "============================================\n";

    cout << "Username: ";
    getline(cin, username);

    if (username.empty()) {
        cout << "\nUsername cannot be empty.\n";
        return false;
    }

    cout << "Password: ";

    HANDLE consoleHandle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD originalMode = 0;
    bool passwordHidden = false;

    if (GetConsoleMode(consoleHandle, &originalMode)) {
        DWORD newMode = originalMode & ~ENABLE_ECHO_INPUT;

        if (SetConsoleMode(consoleHandle, newMode)) {
            passwordHidden = true;
        }
    }

    getline(cin, password);

    if (passwordHidden) {
        SetConsoleMode(consoleHandle, originalMode);
        cout << "\n";
    }

    if (password.empty()) {
        cout << "\nPassword cannot be empty.\n";
        return false;
    }

    const char* values[] = {
        username.c_str(),
        password.c_str()
    };

    string sql =
        "SELECT id, username, role "
        "FROM users "
        "WHERE username = $1 "
        "AND password_hash = crypt($2, password_hash);";

    PGresult* result = database.params(sql, 2, values);

    if (!result || PQresultStatus(result) != PGRES_TUPLES_OK) {
        cout << "\nLogin could not be completed. Check the database connection.\n";

        if (result) {
            cerr << PQresultErrorMessage(result) << "\n";
        }

        database.finish(result);
        return false;
    }

    if (PQntuples(result) != 1) {
        cout << "\nIncorrect username or password.\n";
        database.finish(result);
        return false;
    }

    user.id = stoi(PQgetvalue(result, 0, 0));
    user.username = PQgetvalue(result, 0, 1);
    user.role = PQgetvalue(result, 0, 2);

    database.finish(result);

    cout << "\nLogin successful!\n";
    cout << "Welcome, " << user.username << "!\n";
    cout << "Role: " << user.role << "\n";

    return true;
}
