#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sqlite3.h>

enum ERR { OK = 0, ERROR };

sqlite3 *setupDB();
void menu(sqlite3 *db);
ERR addTodo(sqlite3 *db);
ERR markComplete(sqlite3 *db);
ERR deleteTodo(sqlite3 *db);
ERR updateTodo(sqlite3 *db);
ERR listAll(sqlite3 *db); // select * from Todos

int numberTodos(sqlite3 *db);
int completedTodos(sqlite3 *db);

int main(int, char **) {
  sqlite3 *db = setupDB();
  if (!db) {
    return EXIT_FAILURE;
  }
  menu(db);
  return EXIT_SUCCESS;
}

sqlite3 *setupDB() {
  sqlite3 *db;
  int rc = sqlite3_open("../todo.db", &db);
  if (rc != SQLITE_OK) {
    std::cerr << "Cannot open db" << sqlite3_errcode(db) << std::endl;
    return nullptr;
  }
  return db;
}

void menu(sqlite3 *db) {
  std::cout << "------------------------" << std::endl;
  std::cout << "T E R M I N A L  T O D O" << std::endl;
  std::cout << std::endl;
  char input;
  int count, done;
  while (1) {
    if ((count = numberTodos(db)) == -1)
      return;
    if ((done = completedTodos(db)) == -1)
      return;
    printf("You currently have %d Todos!\n", count);
    printf("You have completed %d Todos!\n", done);
    std::cout << "What would you like to do" << std::endl;
    std::cout << "[A]dd, [L]ist, [C]omplete, [U]pdate, [R]emove, [Q]uit"
              << std::endl;

    input = getchar();
    while (getchar() != '\n')
      ;
    std::cout << std::endl;
    switch (input) {
    case 'a':
    case 'A':
      if (addTodo(db) == ERROR)
        return;
      break;
    case 'l':
    case 'L':
      if (listAll(db) == ERROR)
        return;
      break;
    case 'c':
    case 'C':
      if(completedTodos(db) == ERROR) 
        return;
      break;
    case 'u':
    case 'U':
      if (updateTodo(db) == ERROR)
        return;
      break;
    case 'r':
    case 'R':
      if (deleteTodo(db) == ERROR)
        return;
      break;
    case 'q':
    case 'Q':
      std::cout << "Quit? ah" << std::endl;
      return;
    }
  }
}

ERR addTodo(sqlite3 *db) {
  char todo[128];
  char sql[256];
  printf("\tenter todo: ");
  fgets(todo, sizeof(todo), stdin);
  int loc = strcspn(todo, "\n");
  todo[loc] = '\0';

  sprintf(sql, "insert into Todos(Id, Title, Completed) values(NULL, '%s', 0);",
          todo);
  char *errMsg = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Could not write to DB: %s\n", errMsg);
    sqlite3_free(errMsg);
    return ERROR;
  }
  return OK;
}

ERR markComplete(sqlite3 *db) {
  char sql[256], mark[4];
  char *errMsg = 0;

  printf("\tenter id to mark complete: ");
  fgets(mark, sizeof(mark), stdin);
  int id = atoi(mark);
  sprintf(sql, "update Todos set Completed=1 where id=%d;", id);

  int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Could not Complete TODO: %s\n", errMsg);
    sqlite3_free(errMsg);
    return ERROR;
  }
  return OK;
}

ERR deleteTodo(sqlite3 *db) {
  char sql[256], mark[4];
  char *errMsg;
  printf("\tenter id to delete: ");
  fgets(mark, sizeof(mark), stdin);
  int id = atoi(mark);
  sprintf(sql, "delete from Todos where id=%d;", id);
  int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Could not delete: %s\n", errMsg);
    return ERROR;
  }
  return OK;
}

ERR updateTodo(sqlite3 *db) {
  char sql[256], mark[4], todo[128];
  char *errMsg;
  printf("\tenter id to edit: ");
  fgets(mark, sizeof(mark), stdin);
  int id = atoi(mark);

  printf("\tenter new title for Todo: ");
  fgets(todo, sizeof(todo), stdin);

  todo[strcspn(todo, "\n")] = '\0';

  sprintf(sql, "update Todos set Title='%s' where Id=%d", todo, id);

  int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Could not update Todos: %s\n", errMsg);
    sqlite3_free(errMsg);
    return ERROR;
  }
  return OK;
}

ERR listAll(sqlite3 *db) {
  sqlite3_stmt *stmt;
  const char *sql = "select * from Todos;";
  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    // std::cout << "Cannot read the database: " << sqlite3_errmsg(db) <<
    // std::endl;
    fprintf(stderr, "Could not read the db: %s", sqlite3_errmsg(db));
    return ERROR;
  }
  printf("%-5s%-25s%s\n", "Id", "Todo", "Completed");
  printf("----------------------------------------\n");
  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    char *todo = (char *)sqlite3_column_text(stmt, 1);
    if (!todo) {
      todo = "NULL";
    }
    int completed = sqlite3_column_int(stmt, 2);
    char *done = "done";
    if (!completed) {
      done = "not";
    }
    printf("%-5d%-25s%s\n", id, todo, done);
    printf("----------------------------------------\n");
  }
  return OK;
}

int numberTodos(sqlite3 *db) {
  int count = 0;
  sqlite3_stmt *stmt;
  const char *sql = "select count(*) from Todos;";
  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Could not get count from db: %s\n", sqlite3_errmsg(db));
    return -1;
  }
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    count = sqlite3_column_int(stmt, 0);
  }
  return count;
}

int completedTodos(sqlite3 *db) {
  int count = 0;
  sqlite3_stmt *stmt;
  const char *sql = "select * from Todos where Completed=1;";
  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "could not get number of complete todos: %s\n",
            sqlite3_errmsg(db));
    return -1;
  }

  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    count++;
  }

  return count;
}